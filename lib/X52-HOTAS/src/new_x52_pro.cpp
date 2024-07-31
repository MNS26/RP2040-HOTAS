#include "new_x52_pro.h"


inline void SetUInt(uint32_t b, uint8_t index, uint8_t size, uint8_t value) {
	for (uint8_t i = 0; i<size; i++)
		bitWrite(b, index+i, (value >> i) & 1);
}

inline void SetBit(uint32_t b, uint8_t index, bool value) {
	bitWrite(b, index, value);
}


inline void JoystickConfig::SetFromBinary(const Binary& b) {
	led_brightness = uint8_t(b.UInt(0, 5));
	pov_1_led_blinking = b.Bit(5);
	button_a_led = LEDColor(b.UInt(6, 2));
	pov_2_led = LEDColor(b.UInt(8, 2));
	button_fire_led = !b.Bit(10);
	button_b_led = LEDColor(b.UInt(11, 2));
	button_t1_t2_led = LEDColor(b.UInt(13, 2));
	button_t3_t4_led = LEDColor(b.UInt(15, 2));
	button_t5_t6_led = LEDColor(b.UInt(17, 2));
}


inline void JoystickConfig::ToBinary(Binary& b) const {
	b.SetUInt(0, 5, led_brightness);
	b.SetBit(5, pov_1_led_blinking);
	b.SetUInt(6, 2, button_a_led);
	b.SetUInt(8, 2, pov_2_led);
	b.SetBit(10, !button_fire_led);
	b.SetUInt(11, 2, button_b_led);
	b.SetUInt(13, 2, button_t1_t2_led);
	b.SetUInt(15, 2, button_t3_t4_led);
	b.SetUInt(17, 2, button_t5_t6_led);
}


