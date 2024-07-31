#include "new_x52_std.h"

FakeStdThrottle::FakeStdThrottle(PIO pio, int PIN_C01, int PIN_C02, int PIN_C03, int PIN_C04) {
	pio = this->pio;
	PIN_C01 = this->PIN_C01;
	PIN_C02 = this->PIN_C02;
	PIN_C03 = this->PIN_C03;
	PIN_C04 = this->PIN_C04;
	PIN_C01 = this->PIN_C01;
	#if defined(ARDUINO_ARCH_RP2040)
  // Find a free SM on one of the PIO's
  sm = pio_claim_unused_sm(pio, false); // don't panic
  // Try pio1 if SM not found
  if (sm < 0) {
    pio = pio1;
    sm = pio_claim_unused_sm(pio, true); // panic if no SM is free
  }
  init = true;
	#endif
}

FakeStdThrottle::~FakeStdThrottle() {}
//template <int PIN_C01, int PIN_C02, int PIN_C03, int PIN_C04, typename PulseWaiter=InterruptPulseWaiter<PIN_C04>>
//class FakeStdThrottle {
//public:
	// Call Setup from the setup function of your Arduino project to initialize
	// a FakeStdThrottle instance.
	void FakeStdThrottle::setup() {
		pinMode(PIN_C01, OUTPUT);
		// The value of C01 is allowed to be anything between frames (undefined).
		// digitalWrite(PIN_C01, LOW);
		pinMode(PIN_C02, OUTPUT);
		// On the teensy the digitalWrite seems to work only after pinMode.
		digitalWrite(PIN_C02, LOW);
		pinMode(PIN_C03, INPUT);
		pinMode(PIN_C04, INPUT);
		//m_PulseWaiter.Setup();
	}

// PollJoystickState polls the joystick for its state. It creates a frame
// transmission request, waits for the joystick to respond and handles the
// bidirectional data transfer: sends the JoystickConfig and receives the
// JoystickState. Returns zero on success.
//
// A nonzero return value means error and gives the recommended number
// of microseconds to wait before calling PollJoystickState again.
// In that situation the value of the JoystickState is undefined.
//
// My X52 joystick is willing to send its state at most ~50 times per second.
unsigned long FakeStdThrottle::PollJoystickState(JoystickState& state, const JoystickConfig& cfg, unsigned long wait_micros) {
		// Note: PIN_C02 has to be LOW when this function returns.

		{
			// deadline for the initial response
			unsigned long wait_deadline = micros()+wait_micros;

			if (!wait_for_pin_state(PIN_C04, LOW, wait_deadline))
				return 1;

			// The original joystick's C04 pulse seems to be at least 15us long.

			//auto trigger = [](){
			//	digitalWrite(PIN_C02, HIGH);
			//};
			//auto wait_res = m_PulseWaiter.WaitForPulse(wait_deadline, trigger);
			//if (wait_res != PulseFinished) {
			//	X52DebugPrintln("Timed out while waiting for the C04 pulse before receiving the joystick state.");
			//	digitalWrite(PIN_C02, LOW);
			//	return (wait_res == PulseNotStarted) ? 1 : X52_THROTTLE_UNRESPONSIVE_MICROS;
			//}
		}

		// deadline for the whole frame transmission
		unsigned long deadline = micros() + X52_THROTTLE_TIMEOUT_MICROS;

		JoystickState::Binary recv_buf;

		for (int i=0; i<(JoystickState::NUM_BITS-1); i++) {

			// I don't have an X52 throttle to test this but the throttle must
			// be sampling C03 between falling-C04 and falling-C02.
			// The other sensible option (between rising-C04 and rising-C02)
			// wouldn't work because the joystick often removes the data bit
			// from C03 before the rising-C02 edge.
			recv_buf.SetBit(i, bool(digitalRead(PIN_C03)));

			digitalWrite(PIN_C02, LOW);

			if (!wait_for_pin_state(PIN_C04, HIGH, deadline)) {
				X52DebugPrint("Error waiting for C04=1 while receiving the joystick state. Clock cycle: ");
				X52DebugPrintln(i);
				digitalWrite(PIN_C02, LOW);
				return X52_THROTTLE_UNRESPONSIVE_MICROS;
			}

			digitalWrite(PIN_C02, HIGH);

			if (!wait_for_pin_state(PIN_C04, LOW, deadline)) {
				X52DebugPrint("Error waiting for C04=0 while receiving the joystick state. Clock cycle: ");
				X52DebugPrintln(i);
				digitalWrite(PIN_C02, LOW);
				return X52_THROTTLE_UNRESPONSIVE_MICROS;
			}
		}
		recv_buf.SetBit(JoystickState::NUM_BITS-1, bool(digitalRead(PIN_C03)));

		// The original joystick's C04 pulse seems to be at least 50us long.

		//auto trigger = [](){
		//	digitalWrite(PIN_C02, LOW);
		//};
		//auto wait_res = m_PulseWaiter.WaitForPulse(deadline, trigger);
		//if (wait_res != PulseFinished) {
		//	X52DebugPrintln("Timed out while waiting for the C04 pulse before sending the joystick config.");
		//	return X52_THROTTLE_UNRESPONSIVE_MICROS;
		//}

		JoystickConfig::Binary send_buf;
		cfg.ToBinary(send_buf);

		for (int i=0; i<JoystickConfig::NUM_BITS; i++) {
			// The joystick samples C01 between rising-C02 and rising-C04 (last 8 rising edges of C02)
			digitalWrite(PIN_C01, send_buf.Bit(i));
			digitalWrite(PIN_C02, HIGH);
			if (!wait_for_pin_state(PIN_C04, HIGH, deadline)) {
				X52DebugPrint("Error waiting for C04=1 while sending the joystick config. Clock cycle: ");
				X52DebugPrintln(i);
				digitalWrite(PIN_C02, LOW);
				return X52_THROTTLE_UNRESPONSIVE_MICROS;
			}
			digitalWrite(PIN_C02, LOW);
			if (!wait_for_pin_state(PIN_C04, LOW, deadline)) {
				X52DebugPrint("Error waiting for C04=0 while sending the joystick config. Clock cycle: ");
				X52DebugPrintln(i);
				return X52_THROTTLE_UNRESPONSIVE_MICROS;
			}
		}
		// The value of C01 is allowed to be anything between frames (undefined).
		// digitalWrite(PIN_C01, LOW);

		// SetFromBinary verifies the checksum and returns false on error
		return state.SetFromBinary(recv_buf) ? 0 : X52_THROTTLE_UNRESPONSIVE_MICROS;
	}

