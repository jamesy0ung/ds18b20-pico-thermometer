#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/gpio.h"
#include "tusb.h"

#define PIN_DS18B20 4  // DS18B20 is connected to GPIO 4

// Function prototypes
static int ds18b20_init(uint pin);
static void ds18b20_write_bit(uint pin, uint bit);
static void ds18b20_write_byte(uint pin, uint8_t byte);
static uint ds18b20_read_bit(uint pin);
static uint8_t ds18b20_read_byte(uint pin);
static float ds18b20_read_temperature(uint pin);

int main() {
    stdio_init_all();

    // Initialize the DS18B20 sensor
    if (!ds18b20_init(PIN_DS18B20)) {
        printf("DS18B20 initialization failed!\n");
        return 1;
    }

    while (1) {
        float temperature = ds18b20_read_temperature(PIN_DS18B20);
        
        // Format temperature for USB serial output
        char buffer[10];
        int chars_written = snprintf(buffer, sizeof(buffer), "%.2f\r\n", temperature);

        if (chars_written >= sizeof(buffer)) {
            printf("Buffer overflow detected!\n");
        } else {
            // Write the temperature to USB serial
            tud_cdc_write(buffer, chars_written);
            tud_cdc_write_flush();
        }

        sleep_ms(1000);  // Wait for 1 second before next reading
    }

    return 0;
}

static int ds18b20_init(uint pin) {
    gpio_init(pin);
    gpio_set_dir(pin, GPIO_OUT);
    gpio_put(pin, 1);
    sleep_ms(1);
    
    gpio_put(pin, 0);
    sleep_us(480);
    
    gpio_set_dir(pin, GPIO_IN);
    sleep_us(70);
    
    int presence = gpio_get(pin);
    sleep_us(410);
    
    return presence == 0;
}

static void ds18b20_write_bit(uint pin, uint bit) {
    gpio_set_dir(pin, GPIO_OUT);
    gpio_put(pin, 0);
    sleep_us(1);
    
    if (bit) gpio_put(pin, 1);
    sleep_us(60);
    
    gpio_put(pin, 1);
    sleep_us(1);
}

static void ds18b20_write_byte(uint pin, uint8_t byte) {
    for (int i = 0; i < 8; i++) {
        ds18b20_write_bit(pin, byte & 1);
        byte >>= 1;
    }
}

static uint ds18b20_read_bit(uint pin) {
    uint bit;
    gpio_set_dir(pin, GPIO_OUT);
    gpio_put(pin, 0);
    sleep_us(1);
    
    gpio_set_dir(pin, GPIO_IN);
    sleep_us(14);
    
    bit = gpio_get(pin);
    sleep_us(45);
    
    return bit;
}

static uint8_t ds18b20_read_byte(uint pin) {
    uint8_t byte = 0;
    for (int i = 0; i < 8; i++) {
        byte |= (ds18b20_read_bit(pin) << i);
    }
    return byte;
}

static float ds18b20_read_temperature(uint pin) {
    ds18b20_init(pin);
    ds18b20_write_byte(pin, 0xCC);  // Skip ROM
    ds18b20_write_byte(pin, 0x44);  // Convert temperature
    
    sleep_ms(750);  // Wait for conversion
    
    ds18b20_init(pin);
    ds18b20_write_byte(pin, 0xCC);  // Skip ROM
    ds18b20_write_byte(pin, 0xBE);  // Read scratchpad
    
    uint8_t temp_l = ds18b20_read_byte(pin);
    uint8_t temp_h = ds18b20_read_byte(pin);
    
    int16_t temp = (temp_h << 8) | temp_l;
    return temp / 16.0f;
}