#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <inttypes.h>
#include <avr/io.h>
#include <avr/interrupt.h>
#include "main.h"
#include "uart.h"
#include "gasmeter.h"
#include "util.h"
#include "millis.h"
#include "dht.h"

volatile uint8_t adc_ready = 0;
volatile uint16_t adc_value = 0;

// Interrupt service routine for the ADC completion
ISR(ADC_vect)
{
  // must read low byte first
  adc_value = (uint16_t) ADCL | ((uint16_t) ADCH << 8);

  // reset timer interrupt flag, set OCF0A bit
  TIFR0 |= _BV(OCF0A);

  // done reading
  adc_ready = 1;
}

void ReadAdc(void)
{
  // check if new ADC value ready
  if (!adc_ready) {
    return;
  }

  // reset ADC ready flag
  adc_ready = 0;
}

void SendRawAdc(void)
{
  unsigned long current_millis = millis();
  static unsigned long previous_millis = 0;

  if ((current_millis - previous_millis) > 250)
  { 
    SendValue(adc_value);
    previous_millis = current_millis;
  }
}

int main(void)
{   
  //
  // timer 0
  //

  // set OC0A pin as output, required for output toggling
  IRLED_DDR |= _BV(IRLED_PIN);

  // CLK/256 prescale value  
  TCCR0B = _BV(CS02);

  // phase correct PWM
  // f = F_CPU/(510*prescaler)
  // f = 12 MHz/(510*256) = 92 Hz
  //TCCR0A = _BV(COM0A1) | _BV(WGM00);

  // fast PWM mode, non-inverting mode
  // f = F_CPU/(256*prescaler)
  // f = 12 MHz/(256*256) = 184 Hz
  TCCR0A = _BV(COM0A1) | _BV(WGM01) | _BV(WGM00);

  // duty cycle = 20 / 255 = 8 %
  OCR0A = 20;

  //
  // ADC
  //

  // right adjust result, clear ADLAR bit
  ADMUX &= ~_BV(ADLAR);

  // select AVCC with external capacitor at AREF, set REFS0 bit
  ADMUX |= _BV(REFS0);

  // select ADC channel 0, clear MUX3..0 bits
  ADMUX &= 0xF0;

  // select CLK/128 prescale value, ADPS0..2
  ADCSRA |= _BV(ADPS2) | _BV(ADPS1) | _BV(ADPS0);

  // enable auto-triggering, set ADATE
  ADCSRA |= _BV(ADATE);

  // interrupt on timer/counter0 compare match A
  ADCSRB |= _BV(ADTS1) | _BV(ADTS0);

  // enable ADC, set ADEN bit
  ADCSRA |= _BV(ADEN);

  // enable ADC interrupt, set ADIE bit
  ADCSRA |= _BV(ADIE);

  //
  // UART serial
  //

  // init uart library
  uart_init(UART_BAUD_SELECT(UART_BAUD_RATE, F_CPU));

  // init millis timer
  millis_init();

  // now enable global interrupt
  sei();

  // say hello
  //uart_puts("Gasmeter IR Sensor\n\r");

  //
  // main loop
  //

  for (;;)
  {
    // receive packet from uart
    //ReceivePacket();

    // process packet
    //ProcessPacket();

    // read IR sensor
    ReadAdc();

    // send raw ADC value
    SendRawAdc();

    // read DHT22 sensor
    //GetTempHumidity();
  }
}
