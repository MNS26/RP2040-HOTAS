enum Axis
{
	X = 0,
	Y,
	Z,
	Rx,
	Ry,
	Rz,
  Vx,
  Vy,
  Vz,
	Slider,
  Dial,
  Wheel,
  Hat,
	MouseX,
	MouseY,
};




bool getButtonState(JoystickButtonsReport* Buttons, uint8_t buttonNumber) {
  if (buttonNumber >= 80) {
    // Button number is out of range
    return false;
  }
  if (buttonNumber < 32) {
    return (Buttons->buttons1 & (1ull << buttonNumber)) != 0; // Get the buttons curret state [0 - 31]
  } else if(buttonNumber < 64 && 31 < buttonNumber){
    return (Buttons->buttons2 & (1ull << (buttonNumber - 32))) != 0; // Get the buttons curret state [32 - 63]
  }
  else
  {
    return (Buttons->buttons3 & (1ull << (buttonNumber - 64))) != 0; // Get the buttons curret state [64 - 79]
  }
}

void setAxisValue(JoystickAxisReport* joystick, Axis axis, uint16_t value)
{
  switch (axis)
  {
    case X:
    joystick->X = value;
      break;

    case Y:
    joystick->Y = value;
      break;

    case Z:
    joystick->Z = value;
      break;

    case Rx:
    joystick->Rx = value;
      break;

    case Ry:
    joystick->Ry = value;
      break;

    case Rz:
    joystick->Rz = value;
      break;

    case Slider:
    joystick->Slider = value;
      break;

    case Dial:
    joystick->Dial = value;
      break;

    case Wheel:
    joystick->Wheel = value;
      break;

    case Hat:
    joystick->hat = value;
      break;

    case MouseX:
    joystick->thumbstickX = value;
      break;

    case MouseY:
    joystick->thumbstickY = value;
      break;
  }
}
void setHatValue(JoystickAxisReport* joystick, Axis axis, uint16_t value)
{
}
