#include <string.h>
#include <stdint.h>
#include <avr/io.h>
#include <util/delay.h>
#include "dht22.h"

int GetTempHumidity(uint16_t *temperature, uint16_t *humidity)
{
  uint8_t receive_buffer[5] = {0};
  memset(receive_buffer, 0, sizeof(receive_buffer));

  // set DHT pin as output
  DHT_DDR |= _BV(DHT_PIN);
  // set DHT pin high
  DHT_PORT |= _BV(DHT_PIN);
  _delay_ms(10);  

  // send measure request
  DHT_PORT &= ~(_BV(DHT_PIN)); // low
  _delay_ms(10);
  DHT_PORT |= _BV(DHT_PIN); // high
  DHT_DDR &= ~(_BV(DHT_PIN)); // input
  _delay_us(40);

  //check first start condition
  if (DHT_PORT & _BV(DHT_PIN))
  {
    return -1;
  }
  _delay_us(80);

  //check second start condition
  if (!(DHT_PORT & _BV(DHT_PIN)))
  {
    return -1;
  }
  _delay_us(80);

  return 0;
}