inline void JoystickState::SetFromBinary(const Binary& b) {
	x = uint16_t(b.UInt(0, 8) | (b.UInt(16, 2) << 8));
	y = uint16_t(b.UInt(8, 8) | (b.UInt(18, 2) << 8));
	z = uint16_t(b.UInt(24, 8) | (b.UInt(22, 2) << 8));

	switch (b.UInt(32, 4)) {
		case 1: pov_1 = Down; break;
		case 2: pov_1 = DownRight; break;
		case 3: pov_1 = Right; break;
		case 4: pov_1 = UpRight; break;
		case 5: pov_1 = Up; break;
		case 6: pov_1 = UpLeft; break;
		case 7: pov_1 = Left; break;
		case 8: pov_1 = DownLeft; break;
		default: pov_1 = NoDirection; break;
	}

	pov_2 = Direction(
		(-b.Bit(36) & Up) |
		(-b.Bit(37) & Right) |
		(-b.Bit(38) & Down) |
		(-b.Bit(39) & Left)
	);

	switch (b.UInt(45, 3)) {
		case 1: mode = Mode1; break;
		case 2: mode = Mode2; break;
		case 4: mode = Mode3; break;
		default: mode = ModeUndefined; break;
	}

	trigger_stage_1 = b.Bit(40);
	button_fire = b.Bit(41);
	button_a = b.Bit(42);
	button_c = b.Bit(43);
	trigger_stage_2 = b.Bit(44);
	button_b = b.Bit(48);
	pinkie_switch = b.Bit(49);
	button_t1 = b.Bit(50);
	button_t2 = b.Bit(51);
	button_t3 = b.Bit(52);
	button_t4 = b.Bit(53);
	button_t5 = b.Bit(54);
	button_t6 = b.Bit(55);
}
inline void JoystickState::SetFromBinary2(const uint64_t b) {

	SetUInt((uint32_t)x,0,8,b);
	SetUInt((uint32_t)x,16,2,(b>>15)&3);
	
	SetUInt((uint32_t)y,8,8,b);
	SetUInt((uint32_t)y,18,2,(b>>17)&3);
	
	SetUInt((uint32_t)z,24,8,b);
	SetUInt((uint32_t)z,22,2,(b>>21)&3);

	//x = uint16_t(b.UInt(0, 8) | (b.UInt(16, 2) << 8));
	//y = uint16_t(b.UInt(8, 8) | (b.UInt(18, 2) << 8));
	//z = uint16_t(b.UInt(24, 8) | (b.UInt(22, 2) << 8));

	uint8_t pov = 0;
	SetUInt((uint32_t)pov,32,4,(b>>31)&0xf);

	switch (pov) {
		case 1: pov_1 = Down; break;
		case 2: pov_1 = DownRight; break;
		case 3: pov_1 = Right; break;
		case 4: pov_1 = UpRight; break;
		case 5: pov_1 = Up; break;
		case 6: pov_1 = UpLeft; break;
		case 7: pov_1 = Left; break;
		case 8: pov_1 = DownLeft; break;
		default: pov_1 = NoDirection; break;
	}

	pov = 0;
	SetUInt((uint32_t)pov,36,4,(b>>35)&0xf);
	pov_2 = Direction(
		(bitRead(pov,0) & Up) |
		(bitRead(pov,1) & Right) |
		(bitRead(pov,2) & Down) |
		(bitRead(pov,3) & Left)
	);

	//pov_2 = Direction(
	//	(-b.Bit(36) & Up) |
	//	(-b.Bit(37) & Right) |
	//	(-b.Bit(38) & Down) |
	//	(-b.Bit(39) & Left)
	//);
	uint8_t m = 0;
	SetUInt((uint32_t)m,45,3,(b>>44)&3);
	

	switch (m) {
		case 1: mode = Mode1; break;
		case 2: mode = Mode2; break;
		case 4: mode = Mode3; break;
		default: mode = ModeUndefined; break;
	}
	trigger_stage_1 = bitRead(b,40);
	button_fire = bitRead(b,41);
	button_a = bitRead(b,42);
	button_c = bitRead(b,43);
	trigger_stage_2 = bitRead(b,44);
	button_b = bitRead(b,48);
	pinkie_switch = bitRead(b,49);
  button_t1 = bitRead(b,50);
  button_t2 = bitRead(b,51);
  button_t3 = bitRead(b,52);
  button_t4 = bitRead(b,53);
  button_t5 = bitRead(b,54);
  button_t6 = bitRead(b,55);	

	//trigger_stage_1 = b.Bit(40);
	//button_fire = b.Bit(41);
	//button_a = b.Bit(42);
	//button_c = b.Bit(43);
	//trigger_stage_2 = b.Bit(44);
	//button_b = b.Bit(48);
	//pinkie_switch = b.Bit(49);
	//button_t1 = b.Bit(50);
	//button_t2 = b.Bit(51);
	//button_t3 = b.Bit(52);
	//button_t4 = b.Bit(53);
	//button_t5 = b.Bit(54);
	//button_t6 = b.Bit(55);
}


inline void JoystickState::ToBinary(Binary& b) const {
#if X52_DEBUG
	if (x > MAX_X) {
		X52DebugPrint("x52::pro::JoystickState.x exceeds the maximum value. x=");
		X52DebugPrintln(x);
	}
	if (y > MAX_Y) {
		X52DebugPrint("x52::pro::JoystickState.y exceeds the maximum value. y=");
		X52DebugPrintln(y);
	}
	if (z > MAX_Z) {
		X52DebugPrint("x52::pro::JoystickState.z exceeds the maximum value. z=");
		X52DebugPrintln(z);
	}
#endif

	b.SetUInt(0, 8, x);
	b.SetUInt(8, 8, y);
	b.SetUInt(24, 8, z);

	b.SetUInt(16, 2, x >> 8);
	b.SetUInt(18, 2, y >> 8);
	b.SetUInt(20, 2, 0);
	b.SetUInt(22, 2, z >> 8);

	uint8_t pov;
	switch (pov_1) {
		case Down: pov = 1; break;
		case DownRight: pov = 2; break;
		case Right: pov = 3; break;
		case UpRight: pov = 4; break;
		case Up: pov = 5; break;
		case UpLeft: pov = 6; break;
		case Left: pov = 7; break;
		case DownLeft: pov = 8; break;
		default: pov = 0; break;
	}
	b.SetUInt(32, 4, pov);

	b.SetBit(36, bool(pov_2 & Up));
	b.SetBit(37, bool(pov_2 & Right));
	b.SetBit(38, bool(pov_2 & Down));
	b.SetBit(39, bool(pov_2 & Left));

	b.SetBit(40, trigger_stage_1);
	b.SetBit(41, button_fire);
	b.SetBit(42, button_a);
	b.SetBit(43, button_c);
	b.SetBit(44, trigger_stage_2);
	b.SetBit(45, mode == Mode1);
	b.SetBit(46, mode == Mode2);
	b.SetBit(47, mode == Mode3);
	b.SetBit(48, button_b);
	b.SetBit(49, pinkie_switch);
	b.SetBit(50, button_t1);
	b.SetBit(51, button_t2);
	b.SetBit(52, button_t3);
	b.SetBit(53, button_t4);
	b.SetBit(54, button_t5);
	b.SetBit(55, button_t6);
}

