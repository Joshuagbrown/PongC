#include "system.h"
#include "pacer.h"
#include "navswitch.h"
#include "ir_uart.h"
#include "tinygl.h"
#include "../fonts/font5x7_1.h"
#include "ir_serial.h"

#define PACER_RATE 400
#define MESSAGE_RATE 10
#define RECV_CODE = 123

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
            tinygl_clear();
            return;
        }
    }
}

void display_end(char score)
{
    display_character(score);
    pacer_wait();
    tinygl_update ();
    navswitch_update ();
    tinygl_clear();
    tinygl_text ("Game Over");
    while(1)
    {
        tinygl_update ();
        pacer_wait();
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

Ball_t reset_ball(Ball_t ball)
{
    ball.x = 1;
    ball.y = 1;
    ball.vx = 1;
    ball.vx = 1;
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

void pass_ball(Ball_t ball)
{
    int isPositive = 1;
    if (ball.vy == -1) {
        isPositive = 2;
    }
    ir_serial_transmit((isPositive*10)+(ball.y));
}

Ball_t wait_for_ball(int* sent)
{
    pacer_wait();
    Ball_t newBall;
    ir_serial_ret_t ret = 0;
    uint8_t data;
    ret = ir_serial_receive(&data);
    if(ret == 1) {
        newBall.y = 0;
        newBall.x = 0;
        newBall.vx = 1;
        newBall.vy = 1;
        *sent = 0;
        while(data % 10 != 0)
        {
            data --;
            newBall.y ++;
        }
        //set it to the correct side;
        newBall.y = 6 - newBall.y;
        data = data / 10;
        int isPositive = data;
        if(isPositive == 1)
        {
            newBall.vy = -1;
        }
    }
    
    return(newBall);

}

int check_wall(Ball_t* ball, int sent)
{
    tinygl_point_t ballPoint = {(*ball).x,(*ball).y};
    if ((*ball).x >= 4)
    {
        (*ball).vx = -1;
    }
    if ((*ball).x <= 0 && (*ball).vx == -1)
    {
        pass_ball((*ball));
        *ball = reset_ball(*ball);
        (*ball).vx = 0;
        (*ball).vy = 0;
        sent = 1;
    }
    if ((*ball).y >= 6)
    {
        (*ball).vy = -1;
    }
    if ((*ball).y <= 0) {
        (*ball).vy = 1;
    }
    return(sent);
}

int check_paddle(Ball_t* ball,tinygl_point_t* left, tinygl_point_t* right)
{
    int8_t ballX = (*ball).x;
    int8_t ballY = (*ball).y;
    if(ballX == 3) {
        if(ballY == (*left).y || ballY == ((*left).y + 1) || ballY == (*right).y) {
            (*ball).vx = -1;
            return(1);
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
    pacer_init (PACER_RATE);
    ir_serial_init ();
    // start welcome screen
    welcome_screen();
    // get maxscore
    int8_t lowerScoreBound = 49;
    int8_t upperScoreBound = 57;
    char maxScore = char_in_bound(lowerScoreBound, upperScoreBound);
    tinygl_clear();
    Ball_t ball = {1,1,1,1};
    tinygl_point_t leftLine = {4,2};
    tinygl_point_t rightLine = {4,4};
    int16_t tick = 0;
    char score = '0';
    int8_t collision = 1;
    int sent = 0;
    while (1)
    {
        ball = move_ball(&tick, ball);
        collision = check_paddle(&ball, &leftLine, &rightLine);
        sent = check_wall(&ball, sent);
        if(sent)
        {
            ball = wait_for_ball(&sent);
        }
        if(collision == -1) {
            ball = reset_ball(ball);
            if(score < maxScore) {
                score ++;
                display_score(score);
            } else {
                display_end(score);
            }
        }
        move_line(&leftLine, &rightLine);
        pacer_wait();
        tinygl_update();
    }
}