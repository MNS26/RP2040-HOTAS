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

void setAxisValue(JoystickReport* joystick, Axis axis, uint16_t value)
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
void setHatValue(JoystickReport* joystick, Axis axis, uint16_t value)
{
}