// FakeProThrottle makes it possible to use some of your Arduino pins as a
// connection to the PS/2 socket of an X52 Pro Joystick.
//
// These pin names (C01..C04) were printed on the PCB of my X52 joystick.
// The pinout of the PS/2 connector is the same as that of the X52 non-Pro
// but the protocol is different.
//
// Standard PS/2 (6-pin mini-DIN) female socket pin numbering:
// https://en.wikipedia.org/wiki/Mini-DIN_connector#/media/File:MiniDIN-6_Connector_Pinout.svg
//
// PIN_C01: data output of the throttle   (pin #4 of the PS/2 female socket)
// PIN_C02: clock output of the throttle  (pin #6 of the PS/2 female socket)
// PIN_C03: data output of the joystick   (pin #2 of the PS/2 female socket)
// PIN_C04: clock output of the joystick  (pin #1 of the PS/2 female socket)
//
// Pin #3 of the PS/2 female socket is GND.
// Pin #5 of the PS/2 female socket is VCC.
//
// My X52 Pro throttle uses 4.1-4.2V for both power and GPIO but the joystick
// works with 3.3V too.

FakeProThrottle::FakeProThrottle(PIO pio, int PIN_C01, int PIN_C02, int PIN_C03, int PIN_C04) {
	#if defined(ARDUINO_ARCH_RP2040)
  // Find a free SM on one of the PIO's
  sm = pio_claim_unused_sm(pio, false); // don't panic
  // Try pio1 if SM not found
  if (sm < 0) {
    pio = pio1;
    sm = pio_claim_unused_sm(pio, true); // panic if no SM is free
  }
	#endif
	this->pio = pio;
	this->PIN_C01 = PIN_C01;
	this->PIN_C02 = PIN_C02;
	this->PIN_C03 = PIN_C03;
	this->PIN_C04 = PIN_C04;
	this->PIN_C01 = PIN_C01;
}

FakeProThrottle::~FakeProThrottle() {}

//template <PIO pio, int PIN_C01, int PIN_C02, int PIN_C03, int PIN_C04>
//lass FakeProThrottle {


//public:
	// Call Setup from the setup function of your Arduino project to initialize
	// a FakeProThrottle instance.
void FakeProThrottle::setup() {
	pinMode(PIN_C01, OUTPUT);
	pinMode(PIN_C02, OUTPUT);
	pinMode(PIN_C03, INPUT);
	pinMode(PIN_C04, INPUT);

  uint offset = pio_add_program(pio, &throttle_program);

	throttle_program_init(pio, sm, offset, PIN_C01, PIN_C02, PIN_C03, PIN_C04);

		// On the teensy the digitalWrite seems to work only after pinMode.
#if X52_PRO_IMPROVED_JOYSTICK_CLIENT_DESYNC_DETECTION
	digitalWrite(PIN_C01, HIGH);
#endif
};


inline bool pio_sm_put_blocking_timeout(PIO pio, uint sm, uint32_t data) {
    check_pio_param(pio);
    check_sm_param(sm);
		uint32_t currentTime = millis();
    while (pio_sm_is_tx_fifo_full(pio, sm)) {
			if (millis() - currentTime > 100) 
				return 0;
		}
    pio_sm_put(pio, sm, data);
		return 1;
}
inline uint32_t pio_sm_get_blocking_timeout(PIO pio, uint sm) {
    check_pio_param(pio);
    check_sm_param(sm);
		uint32_t currentTime = millis();
		while (pio_sm_is_rx_fifo_empty(pio, sm)) {
			if (millis() - currentTime > 100)
				return 0;
		}
    return pio_sm_get(pio, sm);
}

