---
layout: post
title:  "Using the SIM 908 with the Odroid U3"
date:   2017-01-01 16:35:52
categories: hardware
author: john
---


# Using the SIM 908 with the Odroid U3

The Odroid U3 is a powerful and compact linux computer powered by the Samsung Exynos Prime Cortex-A9 Quad Core processor. The board's low cost and impressive specifications make it a perfect platform for embedded devices with significant computational needs. 
I'm using the board as part of the logic system in an autonomous marine surface vehicle, and needed low-cost solution for remote data transmission. For my purposes, the vehicle will not likely need to transmit or receive data outside of the range of a typical cell connection, so GPRS was struck a perfect balance between transmission range and cost-effectiveness.  

The SIM 908 is a quad-band GSM/GPRS module with integrated GPS. This module was a great fit for my project, but unfortunately no guides existed for integrating the module with the Odroid U3. 
This guide aims to fill this void, and can serve as a reference for those hoping to integrate the SIM 908 module with other Exynos / Snapdragon based embedded computers.
  
For this guide, I'm using the Adafruit Fona SIM 908 breakout board, and an Odroid U3 running Arch linux.

# Software Configuration


## Automatic Set-up :

* Download this [file](https://gist.github.com/johngroves/2ac7f2be190e5c29f3c36a35745cca97#file-sim908-sh).

* Run `sh sim908.sh` in a terminal window.

## Manual Set-up :

### PPPD configuraion :

Inspired by [Adafruit's FONA tutorial](https://learn.adafruit.com/fona-tethering-to-raspberry-pi-or-beaglebone-black/setup)

The easiest way to enable cellular connectivity on a Linux device is to use [PPP](https://wiki.archlinux.org/index.php/pppd), a Linux implementation of the point-to-point protocol. 

Using PPP requires creating both a PPPD configuration file, as well as a GPRS chat script.


#### Creating a PPPD Configuration :

PPPD configurations specify the behaviors and settings required for establishing a network connection.

These configurations should be stored in the peer connection directory `/etc/ppp/peers/`. The name of the configuration file will serve as the connection's provider name.

```
# Example PPPD configuration for SIM908 on Archlinux-ARM.

# MUST CHANGE: Change the -T parameter value **** to your network's APN value.
# For example if your APN is 'internet' (without quotes), the line would look like:
# connect "/usr/sbin/chat -v -f /etc/chatscripts/gprs -T internet"
# I'm connecting via Ting's 2G cellular plan for IOT, which uses the APN "wholesale"

connect "/usr/sbin/chat -v -f /etc/chatscripts/gprs -T wholesale"

# The Odroid U3's UART port is part of GPIO #1, and named "ttySAC0"

/dev/ttySAC0

# Baud rate of the serial line. I'm using 9600, but it can be increased as needed.
9600

# Assumes that your IP address is allocated dynamically by the ISP.
noipdefault

# Try to get the name server addresses from the ISP.
usepeerdns

# Use this connection as the default route to the internet.
defaultroute

# Makes PPPD "dial again" when the connection is lost.
persist

# Do not ask the remote to authenticate.
noauth

# No hardware flow control on the serial link
nocrtscts

# No modem control lines
local
```

#### Creating a GPRS Chat Script :

The GPRS chat script will dialogue with the modem and includes scripts to provide a SIM code and connection behaviors.
This script should be saved in the chat scripts directory, with the name `gprs`, `/etc/chatscripts/gprs`.

```
# Abort the chat script on the following conditions
 
ABORT           BUSY
ABORT           VOICE
ABORT           "NO CARRIER"
ABORT           "NO DIALTONE"
ABORT           "NO DIAL TONE"
ABORT           "NO ANSWER"
ABORT           "DELAYED"
ABORT           "ERROR"
 
# Abort attempt to connect if the modem is not yet attached to the network.

ABORT           "+CGATT: 0"
 
""				AAAAAA # init the baudrate
""              AT
TIMEOUT         12
OK              ATH
OK              ATE1
 
# Use +CPIN if you need to provide the SIM card PIN example uses code 1234.

#OK             "AT+CPIN=1234"
 
# Set the PDP context, continue on OK
 
OK              AT+CGDCONT=1,"IP","\T","",0,0
OK              ATD*99#

# Timeout connection attempt after 22 seconds

TIMEOUT         22
CONNECT         ""
```


# Hardware Configuration: 

The Odroid U3's UART port is pre-configured by default.  

A pinout and further details of GPIO #1 can be found [here](http://odroid.com/dokuwiki/doku.php?id=en:u3_enhancement_ioport).

## Connecting to Fona SIM 908 :

To connect the Odroid U3 with the Fona SIM908 module, it's best to first make a few connections between pins on the module itself.

### Fona SIM908 Connections :

*  5V > VIO - Connects the 5V pin (used to charge lipoly battery) with V in, to allow for one 5V source to power the board and charge the battery.
*  GND > KEY - Connect KEY to GND to keep the SIM 908 on when under power.

### Odroid to SIM908 Connections :

* Odroid GND (Pin 7) > SIM908 GND
* Odroid 5V (Pin 8) > SIM908 5V
* Odroid UART TX (Pin 6)  > SIM908 RX
* Odroid UART RX (Pin 4) > SIM908 TX

# Making The Connection 

To test the hardware connection, run `chat -t 5 -vs '' 'AT' 'OK' > /dev/ttySAC0 < /dev/ttySAC0`.
This command starts a communication with the modem. If there is not an 'OK' response, there is likely a wiring issue.

## Start PPPD, and watch the logs :

* In your terminal, type `pon <provider>`

* Use `tail -f /var/log/syslog | grep chat` to monitor logs of the serial connection.

* Use `tail -f /var/log/syslog | grep pppd` to monitor the PPPDs logs.

## To close the connection:

* Use `poff <provider>`

### Hardware Connections:

![Odroid U3 SIM 908](http://johngrov.es/img/odroidu3.jpg)