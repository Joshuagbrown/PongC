/**
 * @file game.c
 * @author Joshua Brown, Maxzi Francisco
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
/**Definitions in ascii*/
#define ONE 49
#define TWO 50


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
    //set the initial state
    int state = STARTUP;
    main_init();
    //Intitialise ball with X,Y,VX,VY
    Ball_t ball = {0, 3, 1, 0};
    //Initiate the left and right side of the line. TinyGL draws lines with a left and right point
    tinygl_point_t leftLine = {4, 2};
    tinygl_point_t rightLine = {4, 4};
    //Ball move tick
    int16_t tick = 0;
    int16_t no_resp_time = 0;
    //Retry the send intialisation
    int retry = 0;
    while (1)
    {
        //Startup state, can go back here to restart game
        if(state == STARTUP) {
            //Clear Screen
            tinygl_clear();
            //Set screen text to pong
            tinygl_text("PONG");
            //Call display text function
            display_text();
            //Get character between 1 and 2, 49 = 1 in ascii, 50 = 2 in ascii. Player 1 or 2
            if(char_in_bound(ONE,TWO) == '1') {
                //If player 1 start playing
                state = PLAYING;
            } else {
                //If player 2 start recieving
                state = RECIEVING;
            }
            //Clear screen to get ready for game
            tinygl_clear();
        } 

        //Ball has hit far wall and being sent
        if(state == SENDING) {
            //Run move paddle while sending, allows player to continue to play game when ball off screen
            move_paddle(&leftLine, &rightLine);
            //send the actual ball
            state = pass_ball(ball, &retry);
            //Set resp time to 0, for ball lost during transmission
            no_resp_time = 0;
            //Get ball position
            tinygl_point_t ballPoint = {(ball).x,(ball).y};
            //set ball to off
            tinygl_draw_point (ballPoint, 0);
        }

        //Ball has been recieved or game has been started at player 1
        if(state == PLAYING) {
            //Move the paddle
            move_paddle(&leftLine, &rightLine);
            //Check if the ball is at the wall
            state = check_wall(&ball);
            //Call the move ball function
            ball = move_ball(&tick, ball);
            //If ball at paddle position check if going to go out
            if(ball.x == 3) {
                state = check_paddle(&ball,leftLine, rightLine);
            }    
        }
        
        //Ball has been sent and waiting for ball
        if(state == RECIEVING)
        {
            //Increase the no response time, for lost ball
            no_resp_time ++;
            //Call move paddle function
            move_paddle(&leftLine, &rightLine);
            //Try to revcieve ball over IR
            ball = wait_for_ball(&state, &retry);
            //If response time is too high, Restart game and display lost ball
            if(no_resp_time > MAX_WAIT) {
                //Reset the no response time
                no_resp_time = 0;
                //Clear the screen
                tinygl_clear();
                //Set text to NO BALL
                tinygl_text("NO BALL");
                display_text();
                //Reset the ball to intial position
                ball = reset_ball();
                //Set state back to startup
                state = STARTUP;
            }
        }

        //Game is over, Ball went past paddle
        if(state == GAMEOVER)
        {
            //Clear the screen
            tinygl_clear();
            //Initiate the information to send to the other control that it has won
            Ball_t winBall = {0,0,0,5};
            //Send the winning information
            pass_ball(winBall, &retry);
            //Set text to lost
            tinygl_text("LOST");
            display_text();
            //If the player clicks down again they are recieving
            state = RECIEVING;
        }
        pacer_wait();
        tinygl_update();
    }
}
