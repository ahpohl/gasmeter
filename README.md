# Overview

There are at least two versions of the diaphragm gas volume meter. The .01 m³ digit of the number counter has a reflective sticker on the number six. In older gas meters, this sticker is magnetic, whereas in newer models it is not. The magnetic sticker can be detected with a reed switch or a very sensitive magnetometer used in mobile phones. And indeed the first version of this project used the [SparkFun 3-axis magnetometer](https://www.sparkfun.com/products/retired/12670) to detect a full revolution of the .01 m³ digit wheel.

Last year my gas meter was replaced with the newer gas meter version which is not magnetic anymore. Hence I had to replace the magnetometer with a new device able to capture the revolution of the non-magnetic disc. The new device uses an infrared LED and a phototransistor mounted on the meter panel. The IR dongle looks like this when mounted on the gas meter:

![gasmeter](resources/gasmeter.jpg)

## Build instructions



## Installation

The Gasmeter daemon can be installed from source or through the Arch Linux package manager (no .deb package yet). 

From source:
```
git clone https://github.com/ahpohl/gasmeter.git
make
sudo make install
```
Arch Linux package:
```
yaourt -S gasmeter
```
## Configuration

The gas meter is configured through its own configuration file:
```
serial_device /dev/ttyUSB0
mqtt_broker localhost
mqtt_topic gasmeter
gas_rate 261.80
gas_price 0.0583
gas_factor 10.835
level_low 750
level_high 900
```

### Setting the threshold levels

When the reflective disc on the 0.01 m³ dial passes the phototransistor, the reflected light gradually increases until it reaches a maximum and decreases to a baseline level afterwards. The volume counter should only increase by one during this process. The counting algorithm simulates a Schmitt trigger and introduces a hysteresis. Only when the raw value is higher than the threshold `level_high` and lower than threshold `level_low` the counter increases. The counting algorithm expressed in pseudo code looks like this:
```
if raw_value is greater than level_high then
  hysteresis is true 
else if raw_value < level_low and hysteresis equals true then
  increase volume counter by one
  hysteresis is false
```
Hence the threshold level low needs to be set slightly above the baseline and level high below the maximum of the raw IR value. To find the right threshold levels of your device, you need to record the raw output of the phototransistor by setting `debug_level = raw`. Plotting the raw IR values gives a diagram like this (the gnuplot script is provided in the resources folder):
```
gasmeter --config gasmeter.conf | tee ir.log
gnuplot < sensor.gnuplot > ir.png
```
![IR_raw](resources/ir_raw.png)

The larger the difference between the baseline and the maximum, the better the tolerance against noise and variations in reflected light intensity. Here the baseline is around 600 and the maximum at 950-1000 and a low level of 750 and high level of 900 works well.

## Run in Docker


## JSON output


## Future development



# Read analog gas meter with an IR dongle

This project uses an IR led and a phototransistor to count the revolutions of the shiny non-magnetic sticker on the 0.01 m³ digit of the gas meter. The IR dongle is built around an Atmel AVR 328p and a FT232RL USB-to-TTL converter and fully described in the [wiki](https://github.com/ahpohl/gasmeter/wiki).

The gasmeter daemon outputs a JSON formatted string and sends it to a MQTT broker on the network. From there, the data is forwarded into a time series database for permanent data storage and for visualization. The complete software stack consists of the following components, which need to be installed and configured separately:
- Gasmeter daemon with IR dongle for data readout
- Mosquitto MQTT broker
- Node-RED (MQTT client, PostgreSQL and optional email alerts)
- PostgreSQL with TimescaleDB and pg_cron extensions
- Grafana for visualization

The software stack is light weight in terms of necessary resources and runs on any SBC such as an Odroid C2/C4 or Raspberry Pi 3/4. Step-by-step [instructions](https://github.com/ahpohl/smartmeter/wiki) can be found at the [Smartmeter](https://github.com/ahpohl/smartmeter) sister project wiki pages.