// Copyright 2023 QMK
// SPDX-License-Identifier: GPL-2.0-or-later

#include QMK_KEYBOARD_H

#include "oneshot.h"

// DEBUGGING =========================================================================
#include "print.h"

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
#define LA_SYM OSL(1) // Symbols Layer
#define LA_NUM OSL(2) // Numbers Layer
#define LA_NAV OSL(3) // Navigation Layer
#define LA_FUNC OSL(4) // Function Layer
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

bool os_shft_force_off = false;
bool os_ctrl_force_off = false;
bool os_alt_force_off = false;
bool os_gui_force_off = false;

uint16_t os_shft_timer = 0;
uint16_t os_ctrl_timer = 0;
uint16_t os_alt_timer = 0;
uint16_t os_gui_timer = 0;

// ================================================

bool is_alt_tab_active = false; // ADD this near the begining of keymap.c
uint16_t alt_tab_timer = 0;     // we will be using them soon.

bool is_ctrl_tab_active = false; // ADD this near the begining of keymap.c
uint16_t ctrl_tab_timer = 0;     // we will be using them soon.

// DEFINING CUSTOM KEYCODES
enum custom_keycodes {
	PANIC = SAFE_RANGE,  // Panic button to turn off all layers and mods
	TAB_R,               // Tabs to right window
	TAB_L,               // Tabs to left window
	TAB_CTRL_R,          // Tabs to right window with Ctrl
	TAB_CTRL_L,          // Tabs to left window with Ctrl
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
	COMB_CUT,
	COMB_COPY,
	COMB_PASTE,
	COMB_UNDO,
	COMB_REDO,
	COMB_SAVE,
	COMB_ALL,
	COMB_PAGE_UP,
	COMB_PAGE_DOWN,
	COMB_HOME,
	COMB_END,
};

// COMBOS
const uint16_t PROGMEM DEBUG_LAYER[] = { LA_NAV, LA_NUM, KC_SPC, KC_ENT, OS_SHFT, LA_SYM,        COMBO_END}; // DEBUG LAYER, HIT ALL THUMB KEYS
const uint16_t PROGMEM BOOT[] = { QK_BOOT1, QK_BOOT2, QK_BOOT3, QK_BOOT4, CONF1, CONF2,          COMBO_END}; // BOOT LAYER
const uint16_t PROGMEM RESET[] = { QK_RESET1, QK_RESET2, QK_RESET3, QK_RESET4, CONF1, CONF2,     COMBO_END}; // RESET LAYER
const uint16_t PROGMEM SLEEP[] = { PC_SLEEP1, PC_SLEEP2, PC_SLEEP3, PC_SLEEP4, CONF1, CONF2,     COMBO_END}; // SLEEP LAYER
const uint16_t PROGMEM POWER[] = { PC_POWER1, PC_POWER2, PC_POWER3, PC_POWER4, CONF1, CONF2,     COMBO_END}; // POWER LAYER
const uint16_t PROGMEM CUT[] = { KC_V, KC_X, COMBO_END}; // CUT
const uint16_t PROGMEM COPY[] = { KC_X, KC_C, COMBO_END}; // COPY
const uint16_t PROGMEM PASTE[] = { KC_C, KC_V, COMBO_END}; // PASTE
const uint16_t PROGMEM UNDO[] = { KC_X, KC_Z, COMBO_END}; // UNDO
const uint16_t PROGMEM REDO[] = { KC_Z, KC_C, COMBO_END}; // REDO
const uint16_t PROGMEM SAVE[] = { KC_S, KC_D, COMBO_END}; // SAVE
const uint16_t PROGMEM ALL[] = { KC_Z, KC_V, COMBO_END}; // ALL
const uint16_t PROGMEM PAGE_UP[] = { KC_WBAK, KC_UP, COMBO_END}; // PAGE UP
const uint16_t PROGMEM PAGE_DOWN[] = { KC_LEFT, KC_DOWN, COMBO_END}; // PAGE DOWN
const uint16_t PROGMEM HOME[] = { KC_WFWD, KC_UP, COMBO_END}; // HOME
const uint16_t PROGMEM END[] = { KC_RIGHT, KC_DOWN, COMBO_END}; // END

