# L2-LSM6DSM

Level 2 Integration of LSM6DSM sensor (iNEMO inertial module)

This project covers the development of Level 2 southbound functions that will allow the use of the iNEMO inertial module LSM6DSM. 

This sensor will be used trough a I2C interface of a specific Sinapse hardware (IoT Hub Agro). The L1 functions or framework to be used by the functions here developed is the I2C L1 functions developed here: https://github.com/Sinapse-Energia/M2M-I2C-Development


# Features required

The sensor to be integrated is a very powerful one and only a little part of their features should be accessible. The functions to be developed here is like a wrapper that ease the use of the Sensor in spite of loosing features or flexibility.

Basically should be developed three functions that will allow the configuration of the sensor, its event mode and its reading.

## Config Sensor

This function allows the configuration of the basic sensor parameters. As parameters, the function receives only the type, mode and scale. The function configure all the required data by the sensor to work but this data is not accessible as parameters, it is hardcoded. For example, FIFO should be the most suitable one and TILT shouldb be enable

The signature should be something like:

`result config_LSM6DSM(type, mode, scale)`

Where:

type: Is an enum to config the sensor as Accelerometer, gyroscope or both
mode: Is an enum to set the power usage: Power down, normal, low-power and high-performance
scale: Is an enum to set the precission of the sensor, there are 5 possibilities. For example, 0 = 2G for accelerometer and 125 dps for gyroscope

## Config event mode

After config the sensor is possible to set the event mode. We will work with only 5 events: Free fall, wakeup, sleep, motion detection and step detection

The signature should be something like

`result config_event_LSM6DSM(ff, wu, sl, md, sd)`

Where all the parameters are boolean in order to enable or disable an event

## Read

This function allows the reading of any register of the sensor. It is a generic function that will allow the use of the sensor in a easy way

The signature should be something like

`result read_LSM6DSM(register_start, register_quantity)`

Where:

register_start: Is the ID of the register to start the reading in HEX
register_quantity: Is the quantity of registers to read
result: The content of the registers in HEX


# Ressources

- Datasheet of the sensor : http://www.st.com/content/ccc/resource/technical/document/datasheet/76/27/cf/88/c5/03/42/6b/DM00218116.pdf/files/DM00218116.pdf/jcr:content/translations/en.DM00218116.pdf

- L1 I2C : https://github.com/Sinapse-Energia/M2M-I2C-Development