//private:
//	PulseWaiter m_PulseWaiter;
//};


// FakeStdJoystick makes it possible to use some of your Arduino pins as a
// connection to the PS/2 socket of an X52 (non-Pro) Throttle.
//
// The pin config is the same as that of the FakeThrottle.
//template <int PIN_C01, int PIN_C02, int PIN_C03, int PIN_C04>
//class FakeStdJoystick {
//public:
FakeStdJoystick::FakeStdJoystick(PIO pio, int PIN_C01, int PIN_C02, int PIN_C03, int PIN_C04) {
	pio = this->pio;
	PIN_C01 = this->PIN_C01;
	PIN_C02 = this->PIN_C02;
	PIN_C03 = this->PIN_C03;
	PIN_C04 = this->PIN_C04;
	PIN_C01 = this->PIN_C01;
	#if defined(ARDUINO_ARCH_RP2040)
  // Find a free SM on one of the PIO's
  sm = pio_claim_unused_sm(pio, false); // don't panic
  // Try pio1 if SM not found
  if (sm < 0) {
    pio = pio1;
    sm = pio_claim_unused_sm(pio, true); // panic if no SM is free
  }
  init = true;
	#endif
}

FakeStdJoystick::~FakeStdJoystick() {}

// Call Setup from the setup function of your Arduino project to initialize
// a FakeStdJoystick instance.
void FakeStdJoystick::setup() {
	pinMode(PIN_C01, INPUT);
	pinMode(PIN_C02, INPUT);
	pinMode(PIN_C03, OUTPUT);
	pinMode(PIN_C04, OUTPUT);
	// On the teensy the digitalWrite seems to work only after pinMode.
	digitalWrite(PIN_C04, LOW);
}

