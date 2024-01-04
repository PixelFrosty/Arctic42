// Copyright 2023 QMK
// SPDX-License-Identifier: GPL-2.0-or-later

#include "oneshot.h"

void update_oneshot(
    oneshot_state *state,
    bool *force_off,
    uint16_t *time,
    uint16_t mod,
    uint16_t trigger,
    uint16_t keycode,
    keyrecord_t *record
) {
    if (keycode == trigger) {
        if (record->event.pressed) {
            // Trigger keydown
            if (*state == os_toggled) {
                // If mod is currently toggled, untoggle it.
                *state = os_up_unqueued;
                unregister_code(mod);
            } else {
                if (*state == os_up_unqueued) {
                    // If mod was inactive, then start registering it.
                    register_code(mod);
                    *time = timer_read();
                }
                if (*state == os_up_queued && timer_elapsed(*time) < 200){
                    // If mod was pressed twice (within 200ms) before it was used (i.e. before a non-mod key was pressed), then toggle it.
                    // by the following else statement of the last cycle, toggle the mod.
                    *state = os_toggled;
                } else {
                    // If mod was pressed once, set it to the "os_down_unused" state.
                *state = os_down_unused;
                }
            }
        } else {
            // Trigger keyup
            switch (*state) {
            case os_down_unused:
                if (timer_elapsed(*time) > 300)  {
                    // If mod was held down for long enough, turn it off like a regular modifier.
                    *state = os_up_unqueued;
                    unregister_code(mod);
                } else {
                    // If mod was not held down for long enough, queue it.
                    *state = os_up_queued;
                }
                break;
            case os_down_used:
                // If we did use the mod while trigger was held, unregister it.
                *state = os_up_unqueued;
                unregister_code(mod);
                break;
            default:
                break;
            }
        }
    } else { // Non trigger key
        if (record->event.pressed) {
            if (is_oneshot_cancel_key(keycode) && *state != os_up_unqueued) {
                // Cancel oneshot on designated cancel keydown.
                *state = os_up_unqueued;
                unregister_code(mod);
            }
            if (*force_off && !is_oneshot_ignored_key(keycode)) {
                // When using oneshot mod, if first non mod keypress is still held in, disable it on the next
                // key down before that key's input is registered to pc to prevent miss-fires during key rolling.
                *state = os_up_unqueued;
                unregister_code(mod);
                *force_off = false;
            } else {
                if (*state == os_up_queued && !is_oneshot_ignored_key(keycode)){
                    // After the first non mod keypress, prep force_off to disable the oneshot modifier.
                    *force_off = true;
                }
            }
        } else {
            if (!is_oneshot_ignored_key(keycode)) {
                // On non-ignored keyup, consider the oneshot used.
                switch (*state) {
                case os_down_unused:
                    *state = os_down_used;
                    break;
                case os_up_queued:
                    *state = os_up_unqueued;
                    unregister_code(mod);
                    *force_off = false;
                    break;
                default:
                    break;
                }
            }
        }
    }
}