void pio_sm_print_registers(PIO pio, uint sm) {
		Serial.printf("ctrl: 0x%x\n\r",pio->ctrl);
		Serial.printf("fstat: 0x%x\n\r",pio->fstat);
		Serial.printf("fdebug: 0x%x\n\r",pio->fdebug);
		Serial.printf("flevel: 0x%x\n\r",pio->flevel);
		Serial.printf("DBG_PADOUT: 0x%x\n\r",pio->dbg_padout);
		Serial.printf("DBG_PADOE: 0x%x\n\r",pio->dbg_padoe);
		Serial.print("\n\r");
		Serial.printf("sm: %d\n\r",sm);
		Serial.printf("clkdiv: 0x%x\n\r",pio->sm[sm].clkdiv);
		Serial.printf("           INT: %d\n\r",(pio->sm[sm].clkdiv>>16) & 0xffff);
		Serial.printf("          FRAC: %d\n\r",(pio->sm[sm].clkdiv>>8) & 0xff);
		Serial.print("\n\r");
		Serial.printf("execctrl: 0x%x\n\r",pio->sm[sm].execctrl);
		Serial.printf("  EXEC_STALLED: %d\n\r",(pio->sm[sm].execctrl>>31) & 0x1);
		Serial.printf("       SIDE_EN: %d\n\r",(pio->sm[sm].execctrl>>30) & 0x1);
		Serial.printf("   SIDE_PINDIR: %d\n\r",(pio->sm[sm].execctrl>>29) & 0x1);
		Serial.printf("       JMP_PIN: %d\n\r",(pio->sm[sm].execctrl>>24) & 0xf);
		Serial.printf("    OUT_EN_SEL: %d\n\r",(pio->sm[sm].execctrl>>19) & 0xf);
		Serial.printf(" INLINE_OUT_EN: %d\n\r",(pio->sm[sm].execctrl>>18) & 0x1);
		Serial.printf("    OUT_STICKY: %d\n\r",(pio->sm[sm].execctrl>>17) & 0x1);
		Serial.printf("      WRAP_TOP: %d\n\r",(pio->sm[sm].execctrl>>12) & 0xf);
		Serial.printf("   WRAP_BOTTOM: %d\n\r",(pio->sm[sm].execctrl>>7) & 0xf);
		Serial.printf("      RESERVED: %d\n\r",(pio->sm[sm].execctrl>>5) & 0x3);
		Serial.printf("    STATUS_SEL: %d\n\r",(pio->sm[sm].execctrl>>4) & 0x1);
		Serial.printf("      STATUS_N: %d\n\r",(pio->sm[sm].execctrl>>0) & 0xf);
		Serial.print("\n\r");
		Serial.printf("shiftctrl: 0x%x\n\r",pio->sm[sm].shiftctrl);
		Serial.printf("      FJOIN_RX: %d\n\r",(pio->sm[sm].shiftctrl>>31) & 0x1);
		Serial.printf("      FJOIN_TX: %d\n\r",(pio->sm[sm].shiftctrl>>30) & 0x1);
		
		Serial.print("   PULL_THRESH: ");Serial.println((pio->sm[sm].shiftctrl>>25) & 0xf,BIN);
		//Serial.printf("   PULL_THRESH: %d\n\r",(pio->sm[sm].shiftctrl>>25) & 0xf,BIN);
		Serial.printf("   PUSH_THRESH: ");Serial.println((pio->sm[sm].shiftctrl>>20) & 0xf,BIN);
		//Serial.printf("   PUSH_THRESH: %d\n\r",(pio->sm[sm].shiftctrl>>20) & 0xf,BIN);
		
		Serial.printf("  OUT_SHIFTDIR: %d\n\r",(pio->sm[sm].shiftctrl>>19) & 0x1);
		Serial.printf("   IN_SHIFTDIR: %d\n\r",(pio->sm[sm].shiftctrl>>18) & 0x1);
		Serial.printf("      AUTOPULL: %d\n\r",(pio->sm[sm].shiftctrl>>17) & 0x1);
		Serial.printf("      AUTOPUSH: %d\n\r",(pio->sm[sm].shiftctrl>>16) & 0x1);
		Serial.printf("      RESERVED: %d\n\r",(pio->sm[sm].shiftctrl>>0) & 0xffff);
		Serial.print("\n\r");
		Serial.printf("addr: 0x%x\n\r",pio->sm[sm].addr);
		Serial.printf("      RESERVED: %d\n\r",(pio->sm[sm].addr>>5) );
		Serial.printf("    INSTR_ADDR: %d\n\r",(pio->sm[sm].addr>>0) & 0xf);
		Serial.print("\n\r");		
		Serial.printf("instr: 0x%x\n\r",pio->sm[sm].instr);
		Serial.printf("      RESERVED: %d\n\r",(pio->sm[sm].instr>>16) );
		Serial.printf("         INSTR: %d\n\r",(pio->sm[sm].instr>>0) & 0xffff);
		Serial.print("\n\r");
		Serial.printf("pinctrl: 0x%x\n\r",pio->sm[sm].pinctrl);
		Serial.printf(" SIDESET_COUNT: %d\n\r",(pio->sm[sm].pinctrl>>29) & 0x3);
		Serial.printf("     SET_COUNT: %d\n\r",(pio->sm[sm].pinctrl>>26) & 0x3);
		Serial.printf("     OUT_COUNT: %d\n\r",(pio->sm[sm].pinctrl>>20) & 0x3f);
		Serial.printf("       IN_BASE: %d\n\r",(pio->sm[sm].pinctrl>>15) & 0x1f);
		Serial.printf("  SIDESET_BASE: %d\n\r",(pio->sm[sm].pinctrl>>10) & 0x1f);
		Serial.printf("      SET_BASE: %d\n\r",(pio->sm[sm].pinctrl>>5) & 0x1f);
		Serial.printf("      OUT_BASE: %d\n\r",(pio->sm[sm].pinctrl>>0) & 0x1f);
		Serial.print("\n\r");
}
	// PollJoystickState polls the joystick for its state. It creates a frame
	// transmission request, waits for the joystick to respond and handles the
	// bidirectional data transfer: sends the JoystickConfig and receives the
	// JoystickState. Returns zero on success.
	//
	// A nonzero return value means error and gives the recommended number
	// of microseconds to wait before calling PollJoystickState again.
	// In that situation the value of the JoystickState is undefined.
