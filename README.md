#Project Lasertag

Repository for the [Wearable Computing Lab Course 2014](http://praktikum.ese.uni-freiburg.de/)  
University of Freiburg  

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

###1.3 Other Stuff
I had a problem where the root partition of the Edison was 100% full at some point. This was mostly due to a lot of system boot logs (>100MB) still saved in **/var/log/journal**.  
A good fix for this would be to increase the size of the root partition (standard is 500MB), which is possible, but requires to rebuild the image, by hand, from scratch, and then re-flashing the Edison.  
What I ended up doing was simply limiting the size of the kept logs, after deleting all current saved logs.  

    rm -rf /var/log/journal/*
    sed -i '$a SystemMaxUse=25M' /etc/systemd/journald.conf

**Update:** Apparently limiting the maximum log size didn't work, I ran into the same problem again. I just deleted the logs again and changed the first line to

    Storage=none

No logs will be kept but the root partition won't fill up again. Might try to change this to *volatile*, which temporarily saves the current system log in /run/log/journal.