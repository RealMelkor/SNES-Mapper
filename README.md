# SNES controller mapper

Map SNES controllers input to keyboard input on [FreeBSD](https://freebsd.org/).
This program was made as a workaround to use kiwitatá USB SNES controllers
in [Snes9x](https://www.snes9x.com/) on FreeBSD when Snes9x doesn't detect the
controller.

## Compiling

### Dependencies

This program uses XCB, the library can be installed with the following command :
> pkg install libxcb

To build the program use the command :
> make

To install the program use the command :
> sudo make install

## Usage

Simply run this command to map the controller to keyboard input :
> snes_mapper /dev/uhid_snes0

It is possible to map a second controller using this command :
> snes_mapper /dev/uhid_snes1
