# Gasmeter changelog

## v0.5.0 - 2022-xx-xx
* increased interval to 60 seconds
* fixed interrupt signal in main loop
* removed DHT22 temperature and humidity sensor
* removed flow rate in favour of flow state

## v0.4.4 - 2022-03-30
* TimescaleDB: replaced \_time\_bucket() with timescaledb\_experimental.time\_bucket\_ng()
* created yearly\_view
* output state of gas flame in central heater

## v0.4.3 - 2022-01-03
* initial release
* add readme
