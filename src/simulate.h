/*
 * How to beat the Blackjack dealer
 * Copyright (C) 1992-1999 Jacob Rief
 * $Id: simulate.h,v 1.2 2000/01/10 23:13:13 jacob Exp $
 */

typedef void(*shuffle_func_prototype)(void);

/*
 * Initialize the shoe with nd number of decks.
 * The cards in the shoe are not shuffled
 */
void setup_cards(int nd);


/*
 * Set the initial capital of the player.
 */
void setup_capital(int capital);


/*
 * Returns the value accosiated with a certain card.
 */
int card2value(int card);

/*
 * Return the distribution of cards taken ot of the shoe.
 * The returned value can be from 0 (no cards taken out)
 * to 1 (all cards taken out).
 */
float taken_cards_distribution(void);


/*
 * Returns card in the shoe.
 */
int show_card_in_shoe(int index);


/*
 * Shuffle cards in shoe by simple permutation.
 * This is a very primitive way of shuffeling.
 * factor: the average number of permutations each card is exchanged.
 */
void shuffle_cards_permutation(int factor);

/* 
 * Determine the stake to bet for the player.
 * increment_bet: is the number by which the total_expectation is multiplied to obtain the
 *     height of the bet. Only used if the total_expectation is above zero.
 * minimum_bet, maximum_bet: the table limits for placing the stake.
 */
void determine_player_bet(int increment_bet, int minimum_bet, int maximum_bet);

/*
 * Set the callback function for shuffeling cards.
 * Here the client may override the default shuffeling function, used
 * to shuffle the cards in the shoe. If 'sf' is NULL the default
 * shuffeling function is used.
 */
void set_shuffle_callback(shuffle_func_prototype sf);

/*
 * Simulate a game of having determined the stake.
 * gamblers_before, int gamblers_after: The number of gamblers sitting before and after the
 *     simulating player.
 * shuffle_function: The callback function to be called if the shoe is empty and must be reshuffled.
 */
void simulate_game(int gamblers_before, int gamblers_after);


/*
 * Return the card of the dealer after playing at index.
 * Returns -1 if there is no more card.
 */
int get_dealer_card(int index);


/*
 * Return the score reached by the dealer..
 */
int get_dealer_score(void);


/*
 * Return the card of an other gambler after playing at index.
 * Returns -1 if there is no more card.
 */
int get_gambler_card(int gambler, int index);


/*
 * Return the card of the player after playing at index.
 * split: is the index of the splitted row.
 * Returns -1 if there is no more card.
 */
int get_player_card(int split, int index);


/*
 * Return the score reached by the player.
 * split: is the index of the splitted row.
 */
int get_player_score(int split);


/*
 * Returns true if player was insured.
 */
int get_player_insurance(void);


/*
 * Returns true if player doubled on split row.
 */
int get_player_doubled(int split);


/*
 * Returns the gain for split row.
 */
int get_player_gain(int split);


/*
 * Returns the bet the player did.
 */
int get_player_bet(void);


/*
 * Returns the players capital.
 */
int get_player_capital(void);


/*
 * Returns the minimum capital since the first call
 * to setup_capital.
 */
int get_min_capital(void);


/*
 * Returns the maximum capital since the first call
 * to setup_capital.
 */
int get_max_capital(void);


/*
 * Returns the capital which would be expected.
 */
double get_expected_capital(void);

