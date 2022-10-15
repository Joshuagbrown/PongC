/**
    @file ball.h
    @author your name (you@domain.com)
    @brief 
    @date 2022-10-15
 */


#include "system.h"
#include "tinygl.h"
#include "ir_serial.h"
#include "navswitch.h"
#include "pacer.h"

#include "ball.h"
#include "text.h"

Ball_t move_ball(int16_t* tick, Ball_t ball)
{
    tinygl_point_t ballPoint = {ball.x,ball.y};
    *tick = *tick + 1;
    tinygl_draw_point (ballPoint, 1);
    if (*tick >= 100) {
        *tick = 0;
        tinygl_draw_point (ballPoint, 0);
        ball.y += ball.vy;
        ball.x += ball.vx;
        
    }
    return(ball);
}

Ball_t reset_ball(void)
{
    Ball_t newBall = {0,3,1,0};
    return(newBall);
}

int check_wall(Ball_t* ball)
{
    if ((*ball).x >= 4)
    {
        (*ball).vx = -1;
    }
    if ((*ball).x < 0 && (*ball).vx < 0)
    {
        return(SENDING);
    }
    if (((*ball).y + (*ball).vy >= 7))
    {
        (*ball).vy = (*ball).vy * -1;

    }
    if (((*ball).y + (*ball).vy < 0)) {
        (*ball).vy = (*ball).vy * -1;
    }
    return(PLAYING);
}

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
    *retry = 0;
    if (ret == 1)
    {
        if (code == 128) {
            ir_serial_transmit(bit);
        }
    } 
    return(RECIEVING);
}


Ball_t wait_for_ball(int* state, int* retry)
{
    Ball_t newBall;
    ir_serial_ret_t ret = 3;
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
    } else if (retry == 0) {
        *retry = 1;
        ir_serial_transmit(128);
    }
    return(newBall);
}