// Copyright 2023 QMK
// SPDX-License-Identifier: GPL-2.0-or-later

#include QMK_KEYBOARD_H

#include "oneshot.h"


// DEBUGGING =========================================================================
void keyboard_post_init_user(void) {
  // Customise these values to desired behaviour
  debug_enable=true;
  debug_matrix=true;
  //debug_keyboard=true;
  //debug_mouse=true;
}

bool process_record_user(uint16_t keycode, keyrecord_t *record) {
  // If console is enabled, it will print the matrix position and status of each key pressed
#ifdef CONSOLE_ENABLE
    uprintf("KL: kc: 0x%04X, col: %2u, row: %2u, pressed: %u, time: %5u, int: %u, count: %u\n", keycode, record->event.key.col, record->event.key.row, record->event.pressed, record->event.time, record->tap.interrupted, record->tap.count);
#endif 
  return true;
}
// DEBUGGING =========================================================================

bool is_alt_tab_active = false; // ADD this near the begining of keymap.c 
bool is_alt_shift_tab_active = false;
uint16_t alt_tab_timer = 0;     // we will be using them soon.

void matrix_scan_user(void) {

  if (is_alt_tab_active) { //ALT TAB Encoder Timer
    if (timer_elapsed(alt_tab_timer) > 1000) {
      unregister_code(KC_LALT);
      unregister_code(KC_LSFT);
      is_alt_tab_active = false;
      is_alt_shift_tab_active = false;
    }
  }

};

layer_state_t layer_state_set_kb(layer_state_t state) { // Run every time a layer is updated
  if (IS_LAYER_OFF_STATE(state, 2) && IS_LAYER_ON_STATE(state, 3)) {
    layer_off(3);
  }
  return state;
}

/*
// COMBOS
const uint16_t PROGMEM MEDIA_LAYER[] = {OSL(2), MO(4), COMBO_END}; // MEDIA LAYER
const uint16_t PROGMEM DEBUG_LAYER[] = {OSM(MOD_LCTL), OSL(2), MEH_T(KC_SPC), OSM(MOD_LSFT), OSM(MOD_LALT), MO(4), COMBO_END}; // DEBUG LAYER, HIT ALL THUMB KEYS

combo_t key_combos[] = {
    COMBO(MEDIA_LAYER, KC_NO), // Activate media layer
    COMBO(DEBUG_LAYER, KC_NO), // Activate debug layer
};
*/

// DEFINING LAYER KEYCODES
//#define LA_UPP OSL(UPP)
//#define LA_SYM OSL(SYM)
//#define LA_NAV MO(NAV)

// DEFINING CUSTOM KEYCODES
enum custom_keycodes {
  PANIC = SAFE_RANGE,  // Panic button to turn off all layers and mods
  TAB_R,               // Tabs to right window 
  TAB_L,               // Tabs to left window
  SNIP_T,              // Activate snipping tool
  OS_SHFT,             // Oneshot Shift
  OS_CTRL,             // Oneshot Ctrl
  OS_ALT,              // Oneshot Alt
  OS_WIN,              // Oneshot Windows
};

bool process_record_kb(uint16_t keycode, keyrecord_t *record) {
  switch (keycode) {
    case PANIC: // Panic Button (Used reset all layers and mods)
      if (record->event.pressed) {
        layer_clear();
        reset_oneshot_layer();
        clear_keyboard();
      }
      return false;

    
    case TAB_R: // Tab to the right window
      if (record->event.pressed) {

        if (!is_alt_tab_active) {
          is_alt_tab_active = true;
          unregister_code(KC_LSFT);
          register_code(KC_LALT);
        }
        alt_tab_timer = timer_read();
        tap_code(KC_TAB);
      }
      return false;

    case TAB_L: //Tab to the left window
      if (record->event.pressed) {

        if (!is_alt_shift_tab_active) {
          is_alt_shift_tab_active = true;
          register_code(KC_LALT);
          register_code(KC_LSFT);
        }
        alt_tab_timer = timer_read();
        tap_code(KC_TAB);
      }
      return false;

    case SNIP_T:
      if (record->event.pressed) {
        // Keybinds for snipping tool screenshot
        register_code(KC_WIN);
        register_code(KC_LSFT);
        register_code(KC_S);
        unregister_code(KC_WIN);
        unregister_code(KC_LSFT);
        unregister_code(KC_S);
        }
        return false;
    

  return true; 
  }
  return true;
};