combo_t key_combos[] = {
	[COMB_DEBUG] = COMBO(DEBUG_LAYER, LA_DEV), // Activate debug layer
	[COMB_BOOT] = COMBO(BOOT, QK_BOOTLOADER), // Boot
	[COMB_RESET] = COMBO(RESET, QK_REBOOT), // Reset
	[COMB_SLEEP] = COMBO(SLEEP, KC_SYSTEM_SLEEP), // Sleep
	[COMB_POWER] = COMBO(POWER, KC_SYSTEM_POWER), // Power
	[COMB_CUT] = COMBO(CUT, C(KC_X)), // Cut
	[COMB_COPY] = COMBO(COPY, C(KC_C)), // Copy
	[COMB_PASTE] = COMBO(PASTE, C(KC_V)), // Paste
	[COMB_UNDO] = COMBO(UNDO, C(KC_Z)), // Undo
	[COMB_REDO] = COMBO(REDO, C(KC_Y)), // Redo
	[COMB_SAVE] = COMBO(SAVE, C(KC_S)), // Save
	[COMB_ALL] = COMBO(ALL, C(KC_A)), // Select All
	[COMB_PAGE_UP] = COMBO(PAGE_UP, KC_PGUP), // Page Up
	[COMB_PAGE_DOWN] = COMBO(PAGE_DOWN, KC_PGDN), // Page Down
	[COMB_HOME] = COMBO(HOME, KC_HOME), // Home
	[COMB_END] = COMBO(END, KC_END), // End
};

bool process_record_kb(uint16_t keycode, keyrecord_t *record) {

	update_oneshot(
			&os_shft_state,
			&os_shft_force_off,
			&os_shft_timer,
			KC_LSFT,
			OS_SHFT,
			keycode,
			record
			);
	update_oneshot(
			&os_ctrl_state,
			&os_ctrl_force_off,
			&os_ctrl_timer,
			KC_LCTL,
			OS_CTRL,
			keycode,
			record
			);
	update_oneshot(
			&os_alt_state,
			&os_alt_force_off,
			&os_alt_timer,
			KC_LALT,
			OS_ALT,
			keycode,
			record
			);
	update_oneshot(
			&os_gui_state,
			&os_gui_force_off,
			&os_gui_timer,
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
					register_code(KC_LALT);
				}
				alt_tab_timer = timer_read();
				tap_code(KC_TAB);
			}
			return false;

		case TAB_L: // Tab to the left window
			if (record->event.pressed) {

				if (!is_alt_tab_active) {
					is_alt_tab_active = true;
					register_code(KC_LALT);
				}
				alt_tab_timer = timer_read();
				register_code(KC_LSFT);
				tap_code(KC_TAB);
				unregister_code(KC_LSFT);
			}
			return false;

		case TAB_CTRL_R: // Tab to the right with Ctrl
			if (record->event.pressed) {

				if (!is_ctrl_tab_active) {
					is_ctrl_tab_active = true;
					register_code(KC_LCTL);
				}
				ctrl_tab_timer = timer_read();
				tap_code(KC_TAB);
			}
			return false;

		case TAB_CTRL_L: // Tab to the left with Ctrl
			if (record->event.pressed) {

				if (!is_ctrl_tab_active) {
					is_ctrl_tab_active = true;
					register_code(KC_LCTL);
				}
				ctrl_tab_timer = timer_read();
				register_code(KC_LSFT);
				tap_code(KC_TAB);
				unregister_code(KC_LSFT);
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
				layer_off(1);
				layer_off(2);
				layer_off(3);
				layer_off(4);
				layer_off(7);
			}
			return false;
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
       // │  ESC     │  A       │  S       │  D       │  F       │  G       │                                 │  H       │  J       │  K       │  L       │  ; :     │  ' "     │
           KC_ESC,    KC_A,      KC_S,      KC_D,      KC_F,      KC_G,                                        KC_H,      KC_J,      KC_K,      KC_L,      KC_SCLN,    KC_QUOT,
       // ├──────────┼──────────┼──────────┼──────────┼──────────┼──────────┤                                 ├──────────┼──────────┼──────────┼──────────┼──────────┼──────────┤
       // │  Ctrl    │  Z       │  X       │  C       │  V       │  B       │                                 │  N       │  M       │  , <     │  . >     │  / ?     │  PANIC   │
           OS_CTRL,   KC_Z,      KC_X,      KC_C,      KC_V,      KC_B,                                        KC_N,      KC_M,      KC_COMM,   KC_DOT,    KC_SLSH,   PANIC,
       // └──────────┴──────────┴──────────┼──────────┼──────────┼──────────┼──────────┐           ┌──────────┼──────────┼──────────┼──────────┼──────────┴──────────┴──────────┘
       //                                  │  NAV     │  NUM     │  SPC     │  MOUSE   │           │  MUTE    │  Enter   │  Shift   │  SYM     │
                                            LA_NAV,    LA_NUM,    KC_SPC,    LA_MSE,                KC_MUTE,   KC_ENT,    OS_SHFT,   LA_SYM
       //                                  └──────────┴──────────┴──────────┴──────────┘           └──────────┴──────────┴──────────┴──────────┘
    ),
    [1] = LAYOUT_split_3x6_4( // Symbols layer
       // ┌──────────┬──────────┬──────────┬──────────┬──────────┬──────────┐                                 ┌──────────┬──────────┬──────────┬──────────┬──────────┬──────────┐
       // │          │  !       │  @       │  #       │  $       │  %       │                                 │          │          │          │          │          │  BSP     │
           KC_NO,     KC_EXLM,   KC_AT,     KC_HASH,   KC_DLR,    KC_PERC,                                     KC_NO,     KC_NO,     KC_NO,      KC_NO,     KC_NO,     KC_BSPC,
       // ├──────────┼──────────┼──────────┼──────────┼──────────┼──────────┤                                 ├──────────┼──────────┼──────────┼──────────┼──────────┼──────────┤
       // │  ~       │  |       │  [       │  {       │  )       │  &       │                                 │  -       │  Ctrl    │  Shift   │  Alt     │  Gui     │  = +     │
           KC_TILD,   KC_PIPE,   KC_LBRC,   KC_LCBR,   KC_LPRN,   KC_AMPR,                                     KC_MINS,   OS_CTRL,   OS_SHFT,   OS_ALT,    OS_GUI,    KC_EQL,
       // ├──────────┼──────────┼──────────┼──────────┼──────────┼──────────┤                                 ├──────────┼──────────┼──────────┼──────────┼──────────┼──────────┤
       // │  `       │  \       │  ]       │  {       │  )       │  *       │                                 │          │  ^       │  &       │  *       │  / ?     │  PANIC   │
           KC_GRV,    KC_BSLS,   KC_RBRC,   KC_RCBR,   KC_RPRN,    KC_ASTR,                                    KC_NO,     KC_CIRC,   KC_AMPR,    KC_ASTR,  KC_SLSH,   PANIC,
       // └──────────┴──────────┴──────────┼──────────┼──────────┼──────────┼──────────┐           ┌──────────┼──────────┼──────────┼──────────┼──────────┴──────────┴──────────┘
       //                                  │          │ Tab      │  _       │  MOUSE   │           │  MUTE    │          │          │  TRANS   │
                                            KC_NO,     KC_TAB,    KC_UNDS,   LA_MSE,                KC_MUTE,    KC_NO,    KC_NO,     KC_TRNS
       //                                  └──────────┴──────────┴──────────┴──────────┘           └──────────┴──────────┴──────────┴──────────┘
    ),
    [2] = LAYOUT_split_3x6_4( // Numbers layer
       // ┌──────────┬──────────┬──────────┬──────────┬──────────┬──────────┐                                 ┌──────────┬──────────┬──────────┬──────────┬──────────┬──────────┐
       // │          │          │          │          │          │          │                                 │  +       │  7 &     │  8 *     │  9 (     │  ^       │  BSP     │
           KC_NO,     KC_NO,     KC_NO,     KC_NO,     KC_NO,     KC_NO,                                       KC_PLUS,   KC_7,      KC_8,      KC_9,      KC_CIRC,   KC_BSPC,
       // ├──────────┼──────────┼──────────┼──────────┼──────────┼──────────┤                                 ├──────────┤──────────┼──────────┼──────────┼──────────┼──────────┤
       // │          │  Gui     │  Alt     │  Shift   │  Ctrl    │  ^       │                                 │  - _     │  4 $     │  5 %     │  6 ^     │  *       │  = +     │
           KC_NO,     OS_GUI,    OS_ALT,    OS_SHFT,   OS_CTRL,   KC_CIRC,                                     KC_MINS,   KC_4,      KC_5,      KC_6,      KC_ASTR,   KC_EQL,
       // ├──────────┼──────────┼──────────┼──────────┼──────────├──────────┼                                 ├──────────┼──────────┼──────────┼──────────┼──────────┤──────────┤
       // │          │  /       │  *       │  -       │  +       │          │                                 │  0 )     │  1 !     │  2 @     │  3 #     │  / ?     │  PANIC   │
           KC_NO,     KC_SLSH,   KC_ASTR,   KC_MINS,   KC_PLUS,   KC_NO,                                       KC_0,      KC_1,      KC_2,      KC_3,      KC_SLSH,   PANIC,
       // └──────────┴──────────┴──────────┼──────────┼──────────┼──────────┼──────────┐           ┌──────────┼──────────┼──────────┼──────────┼──────────┴──────────┴──────────┘
       //                                  │          │  TRANS   │          │  MOUSE   │           │  MUTE    │  Enter   │  FUNC    │  . >     │
                                            KC_NO,     KC_TRNS,   KC_NO,     LA_MSE,                KC_MUTE,   KC_ENT,    LA_FUNC,   KC_DOT
       //                                  └──────────┴──────────┴──────────┴──────────┘           └──────────┴──────────┴──────────┴──────────┘
    ),
    [3] = LAYOUT_split_3x6_4( // Navigation layer
       // ┌──────────┬──────────┬──────────┬──────────┬──────────┬──────────┐                                 ┌──────────┬──────────┬──────────┬──────────┬──────────┬──────────┐
       // │          │          │          │          │  Insert  │          │                                 │  Deafen  │  BR BWD  │  ↑       │  BR FWD  │  PrtScn  │  TAB     │
           KC_NO,     KC_NO,     KC_NO,     KC_NO,     KC_INS,    KC_NO,                                       DEAFEN,    KC_WBAK,   KC_UP,     KC_WFWD,   KC_PSCR,    TAB_L,
       // ├──────────┼──────────┼──────────┼──────────┼──────────┼──────────┤                                 ├──────────┤──────────┼──────────┼──────────┼──────────┤──────────┤
       // │ All      │  Gui     │  Alt     │  Shift   │  Ctrl    │          │                                 │  Mute    │  ←       │  ↓       │  →       │  SnipT   │  C TAB   │
           C(KC_A),   OS_GUI,    OS_ALT,    OS_SHFT,   OS_CTRL,   KC_NO,                                       MUTE,      KC_LEFT,   KC_DOWN,   KC_RIGHT,  SNIP_T,   TAB_CTRL_L,
       // ├──────────┼──────────┼──────────┼──────────┼──────────┼──────────┤                                 ├──────────┼──────────┼──────────┼──────────┼──────────┼──────────┤
       // │  WIN     │ UNDO     │ CUT      │ COPY     │ PASTE    │          │                                 │  Insert  │ VIM UP   │ VIM DN   │  SPLIT V │  SPLIT H │  PANIC   │
           KC_LWIN,   C(KC_Z),   C(KC_X),   C(KC_C),   C(KC_V),   KC_NO,                                       KC_INS,    C(KC_U),   C(KC_D),  LSA(KC_EQL),LSA(KC_MINS), PANIC,
       // └──────────┴──────────┴──────────┼──────────┼──────────┼──────────┼──────────┐           ┌──────────┼──────────┼──────────┼──────────┼──────────┴──────────┴──────────┘
       //                                  │  TRANS   │          │          │  MOUSE   │           │  MUTE    │  Esc     │  BSP     │  DEL     │
                                            KC_TRNS,   KC_NO,     KC_NO,     LA_MSE,                KC_MUTE,   KC_ESC,    KC_BSPC,   KC_DEL
       //                                  └──────────┴──────────┴──────────┴──────────┘           └──────────┴──────────┴──────────┴──────────┘
    ),
    [4] = LAYOUT_split_3x6_4( // Function layer
       // ┌──────────┬──────────┬──────────┬──────────┬──────────┬──────────┐                                 ┌──────────┬──────────┬──────────┬──────────┬──────────┬──────────┐
       // │  F13     │  F14     │  F15     │  F16     │  F17     │  F18     │                                 │  F12     │  F7      │  F8      │  F9      │  PrtScn  │          │
           KC_F13,    KC_F14,    KC_F15,    KC_F16,    KC_F17,    KC_F18,                                      KC_F12,    KC_F7,     KC_F8,     KC_F9,     KC_PSCR,   KC_NO,
       // ├──────────┼──────────┼──────────┼──────────┼──────────┼──────────┤                                 ├──────────┼──────────┼──────────┼──────────┼──────────┤──────────┤
       // │          │  Gui     │  Alt     │  Shift   │  Ctrl    │          │                                 │  F11     │  F4      │  F5      │  F6      │  SnipT   │          │
           KC_NO,     OS_GUI,    OS_ALT,    OS_SHFT,   OS_CTRL,   KC_NO,                                       KC_F11,    KC_F4,     KC_F5,     KC_F6,     SNIP_T,    KC_NO,
       // ├──────────┼──────────┼──────────┼──────────┼──────────┼──────────┤                                 ├──────────┼──────────┼──────────┼──────────┼──────────┼──────────┤
       // │  F19     │  F20     │  F21     │  F22     │  F23     │  F24     │                                 │  F10     │  F1      │  F2      │  F3      │          │  PANIC   │
           KC_F19,    KC_F20,    KC_F21,    KC_F22,    KC_F23,    KC_F24,                                      KC_F10,    KC_F1,     KC_F2,     KC_F3,     KC_NO,     PANIC,
       // └──────────┴──────────┴──────────┼──────────┼──────────┼──────────┼──────────┐           ┌──────────┼──────────┼──────────┼──────────┼──────────┴──────────┴──────────┘
       //                                  │          │  TRANS   │          │  MOUSE   │           │  MUTE    │          │  TRANS   │          │
                                            KC_NO,     KC_TRNS,   KC_NO,     LA_MSE,                KC_MUTE,   KC_NO,     KC_TRNS,   KC_NO
       //                                  └──────────┴──────────┴──────────┴──────────┘           └──────────┴──────────┴──────────┴──────────┘
    ),
    [5] = LAYOUT_split_3x6_4( // Left Mouse and Media layer
       // ┌──────────┬──────────┬──────────┬──────────┬──────────┬──────────┐                                 ┌──────────┬──────────┬──────────┬──────────┬──────────┬──────────┐
       // │          │          │          │  M↑      │          │  PrtScn  │                                 │  Deafen  │   PREV   │  ↑       │   NEXT   │  PrtScn  │          │
           KC_NO,     KC_NO,     KC_NO,     KC_MS_U,   KC_NO,     KC_PSCR,                                     DEAFEN,    KC_MPRV,   KC_UP,     KC_MNXT,   KC_PSCR,   KC_NO,
       // ├──────────┼──────────├──────────┼──────────┼──────────┼──────────┤                                 ├──────────┤──────────┼──────────┼──────────┼──────────┤──────────┤
       // │          │  M2      │ M←       │  M↓      │  M→      │  SnipT   │                                 │  Mute    │  ←       │  ↓       │  →       │  SnipT   │  GAME    │
           KC_NO,     KC_BTN2,   KC_MS_L,   KC_MS_D,   KC_MS_R,   SNIP_T,                                      MUTE,      KC_LEFT,   KC_DOWN,   KC_RIGHT,  SNIP_T,    LA_G1,
       // ├──────────┼──────────┼──────────┼──────────┼──────────┼──────────┤                                 ├──────────┼──────────┼──────────┼──────────┼──────────┼──────────┤
       // │          │          │ ACCEL 3  │ ACCEL 2  │ ACCEL 1  │  M3      │                                 │          │          │  , <     │  . >     │          │  PANIC   │
           KC_NO,     KC_NO,     KC_ACL2,  KC_ACL1,   KC_ACL0,   KC_BTN3,                                      KC_NO,     KC_NO,     KC_COMM,   KC_DOT,    KC_NO,     PANIC,
       // └──────────┴──────────┴──────────┼──────────┼──────────┼──────────┼──────────┐           ┌──────────┼──────────┼──────────┼──────────┼──────────┴──────────┴──────────┘
       //                                  │  M4      │  M5      │  M1      │  PANIC   │           │  MUTE    │  Pause   │  Stop    │          │
                                            KC_BTN4,   KC_BTN5,   KC_BTN1,   PANIC,                 KC_MUTE,   KC_MPLY,   KC_MSTP,   KC_NO
       //                                  └──────────┴──────────┴──────────┴──────────┘           └──────────┴──────────┴──────────┴──────────┘
    ),
    [6] = LAYOUT_split_3x6_4( // Game layer 1
       // ┌──────────┬──────────┬──────────┬──────────┬──────────┬──────────┐                                 ┌──────────┬──────────┬──────────┬──────────┬──────────┬──────────┐
       // │  TAB     │  Q       │  W       │  E       │  R       │  T       │                                 │  Y       │  U       │  I       │  O       │  P       │  \ |     │
           KC_TAB,    KC_Q,      KC_W,      KC_E,      KC_R,      KC_T,                                        KC_Y,      KC_U,      KC_I,      KC_O,      KC_P,      KC_BSPC,
       // ├──────────┼──────────┼──────────┼──────────┼──────────┼──────────┤                                 ├──────────┼──────────┼──────────┼──────────┼──────────┼──────────┤
       // │  Alt     │  A       │  S       │  D       │  F       │  G       │                                 │  H       │  J       │  K       │  L       │  ; :     │  ' "     │
           KC_LALT,   KC_A,      KC_S,      KC_D,      KC_F,      KC_G,                                        KC_H,      KC_J,      KC_K,      KC_L,      KC_SCLN,   KC_QUOT,
       // ├──────────┼──────────┼──────────┼──────────┼──────────┼──────────┤                                 ├──────────┼──────────┼──────────┼──────────┼──────────┼──────────┤
       // │  Shift   │  Z       │  X       │  C       │  V       │  B       │                                 │  N       │  M       │  , <     │  . >     │  / ?     │  PANIC   │
           KC_LSFT,   KC_Z,      KC_X,      KC_C,      KC_V,      KC_B,                                        KC_N,      KC_M,      KC_COMM,   KC_DOT,    KC_SLSH,   PANIC,
       // └──────────┴──────────┴──────────┼──────────┼──────────┼──────────┼──────────┐           ┌──────────┼──────────┼──────────┼──────────┼──────────┴──────────┴──────────┘
       //                                  │  Ctrl    │  GAME 2  │  SPC     │  MUTE    │           │  MUTE    │  Enter   │  Shift   │  Ctrl    │
                                            KC_LCTL,   LA_G2,     KC_SPC,    KC_MUTE,               KC_MUTE,   KC_ENT,    KC_LSFT,   KC_LCTL
       //                                  └──────────┴──────────┴──────────┴──────────┘           └──────────┴──────────┴──────────┴──────────┘
    ),
    [7] = LAYOUT_split_3x6_4( // Game layer 2
       // ┌──────────┬──────────┬──────────┬──────────┬──────────┬──────────┐                                 ┌──────────┬──────────┬──────────┬──────────┬──────────┬──────────┐
       // │  ESC     │  1       │  2       │  3       │  4       │  5       │                                 │  Deafen  │  F6      │  ↑       │  F7      │  PrtScn  │  TAB     │
           KC_ESC,    KC_1,      KC_2,      KC_3,      KC_4,      KC_5,                                        DEAFEN,    KC_F6,     KC_UP,     KC_F7,     KC_PSCR,    TAB_L,
       // ├──────────┼──────────┼──────────┼──────────┼──────────┼──────────┤                                 ├──────────┤──────────┼──────────┼──────────┼──────────┤──────────┤
       // │  Alt     │  6       │  7       │  8       │  9       │  0       │                                 │  Mute    │  ←       │  ↓       │  →       │  SnipT   │  C TAB   │
           KC_LALT,   KC_6,      KC_7,      KC_8,      KC_9,      KC_0,                                        MUTE,      KC_LEFT,   KC_DOWN,   KC_RIGHT,  SNIP_T,   TAB_CTRL_L,
       // ├──────────┼──────────┼──────────┼──────────┼──────────┼──────────┤                                 ├──────────┼──────────┼──────────┼──────────┼──────────┼──────────┤
       // │  Shift   │  F1      │  F2      │  F3      │  F4      │  F5      │                                 │  F8      │  F9      │  F10     │  F11     │  F12     │  PANIC   │
           KC_LSFT,   KC_F1,     KC_F2,     KC_F3,     KC_F4,     KC_F5,                                       KC_F8,     KC_F9,     KC_F10,    KC_F11,    KC_F12,    PANIC,
       // └──────────┴──────────┴──────────┼──────────┼──────────┼──────────┼──────────┐           ┌──────────┼──────────┼──────────┼──────────┼──────────┴──────────┴──────────┘
       //                                  │          │  TRANS   │          │  MUTE    │           │  MUTE    │  Alt     │  Shift   │  Ctrl    │
                                            KC_NO,     KC_TRNS,   KC_NO,     KC_MUTE,               KC_MUTE,   KC_LALT,   KC_LSFT,   KC_LCTL
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
       // │          │          │          │          │          │          │                                 │          │          │          │          │          │          │
           KC_NO,     KC_NO,     KC_NO,     KC_NO,     KC_NO,     KC_NO,                                       KC_NO,     KC_NO,     KC_NO,     KC_NO,     KC_NO,     KC_NO,
       // ├──────────┼──────────┼──────────┼──────────┼──────────┼──────────┤                                 ├──────────┼──────────┼──────────┼──────────┼──────────┼──────────┤
       // │          │          │          │          │          │          │                                 │          │          │          │          │          │          │
           KC_NO,     KC_NO,     KC_NO,     KC_NO,     KC_NO,     KC_NO,                                       KC_NO,     KC_NO,     KC_NO,     KC_NO,     KC_NO,     KC_NO,
       // ├──────────┼──────────┼──────────┼──────────┼──────────┼──────────┤                                 ├──────────┼──────────┼──────────┼──────────┼──────────┼──────────┤
       // │          │          │          │          │          │          │                                 │          │          │          │          │          │          │
           KC_NO,     KC_NO,     KC_NO,     KC_NO,     KC_NO,     KC_NO,                                       KC_NO,     KC_NO,     KC_NO,     KC_NO,     KC_NO,     KC_NO,
       // └──────────┴──────────┴──────────┼──────────┼──────────┼──────────┼──────────┐           ┌──────────┼──────────┼──────────┼──────────┼──────────┴──────────┴──────────┘
       //                                  │          │          │          │          │           │          │          │          │          │
                                            KC_NO,     KC_NO,     KC_NO,     KC_NO,                 KC_NO,     KC_NO,     KC_NO,     KC_NO
       //                                  └──────────┴──────────┴──────────┴──────────┘           └──────────┴──────────┴──────────┴──────────┘
    ),

};

