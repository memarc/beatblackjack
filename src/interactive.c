/*
 * How to beat the Blackjack dealer
 * Copyright (C) 1992, 1998 Jacob Rief
 * $Id: interactive.c,v 1.2 2000/01/10 23:13:13 jacob Exp $
 */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "probability.h"
#include "display.h"

enum
{
    CARDS_IN_SHOE = 1,
    PROB_DEALER_LAST_HAND = 2,
    EXP_INSURANCE = 4,
    EXP_BUY_STAY = 8,
    EXP_DOUBLE_BUY = 16,
    EXP_SPLIT_JOINED_DEF = 32,
    EXP_SPLIT_JOINED_PAR = 64,
    EXP_PLAYER_LAST_HAND = 128,
    FINAL_EXPECTATION = 256
};


enum Command
{
    CONT = 0,
    SHOW = 25,
    DISTRIBUTION,
    DISPLAY,
    HIDE,
    DECKS,
    SHUFFLE,
    HELP,
    INVALID,
    TABLE,
    CALC,
    QUIT,
    VERSION
};


static unsigned display_table = CARDS_IN_SHOE|PROB_DEALER_LAST_HAND|EXP_INSURANCE|EXP_BUY_STAY|EXP_DOUBLE_BUY
                                |EXP_SPLIT_JOINED_DEF|EXP_PLAYER_LAST_HAND|FINAL_EXPECTATION;


static void print_version(void)
{
    printf("*** bbjd: Beat the Blackjack dealer (version 1.0.3) ***\n");
    printf("Copyright (C) 2000, Jacob Rief\n");
    printf("bbjd comes with ABSOLUTELY NO WARRANTY\n");
    printf("This is free software, and you are welcome to redistribute it\n");
}


static void print_help(void)
{
    printf("--- Commands for bbjd: ---\n"
       "help:\t\tthis text.\n"
       "display [item], hide [item]: switch on/off which of the tables should be represented.\n"
       "\t[item] may be one of:\n"
       "\tall:\t\tshow all tables.\n"
       "\tfinal:\t\tshow final expectation.\n"
       "\tcards:\t\tshow card distribution in shoe.\n"
       "\tdealer:\t\tshow probabilities for dealers last hand.\n"
       "\tbuy:\t\tshow players expectation for buy contra stay.\n"
       "\tinsurance:\tshow players expectation for insure against dealers ace.\n"
       "\tdouble:\t\tshow players expectation for double contra normal buy.\n"
       "\tsplit:\t\tshow players expectation for split contra normal buy. (default rule)\n"
       "\tpartner:\tshow partners expectation for split contra normal buy. (half bet).\n"
       "\tplayer:\t\tshow probability for players last hand.\n"
       "[card-value] +, -, or = [value]: increase/decrease/set the number of cards with value in shoe.\n"
       "\tcard-value may be any of:\n"
       "\tace, 1, two, 2, three, 3, four, 4, five, 5, six, 6, seven, 7, eight, 8\n"
       "\tnine, 9, ten, jack, queen, king, 0, t or 10.\n"
       "show:\t\tshow card distribution in shoe.\n"
       "run:\t\trecalculate probabilities and expectation with this card distribution.\n"
       "decks [value]:\tsetup a new shoe with value*52 cards.\n"
       "reset, shuffle:\treset the shoe, set 52 cards for each pack.\n"
       "version:\tprint information about this program.\n"
       "quit, exit:\tleave bbjd.\n");
}


static char* skip_spaces(char** str)
{
    while (*str[0]==' ' || *str[0]==',' || *str[0]==';' || *str[0]==':')
        ++*str;
    return *str;
}


static char* parse_card(FILE* in, enum Command com, char* remain)
{
    int parsed, delta, card_value;
    char arg1[32], arg2[32];

    if (skip_spaces(&remain)[0]=='\0')
        return NULL;
    /* start parsing for second argument */
    card_value = com;
    parsed = sscanf(remain, "%[=]%d", arg1, &delta);
    if (parsed>=1) {
        if (parsed>=2) {
            if (set_cards_in_shoe(card_value, delta)<0)
                printf("Value out of range.\n");
            sprintf(arg2, "%d", delta);
            remain = strstr(remain, arg2)+strlen(arg2);
            return remain;
        }
        else {
            printf("Assigned value not a number\n");
            return NULL;
        }
    }
    parsed = sscanf(remain, "%[+]", arg1);
    if (parsed>=1) {
        for (delta = 0; arg1[delta]=='+'; delta++)
            ;
        insert_to_shoe(card_value, delta);
        remain = strstr(remain, arg1)+strlen(arg1);
        return remain;
    }
    parsed = sscanf(remain, "%[-]", arg1);
    if (parsed>=1) {
        for (delta = 0; arg1[delta]=='-'; delta++)
            ;
        remove_from_shoe(card_value, delta);
        remain = strstr(remain, arg1)+strlen(arg1);
        return remain;
    }
    parsed = sscanf(remain, "%s", arg1);
    if (parsed>0)
        printf("Invalid argument passed to card-value: %s\n", arg1);
    else
        printf("No argument passed to card-value\n");
    return NULL;
}


