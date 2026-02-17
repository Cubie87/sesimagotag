# eInk Library Demo

for VUSION 2.6 BWR GL320 eInk Store Pricetags, controlled by an ESP32


Requires the EDP Conn by [ATC1441](https://github.com/atc1441)


## Hookup Guide

| ESP32 Pin |  EPD |  Description  |
| --------- | -----|  -----------  |
|    13     |  SCK |  Serial Clock |
|    14     |  SDA |  Serial Data  |
|    15     |  CS  |  Chip Select  |
|    25     | BUSY |  Busy Signal  |
|    26     |  RST |  Reset        |
|    27     |  DC  |  Data/Command |



## Credits
Credit to [jcyfkimi](https://github.com/jcyfkimi/arduino_esp32_epd_lib) for the original libraries. Fixed some typos in includefiles and refactored directories for standalone operation.