// KEYMAP
const uint16_t PROGMEM keymaps[][MATRIX_ROWS][MATRIX_COLS] = {
     /*
      *                          Keymap Display
      * ┌───┬───┬───┬───┬───┬───┐             ┌───┬───┬───┬───┬───┬───┐
      * │Tab│ Q │ W │ E │ R │ T │             │ Y │ U │ I │ O │ P │Bsp│
      * ├───┼───┼───┼───┼───┼───┤             ├───┼───┼───┼───┼───┼───┤
      * │ESC│ A │ S │ D │ F │ G │             │ H │ J │ K │ L │ ; │Ent│
      * ├───┼───┼───┼───┼───┼───┤             ├───┼───┼───┼───┼───┼───┤
      * │WIN│ Z │ X │ C │ V │ B │ ┌───┐ ┌───┐ │ N │ M │ , │ . │ / │PNC│
      * └───┴───┴───┴───┴───┴───┘ │MSE│ │MUT│ └───┴───┴───┴───┴───┴───┘
      *               ┌───┐       └───┘ └───┘       ┌───┐
      *               │Ctl├───┐                 ┌───┤Alt│
      *               └───┤Num├───┐         ┌───┤Nav├───┘  
      *                   └───┤Spc│         │Sft├───┘
      *                       └───┘         └───┘
      */
    [0] = LAYOUT_split_3x6_4( // Base layer
       // ┌──────────┬──────────┬──────────┬──────────┬──────────┬──────────┐                                 ┌──────────┬──────────┬──────────┬──────────┬──────────┬──────────┐
       // │  TAB     │  Q       │  W       │  E       │  R       │  T       │                                 │  Y       │  U       │  I       │  O       │  P       │  BSP     │
           KC_TAB,    KC_Q,      KC_W,      KC_E,      KC_R,      KC_T,                                        KC_Y,      KC_U,      KC_I,      KC_O,      KC_P,      KC_BSPC,
       // ├──────────┼──────────┼──────────┼──────────┼──────────┼──────────┤                                 ├──────────┼──────────┼──────────┼──────────┼──────────┼──────────┤
       // │  ESC     │  A       │  S       │  D       │  F       │  G       │                                 │  H       │  J       │  K       │  L       │  ;       │  ENT     │
           KC_ESC,    KC_A,      KC_S,      KC_D,      KC_F,      KC_G,                                        KC_H,      KC_J,      KC_K,      KC_L,      KC_SCLN,   KC_ENT,
       // ├──────────┼──────────┼──────────┼──────────┼──────────┼──────────┤                                 ├──────────┼──────────┼──────────┼──────────┼──────────┼──────────┤
       // │  WIN     │  Z       │  X       │  C       │  V       │  B       │                                 │  N       │  M       │  ,       │  .       │  /       │  PANIC   │
           KC_LWIN,   KC_Z,      KC_X,      KC_C,      KC_V,      KC_B,                                        KC_N,      KC_M,      KC_COMM,   KC_DOT,    KC_SLSH,   PANIC,
       // └──────────┴──────────┴──────────┼──────────┼──────────┼──────────┼──────────┐           ┌──────────┼──────────┼──────────┼──────────┼──────────┴──────────┴──────────┘
       //                                  │  CTRL    │  NUMSYM  │  SPC     │  MOUSE   │           │  MUTE    │  SHIFT   │   ALT    │  NAV     │
                                           OSM(MOD_LCTL),OSL(2),MEH_T(KC_SPC),TG(7),                KC_MUTE,OSM(MOD_LSFT),OSM(MOD_LALT),MO(4)
       //                                  └──────────┴──────────┴──────────┴──────────┘           └──────────┴──────────┴──────────┴──────────┘
    ),
    [1] = LAYOUT_split_3x6_4( // Num layer
       // ┌──────────┬──────────┬──────────┬──────────┬──────────┬──────────┐                                 ┌──────────┬──────────┬──────────┬──────────┬──────────┬──────────┐
       // │          │  Q       │  W       │  E       │  R       │  T       │                                 │  Y       │  7       │  8       │  9       │  P       │          │
           KC_TRNS,   KC_Q,      KC_W,      KC_E,      KC_R,      KC_T,                                        KC_Y,      KC_U,      KC_I,      KC_O,      KC_P,      KC_TRNS,
       // ├──────────┼──────────┼──────────┼──────────┼──────────┼──────────┤                                 ├──────────┼──────────┼──────────┼──────────┼──────────┼──────────┤
       // │          │  A       │  S       │  D       │  F       │  G       │                                 │  H       │  4       │  5       │  6       │  ;       │          │
           KC_TRNS,   KC_A,      KC_S,      KC_D,      KC_F,      KC_G,                                        KC_H,      KC_J,      KC_K,      KC_L,      KC_SCLN,   KC_TRNS,
       // ├──────────┼──────────┼──────────┼──────────┼──────────┼──────────┤                                 ├──────────┼──────────┼──────────┼──────────┼──────────┼──────────┤
       // │  SHIFT   │  Z       │  X       │  C       │  V       │  B       │                                 │  0       │  1       │  2       │  3       │  /       │          │
           KC_LSFT,   KC_Z,      KC_X,      KC_C,      KC_V,      KC_B,                                        KC_N,      KC_M,      KC_COMM,   KC_DOT,    KC_SLSH,   KC_TRNS,
       // └──────────┴──────────┴──────────┼──────────┼──────────┼──────────┼──────────┐           ┌──────────┼──────────┼──────────┼──────────┼──────────┴──────────┴──────────┘
       //                                  │  CTRL    │  NUMSYM  │  SPC     │   MUTE   │           │  MUTE    │  SHIFT   │   ALT    │  NAV     │
                                            KC_LCTL,   MO(2),     KC_SPC,    KC_MUTE,               KC_MUTE,    KC_LSFT,   KC_LALT,  MO(4)
       //                                  └──────────┴──────────┴──────────┴──────────┘           └──────────┴──────────┴──────────┴──────────┘
    ),
};

