struct JoystickAxisReport
{
  uint32_t  X:10,
            Y:10,
            Z:10;     // The X,Y, Z axis position

  uint32_t  Rx:10,
            Ry:10,
            Rz:10;  // The Z, Rx, Ry position

  uint32_t  Slider:10,
            Dial:10,
            Wheel:10;    // The slider, Dial, Wheel position

  uint16_t  hat:8,       // The hat position
            thumbstickX:4,
            thumbstickY:4;
}__attribute__((packed));

typedef union JoystickButtonsReport
{

  uint8_t  buttons0:1,
            buttons1:1,
            buttons2:1,
            buttons3:1,
            buttons4:1,
            buttons5:1,
            buttons6:1,
            buttons7:1,
            buttons8:1,
            buttons9:1,
            buttons10:1,
            buttons11:1,
            buttons12:1,
            buttons13:1,
            buttons14:1,
            buttons15:1,
            buttons16:1,
            buttons17:1,
            buttons18:1,
            buttons19:1,
            buttons20:1,
            buttons21:1,
            buttons22:1,
            buttons23:1,
            buttons24:1,
            buttons25:1,
            buttons26:1,
            buttons27:1,
            buttons28:1,
            buttons29:1,
            buttons30:1,
            buttons31:1;

  uint16_t  buttons32:1,
            buttons33:1,
            buttons34:1,
            buttons35:1,
            buttons36:1,
            buttons37:1,
            buttons38:1,
            buttons39:1,
            buttons40:1,
            buttons41:1,
            buttons42:1,
            buttons43:1,
            buttons44:1,
            buttons45:1,
            buttons46:1,
            buttons47:1;

  uint16_t  buttons48:1,
            buttons49:1,
            buttons50:1,
            buttons51:1,
            buttons52:1,
            buttons53:1,
            buttons54:1,
            buttons55:1,
            buttons56:1,
            buttons57:1,
            buttons58:1,
            buttons59:1,
            buttons60:1,
            buttons61:1,
            buttons62:1,
            buttons63:1;

  uint16_t  buttons64:1,
            buttons65:1,
            buttons66:1,
            buttons67:1,
            buttons68:1,
            buttons69:1,
            buttons70:1,
            buttons71:1,
            buttons72:1,
            buttons73:1,
            buttons74:1,
            buttons75:1,
            buttons76:1,
            buttons77:1,
            buttons78:1,
            buttons79:1;
};
