/**
 * @file game.c
 * @author Joshua Brown, Francisco
 * @brief main function controls for the game
 * @date 17 October 2022
 */

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
#include "text.h"

/** Define pacer rate in Hz */
#define PACER_RATE 500
/** Define message rate */
#define MESSAGE_RATE 10
/** Defines maximum wait time */
#define MAX_WAIT 2000


/** Initialises functions */
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

/** Main program */
int main (void)
{
    int state = STARTUP;
    main_init();
    Ball_t ball = {0, 3, 1, 0};
    tinygl_point_t leftLine = {4, 2};
    tinygl_point_t rightLine = {4, 4};
    int16_t tick = 0;
    int16_t no_resp_time = 0;
    int retry = 0;
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
            state = pass_ball(ball, &retry);
            no_resp_time = 0;
            tinygl_point_t ballPoint = {(ball).x,(ball).y};
            tinygl_draw_point (ballPoint, 0);
        }

        if(state == PLAYING) {
            move_paddle(&leftLine, &rightLine);
            state = check_wall(&ball);
            ball = move_ball(&tick, ball);
            if(ball.x == 3) {
                state = check_paddle(&ball,leftLine, rightLine);
            }    
        }
        
        if(state == RECIEVING)
        {
            no_resp_time ++;
            move_paddle(&leftLine, &rightLine);
            ball = wait_for_ball(&state, &retry);
            if(no_resp_time > MAX_WAIT) {
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
            pass_ball(winBall, &retry);
            tinygl_text("LOST");
            display_text();
            state = RECIEVING;
        }
        pacer_wait();
        tinygl_update();
    }
}