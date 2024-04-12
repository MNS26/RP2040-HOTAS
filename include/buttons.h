

bool getButtonState(ButtonReport* Buttons, uint8_t buttonNumber) {
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