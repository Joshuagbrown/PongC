/**
 * @file ball.h
 * @author Joshua Brown, Maxzi Francisco
 * @brief ball component of the game
 * @date 17 October 2022
 */


#ifndef BALL_H
#define BALL_H


/** Directions of the ball as coordinates */
typedef struct {
    int8_t x;
    int8_t y;
    int8_t vx;
    int8_t vy;
} Ball_t;


/** States of the game*/
typedef enum {
    STARTUP,
    SENDING,
    RECIEVING,
    PLAYING,
    GAMEOVER
} state;


/** Initialises ball coordinates using tinygl
    @param tick timer
    @param ball initial position of the ball
    @return ball (with given coordinates)
*/
Ball_t move_ball(int16_t* tick, Ball_t ball);


/** Makes a new ball that's set to the default position
    @return ball (with given coordinates)
*/
Ball_t reset_ball(void);


/** Checks the position of the ball, if it hits a wall or if it needs to be transmitted to the other player's microcontoller 
    @param ball the position of the ball
    @return PLAYING (1) or SENDING (0)
*/
int check_wall(Ball_t* ball);


/** Sends ball to the other player and sets a retry
    @param ball ball coordinates
    @param retry to transmit data one more time
    @return receiving state
*/
int pass_ball(Ball_t ball, int* retry);


/** Receives ball transmission while the game is ongoing and makes a new ball if the game is over
    @param state current state of the game
    @param retry to transmit data one more time
    @return returns a new ball for the retry
*/
Ball_t wait_for_ball(int* state, int* retry);


#endif //BALL_H