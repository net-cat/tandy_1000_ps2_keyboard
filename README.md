# Tandy 1000 Keyboard Adapter

> WARNING: This is all very preliminary. I'm still working on getting everything polished up. I plan to add pictures later.

## What is this for?

Tandy 1000 machines are becoming collectors items and there seems to be a lot of them where the keyboard has been lost or damaged. This is unfortunate because they use a non-standard 8-pin keyboard connection that was unique to Tandy, and there seem to be more of the computers out there than the keyboards.

This is an Arduino Sketch that will allow you to plug a PS/2 keyboard into an older Tandy 1000 machine with that 8-pin DIN keyboard connector.

I personally have only tested this with a Tandy 1000 TX, but in theory the following models should be supported:

* 1000
* 1000 HD
* 1000 SX
* 1000 TX
* 1000 EX (1)
* 1000 HX (1)

(1) The EX and HX have built in keyboards, but they appear to be electrically the same as the external keyboards. This would be a major modification for these systems.


## How do I use this?

There are two ways you can use this:

### External Adapter

The external adapter may be preferrable because it doesn't require modifying the system and because it can be used on multiple systems, but it requires a lot more soldering.

#### Supplies

* 8-Pin DIN Connectors ([example](https://www.amazon.com/gp/product/B07GLQ7PXX/))
* AT to PS/2 Adapter ([example](https://www.amazon.com/gp/product/B07KVF5TV9/))
* Arduino Uno R3 or compatible board. ([example](https://store.arduino.cc/usa/arduino-uno-rev3))
* Arduino IDE ([link](https://www.arduino.cc/en/Main/Software))
* Some wires that you can connect to an Arduino on one end and solder on the other.
* Some soldering skills.

#### 8-pin DIN numbering

Looking straight at the 8-pin DIN **plug**, the top *right* pin is 1, increasing as you move *clockwise*. The center pin is 8.

Looking straight at the 8-pin DIN **socket**, the top *left* hole is 1, increasing as you move *counter-clockwise*. The center pin is 8.

#### Connections 

> NOTE: These directions are tentative. I have not fully tested them yet.

1. Solder some wires onto an 8-pin DIN plug. This will plug into the Tandy. (This will be annoying, but you only need 6.)
2. Wire these signals into the Arduino.

| Pin | Arduino Pin |Signal         |
|-----|-------------|---------------|
| 1   | Not connected. |            |
| 2   | Any `GND`   | `Ground`      |
| 3   | `Vin`       | `+5V`          |
| 4   | *TBD*       | `Busy`(2)      |
| 5   | `6`         | `Clock`        |
| 6   | `5`         | `Data`         |
| 7   | *TBD*       | `Reset`(2)     |
| 8   | Not connected. |            |

(2) Not needed for a functional keyboard, but may cause glitches.

3. Solder some wires onto an 8-pin DIN socket. This what the AT-to-PS/2 Adapter will plug into.
4. Wire these signals into the Arduino.

| Pin | Arduino Pin | Signal         |
|-----|-------------|----------------|
| 1   | Not connected. |             |
| 2   | Not connected. |             |
| 3   | `5V`        | `+5V`         |
| 4   | `4`         |`Data`         |
| 5   | Any `GND`   |`Ground`       |
| 6   | `3`         |`Clock`        |
| 7   | Not connected. |             |
| 8   | Not connected. |             |

5. Connect your PS/2 keyboard to the AT-to-PS/2 adapter.
6. Connect your AT-to-PS/2 adapter to the 8-pin DIN socket on the arduino.
7. Connect the 8-pin plug to the Tandy 1000.
8. Turn the Tandy 1000 on.
9. Continue with the "Configuring the Arduino" section below.

### Internal Adapter

I plan to do an internal version of this, since I only have one machine. It's very tentative right now.

#### Supplies

* PS2 Slot Plate Bracket ([example](https://www.amazon.com/gp/product/B0002AFZV2/))
* Arduino Uno R3 or compatible board. ([example](https://store.arduino.cc/usa/arduino-uno-rev3))
* Arduino IDE ([link](https://www.arduino.cc/en/Main/Software))
* Some wires that you can connect to an Arduino on one end and solder on the other.
* Some soldering skills.

#### Connections

I haven't done the implementation on this yet, but in principle you solder wires onto the keyboard interface on the motherboard, connect them to the Arduino, connect the slot cover to the arduino, then install the slot cover.

### Configuring the Arduino

> NOTE: If your Tandy 1000 is turned off when you plug in the USB, you will have to disconnect the `Vin` pin for the USB interface to work.

1. Connect your Arduino to the system, installing the drivers if you have Windows.
2. Install the [PS2KeyAdvanced](https://github.com/techpaul/PS2KeyAdvanced) library into your Arduino IDE. (Either from GitHub or the `Tools -> Manage Libraries...` menu.)
3. In the Tools menu, set the board to `Arduino Uno` and the Port to whatever says `Arduino Uno` next to it.
4. Open the `tandy_kbd.ino` file in the IDE.
5. Press the `Upload` button.
6. Once the upload is completed, disconnect the USB cable.

If everything is connected correctly, the adapter should be fully functional. You can type on the keyboard and it should work mostly as expected.

## Quirks

This adapter is designed to make using a PS/2 keyboard on a Tandy 1000 as seamless as possible. Since a standard PS/2 keyboard doesn't map perfectly onto a Tandy keyboard, some issues may be encountered with spurious Right Shift and Num Lock presses.

The following keys map to the Tandy's number pad. Num Lock, if active, will be briefly deactivated when they are pressed:

* Grave/Tilde
* Backslash/Pipe
* Page Up
* Page Down
* End
* Insert
* Delete

Num Lock, if inactive, will be briefly activated when these keys are pressed:

* Number Pad Plus
* Number Pad Minus

Other Quirks:
* Number Pad Asterisk will press Right Shift+8
* Number Pad Forward Slash will press the Forward Slash/Question Mark key. (Pressing it with Shift held will result in a question mark.)

The following keys on the Tandy keyboard are not yet implemented:

* Print (Likely to be PrtSc/SysRq)
* Hold (Likely to be Scroll Lock)
* Break (Likely to be Pause/Break)

Keys that have multiple copies on a PS/2 keyboard but only one on a Tandy keyboard cannot be differentiated by the Tandy:

* Alt (AltGr is treated as a second Alt key for keyboards that have it.)
* Ctrl

The following types of buttons have no meaning on a Tandy 1000 and are dead keys:

* Power Keys
* Media Keys
* Logo/Menu Keys
* Meta/Super Keys

## TODO:

* Finish implementing keys.
* Pictures.
* Better connection guides.
* Implement BUSY and RESET signals from Tandy.
* PS2KeyAdvanced supports other languages. Make that easier to configure.


