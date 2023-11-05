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

// DEFINING LAYER KEYCODES
#define LA_SYM MO(1) // Symbols Layer
#define LA_NUM MO(2) // Numbers Layer
#define LA_NAV MO(3) // Navigation Layer
#define LA_FUNC TO(4) // Function Layer
#define LA_MSE TO(5) // Left Mouse and Media Layer
#define LA_G1 TO(6) // Game Layer 1
#define LA_G2 MO(7) // Game Layer 2
#define LA_DEV TO(8) // Developer Layer
#define MUTE MEH(KC_F13) // Toggle Mute
#define DEAFEN MEH(KC_F14) // Toggle Deafen

// Oneshot Mod Stuff
oneshot_state os_shft_state = os_up_unqueued;
oneshot_state os_ctrl_state = os_up_unqueued;
oneshot_state os_alt_state = os_up_unqueued;
oneshot_state os_gui_state = os_up_unqueued;

bool os_shft_tog_state = false;
bool os_ctrl_tog_state = false;
bool os_alt_tog_state = false;
bool os_gui_tog_state = false;
// ================================================

bool is_alt_tab_active = false; // ADD this near the begining of keymap.c 
bool is_alt_shift_tab_active = false;
uint16_t alt_tab_timer = 0;     // we will be using them soon.

// DEFINING CUSTOM KEYCODES
enum custom_keycodes {
  PANIC = SAFE_RANGE,  // Panic button to turn off all layers and mods
  TAB_R,               // Tabs to right window 
  TAB_L,               // Tabs to left window
  SNIP_T,              // Activate snipping tool
  OS_SHFT,             // Oneshot Shift
  OS_CTRL,             // Oneshot Ctrl
  OS_ALT,              // Oneshot Alt
  OS_GUI,              // Oneshot Windows

  //DEV CONFIRMATION BUTTONS (yes, i am paranoid)
 
  CONF1, CONF2, // Confirmation buttons
  QK_BOOT1, QK_BOOT2, QK_BOOT3, QK_BOOT4, // Boot
  QK_RESET1, QK_RESET2, QK_RESET3, QK_RESET4, // Reset
  PC_SLEEP1, PC_SLEEP2, PC_SLEEP3, PC_SLEEP4, // Sleep
  PC_POWER1, PC_POWER2, PC_POWER3, PC_POWER4, // Power
};

enum combos {
  COMB_DEBUG,
  COMB_BOOT,
  COMB_RESET,
  COMB_SLEEP,
  COMB_POWER,
  COMB_M3,
};

// COMBOS
const uint16_t PROGMEM DEBUG_LAYER[] = { LA_NAV, LA_NUM, KC_SPC, KC_ENT, OS_SHFT, LA_SYM,        COMBO_END}; // DEBUG LAYER, HIT ALL THUMB KEYS
const uint16_t PROGMEM BOOT[] = { QK_BOOT1, QK_BOOT2, QK_BOOT3, QK_BOOT4, CONF1, CONF2,          COMBO_END}; // BOOT LAYER
const uint16_t PROGMEM RESET[] = { QK_RESET1, QK_RESET2, QK_RESET3, QK_RESET4, CONF1, CONF2,     COMBO_END}; // RESET LAYER
const uint16_t PROGMEM SLEEP[] = { PC_SLEEP1, PC_SLEEP2, PC_SLEEP3, PC_SLEEP4, CONF1, CONF2,     COMBO_END}; // SLEEP LAYER
const uint16_t PROGMEM POWER[] = { PC_POWER1, PC_POWER2, PC_POWER3, PC_POWER4, CONF1, CONF2,     COMBO_END}; // POWER LAYER
const uint16_t PROGMEM MIDDLE_CLICK[] = { KC_WH_D, KC_MS_WH_UP, COMBO_END}; // M3 LAYER

combo_t key_combos[] = {
    [COMB_DEBUG] = COMBO(DEBUG_LAYER, KC_NO), // Activate debug layer
    [COMB_BOOT] = COMBO(BOOT, QK_BOOTLOADER), // Boot
    [COMB_RESET] = COMBO(BOOT, QK_REBOOT), // Reset
    [COMB_SLEEP] = COMBO(BOOT, KC_SYSTEM_SLEEP), // Sleep
    [COMB_POWER] = COMBO(BOOT, KC_SYSTEM_POWER), // Power
    [COMB_M3] = COMBO(MIDDLE_CLICK, KC_BTN3), // Middle Click
};

