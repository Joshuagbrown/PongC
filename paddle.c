#include "system.h"
#include "pacer.h"
#include "navswitch.h"
#include "tinygl.h"
#include "paddle.h"
#include "ball.h"

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