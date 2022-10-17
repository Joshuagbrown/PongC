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
    navswitch_update ();       
    if (navswitch_push_event_p (NAVSWITCH_SOUTH)) {
        tinygl_draw_line(*left, *right, 0);
        if((*right).y < 6) {
            (*left).y ++;
            (*right).y ++;
        }       
    }     
    if (navswitch_push_event_p (NAVSWITCH_NORTH)) {
        tinygl_draw_line(*left, *right, 0);
        if((*left).y > 0) {
            (*left).y --;
            (*right).y --;
        }     
    }
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
    if((*ball).y == left.y) {
        (*ball).vy = -1;
        (*ball).vx = -1;
        return(PLAYING);

    } if ((*ball).y == left.y + 1) {
        (*ball).vy = 0;
        (*ball).vx = -1;
        return(PLAYING);

    } if ((*ball).y == right.y) {
        (*ball).vy = 1;
        (*ball).vx = -1;
        return(PLAYING);
    }
    return(GAMEOVER);
}