// ROTARY ENCODER KEYMAP FOR LEFT AND RIGHT ENCODERS

#if defined(ENCODER_MAP_ENABLE)
const uint16_t PROGMEM encoder_map[][NUM_ENCODERS][NUM_DIRECTIONS] = {
    [0] =   { ENCODER_CCW_CW(TAB_L, TAB_R), ENCODER_CCW_CW(KC_VOLU, KC_VOLD)  },  // TAB LEFT RIGHT || VOL UP DOWN
    [1] =   { ENCODER_CCW_CW(KC_VOLD, KC_VOLU), ENCODER_CCW_CW(KC_VOLU, KC_VOLD)  },  // VOL DOWN UP || VOL UP DOWN
    [2] =  { ENCODER_CCW_CW(TAB_L, TAB_R), ENCODER_CCW_CW(KC_LEFT, KC_RIGHT)  }, // TAB LEFT RIGHT || ARROW LEFT RIGHT
    [3] =  { ENCODER_CCW_CW(TAB_L, TAB_R), ENCODER_CCW_CW(KC_LEFT, KC_RIGHT)  }, // TAB LEFT RIGHT || ARROW LEFT RIGHT
    [4] =  { ENCODER_CCW_CW(TAB_L, TAB_R), ENCODER_CCW_CW(KC_VOLU, KC_VOLD)  }, // TAB LEFT RIGHT || VOL UP DOWN
    [5] = { ENCODER_CCW_CW(TAB_L, TAB_R), ENCODER_CCW_CW(KC_VOLU, KC_VOLD) }, // TAB LEFT RIGHT || VOL UP VOL DOWN
    [6] = { ENCODER_CCW_CW(KC_BRID, 	KC_BRIU), ENCODER_CCW_CW(KC_VOLU, KC_VOLD) }, // BRIGHTNESS DOWN UP || VOL UP VOL DOWN
    [7] =  { ENCODER_CCW_CW(KC_MS_WH_DOWN, KC_MS_WH_UP), ENCODER_CCW_CW(KC_MS_WH_UP, KC_MS_WH_DOWN)  }, // SCROLL DOWN UP|| SCROLL UP DOWN
    [8] =   { ENCODER_CCW_CW(KC_NO, KC_NO), ENCODER_CCW_CW(KC_NO, KC_NO)  },  // DEBUG LAYER, DO NOTHING
};
#endif

bool is_oneshot_cancel_key(uint16_t keycode) {
    switch (keycode) {
    case PANIC:
        return true;
    default:
        return false;
    }
}

bool is_oneshot_ignored_key(uint16_t keycode) {
    switch (keycode) {
    case LA_SYM:
    case LA_NAV:
    case KC_LSFT:
    case OS_SHFT:
    case OS_CTRL:
    case OS_ALT:
    case OS_GUI:
        return true;
    default:
        return false;
    }
}

oneshot_state os_shft_state = os_up_unqueued;
oneshot_state os_ctrl_state = os_up_unqueued;
oneshot_state os_alt_state = os_up_unqueued;
oneshot_state os_gui_state = os_up_unqueued;

bool process_record_user(uint16_t keycode, keyrecord_t *record) {
    update_oneshot(
        &os_shft_state, KC_LSFT, OS_SHFT,
        keycode, record
    );
    update_oneshot(
        &os_ctrl_state, KC_LCTL, OS_CTRL,
        keycode, record
    );
    update_oneshot(
        &os_alt_state, KC_LALT, OS_ALT,
        keycode, record
    );
    update_oneshot(
        &os_gui_state, KC_LGUI, OS_GUI,
        keycode, record
    );

    return true;
}
