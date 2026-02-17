# eInk Library Demo

for VUSION 2.6 BWR GL320 eInk Store Pricetags, controlled by an ESP32



## Hookup Guide

| ESP32 Pin |  EPD |  Description  |
| --------- | -----|  -----------  |
|    13     |  SCK |  Serial Clock |
|    14     |  SDA |  Serial Data  |
|    15     |  CS  |  Chip Select  |
|    25     | BUSY |  Busy Signal  |
|    26     |  RST |  Reset        |
|    27     |  DC  |  Data/Command |


![which way the ribbon connector should face](/pics/connector-orientation.jpeg)


## Disassembly

Pry the battery cover off using a small flathead screwdriver. There are no screws.

![battery cover off and battery removed](/pics/1-battery-cover-off.jpeg)

Use a thin plastic shim (ideally a guitar pick) to pry the front bezel off. Be careful to not snap the PCB or eInk display.

![front bezel removed and eInk screen out](/pics/2-front-bezel-off.jpeg)

The PCB can then be extracted.

![back of the PCB](/pics/3-pcb-out.jpeg)

Guessing it's a two layer PCB as it has the layers printed in text.

![back of the PCB](/pics/4-pcb-layer-1.jpeg)

![front of the PCB without screen](/pics/5-pcb-layer-2.jpeg)


![the edp connector by ATC1441 used between the ESP32 and eInk screen](/pics/edp-conn1.jpeg)

![other side of edp connector](/pics/edp-conn2.jpeg)

## Credits
Credit to [jcyfkimi](https://github.com/jcyfkimi/arduino_esp32_epd_lib) for the original libraries. Fixed some typos in includefiles and refactored directories for standalone operation.

Credit to [ATC1441](https://github.com/atc1441) for presumably the EDP connector. I cannot find the repository link but assume it is on the internet somewhere.