unsigned long FakeProThrottle::PollJoystickState(JoystickState& state, const JoystickConfig& cfg, unsigned long wait_micros) {
		// PIN_C02 has to be LOW when this function returns.
		//
		// If X52_PRO_IMPROVED_JOYSTICK_CLIENT_DESYNC_DETECTION==1
		// then PIN_C01 has to be HIGH when this function returns.

	JoystickState::Binary recv_buf;
	JoystickConfig::Binary send_buf;
	//cfg.ToBinary(send_buf);

	uint32_t sendbuf=0;
	SetUInt(sendbuf, 0, 5, cfg.led_brightness);
	SetBit(sendbuf, 5, cfg.pov_1_led_blinking);
	SetUInt(sendbuf, 6, 2, cfg.button_a_led);
	SetUInt(sendbuf, 8, 2, cfg.pov_2_led);
	SetBit(sendbuf, 10, !cfg.button_fire_led);
	SetUInt(sendbuf, 11, 2, cfg.button_b_led);
	SetUInt(sendbuf, 13, 2, cfg.button_t1_t2_led);
	SetUInt(sendbuf, 15, 2, cfg.button_t3_t4_led);
	SetUInt(sendbuf, 17, 2, cfg.button_t5_t6_led);

	if (!pio_sm_put_blocking_timeout(pio, sm, sendbuf)){
		Serial.println("timeout");
		pio_sm_print_registers(pio, sm);
	}
	uint64_t a = pio_sm_get_blocking_timeout(pio, sm);
	a = (a<<32) | pio_sm_get_blocking_timeout(pio, sm);
	if (a>0)
		Serial.printf("0x%x\n\r", a);



// 	// deadline for the whole frame transmission
// 	unsigned long deadline = micros() + wait_micros;

// 	// A frame consists of 76 clock pulses on both C02 and C04.

// 	for (int i=0; i<76; i++) {
// #if !X52_PRO_IMPROVED_JOYSTICK_CLIENT_DESYNC_DETECTION
// 		// This is what the original throttle does but we have a potentially better solution.
// 		if (i == 1)
// 			digitalWrite(PIN_C01, HIGH);
// 		else
// #endif
// 		if (i >= 57)
// 			digitalWrite(PIN_C01, send_buf.Bit(i-57));

// 		digitalWrite(PIN_C02, HIGH);

// 		// The original joystick samples C01 here between the
// 		// rising edge of C02 and the rising edge of C04.

// 		if (!wait_for_pin_state(PIN_C04, HIGH, deadline)) {
// 			X52DebugPrint("Error waiting for C04=1. Clock cycle: ");
// 			X52DebugPrintln(i);
// #if X52_PRO_IMPROVED_JOYSTICK_CLIENT_DESYNC_DETECTION
// 			if (i >= 57)
// 			digitalWrite(PIN_C01, HIGH);
// #endif
// 			digitalWrite(PIN_C02, LOW);
// 			// Timing out with i==0 means that the joystick didn't respond to our
// 			// initial C02=1 request within the available time frame defined by `wait_micros`.
// 			if (i == 0)
// 				return 1;
// 			// We are in the middle of a frame (already started talking with the joystick).
// 			// This means that the joystick will also time out and the throttle should
// 			// should try to initiate a new frame only after the joystick's timeout.
// 			return X52_PRO_THROTTLE_UNRESPONSIVE_MICROS;
// 		}

// 		if (i == 0)
// 			// The original throttle times out if the whole frame isn't
// 			// transmitted within X52_PRO_THROTTLE_TIMEOUT_MICROS
// 			// measured from the first falling edge of C02.
// 			// The previous deadline value was set up using `wait_micros`,
// 			// that timeout applies only to the first rising edge of C04.
// 			deadline = micros() + X52_PRO_THROTTLE_TIMEOUT_MICROS;
// 		else if (i == 56)
// 			digitalWrite(PIN_C01, LOW);  // desync detection: the joystick becomes unresponsive if this isn't LOW
// #if X52_PRO_IMPROVED_JOYSTICK_CLIENT_DESYNC_DETECTION
// 		// The original throttle doesn't do this but perhaps it should
// 		// because this makes desync detection more reliable.
// 		// The last iteration of this for loop ends with C01=HIGH
// 		// and C01 stays that way until the i==56 of the next frame.
// 		else if (i >= 57)
// 			digitalWrite(PIN_C01, HIGH);
// #endif

// 		digitalWrite(PIN_C02, LOW);

// 		if (!wait_for_pin_state(PIN_C04, LOW, deadline)) {
// 			X52DebugPrint("Error waiting for C04=0. Clock cycle: ");
// 			X52DebugPrintln(i);
// 			return X52_PRO_THROTTLE_UNRESPONSIVE_MICROS;
// 		}

// 		// The original throttle samples C03 here between the
// 		// falling edge of C04 and the rising edge of C02.
// 		if (i < JoystickState::NUM_BITS)
// 			recv_buf.SetBit(i, bool(digitalRead(PIN_C03)));
// 	}

	state.SetFromBinary2(a);
	//state.SetFromBinary(recv_buf);
	return 0;
}

