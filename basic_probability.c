/*
 * How to beat the Blackjack dealer
 * Copyright (C) 1992, 1999 Jacob Rief
 * $Id: basic_probability.c,v 1.1.1.1 1999/12/12 23:12:31 jacob Exp $
 */

#include "probability.h"
static int num_decks;

float prob_dlh(CARDVALUE dealer_first_hand, int dealer_last_hand)
{
    static float basic_prob_dlh[8][11] = {
        {  1.0, .077, .077, .077, .077, .077, .077, .077, .077, .077, .308 },
        { .145, .130, .140, .134, .131, .122, .166, .369, .129, .120, .112 },
        { .139, .131, .134, .131, .124, .122, .106, .138, .360, .117, .112 },
        { .134, .131, .130, .125, .121, .118, .106, .078, .129, .352, .112 },
        { .180, .131, .124, .121, .116, .112, .102, .079, .069, .120, .340 },
        { .073, .054, .118, .115, .112, .108, .097, .074, .069, .061, .035 },
        { .047, .309,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0, .077 },
        { .282, .115, .354, .374, .396, .418, .423, .262, .244, .229, .212 }
    };
    return basic_prob_dlh[dealer_last_hand>0 ? dealer_last_hand-16 : 0][dealer_first_hand];
}


float expect_buy_stay(CARDVALUE dfh, int ph)
{
    static float basic_expect_buy_stay[7][10] = {
        {  .218,  .039,  .018, -.005, -.028, -.017,  .263,  .240,  .201,  .147 },
        {  .186, -.016, -.040, -.066, -.093, -.082,  .207,  .188,  .154,  .106 },
        {  .155, -.071, -.098, -.127, -.158, -.148,  .153,  .139,  .110,  .067 },
        {  .127, -.126, -.157, -.189, -.223, -.213,  .105,  .094,  .068,  .032 },
        {  .103, -.179, -.213, -.248, -.286, -.277,  .061,  .054,  .032,  .000 },
        {  .205,  .152,  .145,  .138,  .135,  .115,  .159,  .308,  .264,  .202 },
        {  .004, -.059, -.058, -.058, -.053, -.094, -.230, -.068,  .081,  .030 }
    };
    return (basic_expect_buy_stay[ph-12][dfh-1]);
}


float expect_double_buy(CARDVALUE dfh, int ph)
{
    static float basic_expect_double_buy[3][10] = {
        { -.559, -.012,  .022,  .056,  .091,  .123, -.064, -.123, -.244, -.362 },
        { -.371,  .178,  .206,  .234,  .261,  .290,  .139,  .091,  .033, -.104 },
        { -.330,  .233,  .259,  .286,  .311,  .335,  .172,  .121,  .073, -.018 } 
    };
    return (basic_expect_double_buy[ph-9][dfh-1]);
}


float expect_split_joined_default(CARDVALUE dfh, int ph)
{
    static float basic_expect_split_joined_default[10][10] = {
        { -.212,  .395,  .420,  .447,  .466,  .488,  .306,  .262,  .238,  .161 },
        { -.414, -.035, -.015,  .008,  .044,  .067,  .034, -.047, -.139, -.255 },
        { -.411, -.059, -.029,  .005,  .041,  .064,  .042, -.040, -.134, -.251 },
        { -.525, -.211, -.175, -.137, -.094, -.096, -.266, -.264, -.274, -.383 },
        { -.760, -.445, -.402, -.355, -.305, -.296, -.510, -.589, -.665, -.691 },
        { -.484, -.023,  .026,  .071,  .105,  .131, -.084, -.158, -.243, -.345 },
        { -.427,  .077,  .101,  .130,  .159,  .215,  .193, -.042, -.130, -.226 },
        { -.224,  .251,  .270,  .293,  .315,  .387,  .582,  .339,  .089, -.037 },
        { -.330,  .025,  .053,  .082,  .119,  .107, -.058,  .091,  .080, -.194 },
        { -.653, -.280, -.243, -.203, -.159, -.134, -.263, -.401, -.531, -.545 }
    };
    return (basic_expect_split_joined_default[ph-1][dfh-1]);
}


float expect_split_joined_partner(CARDVALUE dfh, int ph)
{
    static float basic_expect_split_joined_parter[10][10] = {
        {  .055,  .156,  .158,  .159,  .153,  .151,  .070,  .083,  .119,  .151 },
        {  .034,  .040,  .034,  .027,  .026,  .028,  .061,  .056,  .051,  .044 },
        {  .055,  .042,  .040,  .038,  .036,  .039,  .099,  .091,  .081,  .070 },
        { -.040, -.095, -.093, -.090, -.085, -.106, -.175, -.102, -.032, -.039 },
        { -.254, -.315, -.305, -.295, -.283, -.293, -.384, -.394, -.391, -.319 },
        {  .035,  .117,  .131,  .140,  .134,  .143,  .067,  .059,  .050,  .043 },
        {  .096,  .185,  .176,  .169,  .161,  .185,  .261,  .169,  .155,  .145 },
        {  .220,  .272,  .261,  .251,  .239,  .271,  .496,  .397,  .297,  .268 },
        {  .024, -.048, -.048, -.047, -.042, -.088, -.229, -.007,  .132,  .023 },
        { -.399, -.460, -.446, -.431, -.416, -.419, -.518, -.597, -.644, -.491 }
    };
    return (basic_expect_split_joined_parter[ph-1][dfh]);
}


float prob_player_bet(CARDVALUE dfh, int plh)
{
    /* to do */
    return 0.0f;
}


float expect_player_bet(CARDVALUE dealer_first_hand)
{
    static float basic_player_bet[11] = {
        -0.00849, -.340 , .089, .120, .155, .192, .223, .142, .057, -.041, -.177
    };
    return basic_player_bet[dealer_first_hand];
}


int get_cards_in_shoe(CARDVALUE card_value)
{
    switch (card_value) {
    case 0:
        return 52*num_decks;
    case ACE: case 2: case 3: case 4: case 5: case 6: case 7: case 8: case 9:
        return 4*num_decks;
    case 10:
        return 16*num_decks;
    case 11:
        return num_decks;
    default:
        return -1;
    }
}


int set_cards_in_shoe(CARDVALUE card_value, int num_cards)
{
    return -1;
}


void insert_to_shoe(CARDVALUE card_value, int delta)
{
    /* dummy */
}


void init_shoe(int decks)
{
    num_decks = decks;
}


void remove_from_shoe(CARDVALUE card_value, int delta)
{
    /* dummy */
}


void calc_distribution(void)
{
    /* dummy */
}


void shuffle()
{
    /* dummy */
}

