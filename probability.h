/*
 * How to beat the Blackjack dealer
 * Copyright (C) 1992, 1998 Jacob Rief
 * $Id: probability.h,v 1.1.1.1 1998/10/26 09:42:51 jacob Exp $
 */

#ifndef _PROBABILITY_H_
#define _PROBABILITY_H_

#define MAXPACKS 10

enum CARDVALUE 
{
    ACE = 1,
    TWO = 2,
    THREE = 3,
    FOUR = 4,
    FIVE = 5,
    SIX = 6,
    SEVEN = 7,
    EIGHT = 8,
    NINE = 9,
    TEN = 10,
    JACK = 10,
    QUEEN = 10,
    KING = 10,
    BLACKJACK = 22,
    BUST = 23 
};


/*
 * Initialise the shoe with # packs
 * ie. reset the shoe to packs*4 ACES, packs*4 KINGS's etc.
 */
extern void init_shoe(int packs);


/*
 * Reset the shoe with packs*4 ACES, packs*4 KINGS's etc.
 */
extern void shuffle(void);


/*
 * Remove card from shoe.
 */
extern void remove_from_shoe(enum CARDVALUE card_value, int delta);


/*
 * Insert delta cards into shoe.
 */
extern void insert_to_shoe(enum CARDVALUE card_value, int delta);


/*
 * Return number of cards in shoe.
 * card_value = 0 returns total number of cards,
 * card_value = 11 returns number of packs started with.
 */
extern int get_cards_in_shoe(enum CARDVALUE card_value);


/*
 * Set number of cards in shoe.
 * card_value = 1..10,
 * num_cards number of cards now in the shoe, must be
 * a valid number according to the number of packs.
 * Returns 0 if input was OK, else returns -1.
 */
extern int set_cards_in_shoe(enum CARDVALUE card_value, int num_cards);


/*
 * Return the probability of dealers last hand under the condition of
 * dealers first hand. Call this function only after calling calc_distribution();
 * dealer_first_hand = 2..10, ACE (a card value),
 * dealer_last_hand = 17..21, BLACKJACK or BUST.
 * dealer_last_hand = 0 returns the probality of this dealers first hand.
 */
extern float prob_dlh(enum CARDVALUE dealer_first_hand, int dealer_last_hand);


/*
 * Return the expectation for BUY contra STAY under the condition of
 * dealers first hand for any score 12..16 (hardhand) 17, 18 (softhand).
 * Call this function only after calling calc_distribution();
 * dealer_first_hand = 1..10, ACE (a card value),
 * player_hand = 12..18.
 */
extern float expect_buy_stay(enum CARDVALUE dealer_first_hand, int player_hand);


/*
 * Return the expectation for DOUBLE contra BUY under the condition of
 * dealers first hand for any score 9, 10 or 11.
 * Call this function only after calling calc_distribution();
 * dealer_first_hand = 2..10, ACE (a card value),
 * player_hand = 9, 10 or 11.
 */
float expect_double_buy(enum CARDVALUE dealer_first_hand, int player_hand);


/*
 * Return the expectation for SPLIT contra BUY/STAY under the condition of
 * dealers first hand for any score ACE, 2..10. This is for the default
 * rule, where You have to double Yours bets stake when You split.
 * Call this function only after calling calc_distribution();
 * dealer_first_hand = 2..10, ACE (a card value),
 * player_hand = 2..10, ACE (the value of the pair).
 */
extern float expect_split_joined_default(enum CARDVALUE dealer_first_hand, int player_hand);


/*
 * Return the expectation for SPLIT contra BUY/STAY under the condition of
 * dealers first hand for any score last_hand. This is only useful
 * if You can also split Your bets stake, which is the case when You are
 * a partners player.
 * Call this function only after calling calc_distribution();
 * dealer_first_hand = 2..10, ACE (a card value),
 * player_hand = 2..10, ACE (the value of the pair).
 */
extern float expect_split_joined_partner(enum CARDVALUE dealer_first_hand, int player_hand);


/*
 * Return the probability of the players last_hand under the condition of dealers 
 * first hand and under the assumption the player did play with the optimum strategy.
 * Call this function only after calling calc_distribution();
 * dealer_first_hand = 2..10, ACE (a card value).
 * player_last_hand = 11..21, BLACKJACK, BUST.
 */
extern float prob_player_bet(enum CARDVALUE dealer_first_hand, int player_last_hand);


/*
 * Return the players expectation under the condition of dealers first hand
 * and under the assumption the player did play with the optimum strategy.
 * dealer_first_hand = 2..10, ACE (a card value).
 * If dealer_first_hand = 0 this function returns the players total expectation.
 * If this value is below zero, You will loose in average with this card constellation,
 * so it is recommendet to keep bets as low as possible.
 * If this value is above zero, You will win in average.
 * The expectation says: With this card constellation You will win in average 
 * expect_total percent of the initial stake.
 * Call this function only after calling calc_distribution();
 */
extern float expect_player_bet(enum CARDVALUE dealer_first_hand);


/*
 * Calaculate all expectations and probabilities according to the card distribution.
 */
extern void calc_distribution(void);

#endif /* _PROBABILITY_H_ */