//void FakeProThrottle::PrepareForPoll() {
//		digitalWrite(PIN_C02, HIGH);
//}


// FakeProJoystick makes it possible to use some of your Arduino pins as a
// connection to the PS/2 socket of an X52 Pro Throttle.
//
// The pin config is the same as that of the FakeThrottle.

FakeProJoystick::FakeProJoystick(PIO pio, int PIN_C01, int PIN_C02, int PIN_C03, int PIN_C04) {
	this->pio = pio;
	this->PIN_C01 = PIN_C01;
	this->PIN_C02 = PIN_C02;
	this->PIN_C03 = PIN_C03;
	this->PIN_C04 = PIN_C04;
	this->PIN_C01 = PIN_C01;
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
FakeProJoystick::~FakeProJoystick() {}

//template <int PIN_C01, int PIN_C02, int PIN_C03, int PIN_C04>
//class FakeProJoystick {
//public:
	// Call Setup from the setup function of your Arduino project to initialize
	// a FakeProJoystick instance.
void FakeProJoystick::setup() {
//  uint offset = pio_add_program(pio, &Fake_Throttle_program);

//	Fake_Throttle_program_init(pio, sm, offset, PIN_C01, PIN_C02, PIN_C03, PIN_C04);
}

// SendJoystickState sends the JoystickState to the throttle and receives
// the JoystickConfig. On the other side there must be a throttle polling/waiting
// for the JoystickState. Returns zero on success.
//
// A nonzero return value means error and gives the number of microseconds
// to wait before calling SendJoystickState again. In that situation the
// value of the JoystickConfig is undefined.
unsigned long FakeProJoystick::SendJoystickState(const JoystickState& state, JoystickConfig& cfg, unsigned long wait_micros) {
	// waiting for the throttle's poll
	if (!wait_for_pin_state(PIN_C02, HIGH, micros()+wait_micros))
		return 1;

	JoystickConfig::Binary recv_buf;
	JoystickState::Binary send_buf;
	state.ToBinary(send_buf);

	auto deadline = micros() + X52_PRO_JOYSTICK_TIMEOUT_MICROS;

	// A frame consists of 76 clock pulses on both C02 and C04.
	for (int i=0; i<76; i++) {
		if (i < JoystickState::NUM_BITS)
			digitalWrite(PIN_C03, send_buf.Bit(i));
		else if (i >= 57)
			// The original joystick samples C01 here between the
			// rising edge of C02 and the rising edge of C04.
			recv_buf.SetBit(i-57, bool(digitalRead(PIN_C01)));

		digitalWrite(PIN_C04, HIGH);

		if (!wait_for_pin_state(PIN_C02, LOW, deadline)) {
			X52DebugPrint("Error waiting for C02=0. Clock cycle: ");
			X52DebugPrintln(i);
			digitalWrite(PIN_C04, LOW);
			return X52_PRO_JOYSTICK_UNRESPONSIVE_MICROS;
		}

#if X52_PRO_IMPROVED_THROTTLE_CLIENT_DESYNC_DETECTION
		if (i >= 1 && i <= 55) {
			// This is something that the original joystick doesn't do.
			// This method leads to very quick and reliable desync detection.
			// It's based on the assumption that the X52 Pro always sends
			// ones over C01 while the joystick is sending its state over C03.
			if (!digitalRead(PIN_C01)) {
				X52DebugPrint("Desync detected: bits 1..55 aren't all ones. Timing out to force a resync. Clock cycle: ");
				X52DebugPrintln(i);
				digitalWrite(PIN_C04, LOW);
				return X52_PRO_JOYSTICK_DESYNC_UNRESPONSIVE_MICROS;
			}
		} else
#endif
		if (i == 56) {
			// This is something that the original joystick also does.
			if (digitalRead(PIN_C01)) {
				X52DebugPrintln("Desync detected: bit 56 isn't zero. Timing out to force a resync.");
				digitalWrite(PIN_C04, LOW);
				return X52_PRO_JOYSTICK_DESYNC_UNRESPONSIVE_MICROS;
			}
		}

		digitalWrite(PIN_C04, LOW);

		// The original throttle samples C03 here between the
		// falling edge of C04 and the rising edge of C02.

		if (!wait_for_pin_state(PIN_C02, HIGH, deadline)) {
			X52DebugPrint("Error waiting for C02=1. Clock cycle: ");
			X52DebugPrintln(i);
			return X52_PRO_JOYSTICK_UNRESPONSIVE_MICROS;
		}
	}

	cfg.SetFromBinary(recv_buf);
	return 0;
}

// IsPollInProgress returns true if the throttle is waiting for the
// JoystickState on the other side of the connection. In that situation
// a call to SendJoystickState is less likely to block in a waiting state
// (or fail as a result of wait timeout).
bool FakeProJoystick::IsPollInProgress() {
	return bool(digitalRead(PIN_C02));
}



//}  // namespace pro
//}  // namespace x52
