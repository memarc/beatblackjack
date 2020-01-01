/*
 * How to beat the Blackjack dealer
 * Copyright (C) 1999 Jacob Rief
 * $Id: simbj.c,v 1.2 2000/01/10 23:13:13 jacob Exp $
 */

#include <stdio.h>
#include <string.h>
#include "simulate.h"
#include "probability.h"

static int num_decks = 6;
static int num_game;
static float shuffle_condition = 0.85;
static int increment_bet = 250;
static int minimum_bet = 10;
static int maximum_bet = 500;
static int gamblers_before = 2;
static int gamblers_after = 1;


/* read options from the command line */
static void setup_options(void)
{
    char buffer[256];
    printf("*** simulate Blackjack (version 1.0.3) ***\n");
    printf("Copyright (C) 2000, Jacob Rief\n");
    printf("simbj comes with ABSOLUTELY NO WARRANTY\n");
    printf("This is free software, and you are welcome to redistribute it\n");

    for (;;) {
        printf("Number of decks in the shoe (default is: %d)? ", num_decks);
        fgets(buffer, 255, stdin);
        sscanf(buffer, "%d", &num_decks);
        if (num_decks<1 || num_decks>10)
            printf("ERROR: value must be between 1 and 10\n");
        else
            break;
    }

    for (;;) {
        int sc;
        printf("Percentage of gone cards, after which the shoe is reshuffled (default is: %d)? ", (int)(100.0f*shuffle_condition));
        fgets(buffer, 255, stdin);
        if (sscanf(buffer, "%d", &sc)==1)
            shuffle_condition = (float)sc/100.0f;
        if (shuffle_condition<0.01f || shuffle_condition>0.99f)
            printf("ERROR: value must be between 1 and 99\n");
        else
            break;
    }

    for (;;) {
        printf("What is the minimum bet (default is: %d)? ", minimum_bet);
        fgets(buffer, 255, stdin);
        sscanf(buffer, "%d", &minimum_bet);
        if (minimum_bet<0.01)
            printf("ERROR: value must be above 0.01\n");
        else
            break;
    }

    for (;;) {
        printf("What is the maximum bet (default is: %d)? ", maximum_bet);
        fgets(buffer, 255, stdin);
        sscanf(buffer, "%d", &maximum_bet);
        if (maximum_bet<minimum_bet)
            printf("ERROR: value must be above %d\n", minimum_bet);
        else
            break;
    }

    for (;;) {
        int pc = 10000;
        printf("What is the initial capital (default is: %d)? ", pc);
        fgets(buffer, 255, stdin);
        sscanf(buffer, "%d", &pc);
        if (pc<minimum_bet)
            printf("ERROR: value must be above %d\n", minimum_bet);
        else {
            setup_capital(pc);
            break;
        }
    }

    for (;;) {
        printf("How strongly should the bet increase whith a total expection above 0 (default is: %d)? ", increment_bet);
        fgets(buffer, 255, stdin);
        sscanf(buffer, "%d", &increment_bet);
        if (increment_bet<0)
            printf("ERROR: value must be above 0\n");
        else
            break;
    }

    for (;;) {
        int rdval = gamblers_before;
        printf("How many gamblers are sitting in front of You (default is: %d)? ", gamblers_before);
        fgets(buffer, 255, stdin);
        sscanf(buffer, "%d", &rdval);
        if (rdval<0 || rdval>6)
            printf("ERROR: value must be between 0 and 6\n");
        else {
            gamblers_before = rdval;
            break;
        }
    }

    while (gamblers_before+gamblers_after<=6) {
        int rdval = gamblers_after;
        printf("How many gamblers are sitting in behind You (default is: %d)? ", gamblers_after);
        fgets(buffer, 255, stdin);
        sscanf(buffer, "%d", &rdval);
        if (rdval<0 || gamblers_before+rdval>6)
            printf("ERROR: value must be between 0 and %d\n", 6-gamblers_before);
        else {
            gamblers_after = rdval;
            break;
        }
    }
}


#define SIMOPTSFILENAME ".simopts"

