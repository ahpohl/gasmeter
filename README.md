# Gasmeter

There are at least two versions of the gas volume meter. The .01 m³ digit of the number counter has a shiny plate on the number six. In older gas meters, this plate is magnetic, whereas in newer models it is not. The magnetic plate can be detected with a reed switch or a very sensitive magnetometer used in mobile phones. And indeed the first version of this project used the [SparkFun 3-axis magnetometer](https://github.com/ahpohl/mag3110) to detect a full revolution of the digit wheel. Last year my gas meter was replaced with the newer gas meter version which is not magnetic anymore.

This project uses the electromagnetic radiation of an infrared diode and a phototransistor to count the revolutions of the shiny plate. The hardware is based on an Atmel atmega328p used in Arduinos, but the firmware written in plain AVR C++ independent of the Arduino core. The hardware is fully described in the [wiki](https://github.com/ahpohl/gasmeter/wiki) and similar to the one used in the [Pulsemeter](https://github.com/ahpohl/pulsemeter) project.

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
