// Copyright 2023 QMK
// SPDX-License-Identifier: GPL-2.0-or-later

#pragma once

#include QMK_KEYBOARD_H

// Represents the four states a oneshot key can be in
typedef enum {
    os_up_unqueued, // This state represents the condition when the oneshot key is not actively held (up) and has not been queued for activation. 
                    // In this state, the associated modifier key is not registered, and no action has been taken.
    os_up_queued, // This state represents the condition where the oneshot key has been pressed, but the associated modifier key has been queued for activation. 
                  // It means that the modifier key has not been registered yet and will only be registered after a subsequent non-modifier key press.
    os_down_unused, // This state represents the condition when the oneshot key is currently pressed (down) and the associated modifier key is registered, 
                    // but the modifier key hasn't been used for any other keypress yet.
    os_down_used, // This state represents the condition when the oneshot key is currently pressed (down), the associated modifier key is registered, 
                  // and the modifier key has been used for at least one keypress since it was registered.
    os_toggled, // Represents the condition that the oneshot key has been pressed twice in a row, causing it to toggle the modifier until the next keypress.
} oneshot_state;

// Custom oneshot mod implementation that doesn't rely on timers. If a mod is
// used while it is held it will be unregistered on keyup as normal, otherwise
// it will be queued and only released after the next non-mod keyup.
// Additionally, double tap to toggle.
    // oneshot_state and force_off must be pre-defined in keymap.c per each oneshot mod in the following manner:
    // oneshot_state os_<your_mod_name>_state = os_up_unqueued;
    // bool force_off_<your_mod_name> = false;
void update_oneshot(
    oneshot_state *state, // &os_<your_mod_name>_state
    bool force_off, // &force_off_<your_mod_name>
    uint16_t mod, // mod keycode
    uint16_t trigger, // <your_mod_name>
    uint16_t keycode, // keycode
    keyrecord_t *record // record
);

// To be implemented by the consumer. Defines keys to cancel oneshot mods.
bool is_oneshot_cancel_key(uint16_t keycode);

// To be implemented by the consumer. Defines keys to ignore when determining
// whether a oneshot mod has been used. Setting this to modifiers and layer
// change keys allows stacking multiple oneshot modifiers, and carrying them
// between layers.
bool is_oneshot_ignored_key(uint16_t keycode);