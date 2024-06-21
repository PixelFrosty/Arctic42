// Copyright 2023 Ethan Gretna (@PixelFrosty)
// SPDX-License-Identifier: GPL-2.0-or-later

#pragma once

// LEFT MCU HAS TAPE  ON IT !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!

/*
 * Feature disable options
 *  These options are also useful to firmware size reduction.
 */

/* disable debug print */
//#define NO_DEBUG

/* disable print */
//#define NO_PRINT

/* disable action features */
//#define NO_ACTION_LAYER
//#define NO_ACTION_TAPPING
//#define NO_ACTION_ONESHOT

#define MATRIX_ROW_PINS \
    { GP29, GP28, GP27, GP26 }

// wiring of each half
#define MATRIX_COL_PINS \
    { GP1, GP2, GP3, GP4, GP5, GP6 }

#define FORCE_NKRO

#define SERIAL_USART_FULL_DUPLEX   // Enable full duplex operation mode.
#define SERIAL_USART_TX_PIN GP8     // USART TX pin
#define SERIAL_USART_RX_PIN GP9     // USART RX pin
#define SPLIT_MAX_CONNECTION_ERRORS 10 //This sets the maximum number of failed communication attempts (one per scan cycle)
                                       //from the master part before it assumes that no slave part is connected.
                                       //This makes it possible to use a master part without the slave part connected.

#define EE_HANDS

#define SPLIT_MODS_ENABLE
#define SPLIT_LAYER_STATE_ENABLE
#define SPLIT_ACTIVITY_ENABLE
#define SPLIT_LAYER_STATE_ENABLE
#define SPLIT_OLED_ENABLE
#define SPLIT_TRANSPORT_MIRROR


#define RP2040_BOOTLOADER_DOUBLE_TAP_RESET // Activates the double-tap behavior
#define RP2040_BOOTLOADER_DOUBLE_TAP_RESET_TIMEOUT 200U // Timeout window in ms in which the double tap can occur.
#define BOOTMAGIC_LITE_ROW 0
#define BOOTMAGIC_LITE_COLUMN 5
#define BOOTMAGIC_LITE_ROW_RIGHT 4
#define BOOTMAGIC_LITE_COLUMN_RIGHT 5


#define ONESHOT_TAP_TOGGLE 2  // Tapping this number of times holds the key until tapped once again.
#define ONESHOT_TIMEOUT 2500  // Time (in ms) before the one shot key is released
#define MK_KINETIC_SPEED

#define ENCODERS_PAD_A { GP14 }
#define ENCODERS_PAD_B { GP15 }
#define ENCODERS_PAD_A_RIGHT { GP14 }
#define ENCODERS_PAD_B_RIGHT { GP15 }
#define ENCODER_MAP_KEY_DELAY 10
#define ENCODER_RESOLUTION 4




