#ifndef DISPLAY_H
#define DISPLAY_H

#include <avr/io.h>

/*
 * Data and control signals are segmented
 * over multiple ports, so it would make
 * little sense to have CONTROL_DDR etc
 * aliases...
 * The following three aliases are used
 * more because it makes the code more
 * readable, than it allows easy re-
 * configuration.
 */

#define ROWSEED PD2
#define ROWCLOCK PC5
#define ROWCLEAR PC4

/*
 * R0..3 on PC0..3
 * R4..7 on PD4..7
 */

void display_setup(void);       // setup ddr directions
void display_seedcol(void);     // feed a 0 to the 74xx164
void display_nextcol(void);     // feed a 1 to the 74xx164
void display_write(uint8_t c);  // write data to the data port

#endif
