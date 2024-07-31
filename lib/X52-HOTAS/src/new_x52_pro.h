#ifndef X52_PRO
#define X52_PRO

// This value is hardcoded into the firmware of my original X52 Pro throttle.
#ifndef X52_PRO_THROTTLE_TIMEOUT_MICROS
	#define X52_PRO_THROTTLE_TIMEOUT_MICROS 17000
#endif

// This value is hardcoded into the firmware of my original X52 Pro joystick.
#ifndef X52_PRO_JOYSTICK_TIMEOUT_MICROS
	#define X52_PRO_JOYSTICK_TIMEOUT_MICROS 23000
#endif

#ifndef X52_PRO_THROTTLE_UNRESPONSIVE_MICROS
	#define X52_PRO_THROTTLE_UNRESPONSIVE_MICROS (X52_PRO_JOYSTICK_TIMEOUT_MICROS + 5000)
#endif

// This value is hardcoded into the firmware of my original X52 Pro joystick.
#ifndef X52_PRO_JOYSTICK_UNRESPONSIVE_MICROS
	#define X52_PRO_JOYSTICK_UNRESPONSIVE_MICROS 2000
#endif

// This value is hardcoded into the firmware of my original X52 Pro joystick.
// It has to be greater than X52_PRO_THROTTLE_TIMEOUT_MICROS.
#ifndef X52_PRO_JOYSTICK_DESYNC_UNRESPONSIVE_MICROS
	#define X52_PRO_JOYSTICK_DESYNC_UNRESPONSIVE_MICROS 23000
#endif

// Enabling this feature allows the FakeProJoystick to behave differently from
// the original joystick in order to make the desync detection more reliable.
#ifndef X52_PRO_IMPROVED_THROTTLE_CLIENT_DESYNC_DETECTION
	#define X52_PRO_IMPROVED_THROTTLE_CLIENT_DESYNC_DETECTION 0
#endif

// Enabling this feature allows the FakeProThrottle to behave differently from
// the original throttle in order to make the desync detection more reliable.
#ifndef X52_PRO_IMPROVED_JOYSTICK_CLIENT_DESYNC_DETECTION
	#define X52_PRO_IMPROVED_JOYSTICK_CLIENT_DESYNC_DETECTION 0
#endif

#ifndef X52_PRO_DEFAULT_POLL_JOYSTICK_STATE_WAIT_MICROS
	#define X52_PRO_DEFAULT_POLL_JOYSTICK_STATE_WAIT_MICROS 25000
#endif

#ifndef X52_PRO_DEFAULT_SEND_JOYSTICK_STATE_WAIT_MICROS
	#define X52_PRO_DEFAULT_SEND_JOYSTICK_STATE_WAIT_MICROS 25000
#endif

#include "new_x52_common.h"
#include "throttle-protocol.pio.h"



enum LEDColor {
	Amber = 0,  // 00
	Green = 1,  // 10
	Red = 2,    // 01
	Off = 3,    // 11
};


// JoystickState is the data sent by the joystick through the PS/2 cable.
struct JoystickState {
	static constexpr uint16_t MAX_X = 1023;
	static constexpr uint16_t MAX_Y = 1023;
	static constexpr uint16_t MAX_Z = 1023;

	static constexpr uint16_t CENTER_X = 512;
	static constexpr uint16_t CENTER_Y = 512;
	static constexpr uint16_t CENTER_Z = 512;

	uint16_t x;  // valid_range: 0..1023(MAX_X)  center: 512(CENTER_X)
	uint16_t y;  // valid_range: 0..1023(MAX_Y)  center: 512(CENTER_Y)
	uint16_t z;  // valid_range: 0..1023(MAX_Z)  center: 512(CENTER_Z)

	Direction pov_1;
	Direction pov_2;
	Mode mode;

	bool trigger_stage_1: 1;
	bool trigger_stage_2: 1;
	bool pinkie_switch: 1;
	bool button_fire: 1;
	bool button_a: 1;
	bool button_b: 1;
	bool button_c: 1;
	bool button_t1: 1;
	bool button_t2: 1;
	bool button_t3: 1;
	bool button_t4: 1;
	bool button_t5: 1;
	bool button_t6: 1;

	JoystickState() {
		memset(this, 0, sizeof(*this));
	}

	// Constructor for the pros who believe they know what they are doing.
	JoystickState(Uninitialized) {}

	static constexpr int NUM_BITS = 56;
	typedef BitField<NUM_BITS> Binary;

	void SetFromBinary(const Binary&);
	void SetFromBinary2(const uint64_t);
	void ToBinary(Binary&) const;
};


// JoystickConfig is the data sent by the throttle through the PS/2 cable.
struct JoystickConfig {
	static constexpr uint8_t MAX_LED_BRIGHTNESS = 31;

	uint8_t led_brightness;   // valid range: 0..31 (MAX_LED_BRIGHTNESS)
	bool pov_1_led_blinking;  // blinks about 4 times per second
	bool button_fire_led;

	LEDColor pov_2_led: 2;
	LEDColor button_a_led: 2;
	LEDColor button_b_led: 2;
	LEDColor button_t1_t2_led: 2;
	LEDColor button_t3_t4_led: 2;
	LEDColor button_t5_t6_led: 2;

	JoystickConfig() {
		led_brightness = MAX_LED_BRIGHTNESS;
		pov_1_led_blinking = false;
		button_fire_led = true;
		pov_2_led = Green;
		button_a_led = Green;
		button_b_led = Green;
		button_t1_t2_led = Green;
		button_t3_t4_led = Green;
		button_t5_t6_led = Green;
	}

	// Constructor for the pros who believe they know what they are doing.
	JoystickConfig(Uninitialized) {}

	static constexpr int NUM_BITS = 19;
	typedef BitField<NUM_BITS> Binary;

	void SetFromBinary(const Binary&);
	void ToBinary(Binary&) const;
};

class FakeProThrottle {
private:
  int sm;
  PIO pio;
  int PIN_C01;
  int PIN_C02;
  int PIN_C03;
  int PIN_C04;
  bool init = false;

  /* data */
public:
  FakeProThrottle(PIO pio, int PIN_C01, int PIN_C02, int PIN_C03, int PIN_C04 /* args */);
  ~FakeProThrottle();
  void setup();
  unsigned long PollJoystickState(JoystickState& state, const JoystickConfig& cfg, unsigned long wait_micros=X52_PRO_DEFAULT_POLL_JOYSTICK_STATE_WAIT_MICROS);
  void PrepareForPoll();

};

class FakeProJoystick {
private:
  int sm;
  PIO pio;
  int PIN_C01;
  int PIN_C02;
  int PIN_C03;
  int PIN_C04;
  bool init = false;
public:
  FakeProJoystick(PIO pio, int PIN_C01, int PIN_C02, int PIN_C03, int PIN_C04 /* args */);
  ~FakeProJoystick();
  void setup();
  unsigned long SendJoystickState(const JoystickState& state, JoystickConfig& cfg, unsigned long wait_micros=X52_PRO_DEFAULT_SEND_JOYSTICK_STATE_WAIT_MICROS);
  bool IsPollInProgress();
};

#endif
