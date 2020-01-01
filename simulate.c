/*
 * How to beat the Blackjack dealer
 * Copyright (C) 1992, 1998 Jacob Rief
 * $Id: simulate.c,v 1.1.1.1 1998/10/26 09:42:51 jacob Exp $
 */

#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include "probability.h"

static int num_packs = 6;
static float shuffle_condition = 0.85;
static int increment_bet = 250;
static int minimum_bet = 10;
static int maximum_bet = 500;
static int capital = 10000;
static int gamblers_before = 2;
static int gamblers_after = 1;


/* read options from the command line */
static void setup_options(void)
{
    char buffer[256];
    printf("*** simulate Blackjack (by Jacob Rief - version 1.01) ***\n");

    for (;;)
    {
        printf("Number of packs in the shoe (default is: %d)? ", num_packs);
        fgets(buffer, 255, stdin);
        sscanf(buffer, "%d", &num_packs);
        if (num_packs<1 || num_packs>10)
            printf("ERROR: value must be between 1 and 10\n");
        else
            break;
    }

    for (;;)
    {
        int sc;
        printf("Percentage of gone cards, after which the shoe is reshuffled (default is: %d)? ", (int)(100.0f*shuffle_condition));
        fgets(buffer, 255, stdin);
        if (sscanf(buffer, "%d", &sc)==1)
            shuffle_condition = (float)sc/100.0f;
        if (shuffle_condition<0.0f || shuffle_condition>1.0f)
            printf("ERROR: value must be between 0 and 100\n");
        else
            break;
    }

    for (;;)
    {
        printf("What is the minimum bet (default is: %d)? ", minimum_bet);
        fgets(buffer, 255, stdin);
        sscanf(buffer, "%d", &minimum_bet);
        if (minimum_bet<0)
            printf("ERROR: value must be above 0\n");
        else
            break;
    }

    for (;;)
    {
        printf("What is the maximum bet (default is: %d)? ", maximum_bet);
        fgets(buffer, 255, stdin);
        sscanf(buffer, "%d", &maximum_bet);
        if (maximum_bet<minimum_bet)
            printf("ERROR: value must be above %d\n", minimum_bet);
        else
            break;
    }

    for (;;)
    {
        printf("What is the initial capital (default is: %d)? ", capital);
        fgets(buffer, 255, stdin);
        sscanf(buffer, "%d", &capital);
        if (capital<minimum_bet)
            printf("ERROR: value must be above %d\n", minimum_bet);
        else
            break;
    }

    for (;;)
    {
        printf("How strongly should the bet increase whith a total expection above 0 (default is: %d)? ", increment_bet);
        fgets(buffer, 255, stdin);
        sscanf(buffer, "%d", &increment_bet);
        if (increment_bet<0)
            printf("ERROR: value must be above 0\n");
        else
            break;
    }

    for (;;)
    {
        printf("How many gamblers are sitting in front of You (default is: %d)? ", gamblers_before);
        fgets(buffer, 255, stdin);
        sscanf(buffer, "%d", &gamblers_before);
        if (gamblers_before<0 || gamblers_before>6)
            printf("ERROR: value must be between 0 and 6\n");
        else
            break;
    }

    for (;;)
    {
        printf("How many gamblers are sitting in behind You (default is: %d)? ", gamblers_after);
        fgets(buffer, 255, stdin);
        sscanf(buffer, "%d", &gamblers_after);
        if (gamblers_before<0 || gamblers_before+gamblers_after>6)
            printf("ERROR: value must be between 0 and %d\n", 6-gamblers_before);
        else
            break;
    }
}


#define SIMOPTSFILENAME ".simopts"

