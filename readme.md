# The Arctic42

- [Revision keymap](keymaps/rev1)

![Arctic42 Keymap](https://i.imgur.com/h5WJSEn.png)

Based originally off the corne, but with a knob, USB-C, and with an RP2040-zero
6x3_3 Columnar Staggered Layout Split Keyboard
My layout was created with the intention of creating a comfortable typing experience primarily for myself.
There are no homerow mods to be found as I dislike the delay that comes with using them.
My layers are callum-styled, meaning that which ever hand is currently being used to hold a layer button in, gets used as minimally as possible. You can mostly only find homerow modifiers on these halves which are setup for oneshot usage. This means that when pressed down, they get queued up for the next keypress. When this next keypress happens, the currently queued modifier will remain active until the current keypress is lifted. This way you can hit CTRL in a layer, and follow up by hitting a letter on the base layer.

QMK has built-in oneshot modifier support, but I was unsatisfied with its functionality. This lead me to heavily modify Callum's custom implimentation of oneshot modifiers to work exactly as I wanted. In addition to Callum's implimentation, they have 3 new features:
- Force off
- Toggle
- Timed hold.

Force off is a logic implimentation that checks if a oneshot modifier has been used, and immediately shuts it off upon the next keypress, regardless if the first keypress after oneshot queueing has been lifted or not. This prevents miss-fires from happening when typing quickly or when rolling your fingers.

Toggle is a basic function that I saw nessesary for full comfort when using a modifier consecutively for long periods of time.

Lastly, timed hold is a logic implimentation that checks how long it took the user to lift their finger when the oneshot modifier was not used. If the user held the modifier for longer than 300ms without using another key, then the modifier acts like a regular modifier and unregisters when the key is lifted.

(W.I.P)

## Building.

* Keyboard Maintainer: [Ethan Gretna](https://github.com/PixelFrosty)
* Hardware Supported: Custom PCB, RP2040-zero
* Hardware Availability: JLCPCB, Amazon, Waveshare

Must be flashed onto MCU's using the following commands:

`qmk flash -kb arctic42 -km rev1 -bl uf2-split-left` for the left side.

`qmk flash -kb arctic42 -km rev1 -bl uf2-split-right` for the right side.

See the [build environment setup](https://docs.qmk.fm/#/getting_started_build_tools) and the [make instructions](https://docs.qmk.fm/#/getting_started_make_guide) for more information. Brand new to QMK? Start with our [Complete Newbs Guide](https://docs.qmk.fm/#/newbs).

## Bootloader

Enter the bootloader in 3 ways:

* **Bootmagic reset**: Hold down the R key on the left side, or the U key on the right side
* **Physical reset button**: Hold down the physical boot button when plugging in USB-C or double tap reset when already plugged in
* **Keycode in layout**: Press `QK_BOOT` on the developer layer
