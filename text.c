#include "pacer.h"
#include "tinygl.h"
#include "navswitch.h"
#include "../fonts/font3x5_1.h"
#include <stdint.h>

#include "display.h" 

void display_character (char character)
{
    char buffer[2];
    buffer[0] = character;
    buffer[1] = '\0';
    tinygl_text (buffer);
}

char char_in_bound(int8_t lowerBound, int8_t upperBound)
{
    char character = '1';
    while(1) {
        pacer_wait ();
        tinygl_update ();
        navswitch_update ();       
        if (navswitch_push_event_p (NAVSWITCH_NORTH)) {
            if(character < upperBound) {
                character++;
            }
        }     
        if (navswitch_push_event_p (NAVSWITCH_SOUTH)) {
            if(character > lowerBound) {
                character--;
            }
        }     
        if (navswitch_push_event_p(NAVSWITCH_PUSH)) {
            return(character);
        }
        display_character (character);
    }   
}

void display_text (void)
{
    while(1) {
        pacer_wait ();
        tinygl_update ();
        navswitch_update ();
        if (navswitch_push_event_p(NAVSWITCH_PUSH)) {
            tinygl_clear();
            return;
        }
    }
}