static char* parse_display(FILE* in, enum Command com, char* remain)
{
    int parsed, valid = 0;
    char arg1[32];
    unsigned dspyhide = 0;

    if (skip_spaces(&remain)[0]=='\0')
        return NULL;
    parsed = sscanf(remain, "%s", arg1);
    if (parsed>=1) {
        if (strcmp(arg1, "all")==0)
            dspyhide = CARDS_IN_SHOE|PROB_DEALER_LAST_HAND|EXP_INSURANCE|EXP_BUY_STAY
                |EXP_DOUBLE_BUY|EXP_SPLIT_JOINED_DEF|EXP_SPLIT_JOINED_PAR
                |EXP_PLAYER_LAST_HAND|FINAL_EXPECTATION, valid = 1;
        if (strcmp(arg1, "final")==0 || strcmp(arg1, "t")==0)
            dspyhide |= FINAL_EXPECTATION, valid = 1;
        if (strcmp(arg1, "cards")==0 || strcmp(arg1, "c")==0)
            dspyhide |= CARDS_IN_SHOE, valid = 1;
        if (strcmp(arg1, "dealer")==0 || strcmp(arg1, "d")==0)
            dspyhide |= PROB_DEALER_LAST_HAND, valid = 1;
        if (strcmp(arg1, "buy")==0 || strcmp(arg1, "b")==0)
            dspyhide |= EXP_BUY_STAY, valid = 1;
        if (strcmp(arg1, "insurance")==0 || strcmp(arg1, "i")==0)
            dspyhide |= EXP_INSURANCE, valid = 1;
        if (strcmp(arg1, "double")==0)
            dspyhide |= EXP_DOUBLE_BUY, valid = 1;
        if (strcmp(arg1, "split")==0)
            dspyhide |= EXP_SPLIT_JOINED_DEF, valid = 1;
        if (strcmp(arg1, "partner")==0)
            dspyhide |= EXP_SPLIT_JOINED_PAR, valid = 1;
        if (strcmp(arg1, "player")==0 || strcmp(arg1, "p")==0)
            dspyhide |= EXP_PLAYER_LAST_HAND, valid = 1;
        if (strcmp(arg1, "none")==0)
            dspyhide = 0, valid = 1;
        if (valid) {
            if (com==DISPLAY)
                display_table |= dspyhide;
            else
                display_table &= ~dspyhide;
            remain = strstr(remain, arg1)+strlen(arg1);
        }
        else {
            printf("'display'/'hide' must be followed by one on the arguments:\n"
                 "    all, final, cards, dealer, buy, insurance, double, partner, player or none\n");
            remain = NULL;
        }
    }
    else {
        printf("'hide' must be followed by an argument\n");
        remain = NULL;
    }
    return remain;
}


static char* parse_decks(FILE* in, enum Command com, char* remain)
{
    int parsed, decks;
    char arg1[32];
    if (skip_spaces(&remain)[0]=='\0')
        return NULL;
    parsed = sscanf(remain, "%d", &decks);
    if (parsed>=1) {
        if (decks>0) {
            init_shoe(decks);
            sprintf(arg1, "%d", decks);
            remain = strstr(remain, arg1)+strlen(arg1);
        }
        else {
            printf("'decks' must be a positive number\n");
            remain = NULL;
        }
    }
    else {
        printf("'decks' must be followed by a numeric argument\n");
        remain = NULL;
    }
    return remain;
}


