# Read analog gas meter with an IR dongle

This project uses an IR led and a phototransistor to count the revolutions of the shiny non-magnetic sticker on the 0.01 m³ digit of the gas meter. The IR dongle is built around an Atmel AVR 328p and a FT232RL USB-to-TTL converter and fully described in the [wiki](https://github.com/ahpohl/gasmeter/wiki).

The gasmeter daemon outputs a JSON formatted string and sends it to a MQTT broker on the network. From there, the data is forwarded into a time series database for permanent data storage and for visualization. The complete software stack consists of the following components, which need to be installed and configured separately:
- Gasmeter daemon with IR dongle for data readout
- Mosquitto MQTT broker
- Node-RED (MQTT client, PostgreSQL and optional email alerts)
- PostgreSQL with TimescaleDB and pg_cron extensions
- Grafana for visualization

The software stack is light weight in terms of necessary resources and runs on any SBC such as an Odroid C2/C4 or Raspberry Pi 3/4. Step-by-step [instructions](https://github.com/ahpohl/smartmeter/wiki) can be found at the [Smartmeter](https://github.com/ahpohl/smartmeter) sister project wiki pages.


## Changelog

All notable changes and releases are documented in the [CHANGELOG](CHANGELOG.md).

## License

This project is licensed under the MIT license - see the [LICENSE](LICENSE) file for details
