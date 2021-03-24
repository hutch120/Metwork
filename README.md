# Metwork
## Expressif 32
https://docs.espressif.com/projects/esp-idf/en/latest/esp32/api-reference/

## TTGO
https://github.com/Xinyuan-LilyGO/TTGO_TWatch_Library

## ESPNow w/ Raspberry Pi

The aim of this integration is to get messages from the T-Watch-2020 to be seen by the Raspberry Pi.

I started with a vanilla Raspberry Pi 3 with latest Raspian installed and all updates applied.

This was very technical, and there were a lot of false starts in getting this working.

The details below are the basics of what I needed to do to get it receiving messages consistantly in Wireshark from the T-Watch-2020 ESPNow signals.

There was a lot of experimentation, so this might not be 100% accurate, but should be the basics.

Run these commands on an existing RPi installation to replace the default wifi firmware driver with one that supports monitor mode.
[Ref](https://null-byte.wonderhowto.com/how-to/enable-monitor-mode-packet-injection-raspberry-pi-0189378/)

```
sudo su
cd /usr/local/src
wget  -O re4son-kernel_current.tar.xz https://re4son-kernel.com/download/re4son-kernel-current/
tar -xJf re4son-kernel_current.tar.xz
cd re4son-kernel_4*
./install.sh
```

Install `airmon-ng`

```
sudo apt-get install airmon-ng
```

Run these commands to stop conflicting programs and start monitor mode. Note this also kills your wifi internet/AP connection.
If you don't kill the conflicting programs there were a lot of missing messages (action frames). [Ref](https://tools.kali.org/wireless-attacks/airmon-ng)

```
sudo airmon-ng check kill
sudo airmon-ng start wlan0 1
```

Run `ifconfig` to check that the new monitor interface is ready... it should be called wlan0mon.

Now you can either run Wireshark (`startx` -> Under the Internet menu -> change Wireshark shortcut... add `sudo`), 
or run `sudo airodump-ng` from command line. [Ref](https://tools.kali.org/wireless-attacks/airodump-ng)

For Wireshark, pick the wlan0mon interface and set the filter to `wlan.fc.type_subtype == 13` (This is a filter for Action Frames or 0x000d)

References:
- https://null-byte.wonderhowto.com/how-to/enable-monitor-mode-packet-injection-raspberry-pi-0189378/
- https://github.com/seemoo-lab/nexmon
- https://www.kali.org/docs/arm/kali-linux-raspberry-pi/
- https://re4son-kernel.com/re4son-pi-kernel/
- https://tools.kali.org/wireless-attacks/airmon-ng
- https://tools.kali.org/wireless-attacks/airodump-ng
