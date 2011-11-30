#define F_CPU 20000000

#include <stdint.h>

#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>

#include "async.h"
#include "sync.h"
#include "io.h"
#include "display.h"
#include "graphics.h"


#define SHOW_TIME 0
#define SET_TIME 1
#define SET_ALARM 2
#define MENU 3
#define SHOW_TIME_ALARM 4
#define DEBUG 5


int main(void) {

    time[0] = 2; time[1] = 3; time[2] = 5; time[3] = 9;
    async_setup();
    display_setup();
    io_buttons_setup();
    io_buzzer_setup();
    sei();

    mode = SHOW_TIME;
    uint8_t pressstate = 0;
    uint8_t menustate = 0;
    uint8_t alarmstate = 0;
    uint8_t numbersel = 0;
    for (;;) {

        if (((alarmstate & ALARM_ON) != 0 && time[0]==alarm[0] && time[1]==alarm[1] && time[2]==alarm[2] && time[3]==alarm[3]) ||
                ((alarmstate & ALARM_SNOOZE) != 0 && snooze_delay == 0)) {
            mode = SHOW_TIME_ALARM;
        }

        switch (mode) {
            case SHOW_TIME:
                if (pressstate == 0 && button_down_time[3] >= 0x1f) {
                    pressstate |= 8;
                    mode = MENU;
                }
                else if (pressstate == 0 && button_down_time[0] >= 0x2f) {
                    pressstate |= 1;
                    mode = DEBUG;
                }
                sync_show_time(0);
                break;
            case SET_TIME:
                if (pressstate == 0) {
                    if (button_down_time[0] >= 0x1f) {
                        pressstate |= 1;
                        mode = MENU;
                        subsubminutes = 0;
                        subminutes = 0;
                        async_enable_timekeeping();
                    }
                    else if (button_down_time[1] >= 0x03) {
                        pressstate |= 2;
                        numbersel = (numbersel-1) & 3;
                    }
                    else if (button_down_time[2] >= 0x03) {
                        pressstate |= 4;
                        numbersel = (numbersel+1) & 3;
                    }
                    else if (button_down_time[3] >= 0x03) {
                        pressstate |= 8;
                        switch (numbersel) {
                            case 0:
                                if (++time[0] == 3)
                                    time[0] = 0;
                                if (time[0] == 2 && time[1] > 3)
                                    time[1] = 0;
                                break;
                            case 1:
                                ++time[1];
                                if (time[1] == 10 || (time[0] == 2 && time[1] > 3))
                                    time[1] = 0;
                                break;
                            case 2:
                                if (++time[2] == 6)
                                    time[2] = 0;
                                break;
                            case 3:
                                if (++time[3] == 10)
                                    time[3] = 0;
                                break;
                        }
                    }
                }
                sync_show_number_editor(time, numbersel);
                break;
            case SET_ALARM:
                if (pressstate == 0) {
                    if (button_down_time[0] >= 0x1f) {
                        pressstate |= 1;
                        mode = MENU;
                    }
                    else if (button_down_time[1] >= 0x03) {
                        pressstate |= 2;
                        numbersel = (numbersel-1) & 3;
                    }
                    else if (button_down_time[2] >= 0x03) {
                        pressstate |= 4;
                        numbersel = (numbersel+1) & 3;
                    }
                    else if (button_down_time[3] >= 0x03) {
                        pressstate |= 8;
                        switch (numbersel) {
                            case 0:
                                if (++alarm[0] == 3)
                                    alarm[0] = 0;
                                if (alarm[0] == 2 && alarm[1] > 3)
                                    alarm[1] = 0;
                                break;
                            case 1:
                                ++alarm[1];
                                if (alarm[1] == 10 || (alarm[0] == 2 && alarm[1] > 3))
                                    alarm[1] = 0;
                                break;
                            case 2:
                                if (++alarm[2] == 6)
                                    alarm[2] = 0;
                                break;
                            case 3:
                                if (++alarm[3] == 10)
                                    alarm[3] = 0;
                                break;
                        }
                    }
                }
                sync_show_number_editor(alarm, numbersel);
                break;
            case MENU:
                if (pressstate == 0) {
                    if (button_down_time[0] >= 0x1f) {
                        pressstate |= 1;
                        mode = SHOW_TIME;
                    }
                    else if (button_down_time[1] >= 0x0f) {
                        pressstate |= 2;
                        if (menustate == MENU_ALARM)
                            menustate = MENU_TIME;
                        else
                            menustate = MENU_ALARM;
                    }
                    else if (button_down_time[2] >= 0x07 &&
                            menustate == MENU_ALARM) {
                        pressstate |= 4;
                        if (alarmstate == ALARM_ON)
                            alarmstate = ALARM_OFF;
                        else
                            alarmstate = ALARM_ON;
                    }
                    else if (button_down_time[3] >= 0x07 &&
                            menustate == MENU_TIME) {
                        pressstate |= 8;
                        mode = SET_TIME;
                        async_inhibit_timekeeping();
                    }
                    else if (button_down_time[3] >= 0x0f &&
                            menustate == MENU_ALARM) {
                        pressstate |= 8;
                        mode = SET_ALARM;
                    }
                }
                sync_show_menu(menustate | alarmstate);
                break;
            case SHOW_TIME_ALARM:
                sync_show_time(1);
                io_buzzer_set((subminutes & 0x12) == 0x12);
                if (pressstate == 0 && button_down_time[0] >= 0x07) {
                    pressstate |= 1;
                    snooze_delay = 10;
                    alarmstate = ALARM_SNOOZE;
                    mode = SHOW_TIME;
                    io_buzzer_set(0);
                }
                break;
            default:
                sync_output_debuginfo();
                if (pressstate == 0 && button_down_time[3] >= 0xff) {
                    pressstate |= 8;
                    mode = SHOW_TIME;
                }
                break;
        }
        for (uint8_t i=0; i<4; ++i) {
            if (button_down_time[i] == 0)
                pressstate &=~ (1<<i);
        }

    }
}



