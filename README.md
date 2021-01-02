# Thumbsplit58 Keyboard Firmware
Firmware for Nordic MCUs used in the Thumbsplit58 Keyboard. You may also want to refer [PCB, case](https://github.com/mohammedari/thumbsplit58-hardware) and [qmk_firmware](https://github.com/mohammedari/qmk_firmware/blob/oneoff/thumbsplit58/keyboards/thumbsplit58/) repository.

![teaser](https://github.com/mohammedari/thumbsplit58-hardware/blob/main/doc/teaser.jpg?raw=true)

The firmware was originally developed by [reversebias](https://github.com/reversebias) for the [Mitosis keyboard](https://github.com/reversebias/mitosis) and has been modified by [satt99](https://github.com/satt99) to work with [Comet46 keyboard](https://github.com/satt99/comet46-firmware).

## Install dependencies

Tested on Ubuntu 20.04, but should be able to find alternatives on all distros. 

```
sudo apt install openocd gcc-arm-none-eabi
```

## Download Nordic SDK

Nordic does not allow redistribution of their SDK or components, so download and extract from their site:

https://developer.nordicsemi.com/nRF5_SDK/nRF5_SDK_v11.x.x/

Current firmware only works with SDK version 11.

```
unzip nRF5_SDK_11.0.0_89a8197.zip  -d nRF5_SDK_11
cd nRF5_SDK_11
```

## Toolchain set-up

A cofiguration file that came with the SDK needs to be changed. Assuming you installed gcc-arm with apt, the compiler root path needs to be changed in /components/toolchain/gcc/Makefile.posix, the line:
```
GNU_INSTALL_ROOT := /usr/local/gcc-arm-none-eabi-4_9-2015q1
```
Replaced with:
```
GNU_INSTALL_ROOT := /usr/
```

## Clone repository
Inside nRF5_SDK_11/
```
git clone https://github.com/mohammedari/thumbsplit58-firmware
```

## Install udev rules
```
sudo cp thumbsplit58-firmware/49-stlinkv2.rules /etc/udev/rules.d/
```
Plug in, or replug in the programmer after this.

## OpenOCD server
The programming header on the keyboard, from right to left:  
(Check the silk screen on PCBs for orders.)
```
SWCLK
SWDIO
GND
PWR(3.3V)
```

It's best to remove the battery during long sessions of debugging, as charging non-rechargeable lithium batteries isn't recommended.

Launch a debugging session with:
```
./openocd.sh
```
Should give you an output ending in:
```
Info : nrf51.cpu: hardware has 4 breakpoints, 2 watchpoints
```
Otherwise you likely have a loose or wrong wire.

## Build and flash receiver firmware

After connecting the receiver board via stlink and launching openocd in another terminal, use the following script.
```
cd thumbsplit58-firmware/thumbsplit58-receiver-basic
./program.sh
```

## Build and flash keyboard firmware

Use the same script under `thumbsplit58-keyboard-basic`.
You have to change define switch `COMPILE_RIGHT` and `COMPILE_LEFT` according to the board you connect before running the script.

