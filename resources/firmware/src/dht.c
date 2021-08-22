#include <string.h>
#include <stdint.h>
#include <avr/io.h>
#include <util/delay.h>
#include "dht.h"
#include "millis.h"
#include "gasmeter.h"

void GetTempHumidity(void)
{
  unsigned long current_millis = millis();
  static unsigned long previous_millis = 0;
  // measure temperature and humidity every 5 minutes
  if (((current_millis - previous_millis) < 60000))
  {
    return;
  }

  static int16_t offset = 0; 
  gasmeter.temperature = -2000 + offset;
  gasmeter.humidity = 5000 + offset;
  offset += 100;

  /*
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
    return;
  }
  _delay_us(80);

  //check second start condition
  if (!(DHT_PORT & _BV(DHT_PIN)))
  {
    return;
  }
  _delay_us(80);
  */

  previous_millis = current_millis;
}
