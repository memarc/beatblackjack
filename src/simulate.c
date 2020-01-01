/*
 * How to beat the Blackjack dealer
 * Copyright (C) 1992, 1998 Jacob Rief
 * $Id: simulate.c,v 1.2 2000/01/10 23:13:13 jacob Exp $
 */

#include <time.h>
#include <stdlib.h>
#include "probability.h"
#include "simulate.h"

static int num_decks = 0;
static int num_cards, take_card;
static int shoe_card[52*MAXDECKS];
static int dealer_card[10];
static int dealer_score;
static int gambler_card[7][10];
static int player_card[4][10];
static int player_score[4], player_doubled[4], player_insurance;
static int player_gain[4];
static int player_bet;
static int player_capital, min_capital, max_capital;
static double expect_capital;
static void(*shuffle_function)(void);


int card2value(int card)
{
    int value = card%13+1;
    if (value>10)
        value = 10;
    return value;
}

/* setup cards in the shoe */
void setup_cards(int nd)
{
    int i, value;
    num_decks = nd;
    num_cards = 0;
    for (i = num_decks; i>0; i--)
        for (value = 0; value<52; value++)
            shoe_card[num_cards++] = value;
    for (i = 52*MAXDECKS-1; i>num_cards; i--)
        shoe_card[i] = -1;
    srand((unsigned int)time(0));
}


void setup_capital(int capital)
{
    player_capital = min_capital = max_capital = capital;
    expect_capital = (double)capital;
}


float taken_cards_distribution(void)
{
    return (float)take_card/(float)num_cards;
}


static int take_card_from_shoe(void)
{
    int card_ret;
    if (take_card>=num_cards) {
        if (shuffle_function)
            shuffle_function();
        else
            shuffle_cards_permutation(2);
    }
    card_ret = shoe_card[take_card];
    take_card++;
    remove_from_shoe(card2value(card_ret), 1);
    return card_ret;
}


int show_card_in_shoe(int index)
{
    return shoe_card[index];
}


static void calc_sum(int cds[], int num, int* result, int* softaces)
{
    int i;
    *result = 0;
    *softaces = 0;
    for (i = 0; i<num; i++) {
        int val = card2value(cds[i]);
        if (val==ACE) {
            ++*softaces;
            *result += 11;
        }
        else
            *result += val;
    }
    for (; *softaces>0 && *result>21; --*softaces)
        *result -= 10;
    if (*result==21 && num==2)
        *result = BLACKJACK;
    else if (*result>21)
        *result = BUST;
}


static void gamblers_game(int gambler)
{
    /* let other gamblers do their game, stragety is just the same as for the dealer */
    int cntr, gasum, softaces;
    for (cntr = 0;;) {
        gambler_card[gambler][cntr] = take_card_from_shoe();
        cntr++;
        calc_sum(gambler_card[gambler], cntr, &gasum, &softaces);
        if (gasum>=17)
            break;
    }
    gambler_card[gambler][cntr] = -1;
}


void shuffle_cards_permutation(int factor)
{
    int i, pick_now, pick_prev, value;

    pick_now = (int)(((float)num_cards*(float)rand())/((float)RAND_MAX+1.0f));
    value = shoe_card[pick_now];
    for (i = 0; i<num_cards*factor; i++) {
        pick_prev = pick_now;
        pick_now = (int)(((float)num_cards*(float)rand())/((float)RAND_MAX+1.0f));
        shoe_card[pick_prev] = shoe_card[pick_now];
    }
    shoe_card[pick_now] = value;
    take_card = 0;
    init_shoe(num_decks);
}


static void players_game(void)
{
    int softaces, cntr, splt;
    int player_splitted = 0;

    /* let the player do his game */
    player_card[0][0] = take_card_from_shoe();
    player_card[1][0] = player_card[2][0] = player_card[3][0] = -1;
    player_doubled[0] = player_doubled[1] = player_doubled[2] = player_doubled[3] = 0;

    /* just in case, dealers first hand is ace, player may insure */
    if (card2value(dealer_card[0])==ACE && 3.0f*prob_dlh(ACE, BLACKJACK)>1.0f)
        player_insurance = 1;
    else
        player_insurance = 0;

    for (splt = 0; player_card[splt][0]>=0 && splt<4; splt++) {
        for (cntr = 1;;) {
            player_card[splt][cntr] = take_card_from_shoe();
            cntr++;
            calc_sum(player_card[splt], cntr, &player_score[splt], &softaces);
            if (player_splitted && player_score[splt]==BLACKJACK) {
                /* downgrade BLACKJACK to 21 after split of aces */
                player_score[splt] = 21;
            }

            if (splt<3 && cntr==2 && card2value(player_card[splt][0])==card2value(player_card[splt][1])) {
                /* player must decide to split */
                if (expect_split_joined_default(card2value(dealer_card[0]), card2value(player_card[splt][0]))>0.0) {
                    /* do split */
                    player_card[splt+1][0] = player_card[splt][1];
                    cntr = 1;
                    player_splitted = 1;
                    continue;
                }
            }
            if (player_score[splt]>=9 && player_score[splt]<=11 && cntr==2) {
                /* player must decide to double */
                if (expect_double_buy(card2value(dealer_card[0]), player_score[splt])>0.0) {
                    player_card[splt][cntr] = take_card_from_shoe();
                    cntr++;
                    calc_sum(player_card[splt], cntr, &player_score[splt], &softaces);
                    player_doubled[splt] = 1;
                    break;
                }
            }
            if (player_splitted && card2value(player_card[splt][0])==ACE)
                break; /* after an ace-split, player gets only one more card */
            if (softaces==0) {
                /* hardhand */
                if (player_score[splt]>16) 
                    break; /* stay */
                else if (player_score[splt]<12 || expect_buy_stay(card2value(dealer_card[0]), player_score[splt])>0.0)
                    continue; /* buy */
            }
            else {
                /* softhand */
                if (player_score[splt]>18)
                    break; /* stay */
                else if (player_score[splt]<17 || expect_buy_stay(card2value(dealer_card[0]), player_score[splt])>0.0)
                    continue; /* buy */
            }
            break;
        }
        player_card[splt][cntr] = -1;
    }
}


