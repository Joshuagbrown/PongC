# PONG

## README for the game of PONG
By Joshua Brown (jbr242) and Maxzi Francisco (mfr91)

17 October 2022

### OBJECTIVE
Pass the ball around for as long as possible until a player is unable to hit the ball with the paddle.


### SET UP
1. Set up pong source code and connect UCFK4 microcontrollers a mini USB cable to a CPU.
2. Run `make program` command.
3. Place the microcontrollers with the IR trasmitter facing each other.


### HOW TO PLAY
1. To start the game, both player needs to push down the navswitch driver (joystick).
2. Each player will have to choose whether they want to be Player 1 and Player 2. 
   Players have to toggle the joystick left or right to choose, and then push the joystick down to confirm.
   Note: 
        Player 1 will have the ball initially and Player 2 is receiving the ball.
3. Pass the ball around using the paddle by controlling it to left and right by moving joystick in the left or right direction.
4. Keep the ball moving around between two players. The player that misses the ball will lose the round


### MESSAGES
    'WIN': You won the game.
    'LOST': You lost the game.
    'NO BALL': The ball got lost during transmission. Press down to restart the game.
    
--------------------------------------------------------------------------------------------