bool process_record_kb(uint16_t keycode, keyrecord_t *record) {
  
  update_oneshot(
      &os_shft_state, 
      os_shft_tog_state,
      KC_LSFT, 
      OS_SHFT,
      keycode, 
      record
  );
  update_oneshot(
      &os_ctrl_state, 
      os_ctrl_tog_state,
      KC_LCTL, 
      OS_CTRL,
      keycode, 
      record
  );
  update_oneshot(
      &os_alt_state, 
      os_alt_tog_state,
      KC_LALT, 
      OS_ALT,
      keycode, 
      record
  );
  update_oneshot(
      &os_gui_state, 
      os_gui_tog_state,
      KC_LGUI, 
      OS_GUI,
      keycode, 
      record
  );

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

    case TAB_L: // Tab to the left window
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

    case SNIP_T: // Snipping tool shortcuts
      if (record->event.pressed) {
        // Keybinds for snipping tool screenshot
        register_code(KC_LWIN);
        register_code(KC_LSFT);
        register_code(KC_S);
        unregister_code(KC_LWIN);
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
    [0] = LAYOUT_split_3x6_4( // Base layer
       // ┌──────────┬──────────┬──────────┬──────────┬──────────┬──────────┐                                 ┌──────────┬──────────┬──────────┬──────────┬──────────┬──────────┐
       // │  TAB     │  Q       │  W       │  E       │  R       │  T       │                                 │  Y       │  U       │  I       │  O       │  P       │  BSP     │
           KC_TAB,    KC_Q,      KC_W,      KC_E,      KC_R,      KC_T,                                        KC_Y,      KC_U,      KC_I,      KC_O,      KC_P,      KC_BSPC,
       // ├──────────┼──────────┼──────────┼──────────┼──────────┼──────────┤                                 ├──────────┼──────────┼──────────┼──────────┼──────────┼──────────┤
       // │  ESC     │  A       │  S       │  D       │  F       │  G       │                                 │  H       │  J       │  K       │  L       │  ; :     │  Menu    │
           KC_ESC,    KC_A,      KC_S,      KC_D,      KC_F,      KC_G,                                        KC_H,      KC_J,      KC_K,      KC_L,      KC_SCLN,   KC_APP,
       // ├──────────┼──────────┼──────────┼──────────┼──────────┼──────────┤                                 ├──────────┼──────────┼──────────┼──────────┼──────────┼──────────┤
       // │  WIN     │  Z       │  X       │  C       │  V       │  B       │                                 │  N       │  M       │  , <     │  . >     │  / ?     │  PANIC   │
           KC_LWIN,   KC_Z,      KC_X,      KC_C,      KC_V,      KC_B,                                        KC_N,      KC_M,      KC_COMM,   KC_DOT,    KC_SLSH,   PANIC,
       // └──────────┴──────────┴──────────┼──────────┼──────────┼──────────┼──────────┐           ┌──────────┼──────────┼──────────┼──────────┼──────────┴──────────┴──────────┘
       //                                  │  NAV     │  NUM     │  SPC     │  MOUSE   │           │  MUTE    │  Enter   │  Shift   │  SYM     │
                                            LA_NAV,    LA_NUM,    KC_SPC,    LA_MSE,                KC_MUTE,   KC_ENT,    OS_SHFT,   LA_SYM
       //                                  └──────────┴──────────┴──────────┴──────────┘           └──────────┴──────────┴──────────┴──────────┘
    ),
    [1] = LAYOUT_split_3x6_4( // Symbols layer
       // ┌──────────┬──────────┬──────────┬──────────┬──────────┬──────────┐                                 ┌──────────┬──────────┬──────────┬──────────┬──────────┬──────────┐
       // │          │  !       │  @       │  #       │  $       │  %       │                                 │  ^       │  &       │  *       │  \       │          │          │
           KC_NO,     KC_EXLM,   KC_AT,     KC_HASH,   KC_DLR,    KC_PERC,                                     KC_CIRC,  KC_AMPR,    KC_ASTR,   KC_BSLS,   KC_NO,     KC_NO,
       // ├──────────┼──────────┼──────────┼──────────┼──────────┼──────────┤                                 ├──────────┼──────────┼──────────┼──────────┼──────────┼──────────┤
       // │          │  ~       │  -       │  [       │  {       │  )       │                                 │  =       │  Ctrl    │  Shift   │  Alt     │  Gui     │          │
           KC_NO,     KC_TILD,   KC_PMNS,   KC_LBRC,   KC_LCBR,   KC_LPRN,                                     KC_NO,     OS_CTRL,   OS_SHFT,   OS_ALT,    OS_GUI,    KC_NO,
       // ├──────────┼──────────┼──────────┼──────────┼──────────┼──────────┤                                 ├──────────┼──────────┼──────────┼──────────┼──────────┼──────────┤
       // │          │  `       │  +       │  ]       │  {       │  )       │                                 │  '       │  "       │  |       │  ?       │  / ?     │  PANIC   │
           KC_NO,     KC_GRV,    KC_PPLS,   KC_RBRC,   KC_RCBR,   KC_RPRN,                                     KC_NO,     KC_DQT,    KC_PIPE,   KC_QUES,   KC_SLSH,   PANIC,
       // └──────────┴──────────┴──────────┼──────────┼──────────┼──────────┼──────────┐           ┌──────────┼──────────┼──────────┼──────────┼──────────┴──────────┴──────────┘
       //                                  │  _       │ Tab      │  SPC     │  MOUSE   │           │  MUTE    │          │          │  TRANS   │
                                            KC_UNDS,   KC_TAB,    KC_SPC,    LA_MSE,                KC_MUTE,    KC_NO,    KC_NO,     KC_TRNS
       //                                  └──────────┴──────────┴──────────┴──────────┘           └──────────┴──────────┴──────────┴──────────┘
    ),
    [2] = LAYOUT_split_3x6_4( // Numbers layer
       // ┌──────────┬──────────┬──────────┬──────────┬──────────┬──────────┐                                 ┌──────────┬──────────┬──────────┬──────────┬──────────┬──────────┐
       // │          │          │          │          │  , <     │  . >     │                                 │  - _     │  7 &     │  8 *     │  9 (     │          │          │
           KC_NO,     KC_NO,     KC_NO,     KC_NO,     KC_COMM,   KC_SLSH,                                     KC_MINS,   KC_7,      KC_8,      KC_9,      KC_NO,     KC_NO,
       // ├──────────┼──────────┼──────────┼──────────┼──────────┼──────────┤                                 ├──────────┼──────────┼──────────┼──────────┼──────────┼──────────┤
       // │          │  Gui     │  Alt     │  Shift   │  Ctrl    │          │                                 │  = +     │  4 $     │  5 %     │  6 ^     │          │          │
           KC_NO,     OS_GUI,    OS_ALT,    OS_SHFT,   OS_CTRL,   KC_NO,                                       KC_EQL,    KC_4,      KC_5,      KC_6,      OS_GUI,   KC_NO,
       // ├──────────┼──────────┼──────────┼──────────┼──────────┼──────────┤                                 ├──────────┼──────────┼──────────┼──────────┼──────────┼──────────┤
       // │          │          │          │          │  <       │    >     │                                 │  0 )     │  1 @     │  2 @     │  3 #     │  / ?     │  PANIC   │
           KC_NO,     KC_NO,     KC_NO,     KC_NO,     KC_LT,     KC_GT,                                       KC_NO,     KC_1,      KC_2,      KC_3,      KC_SLSH,   PANIC,
       // └──────────┴──────────┴──────────┼──────────┼──────────┼──────────┼──────────┐           ┌──────────┼──────────┼──────────┼──────────┼──────────┴──────────┴──────────┘
       //                                  │          │  TRANS   │          │  MOUSE   │           │  MUTE    │  Enter   │  FUNC    │          │
                                            KC_NO,     KC_TRNS,   KC_NO,     LA_MSE,                KC_MUTE,   KC_NO,     LA_FUNC,   KC_NO
       //                                  └──────────┴──────────┴──────────┴──────────┘           └──────────┴──────────┴──────────┴──────────┘
    ),
    [3] = LAYOUT_split_3x6_4( // Navigation layer
       // ┌──────────┬──────────┬──────────┬──────────┬──────────┬──────────┐                                 ┌──────────┬──────────┬──────────┬──────────┬──────────┬──────────┐
       // │          │          │          │  Deafen  │  Mute    │          │                                 │  PgUp    │  Home    │  ↑       │  End     │          │          │
           KC_NO,     KC_NO,     KC_NO,     DEAFEN,    MUTE,      KC_NO,                                       KC_PGUP,   KC_HOME,   KC_UP,     KC_END,    KC_NO,     KC_NO,
       // ├──────────┼──────────┼──────────┼──────────┼──────────┼──────────┤                                 ├──────────┼──────────┼──────────┼──────────┼──────────┼──────────┤
       // │          │  Gui     │  Alt     │  Shift   │  Ctrl    │          │                                 │  PgDn    │  ←       │  ↓       │  →       │          │          │
           KC_NO,     OS_GUI,    OS_ALT,    OS_SHFT,   OS_CTRL,   KC_NO,                                       KC_PGDN,   KC_LEFT,   KC_DOWN,   KC_RIGHT,  KC_NO,     KC_NO,
       // ├──────────┼──────────┼──────────┼──────────┼──────────┼──────────┤                                 ├──────────┼──────────┼──────────┼──────────┼──────────┼──────────┤
       // │          │          │          │  BR BWD  │  BR FWD  │          │                                 │  Insert  │  BSP     │  DEL     │          │          │  PANIC   │
           KC_NO,     KC_NO,     KC_NO,     KC_WBAK,   KC_WFWD,   KC_NO,                                       KC_INS,    KC_BSPC,   KC_DEL,    KC_NO,     KC_NO,     PANIC,
       // └──────────┴──────────┴──────────┼──────────┼──────────┼──────────┼──────────┐           ┌──────────┼──────────┼──────────┼──────────┼──────────┴──────────┴──────────┘
       //                                  │  TRANS   │          │          │  MOUSE   │           │  MOUSE   │  Esc     │  BSP     │  DEL     │
                                            KC_TRNS,   KC_NO,     KC_NO,     LA_MSE,                LA_MSE,    KC_ESC,    KC_BSPC,   KC_DEL
       //                                  └──────────┴──────────┴──────────┴──────────┘           └──────────┴──────────┴──────────┴──────────┘
    ),
    [4] = LAYOUT_split_3x6_4( // Function layer
       // ┌──────────┬──────────┬──────────┬──────────┬──────────┬──────────┐                                 ┌──────────┬──────────┬──────────┬──────────┬──────────┬──────────┐
       // │  F13     │  F14     │  F15     │  F16     │  F17     │  PrtScn  │                                 │  PrtScn  │  F7      │  F8      │  F9      │  F12     │          │
           KC_F13,    KC_F14,    KC_F15,    KC_F16,    KC_F17,    KC_PSCR,                                     KC_PSCR,   KC_F7,     KC_F8,     KC_F9,     KC_F12,    KC_NO,
       // ├──────────┼──────────┼──────────┼──────────┼──────────┼──────────┤                                 ├──────────┼──────────┼──────────┼──────────┼──────────┼──────────┤
       // │  F18     │  Gui     │  Alt     │  Shift   │  Ctrl    │  SnipT   │                                 │  SnipT   │  F4      |  F5      │  F6      │  F11     │          │
           KC_F18,    OS_GUI,    OS_ALT,    OS_SHFT,   OS_CTRL,   SNIP_T,                                      SNIP_T,    KC_F4,     KC_F5,     KC_F6,     KC_F11,    KC_NO,
       // ├──────────┼──────────┼──────────┼──────────┼──────────┼──────────┤                                 ├──────────┼──────────┼──────────┼──────────┼──────────┼──────────┤
       // │  F19     │  F20     │  F21     │  F22     │  F23     │  F24     │                                 │  PauseB  │  F1      │  F2      │  F3      │  F10     │  PANIC   │
           KC_F19,    KC_F20,    KC_F21,    KC_F22,    KC_F23,    KC_F24,                                      KC_PAUS,   KC_F1,     KC_F2,     KC_F3,     KC_F10,    PANIC,
       // └──────────┴──────────┴──────────┼──────────┼──────────┼──────────┼──────────┐           ┌──────────┼──────────┼──────────┼──────────┼──────────┴──────────┴──────────┘
       //                                  │          │  TRANS   │          │  MOUSE   │           │  MUTE    │          │  TRANS   │          │
                                            KC_NO,     KC_TRNS,   KC_NO,     LA_MSE,                KC_MUTE,   KC_NO,     KC_TRNS,   KC_NO
       //                                  └──────────┴──────────┴──────────┴──────────┘           └──────────┴──────────┴──────────┴──────────┘
    ),
    [5] = LAYOUT_split_3x6_4( // Left Mouse and Media layer
       // ┌──────────┬──────────┬──────────┬──────────┬──────────┬──────────┐                                 ┌──────────┬──────────┬──────────┬──────────┬──────────┬──────────┐
       // │  ScrDn   │  M↑      │  ScrUp   │  Deafen  │  Mute    │          │                                 │          │          │          │          │          │          │
           KC_WH_D,   KC_NO,     KC_WH_U,   DEAFEN,    MUTE,      KC_NO,                                       KC_NO,     KC_NO,     KC_NO,     KC_NO,     KC_NO,     KC_NO,
       // ├──────────┼──────────┼──────────┼──────────┼──────────┼──────────┤                                 ├──────────┼──────────┼──────────┼──────────┼──────────┼──────────┤
       // │  M←      │  M↓      │  M→      │  Prev    │  Next    │          │                                 │  GAME    │          │          │          │          │          │
           KC_NO,     KC_NO,     KC_NO,     KC_MPRV,   KC_MNXT,   KC_NO,                                       LA_G1,     KC_NO,     KC_NO,     KC_NO,     KC_NO,     KC_NO,
       // ├──────────┼──────────┼──────────┼──────────┼──────────┼──────────┤                                 ├──────────┼──────────┼──────────┼──────────┼──────────┼──────────┤
       // │          │  M2      │  M1      │  M5      │  M4      │          │                                 │          │          │          │          │          │  PANIC   │
           KC_NO,     KC_BTN2,   KC_BTN1,   KC_BTN5,   KC_BTN4,   KC_NO,                                       KC_NO,     KC_NO,     KC_NO,     KC_NO,     KC_NO,     PANIC,
       // └──────────┴──────────┴──────────┼──────────┼──────────┼──────────┼──────────┐           ┌──────────┼──────────┼──────────┼──────────┼──────────┴──────────┴──────────┘
       //                                  │          │  Stop    │  Pause   │  MOUSE   │           │  MUTE    │          │          │          │
                                            KC_NO,     KC_MSTP,   KC_MPLY,   LA_MSE,                KC_MUTE,   KC_NO,     KC_NO,     KC_NO
       //                                  └──────────┴──────────┴──────────┴──────────┘           └──────────┴──────────┴──────────┴──────────┘
    ),
    [6] = LAYOUT_split_3x6_4( // Game layer 1
       // ┌──────────┬──────────┬──────────┬──────────┬──────────┬──────────┐                                 ┌──────────┬──────────┬──────────┬──────────┬──────────┬──────────┐
       // │  TAB     │  Q       │  W       │  E       │  R       │  T       │                                 │  Y       │  U       │  I       │  O       │  P       │  \ |     │
           KC_TAB,    KC_Q,      KC_W,      KC_E,      KC_R,      KC_T,                                        KC_Y,      KC_U,      KC_I,      KC_O,      KC_P,      KC_BSPC,
       // ├──────────┼──────────┼──────────┼──────────┼──────────┼──────────┤                                 ├──────────┼──────────┼──────────┼──────────┼──────────┼──────────┤
       // │  CAPS    │  A       │  S       │  D       │  F       │  G       │                                 │  H       │  J       │  K       │  L       │  ; :     │  ' "     │
           KC_CAPS,   KC_A,      KC_S,      KC_D,      KC_F,      KC_G,                                        KC_H,      KC_J,      KC_K,      KC_L,      KC_SCLN,   KC_QUOT,
       // ├──────────┼──────────┼──────────┼──────────┼──────────┼──────────┤                                 ├──────────┼──────────┼──────────┼──────────┼──────────┼──────────┤
       // │  Shift   │  Z       │  X       │  C       │  V       │  B       │                                 │  N       │  M       │  , <     │  . >     │  / ?     │  PANIC   │
           KC_LSFT,   KC_Z,      KC_X,      KC_C,      KC_V,      KC_B,                                        KC_N,      KC_M,      KC_COMM,   KC_DOT,    KC_SLSH,   PANIC,
       // └──────────┴──────────┴──────────┼──────────┼──────────┼──────────┼──────────┐           ┌──────────┼──────────┼──────────┼──────────┼──────────┴──────────┴──────────┘
       //                                  │  Ctrl    │  SPC     │  GAME 2  │  MUTE    │           │  MUTE    │  Enter   │  Shift   │  Ctrl    │
                                            KC_LCTL,   KC_SPC,    LA_G2,     KC_MUTE,               KC_MUTE,   KC_ENT,    KC_LSFT,   KC_LCTL
       //                                  └──────────┴──────────┴──────────┴──────────┘           └──────────┴──────────┴──────────┴──────────┘
    ),
    [7] = LAYOUT_split_3x6_4( // Game layer 2
       // ┌──────────┬──────────┬──────────┬──────────┬──────────┬──────────┐                                 ┌──────────┬──────────┬──────────┬──────────┬──────────┬──────────┐
       // │  ESC     │  1       │  2       │  3       │  4       │  5       │                                 │  PrtScn  │  F6      │  ↑       │  F7      │  Deafen  │          │
           KC_NO,     KC_1,      KC_2,      KC_3,      KC_4,      KC_5,                                        KC_NO,     KC_NO,     KC_UP,     KC_NO,     DEAFEN,    KC_NO,
       // ├──────────┼──────────┼──────────┼──────────┼──────────┼──────────┤                                 ├──────────┼──────────┼──────────┼──────────┼──────────┼──────────┤
       // │  Alt     │  6       │  7       │  8       │  9       │  0       │                                 │  SnipT   │  ←       │  ↓       │  →       │  Mute    │          │
           KC_LALT,   KC_6,      KC_7,      KC_8,      KC_9,      KC_0,                                        SNIP_T,    KC_LEFT,   KC_DOWN,   KC_RIGHT,  MUTE,      KC_NO,
       // ├──────────┼──────────┼──────────┼──────────┼──────────┼──────────┤                                 ├──────────┼──────────┼──────────┼──────────┼──────────┼──────────┤
       // │  Shift   │  F1      │  F2      │  F3      │  F4      │  F5      │                                 │  F8      │  F9      │  F10     │  F11     │  F12     │  PANIC   │
           KC_LSFT,   KC_F1,     KC_F2,     KC_F3,     KC_F4,     KC_F5,                                       KC_F8,     KC_F9,     KC_F10,    KC_F11,    KC_F12,    PANIC,
       // └──────────┴──────────┴──────────┼──────────┼──────────┼──────────┼──────────┐           ┌──────────┼──────────┼──────────┼──────────┼──────────┴──────────┴──────────┘
       //                                  │          │          │  TRANS   │  MUTE    │           │  MUTE    │  Alt     │  Shift   │  Ctrl    │
                                            KC_NO,     KC_NO,     KC_TRNS,   KC_MUTE,               KC_MUTE,   KC_LALT,   KC_LSFT,   KC_LCTL
       //                                  └──────────┴──────────┴──────────┴──────────┘           └──────────┴──────────┴──────────┴──────────┘
    ),
    [8] = LAYOUT_split_3x6_4( // DEVELOPER layer
       // ┌──────────┬──────────┬──────────┬──────────┬──────────┬──────────┐                                 ┌──────────┬──────────┬──────────┬──────────┬──────────┬──────────┐
       // │  PANIC   │  QK_BOOT │  QK_BOOT │  QK_BOOT │  QK_BOOT │  PANIC   │                                 │  PANIC   │  PC_POWER│  PC_POWER│  PC_POWER│  PC_POWER│  PANIC   │
           PANIC,     QK_BOOT4,  QK_BOOT3,  QK_BOOT2,  QK_BOOT1,  PANIC,                                       PANIC,     PC_POWER1, PC_POWER2, PC_POWER3, PC_POWER4, PANIC,
       // ├──────────┼──────────┼──────────┼──────────┼──────────┼──────────┤                                 ├──────────┼──────────┼──────────┼──────────┼──────────┼──────────┤
       // │  PANIC   │  QK_RESET│  QK_RESET│  QK_RESET│  QK_RESET│  PANIC   │                                 │  PANIC   │  PC_SLEEP│  PC_SLEEP│  PC_SLEEP│  PC_SLEEP│  PANIC   │
           PANIC,     QK_RESET4, QK_RESET3, QK_RESET2, QK_RESET1, PANIC,                                       PANIC,     PC_SLEEP1, PC_SLEEP2, PC_SLEEP3, PC_SLEEP4, PANIC,
       // ├──────────┼──────────┼──────────┼──────────┼──────────┼──────────┤                                 ├──────────┼──────────┼──────────┼──────────┼──────────┼──────────┤
       // │  PANIC   │  PANIC   │  PANIC   │  PANIC   │  PANIC   │  PANIC   │                                 │  PANIC   │  PANIC   │  PANIC   │  PANIC   │  PANIC   │  PANIC   │
           PANIC,     PANIC,     PANIC,     PANIC,     PANIC,     PANIC,                                       PANIC,     PANIC,     PANIC,     PANIC,     PANIC,     PANIC,
       // └──────────┴──────────┴──────────┼──────────┼──────────┼──────────┼──────────┐           ┌──────────┼──────────┼──────────┼──────────┼──────────┴──────────┴──────────┘
       //                                  │  PANIC   │  PANIC   │  CONF1   │  PANIC   │           │  PANIC   │  CONF2   │  PANIC   │  PANIC   │
                                            PANIC,     PANIC,     CONF1,     PANIC,                 PANIC,     CONF2,     PANIC,     PANIC
       //                                  └──────────┴──────────┴──────────┴──────────┘           └──────────┴──────────┴──────────┴──────────┘
    ),
    [9] = LAYOUT_split_3x6_4( // No Combos layer (Not for use)
       // ┌──────────┬──────────┬──────────┬──────────┬──────────┬──────────┐                                 ┌──────────┬──────────┬──────────┬──────────┬──────────┬──────────┐
       // │  NO      │  NO      │  NO      │  NO      │  NO      │  NO      │                                 │  NO      │  NO      │  NO      │  NO      │  NO      │  NO      |
           KC_NO,     KC_NO,     KC_NO,     KC_NO,     KC_NO,     KC_NO,                                       KC_NO,     KC_NO,     KC_NO,     KC_NO,     KC_NO,     KC_NO,
       // ├──────────┼──────────┼──────────┼──────────┼──────────┼──────────┤                                 ├──────────┼──────────┼──────────┼──────────┼──────────┼──────────┤
       // │  NO      │  NO      │  NO      │  NO      │  NO      │  NO      │                                 │  NO      │  NO      │  NO      │  NO      │  NO      │  NO      |
           KC_NO,     KC_NO,     KC_NO,     KC_NO,     KC_NO,     KC_NO,                                       KC_NO,     KC_NO,     KC_NO,     KC_NO,     KC_NO,     KC_NO,
       // ├──────────┼──────────┼──────────┼──────────┼──────────┼──────────┤                                 ├──────────┼──────────┼──────────┼──────────┼──────────┼──────────┤
       // │  NO      │  NO      │  NO      │  NO      │  NO      │  NO      │                                 │  NO      │  NO      │  NO      │  NO      │  NO      │  NO      |
           KC_NO,     KC_NO,     KC_NO,     KC_NO,     KC_NO,     KC_NO,                                       KC_NO,     KC_NO,     KC_NO,     KC_NO,     KC_NO,     KC_NO,
       // └──────────┴──────────┴──────────┼──────────┼──────────┼──────────┼──────────┐           ┌──────────┼──────────┼──────────┼──────────┼──────────┴──────────┴──────────┘
       //                                  │  NO      │  NO      │  NO      │  NO      │           │  NO      │  NO      │  NO      │  NO      │
                                            KC_NO,     KC_NO,     KC_NO,     KC_NO,                 KC_NO,     KC_NO,     KC_NO,     KC_NO 
       //                                  └──────────┴──────────┴──────────┴──────────┘           └──────────┴──────────┴──────────┴──────────┘
    ),
    
};

// ROTARY ENCODER KEYMAP FOR LEFT AND RIGHT ENCODERS

#if defined(ENCODER_MAP_ENABLE)
const uint16_t PROGMEM encoder_map[][NUM_ENCODERS][NUM_DIRECTIONS] = {
    [0] =   { ENCODER_CCW_CW(TAB_L, TAB_R), ENCODER_CCW_CW(KC_VOLU, KC_VOLD) },      // TAB LEFT RIGHT || VOL UP DOWN
    [1] =   { ENCODER_CCW_CW(TAB_L, TAB_R), ENCODER_CCW_CW(KC_VOLU, KC_VOLD) },      // TAB LEFT RIGHT || VOL UP DOWN
    [2] =   { ENCODER_CCW_CW(TAB_L, TAB_R), ENCODER_CCW_CW(TAB_R, TAB_L) },          // TAB LEFT RIGHT || TAB RIGHT LEFT
    [3] =   { ENCODER_CCW_CW(TAB_L, TAB_R), ENCODER_CCW_CW(KC_VOLU, KC_VOLD) },      // TAB LEFT RIGHT || VOL UP DOWN
    [4] =   { ENCODER_CCW_CW(TAB_L, TAB_R), ENCODER_CCW_CW(KC_VOLU, KC_VOLD) },      // TAB LEFT RIGHT || VOL UP DOWN
    [5] =   { ENCODER_CCW_CW(TAB_L, TAB_R), ENCODER_CCW_CW(KC_VOLU, KC_VOLD) },      // TAB LEFT RIGHT || VOL UP DOWN
    [6] =   { ENCODER_CCW_CW(KC_VOLU, KC_VOLD), ENCODER_CCW_CW(KC_VOLU, KC_VOLD) },  //    VOL DOWN UP || VOL UP DOWN
    [7] =   { ENCODER_CCW_CW(KC_VOLU, KC_VOLD), ENCODER_CCW_CW(KC_VOLU, KC_VOLD) },  //    VOL DOWN UP || VOL UP DOWN
    [8] =   { ENCODER_CCW_CW(PANIC, PANIC), ENCODER_CCW_CW(PANIC, PANIC) },          //    PANIC PANIC || PANIC PANIC
    [9] =   { ENCODER_CCW_CW(KC_NO, KC_NO), ENCODER_CCW_CW(KC_NO, KC_NO) },          //          NO NO || NO NO 
};
#endif

uint8_t combo_ref_from_layer(uint8_t layer){
    switch (get_highest_layer(layer_state)){
        case 5: return 5;
        case 8: return 8;
        case 6: return 9;
        case 7: return 9;
        default: return 0;
    }
    return layer;  // important if default is not in case.
}

bool is_oneshot_cancel_key(uint16_t keycode) {
    switch (keycode) {
    case LA_SYM:
    case LA_NUM:
    case LA_NAV:
    case LA_FUNC:
    case LA_MSE:
    case LA_G1:
    case LA_G2:
    case LA_DEV:
    case PANIC:
        return true;
    default:
        return false;
    }
}

bool is_oneshot_ignored_key(uint16_t keycode) {
    switch (keycode) {
    case OS_SHFT:
    case OS_CTRL:
    case OS_ALT:
    case OS_GUI:
    case LA_SYM:
    case LA_NUM:
    case LA_NAV:
    case LA_FUNC:
    case LA_MSE:
    case LA_G1:
    case LA_G2:
    case LA_DEV:
        return true;
    default:
        return false;
    }
}

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