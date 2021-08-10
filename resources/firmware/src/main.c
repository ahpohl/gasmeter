#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <inttypes.h>
#include <avr/io.h>
#include <avr/interrupt.h>
#include "uart.h"
#include "main.h"
#include "util.h"

volatile uint8_t aread = 0;
volatile uint16_t avalue = 0;
volatile uint8_t txready = 0;

// Interrupt service routine for the ADC completion
ISR(ADC_vect)
{
  // must read low byte first
  avalue = (uint16_t) ADCL | ((uint16_t) ADCH << 8);

  // reset timer interrupt flag, set OCF0A bit
  TIFR0 |= _BV(OCF0A);

  // done reading
  aread = 1;
}

// Interrupt service routine for timer/counter1 output compare A match
ISR(TIMER1_COMPA_vect)
{
  // reset timer interrupt flag, set OCF1A bit
  TIFR1 |= _BV(OCF1A);

  // ready to transmit packet
  txready = 1;
}

void read_sensor(void)
{
  // check if new ADC value ready
  if (!aread) {
    return;
  }

  // reset ADC ready flag
  aread = 0;
}

void transmit_packet(void)
{
  if (!txready) {
    return;
  }

  send_value(avalue);

  // reset txready flag
  txready = 0;
}

int main(void)
{   
  //
  // timer 0
  //

  // set OC0A pin as output, required for output toggling
  OC0A_DDR |= _BV(OC0A_PIN);

  // enable toggle OC0A output on compare match, enable CTC mode
  TCCR0A = _BV(COM0A0) | _BV(WGM01);

  // use CLK/1024 prescale value
  TCCR0B = _BV(CS02) | _BV(CS00);

  // preset timer0 high/low byte
  OCR0A = (F_CPU/2/1024/TIMER0_CLOCK) - 1;

  //
  // timer 1
  //

  // set OC1A pin as output, required for output toggling
  OC1A_DDR |= _BV(OC1A_PIN);

  // enable toggle OC1A output on compare match, enable CTC mode
  TCCR1A = _BV(COM1A0);

  // use CLK/64 prescale value
  TCCR1B = _BV(CS11) | _BV(CS10) | _BV(WGM12);

  // preset timer1 high/low byte
  OCR1A = (F_CPU/2/64/TIMER1_CLOCK) - 1;

  // enable timer/counter1 output compare A match interrupt
  TIMSK1 = _BV(OCIE1A);
  
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

  // now enable global interrupt
  sei();

  // test UART
  uart_puts("String stored in SRAM\n\r");

  //
  // main loop
  //

  for (;;)
  {
    // read IR sensor
    read_sensor();

    // transmit packet
    transmit_packet();
  }
}