void determine_player_bet(int increment_bet, int minimum_bet, int maximum_bet)
{
    float total_expectation;

    /* calculate total expectation and determine the bet according to a usefule formula */
    calc_distribution();
    total_expectation = expect_player_bet(0);
    if (total_expectation<-1.0f || total_expectation>1.0f)
        total_expectation = 0.0f; /* can happen if almost all cards are gone */
    player_bet = (int)(total_expectation*100.0f*(float)increment_bet)/minimum_bet*minimum_bet;
    if (player_bet>maximum_bet)
        player_bet = maximum_bet;
    else if (player_bet<minimum_bet)
        player_bet = minimum_bet;
    expect_capital += (double)player_bet*total_expectation;
}


void set_shuffle_callback(shuffle_func_prototype sf)
{
    shuffle_function = sf;
}


void simulate_game(int gamblers_before, int gamblers_after)
{
    int softaces, cntr, gamb;
    int stake = 0, gain = 0, splt;

    /* dealers first hand */
    dealer_card[0] = take_card_from_shoe();

    /* let other gamblers do their game */
    for (gamb = 0; gamb<gamblers_before; gamb++)
        gamblers_game(gamb);

    /* let the player do his game */
    calc_distribution(); /* do it again, expectations may have changed slighly after gamblers before have played */
    players_game();

    /* let other gamblers do their game */
    for (gamb = gamblers_before; gamb<gamblers_before+gamblers_after; gamb++)
        gamblers_game(gamb);

    /* let the dealer finish his game */
    for (cntr = 1;;) {
        dealer_card[cntr] = take_card_from_shoe();
        cntr++;
        calc_sum(dealer_card, cntr, &dealer_score, &softaces);
        if (dealer_score>=17)
            break;
    }
    dealer_card[cntr] = -1;

    /* find out gain/loss for each split row */
    for (splt = 0; player_card[splt][0]>=0 && splt<4; splt++) {
        player_gain[splt] = 0;
        if (player_doubled[splt])
            stake = 2*player_bet;
        else
            stake = player_bet;

        /* find out how much the player wins or looses */
        if (player_score[splt]==BUST) {
            player_gain[splt] = -stake;
            gain += player_gain[splt];
        }
        else {
            if (player_score[splt]==BLACKJACK && dealer_score!=BLACKJACK) {
                player_gain[splt] = stake*3/2;
                gain += player_gain[splt];
            } else if (dealer_score==BUST || player_score[splt]>dealer_score) {
                player_gain[splt] = stake;
                gain += player_gain[splt];
            } else if (player_score[splt]<dealer_score) {
                player_gain[splt] = -stake;
                gain -= stake;
            }
        }
        if (player_insurance && splt==0) {
            if (dealer_score==BLACKJACK) {
                player_gain[0] += player_bet;
                gain += player_bet;
            } else {
                player_gain[0] -= player_bet/2;
                gain -= player_bet/2;
            }
        }
    }
    player_capital += gain;
    if (player_capital<min_capital)
        min_capital = player_capital;
    if (player_capital>max_capital)
        max_capital = player_capital;
}


int get_dealer_card(int index)
{
    return dealer_card[index];
}


int get_dealer_score()
{
    return dealer_score;
}


int get_gambler_card(int gambler, int index)
{
    return gambler_card[gambler][index];
}


int get_player_card(int split, int index)
{
    return player_card[split][index];
}


int get_player_score(int split)
{
    return player_score[split];
}


int get_player_insurance(void)
{
    return player_insurance;
}


int get_player_doubled(int split)
{
    return player_doubled[split];
}


int get_player_gain(int split)
{
    return player_gain[split];
}


int get_player_bet(void)
{
     return player_bet;
}


int get_player_capital(void)
{
    return player_capital;
}


int get_min_capital(void)
{
    return min_capital;
}


int get_max_capital(void)
{
    return max_capital;
}


double get_expected_capital(void)
{
    return expect_capital;
}

