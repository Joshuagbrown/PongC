/**
 * @file text.h
 * @author Joshua Brown, Maxzi Francisco
 * @brief text to display component for the game
 * @date 17 October 2022
 
 */


#ifndef TEXT_H
#define TEXT_H


/** Displays characters for player options, used by char_in_bound function
    @param character player type ('1' and '2') 
 */
void display_character (char character);


/** Gets the player to choose as player 1 or 2 at the start of the game, player 2 as receiver and player 1 with the ball 
    @param lowerBound 
    @param upperBound 
    @return chosen player type
 */

char char_in_bound(int8_t lowerBound, int8_t upperBound);


/** Displays text updates about the game 
 */
void display_text (void);


#endif //TEXT_H
