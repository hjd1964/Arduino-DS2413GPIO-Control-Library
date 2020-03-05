# Arduino Library for Maxim Temperature Integrated Circuits

## Usage

This library supports the following devices :


* DS2413


You will need a pull-up resistor of about 2.2k KOhm between the 1-Wire data line
and your 5V power.

Finally, please include OneWire from Howard Dutton (based on Paul Stoffregen's library) in the library manager before you begin.

## Credits

The OneWire code has been derived from
http://www.arduino.cc/playground/Learning/OneWire.
Miles Burton <miles@mnetcs.com> originally developed this library.
Tim Newsome <nuisance@casualhacker.net> added support for multiple sensors on
the same bus.
Guil Barros [gfbarros@bappos.com] added getTempByAddress (v3.5)
   Note: these are implemented as getTempC(address) and getTempF(address)
Rob Tillaart [rob.tillaart@gmail.com] added async modus (v3.7.0)


# License

This library is free software; you can redistribute it and/or
modify it under the terms of the GNU Lesser General Public
License as published by the Free Software Foundation; either
version 2.1 of the License, or (at your option) any later version.

This library is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
Lesser General Public License for more details.

You should have received a copy of the GNU Lesser General Public
License along with this library; if not, write to the Free Software
Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA