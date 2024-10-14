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



// LAYER NAMES
enum custom_layers {
    _BASE,
    _SYM,
    _NUM,
    _NAV,
    _FUNC,
    _MEDIA,
    _DEV,
    _NO,
    _MISC,
};

// DEFINING LAYER KEYCODES
#define LA_SYM OSL(_SYM) // Symbols Layer
#define LA_NUM OSL(_NUM) // Numbers Layer
#define LA_NAV OSL(_NAV) // Navigation Layer
#define LA_FUNC MO(_FUNC) // Function Layer
#define LA_MSE TO(_MEDIA) // Left Mouse and Media Layer
#define LA_DEV TO(_DEV) // Developer Layer
#define LA_MISC TO(_MISC) // MISC Layer
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
    Panic1, Panic2, // Panic buttons.
};

enum combos {
	COMB_DEBUG,
    COMB_PANIC,
    COMB_PANIC2,
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
const uint16_t PROGMEM DEBUG_LAYER[] = { LA_NAV, KC_SPC, KC_ENT, LA_SYM,        COMBO_END}; // DEBUG LAYER, HIT NAV, SPACE, ENTER, SYM
const uint16_t PROGMEM PANIC_COMBO[] = { LA_NUM, OS_SHFT , COMBO_END}; // PANIC BUTTON TO GET BACK TO BASE LAYER
const uint16_t PROGMEM PANIC_COMBO2[] = { Panic1, Panic2 , COMBO_END}; // PANIC BUTTON TO GET BACK TO BASE LAYER
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
    [COMB_PANIC] = COMBO(PANIC_COMBO, PANIC), // PANIC BUTTON
    [COMB_PANIC2] = COMBO(PANIC_COMBO2, PANIC), // PANIC BUTTON
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
				layer_off(_SYM);
				layer_off(_NUM);
				layer_off(_NAV);
				layer_off(_FUNC);
			}
			return false;
	}
	return true;
};


// KEYMAP
const uint16_t PROGMEM keymaps[][MATRIX_ROWS][MATRIX_COLS] = {
    [_BASE] = LAYOUT_split_3x6_4( // Base layer
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
    [_SYM] = LAYOUT_split_3x6_4( // Symbols layer
       // ┌──────────┬──────────┬──────────┬──────────┬──────────┬──────────┐                                 ┌──────────┬──────────┬──────────┬──────────┬──────────┬──────────┐
       // │ Tab      │  !       │  @       │  [       │  ]       │  %       │                                 │          │          │          │          │          │  BSP     │
           KC_TAB,    KC_EXLM,   KC_AT,     KC_LBRC,   KC_RBRC,   KC_PERC,                                     KC_NO,     KC_NO,     KC_NO,      KC_NO,     KC_NO,     KC_BSPC,
       // ├──────────┼──────────┼──────────┼──────────┼──────────┼──────────┤                                 ├──────────┼──────────┼──────────┼──────────┼──────────┼──────────┤
       // │  ~       │  ^       │  $       │  (       │  )       │  &       │                                 │          │  Ctrl    │  Shift   │  Alt     │  Gui     │  = +     │
           KC_TILD,   KC_CIRC,   KC_DLR,    KC_LPRN,   KC_RPRN,   KC_AMPR,                                     KC_NO,     OS_CTRL,   OS_SHFT,   OS_ALT,    OS_GUI,    KC_EQL,
       // ├──────────┼──────────┼──────────┼──────────┼──────────┼──────────┤                                 ├──────────┼──────────┼──────────┼──────────┼──────────┼──────────┤
       // │  `       │  #       │  *       │  {       │  }       │  |       │                                 │          │          │  <       │   >      │  ?       │  PANIC   │
           KC_GRV,    KC_HASH,   KC_ASTR,   KC_LCBR,   KC_RCBR,   KC_PIPE,                                     KC_NO,     KC_NO,     KC_LT,      KC_GT,    KC_QUES,   PANIC,
       // └──────────┴──────────┴──────────┼──────────┼──────────┼──────────┼──────────┐           ┌──────────┼──────────┼──────────┼──────────┼──────────┴──────────┴──────────┘
       //                                  │  \       │  /       │  _       │  MOUSE   │           │  MUTE    │          │          │  TRANS   │
                                            KC_BSLS,   KC_SLSH,   KC_UNDS,   LA_MSE,                KC_MUTE,    KC_NO,    KC_NO,     KC_TRNS
       //                                  └──────────┴──────────┴──────────┴──────────┘           └──────────┴──────────┴──────────┴──────────┘
    ),
    [_NUM] = LAYOUT_split_3x6_4( // Numbers layer
       // ┌──────────┬──────────┬──────────┬──────────┬──────────┬──────────┐                                 ┌──────────┬──────────┬──────────┬──────────┬──────────┬──────────┐
       // │          │          │          │          │          │          │                                 │  +       │  7 &     │  8 *     │  9 (     │  ^       │  BSP     │
           KC_NO,     KC_NO,     KC_NO,     KC_NO,     KC_NO,     KC_NO,                                       KC_PLUS,   KC_7,      KC_8,      KC_9,      KC_CIRC,   KC_BSPC,
       // ├──────────┼──────────┼──────────┼──────────┼──────────┼──────────┤                                 ├──────────┼──────────┼──────────┼──────────┼──────────┼──────────┤
       // │          │  Gui     │  Alt     │  Shift   │  Ctrl    │          │                                 │  - _     │  4 $     │  5 %     │  6 ^     │  *       │  = +     │
           KC_NO,     OS_GUI,    OS_ALT,    OS_SHFT,   OS_CTRL,   KC_NO,                                       KC_MINS,   KC_4,      KC_5,      KC_6,      KC_ASTR,   KC_EQL,
       // ├──────────┼──────────┼──────────┼──────────┼──────────┼──────────┤                                 ├──────────┼──────────┼──────────┼──────────┼──────────┼──────────┤
       // │          │  FUNC    │  KATA    │  HIRA    │  ROMAN   │          │                                 │  0 )     │  1 !     │  2 @     │  3 #     │  / ?     │  PANIC   │
           KC_NO,     LA_FUNC,  A(KC_CAPS), C(KC_CAPS), S(KC_CAPS), KC_NO,                                     KC_0,      KC_1,      KC_2,      KC_3,      KC_SLSH,   PANIC,
       // └──────────┴──────────┴──────────┼──────────┼──────────┼──────────┼──────────┐           ┌──────────┼──────────┼──────────┼──────────┼──────────┴──────────┴──────────┘
       //                                  │  LANG    │  TRANS   │ SEARCH   │  MOUSE   │           │  MUTE    │  Enter   │  . >     │  . >     │
                                            G(KC_SPC), KC_TRNS,  LAG(KC_SPC), LA_MSE,               KC_MUTE,   KC_ENT,    KC_DOT,    KC_DOT
       //                                  └──────────┴──────────┴──────────┴──────────┘           └──────────┴──────────┴──────────┴──────────┘
    ),
    [_NAV] = LAYOUT_split_3x6_4( // Navigation layer
       // ┌──────────┬──────────┬──────────┬──────────┬──────────┬──────────┐                                 ┌──────────┬──────────┬──────────┬──────────┬──────────┬──────────┐
       // │ CAPSLOCK │ SAVE     │          │          │          │  MISC    │                                 │  HOME    │  PG DN   │  PG UP   │  END     │  Insert  │          │
           KC_CAPS,   C(KC_S),   KC_NO,     KC_NO,     KC_NO,     LA_MISC,                                     KC_HOME,   KC_PGDN,   KC_PGUP,   KC_END,    KC_INS,    KC_NO,
       // ├──────────┼──────────┼──────────┼──────────┼──────────┼──────────┤                                 ├──────────┼──────────┼──────────┼──────────┼──────────┼──────────┤
       // │ All      │  Gui     │  Alt     │  Shift   │  Ctrl    │          │                                 │  ←       │  ↓       │  ↑       │  →       │  SnipT   │          │
           C(KC_A),   OS_GUI,    OS_ALT,    OS_SHFT,   OS_CTRL,   KC_NO,                                       KC_LEFT,   KC_DOWN,   KC_UP,     KC_RIGHT,  SNIP_T,    KC_NO,
       // ├──────────┼──────────┼──────────┼──────────┼──────────┼──────────┤                                 ├──────────┼──────────┼──────────┼──────────┼──────────┼──────────┤
       // │  WIN     │ UNDO     │ CUT      │ COPY     │ PASTE    │          │                                 │   PREV   │  Pause   │  Stop    │   NEXT   │  PrtScn  │  PANIC   │
           KC_LWIN,   C(KC_Z),   C(KC_X),   C(KC_C),   C(KC_V),   KC_NO,                                       KC_MPRV,   KC_MPLY,   KC_MSTP,   KC_MNXT,   KC_PSCR,   PANIC,
       // └──────────┴──────────┴──────────┼──────────┼──────────┼──────────┼──────────┐           ┌──────────┼──────────┼──────────┼──────────┼──────────┴──────────┴──────────┘
       //                                  │  TRANS   │          │          │  MOUSE   │           │  MUTE    │  Esc     │  BSP     │  DEL     │
                                            KC_TRNS,   KC_NO,     KC_NO,     LA_MSE,                KC_MUTE,   KC_ESC,    KC_BSPC,   KC_DEL
       //                                  └──────────┴──────────┴──────────┴──────────┘           └──────────┴──────────┴──────────┴──────────┘
    ),
    [_FUNC] = LAYOUT_split_3x6_4( // Function layer
       // ┌──────────┬──────────┬──────────┬──────────┬──────────┬──────────┐                                 ┌──────────┬──────────┬──────────┬──────────┬──────────┬──────────┐
       // │  TRANS   │  TRANS   │  TRANS   │  TRANS   │  TRANS   │  TRANS   │                                 │  F12     │  F7      │  F8      │  F9      │  F15     │  F17     │
           KC_TRNS,   KC_TRNS,   KC_TRNS,   KC_TRNS,   KC_TRNS,   KC_TRNS,                                     KC_F12,    KC_F7,     KC_F8,     KC_F9,     KC_F15,    KC_F17,
       // ├──────────┼──────────┼──────────┼──────────┼──────────┼──────────┤                                 ├──────────┼──────────┼──────────┼──────────┼──────────┼──────────┤
       // │  TRANS   │  TRANS   │  TRANS   │  TRANS   │  TRANS   │  TRANS   │                                 │  F11     │  F4      │  F5      │  F6      │  F14     │  F16     │
           KC_TRNS,   KC_TRNS,   KC_TRNS,   KC_TRNS,   KC_TRNS,   KC_TRNS,                                     KC_F11,    KC_F4,     KC_F5,     KC_F6,     KC_F14,    KC_F16,
       // ├──────────┼──────────┼──────────┼──────────┼──────────┼──────────┤                                 ├──────────┼──────────┼──────────┼──────────┼──────────┼──────────┤
       // │  TRANS   │  TRANS   │  NKRO T  │ CONSOLE  │  TRANS   │  TRANS   │                                 │  F10     │  F1      │  F2      │  F3      │  F13     │  PANIC   │
           KC_TRNS,   KC_TRNS,   NK_TOGG,   DB_TOGG,   KC_TRNS,   KC_TRNS,                                     KC_F10,    KC_F1,     KC_F2,     KC_F3,     KC_F13,    PANIC,
       // └──────────┴──────────┴──────────┼──────────┼──────────┼──────────┼──────────┐           ┌──────────┼──────────┼──────────┼──────────┼──────────┴──────────┴──────────┘
       //                                  │  TRANS   │  TRANS   │  TRANS   │  MOUSE   │           │  MUTE    │  F18     │  F19     │  F20     │
                                            KC_TRNS,   KC_TRNS,   KC_TRNS,   LA_MSE,                KC_MUTE,   KC_F18,    KC_F19,    KC_F20
       //                                  └──────────┴──────────┴──────────┴──────────┘           └──────────┴──────────┴──────────┴──────────┘
    ),
    [_MEDIA] = LAYOUT_split_3x6_4( // Left Mouse and Media layer
       // ┌──────────┬──────────┬──────────┬──────────┬──────────┬──────────┐                                 ┌──────────┬──────────┬──────────┬──────────┬──────────┬──────────┐
       // │          │          │          │  M↑      │          │  PrtScn  │                                 │  Deafen  │   PREV   │  ↑       │   NEXT   │  PrtScn  │          │
           KC_NO,     KC_NO,     KC_NO,     KC_MS_U,   KC_NO,     KC_PSCR,                                     DEAFEN,    KC_MPRV,   KC_UP,     KC_MNXT,   KC_PSCR,   KC_NO,
       // ├──────────┼──────────├──────────┼──────────┼──────────┼──────────┤                                 ├──────────┤──────────┼──────────┼──────────┼──────────┤──────────┤
       // │          │  M2      │ M←       │  M↓      │  M→      │  SnipT   │                                 │  Mute    │  ←       │  ↓       │  →       │  SnipT   │          │
           KC_NO,     KC_BTN2,   KC_MS_L,   KC_MS_D,   KC_MS_R,   SNIP_T,                                      MUTE,      KC_LEFT,   KC_DOWN,   KC_RIGHT,  SNIP_T,    KC_NO,
       // ├──────────┼──────────┼──────────┼──────────┼──────────┼──────────┤                                 ├──────────┼──────────┼──────────┼──────────┼──────────┼──────────┤
       // │          │          │ ACCEL 3  │ ACCEL 2  │ ACCEL 1  │  M3      │                                 │          │          │  , <     │  . >     │          │  PANIC   │
           KC_NO,     KC_NO,     KC_ACL2,  KC_ACL1,   KC_ACL0,   KC_BTN3,                                      KC_NO,     KC_NO,     KC_COMM,   KC_DOT,    KC_NO,     PANIC,
       // └──────────┴──────────┴──────────┼──────────┼──────────┼──────────┼──────────┐           ┌──────────┼──────────┼──────────┼──────────┼──────────┴──────────┴──────────┘
       //                                  │  M4      │  M5      │  M1      │  PANIC   │           │  MUTE    │  Pause   │  Stop    │          │
                                            KC_BTN4,   KC_BTN5,   KC_BTN1,   PANIC,                 KC_MUTE,   KC_MPLY,   KC_MSTP,   KC_NO
       //                                  └──────────┴──────────┴──────────┴──────────┘           └──────────┴──────────┴──────────┴──────────┘
    ),
    [_DEV] = LAYOUT_split_3x6_4( // DEVELOPER layer
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
    [_MISC] = LAYOUT_split_3x6_4( // MISC layer for games and other stuff (no special functions here)
       // ┌──────────┬──────────┬──────────┬──────────┬──────────┬──────────┐                                 ┌──────────┬──────────┬──────────┬──────────┬──────────┬──────────┐
       // │          │          │          │          │          │          │                                 │          │          │  ↑       │          │          │          │
           KC_T,      KC_Y,      KC_Q,      KC_W,      KC_E,      KC_R,                                        KC_NO,     KC_NO,     KC_UP,     KC_NO,     KC_NO,     KC_NO,
       // ├──────────┼──────────┼──────────┼──────────┼──────────┼──────────┤                                 ├──────────┼──────────┼──────────┼──────────┼──────────┼──────────┤
       // │          │          │          │          │          │          │                                 │          │  ←       │  ↓       │  →       │  SnipT   │          │
           KC_G,      KC_H,      KC_A,      KC_S,      KC_D,      KC_F,                                        KC_NO,     KC_LEFT,   KC_DOWN,   KC_RIGHT,  SNIP_T,    KC_NO,
       // ├──────────┼──────────┼──────────┼──────────┼──────────┼──────────┤                                 ├──────────┼──────────┼──────────┼──────────┼──────────┼──────────┤
       // │          │          │          │          │          │          │                                 │          │          │          │          │  PrtScn  │  PANIC   │
           KC_B,      KC_N,      KC_Z,      KC_X,      KC_C,      KC_V,                                        KC_NO,     KC_NO,     KC_NO,     KC_NO,     KC_PSCR,   PANIC,
       // └──────────┴──────────┴──────────┼──────────┼──────────┼──────────┼──────────┐           ┌──────────┼──────────┼──────────┼──────────┼──────────┴──────────┴──────────┘
       //                                  │          │          │          │  MUTE    │           │  MUTE    │  Esc     │ Tab      │          │
                                            KC_J,      KC_K,      KC_L,      KC_MUTE,               KC_MUTE,   KC_ESC,    KC_TAB,    KC_NO
       //                                  └──────────┴──────────┴──────────┴──────────┘           └──────────┴──────────┴──────────┴──────────┘
    ),
    [_NO] = LAYOUT_split_3x6_4( // No Combos layer (Not for use)
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
                                            KC_NO,     Panic1,    KC_NO,     KC_NO,                 KC_NO,     KC_NO,     Panic2,    KC_NO
       //                                  └──────────┴──────────┴──────────┴──────────┘           └──────────┴──────────┴──────────┴──────────┘
    ),
};

// ROTARY ENCODER KEYMAP FOR LEFT AND RIGHT ENCODERS

#if defined(ENCODER_MAP_ENABLE)
const uint16_t PROGMEM encoder_map[][NUM_ENCODERS][NUM_DIRECTIONS] = {
    [_BASE] =   { ENCODER_CCW_CW(TAB_L, TAB_R), ENCODER_CCW_CW(KC_VOLU, KC_VOLD) },      // TAB LEFT RIGHT || VOL UP DOWN
    [_SYM] =   { ENCODER_CCW_CW(TAB_L, TAB_R), ENCODER_CCW_CW(KC_VOLU, KC_VOLD) },      // TAB LEFT RIGHT || VOL UP DOWN
    [_NUM] =   { ENCODER_CCW_CW(TAB_L, TAB_R), ENCODER_CCW_CW(KC_VOLU, KC_VOLD) },      // TAB LEFT RIGHT || VOL UP DOWN
    [_NAV] =   { ENCODER_CCW_CW(TAB_L, TAB_R), ENCODER_CCW_CW(TAB_CTRL_R, TAB_CTRL_L) },      // TAB LEFT RIGHT || TAB CTRL LEFT RIGHT
    [_FUNC] =   { ENCODER_CCW_CW(TAB_L, TAB_R), ENCODER_CCW_CW(KC_VOLU, KC_VOLD) },      // TAB LEFT RIGHT || VOL UP DOWN
    [_MEDIA] =   { ENCODER_CCW_CW(KC_WH_U, KC_WH_D), ENCODER_CCW_CW(KC_VOLU, KC_VOLD) },      // SCROLL UP DOWN || VOL UP DOWN
    [_MISC] =   { ENCODER_CCW_CW(KC_VOLU, KC_VOLD), ENCODER_CCW_CW(KC_VOLU, KC_VOLD) },      // SCROLL UP DOWN || VOL UP DOWN
    [_DEV] =   { ENCODER_CCW_CW(PANIC, PANIC), ENCODER_CCW_CW(PANIC, PANIC) },          //    PANIC PANIC || PANIC PANIC
    [_NO] =   { ENCODER_CCW_CW(KC_NO, KC_NO), ENCODER_CCW_CW(KC_NO, KC_NO) },          //          NO NO || NO NO
};
#endif

uint8_t combo_ref_from_layer(uint8_t layer){
    switch (get_highest_layer(layer_state)){
        case _BASE: return _BASE;
        case _DEV: return _DEV;
        default: return _NO;
    }
    return layer;  // important if default is not in case.
}

// bool combo_should_trigger(uint16_t combo_index, combo_t *combo, uint16_t keycode, keyrecord_t *record) {
//     /* Disable combo `COMB_PANIC` on layer `_LAYER_A` */
//     switch (combo_index) {
//         case COMB_PANIC:
//             if (!layer_state_is(8)) {
//                 return true;
//             }
//     }
//
//     return true;
// }

bool is_oneshot_cancel_key(uint16_t keycode) {
    switch (keycode) {
    case KC_ESC:
    case LA_SYM:
    case LA_NUM:
    case LA_NAV:
    case LA_FUNC:
    case LA_MSE:
    case LA_DEV:
    case LA_MISC:
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
    case LA_MISC:
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

    if (layer_state_is(_FUNC) && !layer_state_is(_NUM)) {
        layer_off(_FUNC); // disable function layer when numbers layer is turned off
    }

};

// layer_state_t layer_state_set_user(layer_state_t state) {
//     if (!layer_state_is(2) && layer_state_is(4)) {
//         layer_off(4); // disable function layer when numbers layer is turned off
//     }
//     return state;
// }
