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
    uint8_t buffer[5] = {0};
    memset(buffer, 0, sizeof(buffer));

    // set DHT pin as output
    DHT_DDR |= _BV(DHT_PIN);
    // set DHT pin high
    DHT_PORT |= _BV(DHT_PIN);
    _delay_ms(10);  

    // send measure request
    DHT_PORT &= ~(_BV(DHT_PIN)); // low
    _delay_ms(10);
    DHT_DDR &= ~(_BV(DHT_PIN)); // input
    DHT_PORT |= _BV(DHT_PIN); // high, enable pullup
    _delay_us(40);

    // check first start condition (AM2303 pulls bus low)
    if (DHT_INPUT & _BV(DHT_PIN))
    {
      return;
    }
    _delay_us(80);

    // check second start condition (AM2303 pulls bus high)
    if (!(DHT_INPUT & _BV(DHT_PIN)))
    {
      return;
    }
    _delay_us(80);
    
    // read temperature (2 bytes), humidity (2 bytes) and checksum (1 byte)
    uint8_t payload[5] = {0};
    for (int j = 0; j < 5; j++) // for each byte (5 total)
    {
      uint8_t byte = 0;
      for (int i = 0; i < 8; i++) // for each bit in each byte (8 total)
      {
        while (!(DHT_INPUT & _BV(DHT_PIN))) // wait for a high input (50 µs)
        {
        }
        _delay_us(40); // 26-28 µs = 0, 29-70 µs = 1
        if (DHT_INPUT & _BV(DHT_PIN))
        {
          byte |= (1 << (7-i));
        }
        while (DHT_INPUT & _BV(DHT_PIN)) // wait for a low input
        {
        }
      }
      payload[j] = byte;
    }

    // compare checksum
    if ((uint8_t) (payload[0] + payload[1] + payload[2] + payload[3]) == payload[4])
    {
      gasmeter.humidity = (int32_t) (payload[0] << 8 | payload[1]) * 10;
      gasmeter.temperature = (int32_t) (payload[2] << 8 | payload[3]) * 10; 
    }
    else 
    {
      gasmeter.humidity = 0;
      gasmeter.temperature = 0;
    }
    previous_millis = current_millis;
    startup = 0;
  }
}
