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

/*The recv code check to send through again*/
#define RECV_CODE 123
/*Ball speed, set higher to slow ball down*/
#define BALL_SPEED 100


/** Initialises ball coordinates using tinygl
    @param tick timer
    @param ball initial position of the ball
    @return ball (with given coordinates)
*/
Ball_t move_ball(int16_t* tick, Ball_t ball)
{
    //Get the ball as a tinygl point
    tinygl_point_t ballPoint = {ball.x,ball.y};
    //Increase the tick
    *tick = *tick + 1;
    //Draw the ball a the position
    tinygl_draw_point (ballPoint, 1);
    //If tick is 100
    if (*tick >= BALL_SPEED) {
        //Reset the tick
        *tick = 0;
        //Turn ball off
        tinygl_draw_point (ballPoint, 0);
        //Increase the x and y by vx and vy respetivly
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
    //This prevents the ball from being glitched past the paddle
    if ((*ball).x >= 4)
    {
        (*ball).vx = -1;
    }
    //If the ball is at the top of the screen and its next state is past the screen, set state to sending
    if ((*ball).x < 0 && (*ball).vx < 0)
    {
    //Confirms ball is transmitting
        return(SENDING);
    }
    //If ball hits wall switch velocity
    if (((*ball).y + (*ball).vy >= 7))
    {
        (*ball).vy = (*ball).vy * -1;

    }
    //If ball hits wall switch velocity
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
    ir_serial_ret_t ret = 0;
    int vy = 0;
    uint8_t bit;
    //Set the values of vy to be positive, and therefore transferable via a bit
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

        //Win ball case
        case 5:
        vy = 5;
        break;
    }
    //Turn the data into a bit
    bit = (vy<<3) | (ball.y);
    //Transmit the data
    ir_serial_transmit(bit);
    //Recieve a code if non transmitted
    uint8_t code = 0;
    ret = ir_serial_receive(&code);
    //If ball fails to transmit, retry once again
    *retry = 0;
    //if recieved data
    if (ret == 1)
    {
        //if code is the unrecieved code
        if (code == RECV_CODE) {
            //retry
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
    //recieve the data
    ret = ir_serial_receive(&data);
    pacer_wait();
    //If data code is confirmed
    if(ret == 1) {
        //Set the ball values to the correct values
        newBall.vx = 1;
        newBall.x = 0;
        //Process the data
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

            //Win ball case
            case 5:
            //Clear the ball
            newBall = reset_ball();
            tinygl_clear();
            //Send win
            tinygl_text("WIN");
            display_text();
            //If clicks down sends back a reset ball
            break; 

            default:
            newBall.vy = 0;
        }
        *state = PLAYING;
    } else if (retry == 0) {
        *retry = 1;
        //Resend code if hasnt retried since sending
        ir_serial_transmit(RECV_CODE);
    }
    return(newBall);
}