/**
 * @file ball.h
 * @author your name (you@domain.com)
 * @brief 
 * @version 0.1
 * @date 2022-10-15
 * 
 * @copyright Copyright (c) 2022
 * 
 */

#ifndef BALL_H
#define BALL_H

typedef struct {
    int8_t x;
    int8_t y;
    int8_t vx;
    int8_t vy;
} Ball_t;

typedef enum {
    STARTUP,
    SENDING,
    RECIEVING,
    PLAYING,
    GAMEOVER
} state;

/** Initialises ball coordinates using tinygl
    @param tick
    @param ball
    @return 
*/
Ball_t move_ball(int16_t* tick, Ball_t ball);



Ball_t reset_ball(void);
/** Returns 
    @param ball the position of the ball
    @return PLAYING (1) or SENDING (0)
*/
int check_wall(Ball_t* ball);
/** 
    @param ball
    @param retry
    @return
*/
int pass_ball(Ball_t ball, int* retry);

/** 
    @param state
    @param retry
    @return
*/
Ball_t wait_for_ball(int* state, int* retry);


#endif