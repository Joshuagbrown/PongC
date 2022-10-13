#include "system.h"
#include "pacer.h"
#include "navswitch.h"
#include "timer.h"
#include "timer0.h"
#include "prescale.h"
#include "tinygl.h"
#include "../fonts/font3x5_1.h"
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

int pass_ball(Ball_t ball)
{
    int vy = 2;
    uint8_t bit;
    switch(ball.vy)
    {
        case -1:
        vy = 1;
        break;

        case 0:
        vy = 2;
        break;

        case 1:
        vy = 3;
        break;

        case 5:
        vy = 5;
        break;
    }
    bit = (vy<<3) | (ball.y);
    ir_serial_transmit(bit);
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
        newBall.vx = 1;
        newBall.x = 0;
        newBall.y = 6 - (data & 0b00000111);
        switch((data & 0b00111000) >> 3)
        {
            case 1:
            newBall.vy = 1;
            break;

            case 2:
            newBall.vy = 0;
            break;

            case 3:
            newBall.vy = -1;
            break;

            case 5:
            newBall = reset_ball();
            tinygl_clear();
            tinygl_text("WIN");
            display_text();
            break; 

            default:
            newBall.vy = 0;
        }
        *state = PLAYING;
    }
    return(newBall);
}

void main_init(void)
{
    system_init ();
    tinygl_init (PACER_RATE);
    tinygl_font_set (&font3x5_1);
    tinygl_text_speed_set (MESSAGE_RATE);
    tinygl_text_dir_set (TINYGL_TEXT_DIR_ROTATE);
    tinygl_text_mode_set(TINYGL_TEXT_MODE_SCROLL);
    navswitch_init ();
    pacer_init (PACER_RATE);
    ir_serial_init ();
}

int main (void)
{
    int state = STARTUP;
    main_init();
    Ball_t ball = {0, 3, 1, 0};
    tinygl_point_t leftLine = {4, 2};
    tinygl_point_t rightLine = {4, 4};
    int16_t tick = 0;
    int16_t no_resp_time = 0;
    while (1)
    {
        if(state == STARTUP) {
            tinygl_clear();
            tinygl_text("PONG");
            display_text();
            if(char_in_bound(49,50) == '1') {
                state = PLAYING;
            } else {
                state = RECIEVING;
            }
            tinygl_clear();
        } 

        if(state == SENDING) {
            move_paddle(&leftLine, &rightLine);
            state = pass_ball(ball);
            no_resp_time = 0;
            tinygl_point_t ballPoint = {(ball).x,(ball).y};
            tinygl_draw_point (ballPoint, 0);
        }

        if(state == PLAYING) {
            move_paddle(&leftLine, &rightLine);
            state = check_wall(&ball,tick);
            ball = move_ball(&tick, ball);
            if(ball.x == 3) {
                state = check_paddle(&ball,leftLine, rightLine);
            }    
        }
        
        if(state == RECIEVING)
        {
            no_resp_time ++;
            move_paddle(&leftLine, &rightLine);
            ball = wait_for_ball(&state);
            if(no_resp_time > 1100) {
                no_resp_time = 0;
                tinygl_clear();
                tinygl_text("NO BALL");
                display_text();
                ball = reset_ball();
                state = STARTUP;
            }
        }

        if(state == GAMEOVER)
        {
            tinygl_clear();
            Ball_t winBall = {0,0,0,5};
            pass_ball(winBall);
            tinygl_text("LOST");
            display_text();
            state = RECIEVING;
        }
        pacer_wait();
        tinygl_update();
    }
}