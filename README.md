#Project Lasertag

Repository for the [Wearable Computing Lab Course 2014](http://praktikum.ese.uni-freiburg.de/)  
University of Freiburg  

##0. TODO

- I2C Zuverlaessigkeit testen
- Stromversorgung mit Akkus
- RFID Reader an Edison betreiben
- RFID-Tags einrichten
- Server Programm fertig stellen
- Tagger-Gehaeuse
- Platine Tagger
- Weste-Weste
- Versuch MSPs auf Tagger zu vereinen
- I2C NACK wenn keine Daten vorhanden
- Display-Geschwindigkeit
- LED-Stripes am Tagger
- Berechnen ob Strahlung gefährlich ist
- Hauptplatine für Tagger designen
- Platinen ätzen und bestücken

0.1 ToBuy für 2 Westen-Tagger-Sets:

- 12x MSP
- 1x HC-06 Bluetooth
- 2-3x RFID-Reader
- 4x Plastik-Kugel
- 2x Weste
- 4x Akku
- 5m Kabel für Domes
- Klettband
- 1x Edison inkl. kleines Breakout-Board
- 1-2x Abzugsschalter
- kleine Schrumpfschläuche

- Transistoren ?
- Jumperkabel ?


##1. Edison
###1.1 Image Installation and Preparation
Installation instructions for getting the Intel Edison to work with the **Mini** Breakout board and a pre-built Yocto image.  
Get the latest image and Edison drivers [here](https://communities.intel.com/docs/DOC-23242).  
You can also follow the steps [here](https://communities.intel.com/message/261922), these instructions are mostly based on that.  
~# indicates a command line instruction

1. Install the Edison on the breakout board.
2. Install the [FTDI drivers](http://www.ftdichip.com/Drivers/CDM/CDM%20v2.10.00%20WHQL%20Certified.exe) and Intel Edison drivers.
3. Plug in Edison via USB cables, both cables are needed. When plugging in for the first time, Windows will install the device drivers, which may take some time. When finished, the edison should appear in the Explorer and have a drive letter assigned to it.
4. Format the Edison drive as FAT32, if it is not already.
5. Download the latest image and extract it to the drive.
6. Use PuTTY to connect to the Edison. Serial connection, find out COM port via device manager (Ports/USB Serial Port), speed is 115200.
7. Press Enter until the login prompt appears (1-2 times).
8. Login as **root**.
9. ~# reboot ota  
This automatically flashes the previously loaded image to the Edison, while rebooting. May take a while.
10. Login again as **root**.
11. ~# configure_edison --name
12. ~# configure_edison --password
13. ~# systemctl disable network-gadget-init.service  
This keeps the USB device from loading as a network interface.
14. ~# configure_edison --wifi
15. ~# reboot

So far the standard WiFi configuration tool in step 14 has worked fine for me. After successful connection you can also log in via ssh.

###1.2 Package Repositories
The Edison has no pre-configured package repository. To have access to some basic programs, follow the instructions [here](http://alextgalileo.altervista.org/edison-package-repo-configuration-instructions.html).  
Afterwards you may install some basic packages, like:

    opkg install bash packagegroup-core-buildessential cmake vim git ...

To get some basic IO functionality in your code, install the mraa library [here](https://github.com/intel-iot-devkit/mraa) and the upm repository [here](https://github.com/intel-iot-devkit/upm).  

    echo "src mraa-upm http://iotdk.intel.com/repos/1.1/intelgalactic" > /etc/opkg/mraa-upm.conf
    opkg update
    opkg install libmraa0 upm

A table with pin mappings for the mraa library can be found [here](http://iotdk.intel.com/docs/master/mraa/edison.html).  

You might also have to clone the upm repo and install it from source to compile some of the code here.  
In the cloned upm repo:

    mkdir build
    cd build
    cmake .. -DCMAKE_INSTALL_PREFIX:PATH=/usr
    make

###1.3 Partition Size Problem
I had a problem where the root partition of the Edison was 100% full at some point. This was mostly due to a lot of system boot logs (>100MB) still saved in **/var/log/journal**.  
A good fix for this would be to increase the size of the root partition (standard is 500MB), which is possible, but requires to rebuild the image, by hand, from scratch, and then re-flashing the Edison.  
What I ended up doing was simply limiting the size of the kept logs, after deleting all current saved logs.  

    rm -rf /var/log/journal/*
    sed -i '$a SystemMaxUse=25M' /etc/systemd/journald.conf

**Update:** Apparently limiting the maximum log size didn't work, I ran into the same problem again. I just deleted the logs again and changed the first line to

    Storage=none

No logs will be kept but the root partition won't fill up again. Might try to change this to *volatile*, which temporarily saves the current system log in /run/log/journal.

###1.4 Pin Mappings
Some important pin mappings for the project. Complete tables can be found [here](https://github.com/intel-iot-devkit/mraa/blob/master/docs/edison.md) and in the breakout board hardware guide (see dropbox).

| Edison Pin | Edison Function |   Description   | MRAA # |
|:----------:|:---------------:|:---------------:|:--------:|
|    19-1    |      V_SYS      | 3.15-4.5V input |    28    |
|    19-2    |      V_OUT      |   3.3V output   |    29    |
|    19-3    |       GND       |       GND       |    30    |
|    17-10   |     SPI-5-CS    |    Display CS   |     9    |
|    17-11   |    SPI-5-CLK    |   Display CLK   |    10    |
|    17-12   |    SPI-5-MOSI   |   Display SDI   |    11    |
|    19-5    |     GPIO-46     |   Display RST   |    32    |
|    19-6    |     GPIO-48     |    Display RS   |    33    |
|    18-13   |     UART1-RX    |   BT Module TX  |    26    |
|    19-8    |     UART1-TX    |   BT Module RX  |    35    |
|    17-7    |    I2C-6-SCL    |                 |     6    |
|    17-9    |    I2C-6-SDA    |                 |     8    |
|    18-6    |    I2C-1-SCL    |                 |    19    |
|    17-8    |    I2C-1-SDA    |                 |     7    |
