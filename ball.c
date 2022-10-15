#include "system.h"
#include "tinygl.h"
#include "ir_serial.h"
#include "ball.h"

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

int check_wall(Ball_t* ball, int16_t tick)
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

