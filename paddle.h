/**
    @file paddle.h
    @author Joshua Brown, Maxzi Francisco
    @brief paddle component for pong
    @date 15 October 2022
 */


#ifndef PADDLE_H
#define PADDLE_H

#include "ball.h"


/** Controls the movement of the paddle using tinygl and navswitch
    @param left points to left end of the paddle
    @param right points to tight end of the paddle
*/
void move_paddle(tinygl_point_t* left, tinygl_point_t* right);


/** Checks the state of game by the position of the ball
    @param ball ball on the screen
    @param left coordinates of left end of paddle
    @param right coordinates of right end of paddle
    @return PLAYING (1) or GAMEOVER (0) 
*/
int check_paddle(Ball_t* ball,tinygl_point_t left, tinygl_point_t right);


#endif //PADDLE_H