// SendJoystickState sends the JoystickState to the throttle and receives
// the JoystickConfig. On the other side there must be a throttle polling/waiting
// for the JoystickState. Returns zero on success.
//
// A nonzero return value means error and gives the number of microseconds
// to wait before calling SendJoystickState again. In that situation the
// value of the JoystickConfig is undefined.
unsigned long FakeStdJoystick::SendJoystickState(const JoystickState& state, JoystickConfig& cfg, unsigned long wait_micros) {
	if (!wait_for_pin_state(PIN_C02, HIGH, micros()+wait_micros))
		return 1;

	JoystickState::Binary send_buf;
	state.ToBinary(send_buf);

	auto deadline = micros() + X52_JOYSTICK_TIMEOUT_MICROS;

	// The first data bit has to be on C03 before the falling edge of C04
	digitalWrite(PIN_C03, send_buf.Bit(0));

	// The first C04 pulse that doesn't require an ACK from the throttle
	digitalWrite(PIN_C04, HIGH);
	// The original joystick uses a >=15us pulse and I don't have a throttle to test shorter pulses.
	delayMicroseconds(X52_FIRST_C04_PULSE_MICROS);
	digitalWrite(PIN_C04, LOW);

	// The throttle samples C03 for the first data bit here between falling-C04 and falling-C02.

	if (!wait_for_pin_state(PIN_C02, LOW, deadline)) {
		X52DebugPrintln("Error waiting for C02=0 while sending the first bit of the joystick state.");
		return X52_JOYSTICK_UNRESPONSIVE_MICROS;
	}

	// sending the rest of the joystick state
	for (int i=1; i<JoystickState::NUM_BITS; i++) {

		// I don't have an X52 throttle to test this but the throttle must
		// be sampling C03 between falling-C04 and falling-C02.
		// The other sensible option (between rising-C04 and rising-C02)
		// wouldn't work because the joystick often removes the data bit
		// from C03 before the rising-C02 edge.

		// The data bit has to be on C03 before the falling edge of C04.
		digitalWrite(PIN_C03, send_buf.Bit(i));

		digitalWrite(PIN_C04, HIGH);
		if (!wait_for_pin_state(PIN_C02, HIGH, deadline)) {
			X52DebugPrint("Error waiting for C02=1 while sending the joystick state. Clock cycle: ");
			X52DebugPrintln(i);
			digitalWrite(PIN_C04, LOW);
			return X52_JOYSTICK_UNRESPONSIVE_MICROS;
		}

		digitalWrite(PIN_C04, LOW);
		// This is where the throttle samples C03 for the data bit
		if (!wait_for_pin_state(PIN_C02, LOW, deadline)) {
			X52DebugPrint("Error waiting for C02=0 while sending the joystick state. Clock cycle: ");
			X52DebugPrintln(i);
			return X52_JOYSTICK_UNRESPONSIVE_MICROS;
		}
	}

	// The second C04 pulse that doesn't require an ACK from the throttle
	digitalWrite(PIN_C04, HIGH);
	// The original joystick uses a >=50us pulse and I don't have a throttle to test shorter pulses.
	delayMicroseconds(X52_SECOND_C04_PULSE_MICROS);
	digitalWrite(PIN_C04, LOW);

	JoystickConfig::Binary recv_buf;

	// receiving the config from the throttle
	for (int i=0; i<JoystickConfig::NUM_BITS; i++) {
		if (!wait_for_pin_state(PIN_C02, HIGH, deadline)) {
			X52DebugPrint("Error waiting for C02=1 while receiving the joystick config. Clock cycle: ");
			X52DebugPrintln(i);
			return X52_JOYSTICK_UNRESPONSIVE_MICROS;
		}
		// The joystick samples C01 between rising-C02 and rising-C04 (last 8 rising edges of C02)
		recv_buf.SetBit(i, bool(digitalRead(PIN_C01)));
		digitalWrite(PIN_C04, HIGH);
		if (!wait_for_pin_state(PIN_C02, LOW, deadline)) {
			X52DebugPrint("Error waiting for C02=0 while receiving the joystick config. Clock cycle: ");
			X52DebugPrintln(i);
			digitalWrite(PIN_C04, LOW);
			return X52_JOYSTICK_UNRESPONSIVE_MICROS;
		}
		digitalWrite(PIN_C04, LOW);
	}

	cfg.SetFromBinary(recv_buf);
	return 0;
}

// IsPollInProgress returns true if the throttle is waiting for the
// JoystickState on the other side of the connection. In that situation
// a call to SendJoystickState is less likely to block in a waiting state
// (or fail as a result of wait timeout).
bool FakeStdJoystick::IsPollInProgress() {
	return bool(digitalRead(PIN_C02));
}


