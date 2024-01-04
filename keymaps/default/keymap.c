// Copyright 2023 QMK
// SPDX-License-Identifier: GPL-2.0-or-later

#include QMK_KEYBOARD_H


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

bool is_shift_select_active = false;
uint16_t shift_select_timer = 0;
bool is_swap_LR_for_DNUP = false;

bool is_debug_active = false;
uint16_t debug_timer = 0;

void matrix_scan_user(void) {

  if (is_alt_tab_active) { //ALT TAB Encoder Timer
    if (timer_elapsed(alt_tab_timer) > 1000) {
      unregister_code(KC_LALT);
      unregister_code(KC_LSFT);
      is_alt_tab_active = false;
      is_alt_shift_tab_active = false;
    }
  }

  if (is_shift_select_active) { //SHIFT SELECTION Encoder Timer
    if (timer_elapsed(shift_select_timer) > 1000) {
      unregister_code(KC_LSFT);
      is_shift_select_active = false;
    }
  }

};

layer_state_t layer_state_set_kb(layer_state_t state) { // Run every time a layer is updated
  if (IS_LAYER_OFF_STATE(state, 2) && IS_LAYER_ON_STATE(state, 3)) {
    layer_off(3);
  }
  return state;
}

// COMBOS
const uint16_t PROGMEM MEDIA_LAYER[] = {OSL(2), MO(4), COMBO_END}; // MEDIA LAYER
const uint16_t PROGMEM DEBUG_LAYER[] = {OSM(MOD_LCTL), OSL(2), MEH_T(KC_SPC), OSM(MOD_LSFT), OSM(MOD_LALT), MO(4), COMBO_END}; // DEBUG LAYER, HIT ALL THUMB KEYS
const uint16_t PROGMEM CTRL_BACK_SPC[] = {KC_BSPC, KC_UP, COMBO_END};
const uint16_t PROGMEM CTRL_FWD_DEL[] = {KC_DEL, KC_UP, COMBO_END};

combo_t key_combos[] = {
    COMBO(MEDIA_LAYER, TG(6)), // Activate media layer
    COMBO(DEBUG_LAYER, TG(8)), // Activate debug layer
    COMBO(CTRL_BACK_SPC, C(KC_BSPC)), // CTRL backspace
    COMBO(CTRL_FWD_DEL, C(KC_DEL)), // CTRL forward delete
};

// DEFINING CUSTOM KEYCODES
enum custom_keycodes {
  PANIC = SAFE_RANGE,  // Panic button to turn off all layers and mods
  TabR,                // Tabs to right window
  TabL,                // Tabs to left window
  ShiftSel,            // Press to hold shift and start selecting with the arrow key
  L_DN,                // Special up for multi-function
  R_UP,                // Special up for multi-function
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


    case TabR: // Tab to the right window
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

    case TabL: //Tab to the left window
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

    case ShiftSel: //Press to hold shift and start selecting with the arrow key (Primarily for knob)
      if (record->event.pressed) {

        if (!is_shift_select_active) {
          is_shift_select_active = true;
          register_code(KC_LSFT);

        }
        shift_select_timer = timer_read();
      }
      return false;

    case L_DN:
      if (record->event.pressed) {

        if (!is_shift_select_active) {
          is_shift_select_active = true;
          register_code(KC_LSFT);

        }
        shift_select_timer = timer_read();
        if (is_swap_LR_for_DNUP) {
          tap_code(KC_DOWN);
        } else {
          tap_code(KC_LEFT);
        }
      }
      return false;

    case R_UP:
      if (record->event.pressed) {

        if (!is_shift_select_active) {
          is_shift_select_active = true;
          register_code(KC_LSFT);

        }
        shift_select_timer = timer_read();
        if (is_swap_LR_for_DNUP) {
          tap_code(KC_UP);
        } else {
          tap_code(KC_RIGHT);
        }

      }
      return false;


