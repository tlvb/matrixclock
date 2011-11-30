#ifndef ASYNC_H
#define ASYNC_H

#include <stdint.h>
#include <avr/interrupt.h>

#include "display.h"
#include "io.h"

#define SHIFTLAG 8

extern volatile uint8_t subsubminutes;
extern volatile uint16_t subminutes;

extern volatile uint8_t framebuffer[24];
extern volatile uint8_t mode;
extern volatile uint8_t column;
extern volatile uint8_t time[4];
extern volatile uint8_t time_p[4];
extern volatile uint8_t shift[4];
extern volatile uint8_t button_down_time[4];
extern volatile uint8_t alarm[4];
extern volatile uint8_t snooze_delay;

void async_setup(void);
void async_enable_timekeeping(void);
void async_inhibit_timekeeping(void);

#endif
