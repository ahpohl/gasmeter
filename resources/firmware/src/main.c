#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <inttypes.h>
#include <avr/io.h>
#include <avr/interrupt.h>
#include "main.h"
#include "gasmeter.h"
#include "dht.h"
#include "util.h"
#include "uart.h"
#include "millis.h"

volatile uint8_t adc_ready = 0;
volatile uint16_t adc_value = 0;
volatile uint8_t timer_ready = 0;

// Interrupt service routine for the ADC completion
ISR(ADC_vect)
{
  // must read low byte first
  adc_value = (uint16_t) ADCL | ((uint16_t) ADCH << 8);

  // done reading
  adc_ready = 1;
}

ISR(TIMER0_COMPA_vect)
{
  timer_ready = 1;
}

void ReadAdc(void)
{
  if (TCNT0 == OCR0A + 8)
  {
    // trigger single ADC measurement
    ADCSRA |= _BV(ADSC);
    timer_ready = 0;
  }
}

void ReadGasMeter(void)
{
  // check if new ADC value ready
  if (!adc_ready) {
    return;
  }
  
  unsigned long current_millis = millis();
  static unsigned long previous_millis = 0;

  // evaluate adc value
  if ((current_millis - previous_millis) > 100)
  {
    static uint8_t hysteresis = 0;
    if ((adc_value > gasmeter.level_high) && !hysteresis)
    {
      hysteresis = 1;
    }
    else if ((adc_value < gasmeter.level_low) && hysteresis)
    {
      gasmeter.volume++;
      hysteresis = 0;
    }
    previous_millis = current_millis;
  }

  // reset ADC ready flag
  adc_ready = 0;
}

int main(void)
{   
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

  // enable output compare match A interupt
  TIMSK0 = _BV(OCIE0A);

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
  // ADC freq = 93.7 kHz
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

  // now enable global interrupt
  sei();

  //
  // main loop
  //

  for (;;)
  {
    // read IR sensor
    ReadAdc();

    // read gas meter
    ReadGasMeter();
 
    // receive packet from uart
    ReceivePacket();

    // process packet
    ProcessPacket();

    // read DHT22 sensor
    //GetTempHumidity();
  }
}
