# Gasmeter changelog

## v0.5.3 - 2023-03-24
* updated retention policies
* remove outer array in json output

## v0.5.2 - 2022-12-29
* fix mqtt status topic not retained
* automatically reconnect to broker

## v0.5.1 - 2022-10-29
* output current reading on startup

## v0.4.6 - 2022-04-02
* improved interrupt signal handling

## v0.4.5 - 2022-04-01
* removed DHT22 sensor code in firmware in daemon
* altered live database table

## v0.4.4 - 2022-03-30
* TimescaleDB: replaced \_time\_bucket() with timescaledb\_experimental.time\_bucket\_ng()
* created yearly\_view
* output state of gas flame in central heater

## v0.4.3 - 2022-01-03
* initial release
* add readme
