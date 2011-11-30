#include "sync.h"

void sync_clear_screen(void) {
    for (uint8_t i=0; i<24; ++i)
        framebuffer[i] = 0;
}

void sync_show_time(uint8_t alarm_blink) {

    for (uint8_t i=0; i<5; ++i) {

        uint8_t byte0, byte1;

        byte0 = pgm_read_byte(&font[time[0]*5+i]);
        byte1 = pgm_read_byte(&font[time_p[0]*5+i]);
        framebuffer[i] = (byte0>>shift[0])|(byte1<<(11-shift[0])) | (subminutes & 0x04 && alarm_blink?128:0);

        byte0 = pgm_read_byte(&font[time[1]*5+i]);
        byte1 = pgm_read_byte(&font[time_p[1]*5+i]);
        framebuffer[6+i] = (byte0>>shift[1])|(byte1<<(11-shift[1])) | (subminutes & 0x04 && alarm_blink?128:0);

        byte0 = pgm_read_byte(&font[time[2]*5+i]);
        byte1 = pgm_read_byte(&font[time_p[2]*5+i]);
        framebuffer[13+i] = (byte0>>shift[2])|(byte1<<(11-shift[2])) | (subminutes & 0x04 && alarm_blink?128:0);

        byte0 = pgm_read_byte(&font[time[3]*5+i]);
        byte1 = pgm_read_byte(&font[time_p[3]*5+i]);
        framebuffer[19+i] = (byte0>>shift[3])|(byte1<<(11-shift[3])) | (subminutes & 0x04 && alarm_blink?128:0);
    }
}

uint8_t munch_t = 0;
uint8_t munch_flag = 0;
void sync_output_debuginfo(void) {
    /*
    framebuffer[0] = time[0];
    framebuffer[1] = time[1];
    framebuffer[2] = time[2];
    framebuffer[3] = time[3];

    framebuffer[4] = time_p[0];
    framebuffer[5] = time_p[1];
    framebuffer[6] = time_p[2];
    framebuffer[7] = time_p[3];

    framebuffer[8] = shift[0];
    framebuffer[9] = shift[1];
    framebuffer[10] = shift[2];
    framebuffer[11] = shift[3];

    framebuffer[12] = subminutes >> 8;
    framebuffer[13] = subminutes & 0xff;
    framebuffer[14] = subsubminutes;

    uint8_t b = io_buttons_read();
    framebuffer[15] = b;
    if (b == 0)
        io_buzzer_set(1);
    else
        io_buzzer_set(0);
    if (b != 0xf) {
        framebuffer[16] = button_down_time[0];
        framebuffer[17] = button_down_time[1];
        framebuffer[18] = button_down_time[2];
        framebuffer[19] = button_down_time[3];
    }
    else {
    */
    if ((subminutes & 0x01) != 0) {
        if (munch_flag == 0) {
            munch_flag = 1;
            munch_t = (munch_t+1) & 7;
            for (uint8_t munch_x=0; munch_x<8; ++munch_x) {
                framebuffer[munch_x] ^= 1<<(munch_x^munch_t);
                framebuffer[8+munch_x] ^= (1<<(munch_x^munch_t));
                framebuffer[16+munch_x] ^= (1<<(munch_x^munch_t));
            }
        }
    }
    else {
        munch_flag = 0;
    }
}

void sync_show_menu(uint8_t state) {
    for (uint8_t i=0; i<8; ++i) {
        framebuffer[i] = pgm_read_byte(&symbols[i]);
        framebuffer[16+i] = 0;
    }
    framebuffer[8] = 0;
    switch (state & MENU_MASK) {
        case MENU_ALARM:
            if (((state & ALARM_ON) != 0) || subminutes & 0x10) {
                for (uint8_t i=0; i<7; ++i)
                    framebuffer[9+i] = pgm_read_byte(&symbols[15+i]);
            }
            else {
                if ((state & ALARM_SNOOZE) != 0) {
                    for (uint8_t i=0; i<7; ++i)
                        framebuffer[9+i] = 0;
                }
                else {
                    for (uint8_t i=0; i<7; ++i)
                        framebuffer[9+i] = pgm_read_byte(&symbols[22+i]);
                }
            }
            break;
        case MENU_TIME:
            for (uint8_t i=0; i<7; ++i)
                framebuffer[9+i] = pgm_read_byte(&symbols[8+i]);
            break;
    }
}

void sync_show_number_editor(volatile uint8_t *numbers, uint8_t select) {
    for (uint8_t i=0; i<5; ++i) {
        framebuffer[i] = pgm_read_byte(&font[numbers[0]*5+i]) | (subminutes & 0x04 && select==0?128:0);
        framebuffer[6+i] = pgm_read_byte(&font[numbers[1]*5+i]) | (subminutes & 0x04 && select==1?128:0);
        framebuffer[13+i] = pgm_read_byte(&font[numbers[2]*5+i]) | (subminutes & 0x04 && select==2?128:0);
        framebuffer[19+i] = pgm_read_byte(&font[numbers[3]*5+i]) | (subminutes & 0x04 && select==3?128:0);
    }
}
