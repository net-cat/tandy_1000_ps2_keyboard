# Tandy 1000 Keyboard Adapter

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

### Supplies

* PS2 Slot Plate Bracket ([example](https://www.amazon.com/gp/product/B0002AFZV2/))
* Arduino Pro Mini or compatible board. ([example](https://store.arduino.cc/usa/arduino-pro-mini))
* Some female Dupont connectors you can sacrifice. ([example](https://www.amazon.com/gp/product/B01EV70C78/))
* Arduino IDE ([link](https://www.arduino.cc/en/Main/Software))
* Small gauge wire. ([example](https://www.amazon.com/gp/product/B0791BL9HN/))
* A USB to TTL Serial adapter for programming the arduino. ([example](https://www.amazon.com/gp/product/B00IJXZQ7C/))
* A 5V molex connector you can sacrifice. ([example](https://www.amazon.com/gp/product/B07V5919H8/))

> NOTE: These directions assume you have some familiarity with DIY electronics and soldering, as well as with programming an Arduino.

### Configuring the Arduino

> NOTE: Please be careful about programming the Arduino while it is installed. Your USB port probably can't power the Tandy 1000's 5V rail and it's probably not great for the Tandy to have only its 5V rail powered.

1. Connect your USB/TTL Adapter to your system, installing the drivers if you have Windows.
2. Install the [PS2KeyAdvanced](https://github.com/techpaul/PS2KeyAdvanced) library into your Arduino IDE. (Either from GitHub or the `Tools -> Manage Libraries...` menu.)
3. In the Tools menu, set the board to `Arduino Pro or Pro Mini` and the Port to whatever whatever your USB/TTL adater is.
4. Connect your USB/TTL Adapter to the Arduino Pro Mini. (Make sure to hook up the DTR line, as it's needed for programming. You may need to solder the serial header onto the Pro Mini.)
4. Open the `tandy_kbd.ino` file in the IDE.
5. Press the `Upload` button.
6. Once the upload is completed, disconnect the USB cable.

### Connections

1. Remove the Dupont connector block from the PS2 slot plate bracket. Replace the four wires with individual Dupont connectors.
2. Most Arduino Pro Mini boards come with pin headers you can solder onto the board. Solder three pin headers into pins `3` and `4` and `GND`. These are your PS/2 `CLK`, `DATA` and `GND` connections, respectively.
3. Solder the 5V molex connector to `VCC` and `GND` on the board. (Note that if you use a fan connector, those are connected to 12V. You will have to modify it to connect to 5V instead. The Pro Mini doesn't have the voltage regulation of the Uno.)
4. Solder two long wires to pins `5`, `6` and `7` on the Pro Mini. (Use different colors to make it easier to tell them apart.) These will be your Tandy `DATA`, `CLK` and `BUSY` lines, respectively. I suggest 18-24 inches. You can always cut off excess later.
5. Strip about a half an inch of insulation off the other ends of the wires from step 4.
6. Plug the PS/2's `VCC` connection into the corresponding port on the Pro Mini's serial pin header. (Looking at the female PS/2 connector, middle left for `VCC`.)
7. Plug the PS/2's `CLK`, `DATA` and `GND` lines into the pin headers you soldered onto pins `3`, `4` and `GND`, respectively. (Looking at the female PS/2 connector, top right for `CLK`, bottom right for `DATA` and and middle right for `GND`.)
8. Feed the Tandy `CLK`, `DATA` and `BUSY` wires from step 4 through the front of the case and plug the bare wire into the keyboard socket. (Looking at the Tandy 1000 keyboard connector, `CLK` is the far left, `DATA` is the bottom left (immediately below `CLK`) and `BUSY` is bottom.)
9. Place a piece of tape over the keyboard connector to secure the wires. Also secure the wires inside the case to prevent them from being pulled out when the case is open.
9. Plug the molex connector into a spare connector from the power supply.
10. Mount the Pro Mini in the case in a way that makes you feel comfortable that it won't short anything out.

If everything is connected correctly, the adapter should be fully functional. You can type on the keyboard and it should work mostly as expected.

### Images

This is what the finished project looks like in my Tandy 1000 TX:

![Image of Arduino](https://raw.githubusercontent.com/net-cat/tandy_1000_ps2_keyboard/master/images/pro_mini_connected.jpg)

![Image of Keyboard Connector](https://raw.githubusercontent.com/net-cat/tandy_1000_ps2_keyboard/master/images/keyboard_connector_wired.jpg)

![Image of keyboard connector after it's secured](https://raw.githubusercontent.com/net-cat/tandy_1000_ps2_keyboard/master/images/front_connector_protected.jpg) ![Image of internal keyboard wires after they're secured](https://raw.githubusercontent.com/net-cat/tandy_1000_ps2_keyboard/master/images/keyboard_wires_secured.jpg)


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

* Print
* Hold
* Break

Keys that have multiple copies on a PS/2 keyboard but only one on a Tandy keyboard cannot be differentiated by the Tandy:

* Alt (AltGr is treated as a second Alt key for keyboards that have it.)
* Ctrl

The following types of buttons have no meaning on a Tandy 1000 and are dead keys:

* Power Keys
* Media Keys
* Logo/Menu Keys
* Meta/Super Keys

Any PS/2 inputs recieved while the BUSY line from the Tandy is LOW will be ignored. Scancodes will not be transmitted to the Tandy, but any codes already in the queue will be transmitted as soon as the BUSY line is HIGH again.

## XT Compatibility Mode

XT Compatibility Mode exists to work around issues with certain games and programs that directly interact with the keyboard hardware. To activate it, simply turn on the Scroll Lock. While active, the arrow keys and F11 and F12 will send XT-compatible scan codes rather than the original Tandy scan codes.

## TODO:

These are not things I necessarily plan to do. I will accept pull requests that implement them. (Or if enough people ask for them.)

In no particular order:

* Implement Print, Hold and Break keys.
* PS2KeyAdvanced supports other languages. Make that easier to configure.
* Implement RESET signal from Tandy.
* Figure out why the Num/Caps Lock keys cause the Arduino to lock up without the `delay(100)` call.
