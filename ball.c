/**
 * @file ball.c
 * @author Joshua Brown, Maxzi Francisco
 * @brief ball component of the game
 * @date 17 October 2022
 */

#include "system.h"
#include "tinygl.h"
#include "ir_serial.h"
#include "navswitch.h"
#include "pacer.h"

#include "ball.h"
#include "text.h"

//Bit for recieve and send transmission
#define RECV_CODE = 123


/** Initialises ball coordinates using tinygl
    @param tick timer
    @param ball initial position of the ball
    @return ball (with given coordinates)
*/
Ball_t move_ball(int16_t* tick, Ball_t ball)
{
    //Set struct for ball
    tinygl_point_t ballPoint = {ball.x,ball.y};
    //Increments tick 
    *tick = *tick + 1;
    //Initialise ball on LED matrix
    tinygl_draw_point (ballPoint, 1);
    //Checks if tick within bounds
    if (*tick >= 100) {
        *tick = 0;
        //Set new position of ball
        tinygl_draw_point (ballPoint, 0);
        ball.y += ball.vy;
        ball.x += ball.vx;
        
    }
    //Returns ball
    return(ball);
}


/** Makes a new ball that's set to the default position
    @return ball (with given coordinates)
*/
Ball_t reset_ball(void)
{
    //Make new ball in the stating position
    Ball_t newBall = {0,3,1,0};
    return(newBall);
}


/** Checks the position of the ball, if it hits a wall or if it needs to be transmitted to the other player's microcontoller 
    @param ball the position of the ball
    @return PLAYING (1) or SENDING (0)
*/
int check_wall(Ball_t* ball)
{
    //Prevents glitches during transmission, if ball is recieved past paddle, doesnt break the game
    if ((*ball).x >= 4)
    {
        (*ball).vx = -1;
    }
    if ((*ball).x < 0 && (*ball).vx < 0)
    {
    //Confirms ball is transmitting
        return(SENDING);
    }
    //Prevents glitches during transmission, if ball is recieved past paddle, doesnt break the game
    if (((*ball).y + (*ball).vy >= 7))
    {
        (*ball).vy = (*ball).vy * -1;

    }
    if (((*ball).y + (*ball).vy < 0)) {
        (*ball).vy = (*ball).vy * -1;
    }
    //Confrims game is still ongoing
    return(PLAYING);
}



/** Sends ball to the other player and sets a retry
    @param ball ball coordinates
    @param retry to transmit data one more time
    @return receiving state
*/
int pass_ball(Ball_t ball, int* retry)
{
    ir_serial_ret_t ret = 3;
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
    uint8_t code = 0;
    ret = ir_serial_receive(&code);
    //If ball fails to transmit, retry once again
    *retry = 0;
    if (ret == 1)
    {
    //Transmits the ball
        if (code == 128) {
            ir_serial_transmit(bit);
        }
    } 
    //Set state as receiving
    return(RECIEVING);
}


/** Receives ball transmission while the game is ongoing and makes a new ball if the game is over
    @param state current state of the game
    @param retry to transmit data one more time
    @return returns a new ball for the retry
*/
Ball_t wait_for_ball(int* state, int* retry)
{
    //Initialises a ball
    Ball_t newBall;
    //Initialises IR transmission
    ir_serial_ret_t ret = 3;
    //Initialises data
    uint8_t data;
    //Set IR trasmission
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
    } else if (retry == 0) {
        *retry = 1;
        ir_serial_transmit(128);
    }
    return(newBall);
}