    case LT(0,KC_EQL): // Tap is EQUALS, Hold is SWAP left/right with down/up
      if (!record->tap.count && record->event.pressed) {
        is_swap_LR_for_DNUP = true;
      } else {
        is_swap_LR_for_DNUP = false;
      }
      return false;
    case KC_MYCM: //The following 5 lines turn off the MEDIA layer when pressed
      if (record->event.pressed) {
        layer_off(6);
        }
        return true;
    case KC_CALC:
      if (record->event.pressed) {
        layer_off(6);
        }
        return true;
    case KC_WSCH:
      if (record->event.pressed) {
        layer_off(6);
        }
        return true;
    case KC_MAIL:
      if (record->event.pressed) {
        layer_off(6);
        }
        return true;
    case KC_CPNL:
      if (record->event.pressed) {
        layer_off(6);
        }
        return true;
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
    [0] = LAYOUT_split_3x6_4( // Base layer 0
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
    [1] = LAYOUT_split_3x6_4( // Game layer 1
       // ┌──────────┬──────────┬──────────┬──────────┬──────────┬──────────┐                                 ┌──────────┬──────────┬──────────┬──────────┬──────────┬──────────┐
       // │          │  Q       │  W       │  E       │  R       │  T       │                                 │  Y       │  U       │  I       │  O       │  P       │          │
           KC_TRNS,   KC_Q,      KC_W,      KC_E,      KC_R,      KC_T,                                        KC_Y,      KC_U,      KC_I,      KC_O,      KC_P,      KC_TRNS,
       // ├──────────┼──────────┼──────────┼──────────┼──────────┼──────────┤                                 ├──────────┼──────────┼──────────┼──────────┼──────────┼──────────┤
       // │          │  A       │  S       │  D       │  F       │  G       │                                 │  H       │  J       │  K       │  L       │  ;       │          │
           KC_TRNS,   KC_A,      KC_S,      KC_D,      KC_F,      KC_G,                                        KC_H,      KC_J,      KC_K,      KC_L,      KC_SCLN,   KC_TRNS,
       // ├──────────┼──────────┼──────────┼──────────┼──────────┼──────────┤                                 ├──────────┼──────────┼──────────┼──────────┼──────────┼──────────┤
       // │  SHIFT   │  Z       │  X       │  C       │  V       │  B       │                                 │  N       │  M       │  ,       │  .       │  /       │          │
           KC_LSFT,   KC_Z,      KC_X,      KC_C,      KC_V,      KC_B,                                        KC_N,      KC_M,      KC_COMM,   KC_DOT,    KC_SLSH,   KC_TRNS,
       // └──────────┴──────────┴──────────┼──────────┼──────────┼──────────┼──────────┐           ┌──────────┼──────────┼──────────┼──────────┼──────────┴──────────┴──────────┘
       //                                  │  CTRL    │  NUMSYM  │  SPC     │   MUTE   │           │  MUTE    │  SHIFT   │   ALT    │  NAV     │
                                            KC_LCTL,   MO(2),     KC_SPC,    KC_MUTE,               KC_MUTE,    KC_LSFT,   KC_LALT,  MO(4)
       //                                  └──────────┴──────────┴──────────┴──────────┘           └──────────┴──────────┴──────────┴──────────┘
    ),
    [2] = LAYOUT_split_3x6_4( // Num and Symbols Layer 2
       // ┌──────────┬──────────┬──────────┬──────────┬──────────┬──────────┐                                 ┌──────────┬──────────┬──────────┬──────────┬──────────┬──────────┐
       // │          │  1 !     │  2 @     │  3 #     │  4 $     │  5 %     │                                 │  6 ^     │  7 &     │  8 *     │  9 (     │  0 )     │          │
           KC_TRNS,   KC_1,      KC_2,      KC_3,      KC_4,      KC_5,                                        KC_6,      KC_7,      KC_8,      KC_9,      KC_0,      KC_TRNS,
       // ├──────────┼──────────┼──────────┼──────────┼──────────┼──────────┤                                 ├──────────┼──────────┼──────────┼──────────┼──────────┼──────────┤
       // │ NUMB TOG │  `       │  (       │  )       │  '       │  =       │                                 │  \       │  -       │  [       │  ]       │  ;       │          │
          TG(3),      KC_GRV,LT(5,KC_LPRN), KC_RPRN,   KC_QUOT,   LT(0,KC_EQL),                                KC_BSLS,   KC_MINS,   KC_LBRC, LT(5,KC_RBRC), KC_SCLN, KC_TRNS,
       // ├──────────┼──────────┼──────────┼──────────┼──────────┼──────────┤                                 ├──────────┼──────────┼──────────┼──────────┼──────────┼──────────┤
       // │   CTRL   │  ~       │  <       │  >       │  "       │  +       │                                 │  |       │  _       │  {       │  }       │  :       │          │
       OSM(MOD_LCTL), KC_TILD,   KC_LT,     KC_GT,     KC_DQUO,   KC_PLUS,                                     KC_PIPE,   KC_UNDS,   KC_LCBR,   KC_RCBR,   KC_COLN,   KC_TRNS,
       // └──────────┴──────────┴──────────┼──────────┼──────────┼──────────┼──────────┐           ┌──────────┼──────────┼──────────┼──────────┼──────────┴──────────┴──────────┘
       //                                  │          │   HELD   │          │          │           │ SFT SLCT │          │          │  NAV     │
                                            KC_TRNS,   KC_TRNS,   KC_TRNS,   KC_TRNS,               ShiftSel,  KC_TRNS,   KC_TRNS,   KC_TRNS
       //                                  └──────────┴──────────┴──────────┴──────────┘           └──────────┴──────────┴──────────┴──────────┘
    ),
    [3] = LAYOUT_split_3x6_4( // Numpad Layer 3
       // ┌──────────┬──────────┬──────────┬──────────┬──────────┬──────────┐                                 ┌──────────┬──────────┬──────────┬──────────┬──────────┬──────────┐
       // │ NUM LOCK │  ;       │  [       │  ]       │  -       │  \       │                                 │  *       │  7       │  8       │  9       │  -       │          │
           KC_NUM,    KC_SCLN,   KC_LBRC,   KC_RBRC,   KC_MINS,   KC_BSLS,                                     KC_PAST,   KC_P7,     KC_P8,     KC_P9,     KC_PMNS,   KC_TRNS,
       // ├──────────┼──────────┼──────────┼──────────┼──────────┼──────────┤                                 ├──────────┼──────────┼──────────┼──────────┼──────────┼──────────┤
       // │          │          │          │          │          │          │                                 │  /       │  4       │  5       │  6       │  +       │          │
           KC_TRNS,   KC_TRNS,   KC_TRNS,   KC_TRNS,   KC_TRNS,   KC_TRNS,                                     KC_PSLS,   KC_P4,     KC_P5,     KC_P6,     KC_PPLS,   KC_TRNS,
       // ├──────────┼──────────┼──────────┼──────────┼──────────┼──────────┤                                 ├──────────┼──────────┼──────────┼──────────┼──────────┼──────────┤
       // │          │          │          │          │          │  ^       │                                 │  0       │  1       │  2       │  3       │  .       │          │
           KC_TRNS,   KC_TRNS,   KC_TRNS,   KC_TRNS,   KC_TRNS,   KC_6,                                        KC_P0,     KC_P1,     KC_P2,     KC_P3,     KC_PDOT,   KC_TRNS,
       // └──────────┴──────────┴──────────┼──────────┼──────────┼──────────┼──────────┐           ┌──────────┼──────────┼──────────┼──────────┼──────────┴──────────┴──────────┘
       //                                  │          │   HELD   │          │          │           │          │          │          │  NAV     │
                                            KC_TRNS,   KC_TRNS,   KC_TRNS,   KC_TRNS,               KC_TRNS,   KC_TRNS,   KC_TRNS,   KC_TRNS
       //                                  └──────────┴──────────┴──────────┴──────────┘           └──────────┴──────────┴──────────┴──────────┘
    ),
    [4] = LAYOUT_split_3x6_4( // Navigation layer 4
       // ┌──────────┬──────────┬──────────┬──────────┬──────────┬──────────┐                                 ┌──────────┬──────────┬──────────┬──────────┬──────────┬──────────┐
       // │  Tab     │ BR FAV   │ BR BWD   │ BR REFR  │ BR FWD   │  PG UP   │                                 │  XXXXXX  │ BACK SPC │  ↑       │ FWD DEL  │  XXXXXX  │  XXXXXX  │
           KC_TAB,    KC_WFAV,   KC_WBAK,   KC_WREF,   KC_WFWD,   KC_PGUP,                                     KC_NO,     KC_BSPC,   KC_UP,     KC_DEL,      KC_NO,     KC_NO,
       // ├──────────┼──────────┼──────────┼──────────┼──────────┼──────────┤                                 ├──────────┼──────────┼──────────┼──────────┼──────────┼──────────┤
       // │  ESC     │  XXXXXX  │  HOME    │ CTX MENU │  END     │  PG DN   │                                 │  INSERT  │  ←       │  ↓       │  →       │  XXXXXX  │  XXXXXX  │
           KC_ESC,    KC_NO,     KC_HOME,   KC_APP,    KC_END,    KC_PGDN,                                     KC_INS,    KC_LEFT,   KC_DOWN,   KC_RIGHT,    KC_NO,     KC_NO,
       // ├──────────┼──────────┼──────────┼──────────┼──────────┼──────────┤                                 ├──────────┼──────────┼──────────┼──────────┼──────────┼──────────┤
       // │  WIN     │  XXXXXX  │  XXXXXX  │  XXXXXX  │  XXXXXX  │  XXXXXX  │                                 │  XXXXXX  │ RETURN   │  XXXXXX  │  XXXXXX  │  XXXXXX  │          │
           KC_LWIN,   KC_NO,     KC_NO,       KC_NO,     KC_NO,     KC_NO,                                     KC_NO,     KC_ENT,      KC_NO,     KC_NO,     KC_NO,   KC_TRNS,
       // └──────────┴──────────┴──────────┼──────────┼──────────┼──────────┼──────────┐           ┌──────────┼──────────┼──────────┼──────────┼──────────┴──────────┴──────────┘
       //                                  │          │   ALT    │  SHIFT   │          │           │  MUTE    │  XXXXXX  │  XXXXXX  │   HELD   │
                                            KC_TRNS, OSM(MOD_LALT),OSM(MOD_LSFT),KC_TRNS,           KC_MUTE,   KC_NO,     KC_NO,     KC_TRNS
       //                                  └──────────┴──────────┴──────────┴──────────┘           └──────────┴──────────┴──────────┴──────────┘
    ),
    [5] = LAYOUT_split_3x6_4( // Function layer 5
       // ┌──────────┬──────────┬──────────┬──────────┬──────────┬──────────┐                                 ┌──────────┬──────────┬──────────┬──────────┬──────────┬──────────┐
       // │  XXXXXX  │  F1      │  F2      │  F3      │  F4      │  F5      │                                 │  F6      │  F7      │  F8      │  F9      │  F10     │  XXXXXX  │
           KC_NO,     KC_F1,     KC_F2,     KC_F3,     KC_F4,     KC_F5,                                       KC_F6,     KC_F7,     KC_F8,     KC_F9,     KC_F10,    KC_NO,
       // ├──────────┼──────────┼──────────┼──────────┼──────────┼──────────┤                                 ├──────────┼──────────┼──────────┼──────────┼──────────┼──────────┤
       // │  XXXXXX  │  XXXXXX  │   HELD   │  XXXXXX  │  XXXXXX  │  XXXXXX  │                                 │  F11     │  F12     │  XXXXXX  │   HELD   │  XXXXXX  │  XXXXXX  │
           KC_NO,     KC_NO,     KC_TRNS,   KC_NO,     KC_NO,     KC_NO,                                       KC_F11,    KC_F12,    KC_NO,     KC_TRNS,   KC_NO,     KC_NO,
       // ├──────────┼──────────┼──────────┼──────────┼──────────┼──────────┤                                 ├──────────┼──────────┼──────────┼──────────┼──────────┼──────────┤
       // │  WIN     │  XXXXXX  │  XXXXXX  │  XXXXXX  │  XXXXXX  │  XXXXXX  │                                 │  XXXXXX  │  XXXXXX  │  XXXXXX  │  XXXXXX  │  XXXXXX  │          │
           KC_LWIN,   KC_NO,     KC_NO,     KC_NO,     KC_NO,     KC_NO,                                       KC_NO,     KC_NO,     KC_NO,     KC_NO,     KC_NO,     KC_TRNS,
       // └──────────┴──────────┴──────────┼──────────┼──────────┼──────────┼──────────┐           ┌──────────┼──────────┼──────────┼──────────┼──────────┴──────────┴──────────┘
       //                                  │  XXXXXX  │   HELD   │  XXXXXX  │          │           │  XXXXXX  │          │          │   CTRL   │
                                            KC_NO,     KC_TRNS,   KC_NO,     KC_TRNS,               KC_NO,     KC_TRNS,   KC_TRNS,  OSM(MOD_LCTL)
       //                                  └──────────┴──────────┴──────────┴──────────┘           └──────────┴──────────┴──────────┴──────────┘
    ),
    [6] = LAYOUT_split_3x6_4( // Media layer and Apps 6
       // ┌──────────┬──────────┬──────────┬──────────┬──────────┬──────────┐                                 ┌──────────┬──────────┬──────────┬──────────┬──────────┬──────────┐
       // │  XXXXXX  │  XXXXXX  │  XXXXXX  │  XXXXXX  │  XXXXXX  │  XXXXXX  │                                 │   GAME   │  XXXXXX  │  XXXXXX  │  XXXXXX  │  XXXXXX  │  XXXXXX  │
           KC_NO,     KC_NO,     KC_NO,      KC_NO,    KC_NO,     KC_NO,                                       TO(1),     KC_NO,     KC_NO,     KC_NO,     KC_NO,     KC_NO,
       // ├──────────┼──────────┼──────────┼──────────┼──────────┼──────────┤                                 ├──────────┼──────────┼──────────┼──────────┼──────────┼──────────┤
       // │  ESC     │  XXXXXX  │   PREV   │  STOP    │   NEXT   │  XXXXXX  │                                 │ THIS PC  │   CALC   │  BROWSE  │   MAIL   │  CTRL P  │  XXXXXX  │
           KC_ESC,    KC_NO,     KC_MPRV,   KC_MSTP,   KC_MNXT,   KC_NO,                                       KC_MYCM,   KC_CALC,   KC_WSCH,   KC_MAIL,   KC_CPNL,   KC_NO,
       // ├──────────┼──────────┼──────────┼──────────┼──────────┼──────────┤                                 ├──────────┼──────────┼──────────┼──────────┼──────────┼──────────┤
       // │  WIN     │  XXXXXX  │  REWIND  │  XXXXXX  │ FAST FWD │  XXXXXX  │                                 │  XXXXXX  │  XXXXXX  │  XXXXXX  │  XXXXXX  │  XXXXXX  │          │
           KC_LWIN,   KC_NO,     KC_MRWD,   KC_NO,     KC_MFFD,     KC_NO,                                       KC_NO,     KC_NO,     KC_NO,     KC_NO,     KC_NO,   KC_TRNS,
       // └──────────┴──────────┴──────────┼──────────┼──────────┼──────────┼──────────┐           ┌──────────┼──────────┼──────────┼──────────┼──────────┴──────────┴──────────┘
       //                                  │  XXXXXX  │   HELD   │  PAUSE   │          │           │  MUTE    │  XXXXXX  │ XXXXXX   │   HELD   │
                                            KC_NO,     KC_TRNS,   KC_MPLY,   KC_TRNS,               KC_MUTE,   KC_NO,     KC_NO,     KC_TRNS
       //                                  └──────────┴──────────┴──────────┴──────────┘           └──────────┴──────────┴──────────┴──────────┘
    ),
    [7] = LAYOUT_split_3x6_4( // Mouse Layer 7
       // ┌──────────┬──────────┬──────────┬──────────┬──────────┬──────────┐                                 ┌──────────┬──────────┬──────────┬──────────┬──────────┬──────────┐
       // │  XXXXXX  │   CTRL   │ BACKWARD │  M↑      │ FORWARDS │  XXXXXX  │                                 │  XXXXXX  │ BACKWARD │  M↑      │ FORWARDS │   CTRL   │  XXXXXX  │
           KC_NO,   OSM(MOD_LCTL),KC_BTN4,  KC_MS_U,   KC_BTN5,   KC_NO,                                       KC_NO,     KC_BTN4,   KC_MS_U,   KC_BTN5, OSM(MOD_LCTL), KC_NO,
       // ├──────────┼──────────┼──────────┼──────────┼──────────┼──────────┤                                 ├──────────┼──────────┼──────────┼──────────┼──────────┼──────────┤
       // │  ESC     │   ALT    │  M←      │  M↓      │  M→      │  XXXXXX  │                                 │  XXXXXX  │  M←      │  M↓      │  M→      │   ALT    │  XXXXXX  │
           KC_ESC,  OSM(MOD_LALT),KC_MS_L,  KC_MS_D,   KC_MS_R,   KC_NO,                                       KC_NO,     KC_MS_L,   KC_MS_D,   KC_MS_R, OSM(MOD_LALT), KC_NO,
       // ├──────────┼──────────┼──────────┼──────────┼──────────┼──────────┤                                 ├──────────┼──────────┼──────────┼──────────┼──────────┼──────────┤
       // │  WIN     │  SHIFT   │ ACCEL 3  │ ACCEL 2  │ ACCEL 1  │ M CLICK  │                                 │ M CLICK  │ ACCEL 1  │ ACCEL 2  │ ACCEL 3  │  SHIFT   │          │
           KC_LWIN, OSM(MOD_LSFT),KC_ACL2,  KC_ACL1,   KC_ACL0,   KC_BTN3,                                     KC_BTN3,   KC_ACL0,   KC_ACL1,   KC_ACL2, OSM(MOD_LSFT), KC_TRNS,
       // └──────────┴──────────┴──────────┼──────────┼──────────┼──────────┼──────────┐           ┌──────────┼──────────┼──────────┼──────────┼──────────┴──────────┴──────────┘
       //                                  │ PNT SCN  │ R CLICK  │ L CLICK  │          │           │ M CLICK  │ L CLICK  │ R CLICK  │ PNT SCN  │
                                            KC_PSCR,   KC_BTN2,   KC_BTN1,   KC_TRNS,               KC_BTN3,   KC_BTN1,   KC_BTN2,   KC_PSCR
       //                                  └──────────┴──────────┴──────────┴──────────┘           └──────────┴──────────┴──────────┴──────────┘
    ),
    [8] = LAYOUT_split_3x6_4( // DEBUGGING LAYER!!! NOT MEANT FOR REGULAR USE!!! 8
       // ┌──────────┬──────────┬──────────┬──────────┬──────────┬──────────┐                                 ┌──────────┬──────────┬──────────┬──────────┬──────────┬──────────┐
       // │  XXXXXX  │  XXXXXX  │  XXXXXX  │  XXXXXX  │  XXXXXX  │  XXXXXX  │                                 │  XXXXXX  │  XXXXXX  │  XXXXXX  │  XXXXXX  │  XXXXXX  │  XXXXXX  │
           KC_NO,     KC_NO,     KC_NO,     KC_NO,     KC_NO,     KC_NO,                                       KC_NO,     KC_NO,     KC_NO,     KC_NO,     KC_NO,     KC_NO,
       // ├──────────┼──────────┼──────────┼──────────┼──────────┼──────────┤                                 ├──────────┼──────────┼──────────┼──────────┼──────────┼──────────┤
       // │  XXXXXX  │  XXXXXX  │  XXXXXX  │ QK RESET │ QK BOOT  │  XXXXXX  │                                 │  XXXXXX  │ PC SLEEP │ PWR DOWN │  XXXXXX  │  XXXXXX  │  XXXXXX  │
           KC_NO,     KC_NO,     KC_NO,     QK_RBT,    QK_BOOT,   KC_NO,                                       KC_NO,     KC_SLEP,   KC_PWR,    KC_NO,     KC_NO,     KC_NO,
       // ├──────────┼──────────┼──────────┼──────────┼──────────┼──────────┤                                 ├──────────┼──────────┼──────────┼──────────┼──────────┼──────────┤
       // │  XXXXXX  │  XXXXXX  │  XXXXXX  │  XXXXXX  │  XXXXXX  │  XXXXXX  │                                 │  XXXXXX  │  XXXXXX  │  XXXXXX  │  XXXXXX  │  XXXXXX  │          │
           KC_NO,     KC_NO,     KC_NO,     KC_NO,     KC_NO,     KC_NO,                                       KC_NO,     KC_NO,     KC_NO,     KC_NO,     KC_NO,     KC_TRNS,
       // └──────────┴──────────┴──────────┼──────────┼──────────┼──────────┼──────────┐           ┌──────────┼──────────┼──────────┼──────────┼──────────┴──────────┴──────────┘
       //                                  │  XXXXXX  │  XXXXXX  │  XXXXXX  │  XXXXXX  │           │  XXXXXX  │  XXXXXX  │  XXXXXX  │  XXXXXX  │
                                            KC_NO,     KC_NO,     KC_NO,     KC_NO,                 KC_NO,     KC_NO,     KC_NO,     KC_NO
       //                                  └──────────┴──────────┴──────────┴──────────┘           └──────────┴──────────┴──────────┴──────────┘
    ),
};

// ROTARY ENCODER KEYMAP FOR LEFT AND RIGHT ENCODERS

#if defined(ENCODER_MAP_ENABLE)
const uint16_t PROGMEM encoder_map[][NUM_ENCODERS][NUM_DIRECTIONS] = {
    [0] =   { ENCODER_CCW_CW(TabL, TabR), ENCODER_CCW_CW(KC_VOLU, KC_VOLD)  },  // TAB LEFT RIGHT || VOL UP DOWN
    [1] =   { ENCODER_CCW_CW(KC_VOLD, KC_VOLU), ENCODER_CCW_CW(KC_VOLU, KC_VOLD)  },  // VOL DOWN UP || VOL UP DOWN
    [2] =  { ENCODER_CCW_CW(TabL, TabR), ENCODER_CCW_CW(KC_LEFT, KC_RIGHT)  }, // TAB LEFT RIGHT || ARROW LEFT RIGHT
    [3] =  { ENCODER_CCW_CW(TabL, TabR), ENCODER_CCW_CW(KC_LEFT, KC_RIGHT)  }, // TAB LEFT RIGHT || ARROW LEFT RIGHT
    [4] =  { ENCODER_CCW_CW(TabL, TabR), ENCODER_CCW_CW(KC_VOLU, KC_VOLD)  }, // TAB LEFT RIGHT || VOL UP DOWN
    [5] = { ENCODER_CCW_CW(TabL, TabR), ENCODER_CCW_CW(KC_VOLU, KC_VOLD) }, // TAB LEFT RIGHT || VOL UP VOL DOWN
    [6] = { ENCODER_CCW_CW(KC_BRID, 	KC_BRIU), ENCODER_CCW_CW(KC_VOLU, KC_VOLD) }, // BRIGHTNESS DOWN UP || VOL UP VOL DOWN
    [7] =  { ENCODER_CCW_CW(KC_MS_WH_DOWN, KC_MS_WH_UP), ENCODER_CCW_CW(KC_MS_WH_UP, KC_MS_WH_DOWN)  }, // SCROLL DOWN UP|| SCROLL UP DOWN
    [8] =   { ENCODER_CCW_CW(KC_NO, KC_NO), ENCODER_CCW_CW(KC_NO, KC_NO)  },  // DEBUG LAYER, DO NOTHING
};
#endif