// ROTARY ENCODER KEYMAP FOR LEFT AND RIGHT ENCODERS

#if defined(ENCODER_MAP_ENABLE)
const uint16_t PROGMEM encoder_map[][NUM_ENCODERS][NUM_DIRECTIONS] = {
    [0] =   { ENCODER_CCW_CW(TAB_L, TAB_R), ENCODER_CCW_CW(KC_VOLU, KC_VOLD) },      // TAB LEFT RIGHT || VOL UP DOWN
    [1] =   { ENCODER_CCW_CW(TAB_L, TAB_R), ENCODER_CCW_CW(KC_VOLU, KC_VOLD) },      // TAB LEFT RIGHT || VOL UP DOWN
    [2] =   { ENCODER_CCW_CW(TAB_L, TAB_R), ENCODER_CCW_CW(KC_VOLU, KC_VOLD) },      // TAB LEFT RIGHT || VOL UP DOWN
    [3] =   { ENCODER_CCW_CW(TAB_L, TAB_R), ENCODER_CCW_CW(TAB_CTRL_L, TAB_CTRL_R) },      // TAB LEFT RIGHT || TAB CTRL LEFT RIGHT
    [4] =   { ENCODER_CCW_CW(TAB_L, TAB_R), ENCODER_CCW_CW(KC_VOLU, KC_VOLD) },      // TAB LEFT RIGHT || VOL UP DOWN
    [5] =   { ENCODER_CCW_CW(KC_WH_U, KC_WH_D), ENCODER_CCW_CW(KC_VOLU, KC_VOLD) },      // SCROLL UP DOWN || VOL UP DOWN
    [6] =   { ENCODER_CCW_CW(KC_VOLD, KC_VOLU), ENCODER_CCW_CW(KC_VOLU, KC_VOLD) },  //    VOL UP DOWN || VOL UP DOWN
    [7] =   { ENCODER_CCW_CW(KC_VOLD, KC_VOLU), ENCODER_CCW_CW(KC_VOLU, KC_VOLD) },  //    VOL UP DOWN || VOL UP DOWN
    [8] =   { ENCODER_CCW_CW(PANIC, PANIC), ENCODER_CCW_CW(PANIC, PANIC) },          //    PANIC PANIC || PANIC PANIC
    [9] =   { ENCODER_CCW_CW(KC_NO, KC_NO), ENCODER_CCW_CW(KC_NO, KC_NO) },          //          NO NO || NO NO
};
#endif

uint8_t combo_ref_from_layer(uint8_t layer){
    switch (get_highest_layer(layer_state)){
        case 0: return 0;
        case 3: return 3;
        case 5: return 5;
        case 8: return 8;
        default: return 9;
    }
    return layer;  // important if default is not in case.
}

bool is_oneshot_cancel_key(uint16_t keycode) {
    switch (keycode) {
    case KC_ESC:
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
    if (timer_elapsed(alt_tab_timer) > 800 ) {
      unregister_code(KC_LALT);
      is_alt_tab_active = false;
    }
  }

    if (is_ctrl_tab_active) { //CTRL TAB Encoder Timer
        if (timer_elapsed(ctrl_tab_timer) > 800 ) {
        unregister_code(KC_LCTL);
        is_ctrl_tab_active = false;
        }
    }

};

layer_state_t layer_state_set_user(layer_state_t state) {
    if (!layer_state_cmp(state, 2) && layer_state_cmp(state, 4)) {
        layer_off(4); // disable function layer when numbers layer is turned off
    }
    return state;
}
