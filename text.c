/**
 * @file text.c
 * @author Joshua Brown, Maxzi Francisco
 * @brief text to display component for the game
 * @date 17 October 2022 
 */

#include "tinygl.h"
#include "navswitch.h"
#include "../fonts/font3x5_1.h"
#include <stdint.h>
#include "pacer.h"
#include "display.h" 


/** Displays characters for player options, used by char_in_bound function
    @param character player type ('1' and '2') 
 */
void display_character (char character)
{
    char buffer[2];
    buffer[0] = character;
    buffer[1] = '\0';
    tinygl_text (buffer);
}


/** Gets the player to choose as player 1 or 2 at the start of the game, player 2 as receiver and player 1 with the ball 
    @param lowerBound 
    @param upperBound 
    @return chosen player type
 */
char char_in_bound(int8_t lowerBound, int8_t upperBound)
{
    //Initialises first character
    char character = '1';
    while(1) {
        pacer_wait ();
        //Initialises tinygl updates
        tinygl_update ();
        //Initialises navswitch updates
        navswitch_update ();       
        //Checks navswitch driver direction
        if (navswitch_push_event_p (NAVSWITCH_NORTH)) {
        //Checks conditions
            if(character < upperBound) {
        //Increments character
                character++;
            }
        } 
        //Checks navswitch driver direction
        if (navswitch_push_event_p (NAVSWITCH_SOUTH)) {
            if(character > lowerBound) {
        //Decrements character
                character--;
            }
        }     
        //Checks navswitch driver direction (pushed)
        if (navswitch_push_event_p(NAVSWITCH_PUSH)) {
        //Returns character
            return(character);
        }
        //Displays character on the matrix
        display_character (character);
    }   
}


/** Displays text updates about the game */
void display_text (void)
{
    while(1) {
        pacer_wait ();
        //Initialises tinygl 
        tinygl_update ();
        //Inititalises navswitch
        navswitch_update ();
        //Checks if navswitch pushed
        if (navswitch_push_event_p(NAVSWITCH_PUSH)) {
        //Clear crean
            tinygl_clear();
            return;
        }
    }
}