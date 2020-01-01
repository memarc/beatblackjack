/*
 * How to beat the Blackjack dealer
 * Copyright (C) 1992, 1997 Jacob Rief
 * $Id: display.c,v 1.1.1.1 1999/12/12 23:12:31 jacob Exp $
 */

#include <stdio.h>
#include "display.h"
#include "probability.h"


void display_shoe(FILE* out)
{
    int i;
    fprintf(out, "--- Card distribution in shoe ---\n");
    fprintf(out, "Cards\t   2\t   3\t   4\t   5\t   6\t   7\t   8\t   9\t   X\t   A\tDecks\n");
    /* absolute */
    fprintf(out, " %3d\t", get_cards_in_shoe(0));
    for (i = 2; i<=10; i++)
	fprintf(out, " %3d\t", get_cards_in_shoe(i));
    fprintf(out, " %3d\t %3d\n", get_cards_in_shoe(ACE), get_cards_in_shoe(11));
    /* relative */
    fprintf(out, "percent\t");
    for (i=2; i<=10; i++)
        fprintf(out, "%5.1f\t", ((float) get_cards_in_shoe(i)/get_cards_in_shoe(0))*100.0f);
    fprintf(out, "%5.1f\n", ((float) get_cards_in_shoe(ACE)/get_cards_in_shoe(0))*100.0f);
}


void display_prob_dlh(FILE* out)
{
    int i, k;
    fprintf(out, "--- Probablitiy of dealers last hand under the condition of dealers first hand ---\n");
    fprintf(out, "dfh\t   2\t   3\t   4\t   5\t   6\t   7\t   8\t   9\t   X\t   A\tTOTAL\n");
    fprintf(out, "p(dfh)\t");
    for (i = 2; i<=10; i++)
        fprintf(out, "%5.1f\t", prob_dlh(i, 0)*100.0);
    fprintf(out, "%5.1f\t%5.1f\n", prob_dlh(1, 0)*100.0, prob_dlh(0, 0)*100.0);
    for (k = BUST; k>=17; k--) {
        switch (k) {
        case BUST:
            fprintf(out, "p(BUST)\t");
            break;
        case BLACKJACK:
            fprintf(out, "p(BJ)\t");
            break;
        default:
            fprintf(out, "p(%d)\t", k);
            break;
        }
        for (i = 2; i<=10; i++)
            fprintf(out, "%5.1f\t", prob_dlh(i, k)*100.0);
        fprintf(out, "%5.1f\t%5.1f\n", prob_dlh(ACE, k)*100.0, prob_dlh(0, k)*100.0);
    }
}



void display_exp_insu(FILE* out)
{
    fprintf(out, "--- Expectation for INSURANCE under the condition of dealers first hand is ACE ---\n");
    fprintf(out, "E(INSURE|ACE)\t\t\t\t\t\t\t\t\t%5.1f\n", (3.0f*prob_dlh(ACE, BLACKJACK)-1.0f)*100.0);
}



void display_exp_buy_stay(FILE* out)
{
    int i, k;
    fprintf(out, "--- Expectation for BUY contra STAY under the condition of dealers first hand ---\n");
    fprintf(out, "dfh\t   2\t   3\t   4\t   5\t   6\t   7\t   8\t   9\t   X\t   A\n");
    for (k = 18; k>=12; k--) {
        if (k<17)
            fprintf(out, "E(%2d)\t", k);
        else
            fprintf(out, "E(%1d/%2d)\t", k-10, k);
        for (i = 2; i<=10; i++)
            fprintf(out, "%5.1f\t", expect_buy_stay(i, k)*100.0f);
        fprintf(out, "%5.1f\n", expect_buy_stay(ACE, k)*100.0f);
    }
}


void display_exp_double_buy(FILE* out)
{
    int i, k;
    fprintf(out, "--- Expectation for DOUBLE contra BUY under the condition of dealers first hand ---\n");
    fprintf(out, "dfh\t   2\t   3\t   4\t   5\t   6\t   7\t   8\t   9\t   X\t   A\n");
    for (k=11; k>=9; k--) {
        fprintf(out, "E(%2d)\t", k);
        for (i=2; i<=10; i++)
	  fprintf(out, "%5.1f\t", expect_double_buy(i, k)*100.0);
        fprintf(out, "%5.1f\n", expect_double_buy(ACE, k)*100.0);
    }
}


void display_exp_split_joined_default(FILE* out)
{
    int i, k;
    fprintf(out, "--- Expectation for SPLIT contra BUY/STAY for splitting player (default rule) ---\n");
    fprintf(out, "dfh\t   2\t   3\t   4\t   5\t   6\t   7\t   8\t   9\t   X\t   A\n");
    for (k=1; k<=10; k++) {
        switch (k) {
        case 1:
            fprintf(out, "E(A-A)\t");
            break;
        case 10:
            fprintf(out, "E(X-X)\t");
            break;
        default:
            fprintf(out, "E(%1d-%1d)\t", k, k);
            break;
        }
        for (i = 2; i<=10; i++)
            fprintf(out, "%5.1f\t", expect_split_joined_default(i, k)*100.0);
        fprintf(out, "%5.1f\n", expect_split_joined_default(ACE, k)*100.0);
    }
}


void display_exp_split_joined_partner(FILE* out)
{
    int i, k;
    fprintf(out, "--- Expectation for SPLIT contra BUY/STAY for splitting partner (half bet) ---\n");
    fprintf(out, "dfh\t   2\t   3\t   4\t   5\t   6\t   7\t   8\t   9\t   X\t   A\n");
    for (k=1; k<=10; k++) {
        switch (k) {
        case 1:
            fprintf(out, "E(A-A)\t");
            break;
        case 10:
            fprintf(out, "E(X-X)\t");
            break;
        default:
            fprintf(out, "E(%1d-%1d)\t", k, k);
            break;
        }
        for (i = 2; i<=10; i++)
            fprintf(out, "%5.1f\t", expect_split_joined_partner(i, k)*100.0);
        fprintf(out, "%5.1f\n", expect_split_joined_partner(ACE, k)*100.0);
    }
}


void display_player_last_hand(FILE* out)
{
    int i, k;
    fprintf(out, "--- Probalility of players last hand (when played with optimal strategy) ---\n");
    fprintf(out, "dfh\t   2\t   3\t   4\t   5\t   6\t   7\t   8\t   9\t   X\t   A\tTOTAL\n");
    for (k = BUST; k>=11; k--) {
        switch (k) {
        case BUST:
            fprintf(out, "p(BUST)\t");
            break;
        case BLACKJACK:
            fprintf(out, "p(BJ)\t");
            break;
        default:
            fprintf(out, "p(%2d)\t", k);
            break;
        }
        for (i = 2; i<=10; i++)
            fprintf(out, "%5.1f\t", prob_player_bet(i, k)*100.0);
        fprintf(out, "%5.1f\t%5.1f\n", prob_player_bet(ACE, k)*100.0, prob_player_bet(0, k)*100.0);
    }
}


void display_final_expectation(FILE* out)
{
    int i;
    fprintf(out, "--- Final expectation (when played with optimal strategy) ---\n");
    fprintf(out, "dfh\t   2\t   3\t   4\t   5\t   6\t   7\t   8\t   9\t   X\t   A\tTOTAL\n");
    fprintf(out, "expect:\t");
    for (i = 2; i<=10; i++)
        fprintf(out, "%5.1f\t", expect_player_bet(i)*100.0);
    fprintf(out, "%5.1f\t%6.3f%%\n", expect_player_bet(ACE)*100.0, expect_player_bet(0)*100.0);
}

