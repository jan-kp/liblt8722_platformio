# LT8722 Library for PlatformIO

[![DOI](https://zenodo.org/badge/DOI/10.5281/zenodo.15052631.svg)](https://doi.org/10.5281/zenodo.15052631)

## Overview
This library is used to control the LT8722 on the DC3145A evaluation board from Analog Devices. By default, FSPI is used with pins 10-13 for the communication via SPI. This can be changed if required. With the help of this library it is possible to start the LT8722 with a softstart to avoid high inrush currents. In addition, the most important functions are defining the voltage and current limits and setting the output voltage for the peltier element. All available configuration options for the command register can be set.

## Usage
For correct use, it is important to use the softstart function before making any further changes to the control registers. This is the case because all registers are reset during the softstart to avoid undesired behavior of the LT8722. 

## Standard PINs
MISO:  13
MOSI:  11
SCK:   12
SC:    10

VDDIO: 3.3V
GND:   GND

SPI:   FSPI