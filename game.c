#include "system.h"
#include "pacer.h"
#include "navswitch.h"
#include "ir_uart.h"
#include "tinygl.h"
#include "../fonts/font5x7_1.h"
#include "boing.h"


#define PACER_RATE 500
#define MESSAGE_RATE 10

typedef struct {
    int8_t velocity;
    int8_t x;
    int8_t y;
} Ball_t;

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

void welcome_screen (void)
{
    tinygl_text ("Pong, press down to start");
    tinygl_text_mode_set (TINYGL_TEXT_MODE_SCROLL);
    while(1) {
        pacer_wait ();
        tinygl_update ();
        navswitch_update ();
        if (navswitch_push_event_p(NAVSWITCH_PUSH)) {
            return;
        }
    }
}

boing_state_t move_ball(int* tick, boing_state_t ballState)
{
    *tick = *tick + 1;
    tinygl_draw_point (ballState.pos, 1);
    if (*tick >= 40) {
        *tick = 0;
        tinygl_draw_point (ballState.pos, 0);
        ballState = boing_update(ballState);
    }
    tinygl_update();
    return(ballState);
}

int main (void)
{
    system_init ();
    tinygl_init (PACER_RATE);
    tinygl_font_set (&font5x7_1);
    tinygl_text_speed_set (MESSAGE_RATE);
    navswitch_init ();
    ir_uart_init ();
    pacer_init (PACER_RATE);
    
    //start welcome screen
    welcome_screen();
    //get maxscore
    int8_t lowerScoreBound = 49;
    int8_t upperScoreBound = 57;

    char maxScore = char_in_bound(lowerScoreBound, upperScoreBound);
    maxScore = maxScore;
    tinygl_clear();
    tinygl_clear();
    boing_state_t ballState = boing_init(1,0,DIR_NE);
    int tick = 0;
    while (1)
    {
        ballState = move_ball(&tick, ballState);
        pacer_wait();
    }
}