/* save options to file .simopts */
static void save_options(void)
{
    FILE* fh = fopen(SIMOPTSFILENAME, "w");
    if (fh==NULL)
        return;
    fprintf(fh, "num_packs %d\n", num_packs);
    fprintf(fh, "shuffle_condition %d\n", (int)(shuffle_condition*100.0f));
    fprintf(fh, "increment_bet %d\n", increment_bet);
    fprintf(fh, "minimum_bet %d\n", minimum_bet);
    fprintf(fh, "maximum_bet %d\n", maximum_bet);
    fprintf(fh, "capital %d\n", capital);
    fprintf(fh, "gamblers_before %d\n", gamblers_before);
    fprintf(fh, "gamblers_after %d\n", gamblers_after);
    fclose(fh);
}


/* read options from file .simopts */
static void load_options(void)
{
    char buffer[256];
    FILE* fh = fopen(SIMOPTSFILENAME, "r");
    if (fh==NULL)
        return;
    while (fgets(buffer, 255, fh))
    {
        char token[256];
        int value;
        if (sscanf(buffer, "%s %d", token, &value)==2)
        {
            if (strcmp(token, "num_packs")==0)
                num_packs = value;
            if (strcmp(token, "minimum_bet")==0)
                minimum_bet = value;
            if (strcmp(token, "maximum_bet")==0)
                maximum_bet = value;
            if (strcmp(token, "capital")==0)
                capital = value;
            if (strcmp(token, "gamblers_before")==0)
                gamblers_before = value;
            if (strcmp(token, "gamblers_after")==0)
                gamblers_after = value;
            if (strcmp(token, "increment_bet")==0)
                increment_bet = value;
            if (strcmp(token, "shuffle_condition")==0)
                shuffle_condition = (float)value/100.0f;
        }
    }
    fclose(fh);
}


static double expect_capital;
static int num_game, num_cards, take_card, min_capital, max_capital;
static enum CARDVALUE shoe_card[52*MAXPACKS];
static enum CARDVALUE dealer_card[10];
static enum CARDVALUE gambler_card[7][10];
static enum CARDVALUE player_card[4][10];
static int player_sum[4], player_doubled[4], player_insurance;


static void print_card(int cv)
{
    switch (cv)
    {
    case ACE:
        printf("ACE ");
        break;
    case BLACKJACK:
        printf("BLACKJACK ");
        break;
    case BUST:
        printf("BUST ");
        break;
    default:
        printf("%d ", cv);
    }
}


/* show cards distribution in shoe */
static void show_distribution(void)
{
    int i;

    printf("---- cards in shoe ----\n");
    for (i = 0; i<num_packs*52; i++)
    {
        print_card((int)shoe_card[i]);
        if (i%32==31)
            printf("\n");
    }
}


/* setup cards in the shoe */
static void setup_cards(void)
{
    int i;
    enum CARDVALUE value;

    num_cards = 0;
    for (value = ACE; value<TEN; value++)
        for (i = num_packs*4; i>0; i--)
            shoe_card[num_cards++] = value;
    for (i = num_packs*16; i>0; i--)
        shoe_card[num_cards++] = TEN;
    for (i = 52*MAXPACKS-1; i>num_cards; i--)
        shoe_card[i] = 0;
}


/* shuffle cards in shoe */
static void shuffle_cards(void)
{
    int i, pick_now, pick_prev;
    enum CARDVALUE value;

    printf("\n---- shuffeling ----\r");
    pick_now = (int)(((float)num_cards*(float)rand())/((float)RAND_MAX+1.0f));
    value = shoe_card[pick_now];
    for (i = 0; i<num_cards*3; i++)
    {
        pick_prev = pick_now;
        pick_now = (int)(((float)num_cards*(float)rand())/((float)RAND_MAX+1.0f));
        shoe_card[pick_prev] = shoe_card[pick_now];
    }
    shoe_card[pick_now] = value;
    take_card = 0;
    init_shoe(num_packs);
    show_distribution();
}


static enum CARDVALUE take_card_from_shoe(void)
{
    enum CARDVALUE card_ret;
    if (take_card>=num_cards)
          shuffle_cards();
    card_ret = shoe_card[take_card];
    take_card++;
    remove_from_shoe(card_ret, 1);
    return card_ret;
}


