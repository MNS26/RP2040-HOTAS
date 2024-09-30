//===================================================//
// i cant be bothered to properly rewrite the stuff  //
// so i just partially copied function and partially //
// remade them                                       //
//===================================================//

bool LOW = 0;
bool HIGH = 1;

#define bitRead(value, bit) (((value) >> (bit)) & 0x01)
#define bitSet(value, bit) ((value) |= (1UL << (bit)))
#define bitClear(value, bit) ((value) &= ~(1UL << (bit)))
#define bitToggle(value, bit) ((value) ^= (1UL << (bit)))
#define bitWrite(value, bit, bitvalue) (bitvalue ? bitSet(value, bit) : bitClear(value, bit))

bool digitalRead(uint pin) {
    return gpio_get(pin);
}
void digitalWrite(uint pin, bool state) {
    gpio_put(pin, state);
}

void delay(unsigned long ms) {
    if (!ms) {
        return;
    }
    sleep_ms(ms);
}

void delayMicroseconds(unsigned int usec) {
    if (!usec) {
        return;
    }
    sleep_us(usec);
}

uint32_t millis() {
    return to_ms_since_boot(get_absolute_time());
}
uint32_t micros() {
    return to_us_since_boot(get_absolute_time());
}