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

Ball_t move_ball(int16_t* tick, Ball_t ball);

Ball_t reset_ball(Ball_t ball);

int check_wall(Ball_t* ball, int16_t tick);

#endif