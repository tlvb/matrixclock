#ifndef SYNC_H
#define SYNC_H

#include <avr/pgmspace.h>

#include "async.h"
#include "graphics.h"
#include "io.h"

#define MENU_ALARM 0
#define MENU_TIME 1
#define MENU_MASK 1
#define ALARM_OFF 0
#define ALARM_ON 2
#define ALARM_SNOOZE 4


void sync_show_time(uint8_t alarm_blink);
void sync_output_debuginfo(void);
void sync_show_menu(uint8_t state);
void sync_show_number_editor(volatile uint8_t *numbers, uint8_t select);

#endif