/* save options to file .simopts */
static void save_options(void)
{
    FILE* fh = fopen(SIMOPTSFILENAME, "w");
    if (fh==NULL)
        return;
    fprintf(fh, "num_decks %d\n", num_decks);
    fprintf(fh, "shuffle_condition %d\n", (int)(shuffle_condition*100.0f));
    fprintf(fh, "increment_bet %d\n", increment_bet);
    fprintf(fh, "minimum_bet %d\n", minimum_bet);
    fprintf(fh, "maximum_bet %d\n", maximum_bet);
    fprintf(fh, "capital %d\n", get_player_capital());
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
    while (fgets(buffer, 255, fh)) {
        char token[256];
        int value;
        if (sscanf(buffer, "%s %d", token, &value)==2) {
            if (strcmp(token, "num_decks")==0)
                num_decks = value;
            if (strcmp(token, "minimum_bet")==0)
                minimum_bet = value;
            if (strcmp(token, "maximum_bet")==0)
                maximum_bet = value;
            if (strcmp(token, "capital")==0)
                setup_capital(value);
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


static void print_card(int card)
{
    int value = card2value(card);
    switch (value) {
    case 10:
        printf("X");
        break;
    case 11: 
        printf("J");
        break;
    case 12: 
        printf("Q");
        break;
    case 13:
        printf("K");
        break;
    case 1:
        printf("A");
        break;
    default:
        printf("%d", value);
    }
    switch (card/13) {
    case 0:
        printf("$ ");
        break;
    case 1:
        printf("+ ");
        break;
    case 2:
        printf("& ");
        break;
    case 3:
        printf("* ");
        break;
    }
}


/* show cards distribution in shoe */
static void show_distribution(void)
{
    int i;

    printf("---- cards in shoe ----\n");
    for (i = 0; i<num_decks*52; i++) {
        print_card(show_card_in_shoe(i));
        if (i%24==23)
            printf("\n");
    }
}


static void print_score(int sum)
{
    if (sum==22)
        printf("BLACKJACK ");
    else if (sum>22)
        printf("BUST ");
    else
        printf("%d ", sum);
}

static void a_game(void)
{
    int g, k, i;

    determine_player_bet(minimum_bet, maximum_bet, 0.05f);
    printf("\nexpect: %6.3f%%, player bets for: %d", 100.0f*expect_player_bet(0), get_player_bet());
    simulate_game(gamblers_before, gamblers_after);

    /* print the cards drawn in the game and the result of the bet */
    printf("\ndealer: ");
    for (i = 0; get_dealer_card(i)>=0; i++)
        print_card(get_dealer_card(i));
    printf("= ");
    print_score(get_dealer_score());

    for (g = 0; g<gamblers_before; g++) {
        printf("\nother%d: ", g);
        for (k = 0; get_gambler_card(g, k)>=0; k++)
            print_card(get_gambler_card(g, k));
    }

    for (k = 0; get_player_card(k, 0)>=0 && k<4; k++) {
        if (k==0)
            printf("\nplayer: ");
        else
            printf("\n split: ");
        for (i = 0; get_player_card(k, i)>=0; i++)
            print_card(get_player_card(k, i));
        if (k==0 && get_player_insurance())
            printf("(insured) ");
        if (get_player_doubled(k))
            printf("(doubled) ");
        printf("= ");
        print_score(get_player_score(k));

        /* find out how much the player wins or looses */
        if (get_player_gain(k)<0)
            printf("--> loose: %d", -get_player_gain(k));
        else if (get_player_gain(k)>0)
            printf("--> win: %d", get_player_gain(k));
        else
            printf("--> standoff");
    }
    
    /* print gamblers cards */
    for (g = gamblers_before; g<gamblers_before+gamblers_after; g++) {
        printf("\nother%d: ", g);
        for (k = 0; get_gambler_card(g, k)>=0; k++)
            print_card(get_gambler_card(g, k));
    }
}


int main(int argc, char** argv)
{
	load_options();
	setup_options();
	save_options();
	setup_cards(num_decks);
	show_distribution();
	while (get_player_capital()>0) {
		printf("\n==== shuffeling ====\n");
		shuffle_cards();
		show_distribution();
		while (get_player_capital()>0 && taken_cards_distribution()<shuffle_condition) {
			num_game++;
			printf("\n---- game: %d capital: %d (min: %d, max: %d, expect: %6.0f) ----", num_game, get_player_capital(), get_min_capital(), get_max_capital(), get_expected_capital());
			a_game();
		}
	}
	printf("Bankrupted. Exiting from simbj\n");
	return 0;
}

