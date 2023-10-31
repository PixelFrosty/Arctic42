# arctic42

![Arctic42 Keymap](https://i.imgur.com/vb8BvgZ.png)

Based originally off the corne, but with a knob, USB-C, and with an RP2040-zero.
6x3_3 Columnar Staggered Layout Split Keyboard

* Keyboard Maintainer: [Ethan Gretna](https://github.com/PixelFrosty)
* Hardware Supported: Custom PCB, RP2040-zero
* Hardware Availability: JLCPCB, Amazon, Waveshare

Make example for this keyboard (after setting up your build environment):

    make arctic42:default

Flashing example for this keyboard:

    make arctic42:default:flash

See the [build environment setup](https://docs.qmk.fm/#/getting_started_build_tools) and the [make instructions](https://docs.qmk.fm/#/getting_started_make_guide) for more information. Brand new to QMK? Start with our [Complete Newbs Guide](https://docs.qmk.fm/#/newbs).

## Bootloader

Enter the bootloader in 3 ways:

* **Bootmagic reset**: Hold down the key at (0,0) in the matrix (usually the top left key or Escape) and plug in the keyboard
* **Physical reset button**: Briefly press the button on the back of the PCB - some may have pads you must short instead
* **Keycode in layout**: Press the key mapped to `QK_BOOT` if it is available
