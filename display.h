/*
 * How to beat the Blackjack dealer
 * Copyright (C) 1992, 1997 Jacob Rief
 * $Id: display.h,v 1.1.1.1 1999/12/12 23:12:31 jacob Exp $
 */

#ifndef __DISPLAY_H__
#define __DISPLAY_H__


/*
 * Show the distribution of the cards
 */
extern void display_shoe(FILE * out);


/*
 * Show the probabilities of the dealers last hand, under the condition of the dealers first hand
 */
extern void display_prob_dlh(FILE * out);


/*
 * Show the expectation for INSURANCE, under the condition of dealers first hand is ACE
 */
extern void display_exp_insu(FILE * out);


/*
 * Show the expectation for BUY contra STAY, under the condition of the dealers first hand.
 */
extern void display_exp_buy_stay(FILE * out);


/*
 * Show the expectation for DOUBLE contra BUY, under the condition of the dealers first hand.
 */
extern void display_exp_double_buy(FILE * out);


/*
 * Show the expectation for SPLIT contra BUY/STAY, under the condition of the dealers first hand.
 * For default rule and for the partner who can split the bets stake.
 */
extern void display_exp_split_joined_default(FILE * out);
extern void display_exp_split_joined_partner(FILE * out);


/*
 * Show the probability for players last hands, under the condition of the dealers first hand.
 */
extern void display_player_last_hand(FILE * out);


/*
 * Show the final expectation for the player if he plays according the the optimal strategy,
 * under the condition of the dealers first hand.
 */
extern void display_final_expectation(FILE * out);

#endif
