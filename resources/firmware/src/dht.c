#include <string.h>
#include <stdint.h>
#include <avr/io.h>
#include <util/delay.h>
#include "dht.h"
#include "millis.h"
#include "gasmeter.h"
#include "util.h"

void GetTempHumidity(void)
{
  unsigned long current_millis = millis();
  static unsigned long previous_millis = 0;
  static uint8_t startup = 1;
  
  // measure temperature and humidity
  if (((current_millis - previous_millis) > 5000) || startup)
  {
    static float offset = 0; 
    gasmeter.temperature = -20 + offset;
    gasmeter.humidity = 50 + offset;

    //uint8_t b[sizeof(float)] = {0};
    //memcpy(&b, &gasmeter.temperature, sizeof(b));
    //SendBuffer(b, 4);

    offset += 0.1;
    previous_millis = current_millis;
    startup = 0;
  }

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
}
