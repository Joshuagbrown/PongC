#include "system.h"
#include "pacer.h"
#include "navswitch.h"
#include "ir_uart.h"
#include "tinygl.h"
#include "../fonts/font5x7_1.h"


#define PACER_RATE 500
#define MESSAGE_RATE 10

typedef struct {
    int8_t x;
    int8_t y;
    int8_t vx;
    int8_t vy;
} Ball_t;

void display_character (char character)
{
    char buffer[2];
    buffer[0] = character;
    buffer[1] = '\0';
    tinygl_text (buffer);
}
char char_in_bound(int8_t lowerBound, int8_t upperBound)
{
    char character = '1';
    while(1) {
        pacer_wait ();
        tinygl_update ();
        navswitch_update ();       
        if (navswitch_push_event_p (NAVSWITCH_NORTH)) {
            if(character < upperBound) {
                character++;
            }
        }     
        if (navswitch_push_event_p (NAVSWITCH_SOUTH)) {
            if(character > lowerBound) {
                character--;
            }
        }     
        if (navswitch_push_event_p(NAVSWITCH_PUSH)) {
            return(character);
        }
        display_character (character);
    }   
}

void welcome_screen (void)
{
    tinygl_text ("Pong, press down to start");
    tinygl_text_mode_set (TINYGL_TEXT_MODE_SCROLL);
    while(1) {
        pacer_wait ();
        tinygl_update ();
        navswitch_update ();
        if (navswitch_push_event_p(NAVSWITCH_PUSH)) {
            return;
        }
    }
}

void display_score(char score)
{
    while(1) {
        display_character(score);
        pacer_wait ();
        tinygl_update ();
        navswitch_update ();
        if(navswitch_push_event_p(NAVSWITCH_PUSH)) {
            return;
        }
    }
}



Ball_t move_ball(int16_t* tick, Ball_t ball)
{
    tinygl_point_t ballPoint = {ball.x,ball.y};
    *tick = *tick + 1;
    tinygl_draw_point (ballPoint, 1);
    if (*tick >= 100) {
        *tick = 0;
        tinygl_draw_point (ballPoint, 0);
        ball.x += ball.vx;
        ball.y += ball.vy;
    }
    return(ball);
}

void move_line(tinygl_point_t* left, tinygl_point_t* right)
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

Ball_t reverse_direction(Ball_t ball)
{
    ball.vx *= -1;
    ball.vy *= -1;
    return(ball);
}
Ball_t check_wall(Ball_t ball)
{
    if (ball.x >= 4)
    {
        ball.vx = -1;
        //ball.x += 2;
    }
    if (ball.x <= 0)
    {
        ball.vx = 1;
        //ball.x += 2;
    }
    if (ball.y >= 6)
    {
        ball.vy = -1;
        //ball.y += 2;
    }
    if (ball.y <= 0) {
        ball.vy = -1;
        //ball.y += 2;
    }
    return(ball);
}
int check_collision(Ball_t* ball,tinygl_point_t* left, tinygl_point_t* right)
{
    *ball = check_wall(*ball);
    int8_t ballX = (*ball).x;
    int8_t ballY = (*ball).y;
    if(ballX == 10) {
        if(ballY == (*left).y || ballY == ((*left).y + 1) || ballY == (*right).y) {
            *ball = reverse_direction(*ball);
        } else {
            return(-1);
        }
    }
    return(1);
}


int main (void)
{
    system_init ();
    tinygl_init (PACER_RATE);
    tinygl_font_set (&font5x7_1);
    tinygl_text_speed_set (MESSAGE_RATE);
    navswitch_init ();
    ir_uart_init ();
    pacer_init (PACER_RATE);
    //start welcome screen
    //welcome_screen();
    //get maxscore
    // int8_t lowerScoreBound = 49;
    // int8_t upperScoreBound = 57;
    // char maxScore = char_in_bound(lowerScoreBound, upperScoreBound);
    // maxScore = maxScore;
    tinygl_clear();
    Ball_t ball = {1,1,1,1};
    tinygl_point_t leftLine = {4,2};
    tinygl_point_t rightLine = {4,4};
    int16_t tick = 0;
    // char score = '0';
    int8_t collision = 1;
    while (1)
    {
        ball = move_ball(&tick, ball);
        collision = check_collision(&ball, &leftLine, &rightLine);
        // if(collision == -1) {
        //     if(score < maxScore)
        //     {
        //         score ++;
        //         display_score(score);
        //         collision = 1;
        //     } else {
        //         break;
        //     }
        // }
        move_line(&leftLine, &rightLine);
        pacer_wait();
        tinygl_update();
    }
}