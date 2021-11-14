#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <inttypes.h>
#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/eeprom.h>
#include "main.h"
#include "gasmeter.h"
#include "dht.h"
#include "util.h"
#include "uart.h"
#include "millis.h"

// Interrupt service routine for the ADC completion
ISR(ADC_vect)
{
  // must read low byte first
  gasmeter.adc_value = (int16_t) ADCL | ((int16_t) ADCH << 8);
}

void ReadAdc(void)
{
  if (TCNT0 == (OCR0A + 8))
  {
    // trigger single ADC measurement
    ADCSRA |= _BV(ADSC);
  }
}

void ReadGasMeter(void)
{
  unsigned long current_millis = millis();
  static unsigned long previous_millis = 0;

  // measure temperature and humidity
  if (((current_millis - previous_millis) > 50))
  {
    // evaluate counter
    static uint8_t hysteresis = 0;
    if ((gasmeter.adc_value > gasmeter.level_high))
    {
      hysteresis = 1;
      CLED_PORT |= _BV(CLED_PIN);
    }
    else if ((gasmeter.adc_value < gasmeter.level_low) && hysteresis)
    {
      ADCSRA &= ~(_BV(ADEN));
      gasmeter.volume++;
      ADCSRA |= _BV(ADEN);
      eeprom_write_dword(&AddrVolume, gasmeter.volume);
      hysteresis = 0;
      CLED_PORT &= ~(_BV(CLED_PIN));
    }
    previous_millis = current_millis;
    //SendRaw(gasmeter.adc_value, gasmeter.volume, hysteresis);  
  }
}

int main(void)
{   
  //
  // setup counter LED, optional
  //
  
  // set LED pin as output
  CLED_DDR |= _BV(CLED_PIN);
 
  //
  // timer 0 for IR led PWM
  //

  // set OC0A pin as output, required for output toggling
  IRLED_DDR |= _BV(IRLED_PIN);

  // CLK/256 prescale value  
  TCCR0B = _BV(CS02);

  // fast PWM mode, inverting mode
  // set OC0A on compare match, clear OC0A at BOTTOM
  // f = F_CPU/(256*prescaler)
  // f = 12 MHz/(256*256) = 184 Hz
  TCCR0A = _BV(WGM01) | _BV(WGM00) | _BV(COM0A1) | _BV(COM0A0);

  // duty cycle = 230 / 255 = 90 % off, 10 % on
  OCR0A = 230;

  //
  // ADC for reading IR phototransistor
  //

  // right adjust result, clear ADLAR bit
  ADMUX &= ~_BV(ADLAR);

  // select AVCC with external capacitor at AREF, set REFS0 bit
  ADMUX |= _BV(REFS0);

  // select ADC channel 0, clear MUX3..0 bits
  ADMUX &= 0xF0;

  // select CLK/128 prescale value, ADPS0..2
  // ADC freq = 93.5 kHz
  ADCSRA |= _BV(ADPS2) | _BV(ADPS1) | _BV(ADPS0);

  // enable ADC interrupt, set ADIE bit
  ADCSRA |= _BV(ADIE);

  // enable ADC, set ADEN bit
  ADCSRA |= _BV(ADEN);

  //
  // UART serial
  //

  // init uart library
  uart_init(UART_BAUD_SELECT(UART_BAUD_RATE, F_CPU));

  // init millis timer
  millis_init();

  // read defaults from eeprom
  gasmeter.volume = eeprom_read_dword(&AddrVolume);
  gasmeter.level_low = eeprom_read_word(&AddrLevelLow);
  gasmeter.level_high = eeprom_read_word(&AddrLevelHigh);

  // now enable global interrupt
  sei();

  //
  // main loop
  //

  for (;;)
  {
    // read IR sensor
    ReadAdc();

    // evaluate gasmeter volume
    ReadGasMeter();

    // receive packet from uart
    ReceivePacket();

    // process packet
    ProcessPacket();

    // read DHT22 sensor
    //GetTempHumidity();
  }
}