static int next_command(FILE* in)
{
    int parsed, card_value;
    enum Command com = CONT;
    char inputline[256], arg1[32], arg2[32];
    char* string = NULL;
    while (1) {
        arg1[0] = '\0'; arg2[0] = '\0';
        if (string==NULL) {
            printf("bbjd> ");
            if ((string = fgets(inputline, 256, in))==NULL)
                continue;
        }
        if (skip_spaces(&string)[0]=='\0') {
            string = NULL;
            continue;
        }

        /* find the command */
        parsed = sscanf(string, "%d%s", &card_value, arg2);
        if (parsed>=1) {
            /* arg1 seems to be a number */
            sprintf(arg1, "%d", card_value);
            if (card_value>=1 && card_value<=10)
                com = card_value;
            else if (card_value==0)
                com = 10;
            else
                com = INVALID;
        }
        else {
            /* arg1 seems to be a string */
            parsed = sscanf(string, "%[^ +-=\n]%s", arg1, arg2);
            if (parsed>=1) {
                if (strcmp(arg1, "ace")==0 || strcmp(arg1, "a")==0)
                    com = ACE;
                else if (strcmp(arg1, "two")==0)
                    com = TWO;
                else if (strcmp(arg1, "three")==0)
                    com = THREE;
                else if (strcmp(arg1, "four")==0)
                    com = FOUR;
                else if (strcmp(arg1, "five")==0)
                    com = FIVE;
                else if (strcmp(arg1, "six")==0)
                    com = SIX;
                else if (strcmp(arg1, "seven")==0)
                    com = SEVEN;
                else if (strcmp(arg1, "eight")==0)
                    com = EIGHT;
                else if (strcmp(arg1, "nine")==0)
                    com = NINE;
                else if (strcmp(arg1, "ten")==0 || strcmp(arg1, "jack")==0 
                         ||strcmp(arg1, "queen")==0 || strcmp(arg1, "king")==0
                         || strcmp(arg1, "t")==0)
                    com = TEN;
                else if (strcmp(arg1, "show")==0 || strcmp(arg1, "s")==0)
                    com = DISTRIBUTION;
                else if (strcmp(arg1, "display")==0 || strcmp(arg1, "d")==0)
                    com = DISPLAY;
                else if (strcmp(arg1, "hide")==0 || strcmp(arg1, "h")==0)
                    com = HIDE;
                else if (strcmp(arg1, "decks")==0)
                    com = DECKS;
                else if (strcmp(arg1, "shuffle")==0 || strcmp(arg1, "/")==0 || strcmp(arg1, "reset")==0)
                    com = SHUFFLE;
                else if (strcmp(arg1, "run")==0 || strcmp(arg1, "r")==0)
                    com = CALC;
                else if (strcmp(arg1, "help")==0 || strcmp(arg1, "?")==0)
                    com = HELP;
                else if (strcmp(arg1, "version")==0)
                    com = VERSION;
                else if (strcmp(arg1, "quit")==0 || strcmp(arg1, "q")==0
                         || strcmp(arg1, "exit")==0)
                    com = QUIT;
                else
                    com = INVALID;
            }
        }
        switch (com) {
        case ACE: case TWO: case THREE: case FOUR: case FIVE:
        case SIX: case SEVEN: case EIGHT: case NINE: case TEN:
            if (parsed==0)
                return SHOW;
            string = strstr(string, arg1)+strlen(arg1);
            string = parse_card(in, com, string);
            break;

        case DISTRIBUTION:
            return DISTRIBUTION;

        case DISPLAY:
            if (parsed<=1)
                return TABLE;
            string = strstr(string, arg1)+strlen(arg1);
            string = parse_display(in, com, string);
            break;

        case HIDE:
            if (parsed==0)
                return TABLE;
            string = strstr(string, arg1)+strlen(arg1);
            string = parse_display(in, com, string);
            break;

        case CALC:
            return CALC;
                  
        case DECKS:
            if (parsed==0)
                return CALC;
            string = strstr(string, arg1)+strlen(arg1);
            string = parse_decks(in, com, string);
            break;

        case SHUFFLE:
            if (parsed==0)
                return CALC;
            shuffle();
            string = strstr(string, arg1)+strlen(arg1);
            break;

        case HELP:
            print_help();
            com = CONT;
            string = NULL;
            break;

        case VERSION:
            print_version();
            com = CONT;
            string = NULL;
            break;

        case INVALID:
            printf("Invalid command: '%s'\n", arg1);
            com = CONT;
            string = NULL;
            break;

        case QUIT:
            return QUIT;

        default:
            com = CONT;
            string = NULL;
            break;
        }
    }
    return com;
}
 

int main(int argc, char** argv)
{
    int run = CALC;
    print_version();
    init_shoe(6);
    while (run!=QUIT) {
        switch (run) {
        case CALC:
            calc_distribution();
            printf("============================================"
                   "=========================================\n");
        case TABLE:
            if (display_table&CARDS_IN_SHOE)
                display_shoe(stdout);
            if (display_table&PROB_DEALER_LAST_HAND)
                display_prob_dlh(stdout);
            if (display_table&EXP_INSURANCE)
                display_exp_insu(stdout);
            if (display_table&EXP_BUY_STAY)
                display_exp_buy_stay(stdout);
            if (display_table&EXP_DOUBLE_BUY)
                display_exp_double_buy(stdout);
            if (display_table&EXP_SPLIT_JOINED_DEF)
                display_exp_split_joined_default(stdout);
            if (display_table&EXP_SPLIT_JOINED_PAR)
                display_exp_split_joined_partner(stdout);
            if (display_table&EXP_PLAYER_LAST_HAND)
                display_player_last_hand(stdout);
            if (display_table&FINAL_EXPECTATION)
                display_final_expectation(stdout);
            break;

        case SHOW:
            if (display_table&CARDS_IN_SHOE)
                display_shoe(stdout);
            break;

        case DISTRIBUTION:
            display_shoe(stdout);
            break;

        default:
            break;
        }
        run = next_command(stdin);
    }
    return 0;
}
