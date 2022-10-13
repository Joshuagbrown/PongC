#include "system.h"
#include "pacer.h"
#include "navswitch.h"
#include "timer.h"
#include "timer0.h"
#include "prescale.h"
#include "tinygl.h"
#include "../fonts/font5x7_1.h"
#include "ir_serial.h"
#include "ball.h"
#include "paddle.h"

#define PACER_RATE 500
#define MESSAGE_RATE 10
#define RECV_CODE = 123


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

int pass_ball(Ball_t ball)
{
    int isPositive = 1;
    if (ball.vy == -1) {
        isPositive = 2;
    }
    ir_serial_transmit((isPositive*10)+(ball.y));
    return(RECIEVING);
}

Ball_t wait_for_ball(int* state)
{
    Ball_t newBall;
    ir_serial_ret_t ret = 0;
    uint8_t data;
    ret = ir_serial_receive(&data);
    pacer_wait();
    if(ret == 1) {
        newBall.y = 0;
        newBall.x = 0;
        newBall.vx = 1;
        newBall.vy = 1;
        while(data % 10 != 0)
        {
            data --;
            newBall.y ++;
        }
        //set it to the correct side;
        newBall.y = 6 - newBall.y;
        data = data / 10;
        int isPositive = data;
        if(isPositive == 1)
        {
            newBall.vy = -1;
        }
        *state = PLAYING;
    }
    return(newBall);
}

void main_init(void)
{
    system_init ();
    tinygl_init (PACER_RATE);
    tinygl_font_set (&font5x7_1);
    tinygl_text_speed_set (MESSAGE_RATE);
    navswitch_init ();
    pacer_init (PACER_RATE);
    ir_serial_init ();
}

int main (void)
{
    int state = STARTUP;
    main_init();
    welcome_screen();
    char maxScore = char_in_bound(49, 57);
    maxScore = maxScore;
    tinygl_clear();
    Ball_t ball = {2,3, 1,1};
    tinygl_point_t leftLine = {4,2};
    tinygl_point_t rightLine = {4,4};
    int16_t tick = 0;
    while (1)
    {
        if(state == STARTUP) {
            if(char_in_bound(49,50) == '1') {
                state = PLAYING;
            } else {
                state = RECIEVING;
            }
            tinygl_clear();
        } else if(state == SENDING) {
            state = pass_ball(ball);
            ball = reset_ball(ball);
            ball.vx = 0;
            ball.vy = 0;
        } else if(state == PLAYING) {
            state = check_wall(&ball,tick);
            ball = move_ball(&tick, ball);
            if(ball.x == 3) {
                state = check_paddle(&ball,leftLine, rightLine);
                if(state == PLAYING) {
                    ball.vx = -1;
                } else {
                    state = PLAYING;
                    ball.vx = -1;
                }
            }    
        }
        
        if(state == RECIEVING)
        {
            ball = wait_for_ball(&state);
        }
        move_paddle(&leftLine, &rightLine);
        pacer_wait();
        tinygl_update();
    }
}