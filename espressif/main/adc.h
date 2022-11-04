#ifndef adc_h
#define adc_h

#include "driver/adc.h"
#include "esp_adc_cal.h"

#define DEFAULT_VREF    1100        //Use adc2_vref_to_gpio() to obtain a better estimate
#define NO_OF_SAMPLES   64          //Multisampling

static esp_adc_cal_characteristics_t *adc_chars;
static const adc_channel_t channel;
static const adc_bits_width_t width;
static const adc_atten_t atten;
static const adc_unit_t unit;

static void check_efuse(void);
static void print_char_val_type(esp_adc_cal_value_t val_type);

#endif // adc_h