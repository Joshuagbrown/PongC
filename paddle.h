#ifndef PADDLE_H
#define PADDLE_H

#include "ball.h"

void move_paddle(tinygl_point_t* left, tinygl_point_t* right);

int check_paddle(Ball_t* ball,tinygl_point_t left, tinygl_point_t right);


#endif