static void calc_sum(enum CARDVALUE cds[], int num, int* result, int* softaces)
{
    int i;
    *result = 0;
    *softaces = 0;
    for (i = 0; i<num; i++)
    {
        if (cds[i]==ACE)
        {
            ++*softaces;
            *result += 11;
        }
        else
            *result += cds[i];
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
    for (cntr = 0;;)
    {
        gambler_card[gambler][cntr] = take_card_from_shoe();
        cntr++;
        calc_sum(gambler_card[gambler], cntr, &gasum, &softaces);
        if (gasum>=17)
            break;
    }
    gambler_card[gambler][cntr] = 0;
}


static void players_game(void)
{
    int softaces, cntr, splt;
    int player_splitted = 0;

    /* let the player do his game */
    player_card[0][0] = take_card_from_shoe();
    player_card[1][0] = 0; player_card[2][0] = 0; player_card[3][0] = 0;
    player_doubled[0] = player_doubled[1] = player_doubled[2] = player_doubled[3] = 0;

    /* just in case, dealers first hand is ace, player may insure */
    if (dealer_card[0]==ACE && 3.0f*prob_dlh(ACE, BLACKJACK)>1.0f)
        player_insurance = 1;
    else
        player_insurance = 0;

    for (splt = 0; player_card[splt][0] && splt<4; splt++)
    {
        for (cntr = 1;;)
        {
            player_card[splt][cntr] = take_card_from_shoe();
            cntr++;
            calc_sum(player_card[splt], cntr, &player_sum[splt], &softaces);
            if (player_splitted && player_sum[splt]==BLACKJACK )
                player_sum[splt] = 21;

            /* player must decide what to do */
            if (splt<3 && cntr==2 && player_card[splt][0]==player_card[splt][1])
                if (expect_split_joined_default(dealer_card[0], (int)player_card[splt][0])>0.0)
                {
                    /* split */
                    player_card[splt+1][0] = player_card[splt][1];
                    cntr = 1;
                    player_splitted = 1;
                    continue;
                }
            if (player_sum[splt]>=9 && player_sum[splt]<=11 && cntr==2)
                if (expect_double_buy(dealer_card[0], player_sum[splt])>0.0)
                {
                    /* double, ie. buy the last card and stop */
                    player_card[splt][cntr] = take_card_from_shoe();
                    cntr++;
                    calc_sum(player_card[splt], cntr, &player_sum[splt], &softaces);
                    player_doubled[splt] = 1;
                    break;
                }
            if (player_splitted && player_card[splt][0]==ACE)
                break; /* after an ace-split, player gets only one more card */
            if (softaces==0)
            {
                /* hardhand */
                if (player_sum[splt]>16) 
                    break; /* stay */
                else if (player_sum[splt]<12 || expect_buy_stay(dealer_card[0], player_sum[splt])>0.0)
                    continue; /* buy */
            }
            else
            {
                /* softhand */
                if (player_sum[splt]>18)
                    break; /* stay */
                else if (player_sum[splt]<17 || expect_buy_stay(dealer_card[0], player_sum[splt])>0.0)
                    continue; /* buy */
            }
            break;
        }
        player_card[splt][cntr] = 0;
    }
}


static void a_game(void)
{
    int desum, softaces, cntr, gamb;
    float total_expectation;
    int bet, stake, gain, splt;

    /* calculate total expectation and place the bet accordingly */
    calc_distribution();
    total_expectation = expect_player_bet(0);
    if (total_expectation<-1.0f || total_expectation>1.0f)
        total_expectation = 0.0f; /* can happen if almost all cards are gone */
    bet = (int)(total_expectation*100.0f*(float)increment_bet)/minimum_bet*minimum_bet;
    if (bet>maximum_bet)
        bet = maximum_bet;
    else if (bet<minimum_bet)
        bet = minimum_bet;
    expect_capital += (double)bet*total_expectation;
    stake = gain = 0;

    /* dealers first hand */
    dealer_card[0] = take_card_from_shoe();

    /* let other gamblers do their game */
    for (gamb = 0; gamb<gamblers_before; gamb++)
        gamblers_game(gamb);

    /* let the player do his game */
    players_game();
    calc_distribution(); /* do it again, may have changed slighly */

    /* let other gamblers do their game */
    for (gamb = gamblers_before; gamb<gamblers_before+gamblers_after; gamb++)
        gamblers_game(gamb);

    /* let the dealer finish his game */
    for (cntr = 1;;)
    {
        dealer_card[cntr] = take_card_from_shoe();
        cntr++;
        calc_sum(dealer_card, cntr, &desum, &softaces);
        if (desum>=17)
            break;
    }
    dealer_card[cntr] = 0;

    /* print the the cards drawn in the game and the result of the bet */
    printf("\ndealer: ");
    for (cntr = 0; dealer_card[cntr]; cntr++)
        print_card((int)dealer_card[cntr]);
    printf("= ");
    print_card(desum);

    for (gamb = 0; gamb<gamblers_before; gamb++)
    {
        printf("\nother%d: ", gamb);
        for (cntr = 0; gambler_card[gamb][cntr]; cntr++)
            print_card((int)gambler_card[gamb][cntr]);
    }

    for (splt = 0; player_card[splt][0] && splt<4; splt++)
    {
        if (splt==0)
            printf("\nplayer: ");
        else
            printf("\n split: ");
        for (cntr = 0; player_card[splt][cntr]; cntr++)
            print_card((int)player_card[splt][cntr]);
        if (splt==0 && player_insurance)
            printf("(insured) ");
        if (player_doubled[splt])
        {
            printf("(doubled) ");
            stake = 2*bet;
        }
        else
            stake = bet;
        printf("= ");
        print_card((int)player_sum[splt]);

        /* find out how the player wins or looses */
        if (player_sum[splt]==BUST)
        {
            gain -= stake;
            printf("--> loose");
        }
        else
        {
            if (player_sum[splt]==BLACKJACK && desum!=BLACKJACK)
            {
                gain += stake*3/2;
                printf("--> win 3 for 2");
            }
            else if (desum==BUST || player_sum[splt]>desum)
            {
                gain += stake;
                printf("--> win");
            }
            else if (player_sum[splt]==desum)
            {
                printf("--> equal");
            }
            else
            {
                gain -= stake;
                printf("--> loose");
            }
        }
    }
    if (player_insurance)
    {
        if (desum==BLACKJACK)
            gain += bet;
        else
            gain -= bet/2;
    }
    capital += gain;
    printf("\nexpect: %6.3f%%, bet: %d, gain: %d", 100.0f*total_expectation, bet, gain);

    for (gamb = gamblers_before; gamb<gamblers_before+gamblers_after; gamb++)
    {
        printf("\nother%d: ", gamb);
        for (cntr = 0; gambler_card[gamb][cntr]; cntr++)
            print_card((int)gambler_card[gamb][cntr]);
    }
}


int main(int argc, char** argv)
{
    srand((unsigned int)time(0));
    load_options();
    setup_options();
    save_options();
    setup_cards();
    min_capital = max_capital = capital;
    expect_capital = (double)capital;
    while (capital>0)
    {
        shuffle_cards();
        while ((float)take_card/(float)num_cards<shuffle_condition)
        {
            num_game++;
            if (capital<min_capital)
                min_capital = capital;
            if (capital>max_capital)
                max_capital = capital;
            printf("\n---- game: %d capital: %d (min: %d, max: %d, expect: %6.0f) ----", num_game, capital, min_capital, max_capital, expect_capital);
            a_game();
        }
    }
    return 0;
}
