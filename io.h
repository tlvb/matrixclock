#ifndef IO_H
#define IO_H

#include <avr/io.h>

void io_buttons_setup(void);
uint8_t io_buttons_read(void);
void io_buzzer_setup(void);
void io_buzzer_set(uint8_t beep);

#endif
