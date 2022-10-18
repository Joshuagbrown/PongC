/**
 * @file paddle.c
 * @author Joshua Brown, Maxzi Francisco
 * @brief paddle component for pong
 * @date 17 October 2022
 */

#include "system.h"
#include "pacer.h"
#include "navswitch.h"
#include "tinygl.h"
#include "paddle.h"
#include "ball.h"



/** Controls the movement of the paddle using tinygl and navswitch
    @param left points to left end of the paddle
    @param right points to tight end of the paddle
*/
void move_paddle(tinygl_point_t* left, tinygl_point_t* right)
{
    //Update the navswitch with the new state of line
    navswitch_update ();
    //Navswitch pushed South, move line south  
    if (navswitch_push_event_p (NAVSWITCH_SOUTH)) {
        //Turn the current line off
        tinygl_draw_line(*left, *right, 0);
        if((*right).y < 6) {
            //Set line to new position
            (*left).y ++;
            (*right).y ++;
        }       
    }     
    //Navswitch pushed North, move line north 
    if (navswitch_push_event_p (NAVSWITCH_NORTH)) {
        //Turn the current line off
        tinygl_draw_line(*left, *right, 0);
        if((*left).y > 0) {
            //Set line to new position
            (*left).y --;
            (*right).y --;
        }     
    }
    //Turn line on at new position
    tinygl_draw_line(*left, *right, 1);
}


/** Checks the state of game by the position of the ball
    @param ball ball on the screen
    @param left coordinates of left end of paddle
    @param right coordinates of right end of paddle
    @return PLAYING (1) or GAMEOVER (0) 
*/
int check_paddle(Ball_t* ball, tinygl_point_t left, tinygl_point_t right)
{
    //If ball hits the left side of the paddle, set the vy to left and vx flipped
    if((*ball).y == left.y) {
        (*ball).vy = -1;
        (*ball).vx = -1;
        return(PLAYING);

    //If ball hits the middle of the paddle, set the velocity to left
    } if ((*ball).y == left.y + 1) {
        (*ball).vy = 0;
        (*ball).vx = -1;
        return(PLAYING);

    //If ball hits the right, set vy to right, and vx flipped
    } if ((*ball).y == right.y) {
        (*ball).vy = 1;
        (*ball).vx = -1;
        return(PLAYING);
    }
    //If the ball is not hitting the paddle, return game over.
    return(GAMEOVER);
}