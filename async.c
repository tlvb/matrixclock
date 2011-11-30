#include "async.h"

volatile uint8_t subsubminutes;
volatile uint16_t subminutes;

volatile uint8_t inhibit;
volatile uint8_t framebuffer[24];
volatile uint8_t mode;
volatile uint8_t column;
volatile uint8_t time[4];
volatile uint8_t time_p[4];
volatile uint8_t shift[4];
volatile uint8_t button_down_time[4];
volatile uint8_t alarm[4];
volatile uint8_t snooze_delay;

void async_setup(void) {
    TCCR1B = _BV(WGM12) | _BV(CS10);
    TIMSK1 = _BV(OCIE1A);
    OCR1AH = 0x0f;
    OCR1AL = 0xa0;
    inhibit = 0;
}

void async_enable_timekeeping(void) {
    inhibit = 0;
}

void async_inhibit_timekeeping(void) {
    inhibit = 1;
}

ISR(TIMER1_COMPA_vect) {

    if (++subsubminutes == 125) {
        subsubminutes = 0;
        if (++subminutes >= 2400) {
            subminutes = 0;
            if (!inhibit) {
                time_p[3] = time[3];
                shift[3] = 12;
                if (++time[3] == 10)
                    time[3] = 0;
            }
            if (snooze_delay > 0)
                --snooze_delay;
        }
        if (time[3] == 0 && shift[3] == (12-SHIFTLAG)) {
            time_p[2] = time[2];
            shift[2] = 12;
            if (++time[2] == 6) 
                time[2] = 0;
        }
        if (time[2] == 0 && shift[2] == (12-SHIFTLAG)) {
            time_p[1] = time[1];
            shift[1] = 12;
            ++time[1];
            if (time[1] == 10 || (time[0] == 2 && time[1] == 4))
                time[1] = 0;
        }
        if (time[1] == 0 && shift[1] == (12-SHIFTLAG)) {
            time_p[0] = time[0];
            shift[0] = 12;
            if (++time[0] == 3)
                time[0] = 0;
        }
        for (uint8_t i=0; i<4; ++i)
            if (shift[i] > 0)
                --shift[i];
        uint8_t b = io_buttons_read();
        for (uint8_t i=0; i<4; ++i) {
            if ((b&(1<<i)) == 0) {
                if (button_down_time[i] < 0xff)
                    ++button_down_time[i];
            }
            else {
                button_down_time[i] = 0;
            }
        }
    }

    if (++column == 24) {
        column = 0;
        display_seedcol();
    }
    else {
        display_nextcol();
    }

    display_write(framebuffer[column]);



}
