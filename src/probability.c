/*
 * How to beat the Blackjack dealer
 * Copyright (C) 1992, 1997 Jacob Rief
 * $Id: probability.c,v 1.1.1.1 1999/12/12 23:12:31 jacob Exp $
 */

#include "probability.h"

#define DEPTH 12

static int cards[12];
static float prob_dealer[11][24], expect_buy[11][24], expect_stay[11][24];
static float expect_split[11][24], expect_pair[11][24];
static float prob_single_bet[11][24], prob_double_bet[11][24], expect_player[11];
static float prob_stay_buy[22][24], prob_pair_split[22][22][24];
static int dealer_card = 0, depth = 0;


/* Initialise the shoe with # decks */
void init_shoe(int decks)
{
    int i;
    cards[0] = 52*decks;
    for (i = 1; i<10; i++)
        cards[i] = 4*decks;
    cards[10] = 16*decks;
    cards[11] = decks;
}


void shuffle()
{
    int i;
    cards[0] = 52*cards[11];
    for (i = 1; i<10; i++)
        cards[i] = 4*cards[11];
    cards[10] = 16*cards[11];
}


void remove_from_shoe(CARDVALUE card_value, int delta)
{
    if (card_value>=1 && card_value<=10) {
        if (delta==0)
            delta = 1;
        if (cards[card_value]-delta<0)
            delta = cards[card_value];
        cards[card_value] -= delta;
        cards[0] -= delta;
    }
}


void insert_to_shoe(CARDVALUE card_value, int delta)
{
    if (card_value>=1 && card_value<=10) {
        if (delta==0)
            delta = 1;
        if (cards[card_value]+delta>cards[11]*((card_value<10) ? 4:16))
            delta = cards[11]*((card_value<10) ? 4:16)-cards[card_value];
        cards[card_value] += delta;
        cards[0] += delta;
    }
}


int get_cards_in_shoe(CARDVALUE card_value)
{
    if (card_value>=0 && card_value<=11)
        return cards[card_value];
    else
        return -1;
}


int set_cards_in_shoe(CARDVALUE card_value, int num_cards)
{
    int i;
    if (card_value>=1 && card_value<=10) {
        if (num_cards<0 || num_cards>cards[11]*((card_value<10) ? 4:16))
            return -1;
        cards[card_value] = num_cards;
        cards[0] = 0;
        for (i = 1; i<=10; i++)
            cards[0] += cards[i];
        return cards[card_value] = num_cards;
    }    
    return -1;
}


float prob_dlh(CARDVALUE dfh, int dlh)
{
    float sum;
    int i;
    if (dlh==0) {
        if (dfh==0) {
            /* summerize the probabilities */
            sum = 0.0f;
            for (i = 1; i<=10; i++)
                sum += prob_dealer[i][0];
            return sum;
        }
        return prob_dealer[dfh][0];
    }
    if (dlh>=17 && dlh<=BUST && dfh>=0 && dfh<=10)
        return prob_dealer[dfh][dlh];
    return -1.0f;
}


float expect_buy_stay(CARDVALUE dfh, int ph)
{
    return (expect_buy[dfh][ph]-expect_stay[dfh][ph]);
}


float expect_double_buy(CARDVALUE dfh, int ph)
{
    return (2.0*expect_stay[dfh][ph]-expect_buy[dfh][ph]);
}


float expect_split_joined_default(CARDVALUE dfh, int ph)
{
    return (2.0*expect_split[dfh][ph]-expect_pair[dfh][ph]);
}


float expect_split_joined_partner(CARDVALUE dfh, int ph)
{
    return (expect_split[dfh][ph]-expect_pair[dfh][ph]);
}


float prob_player_bet(CARDVALUE dfh, int plh)
{
    int i;
    float prob = prob_single_bet[dfh][plh]+prob_double_bet[dfh][plh];
    if (dfh==0) {
        prob = 0.0f;
        for (i = 1; i<=10; i++)
            prob += (prob_single_bet[i][plh]+prob_double_bet[i][plh])*prob_dealer[i][0];
    }
    return prob;
}


float expect_player_bet(CARDVALUE dfh)
{
    return expect_player[dfh];
}


/* initialse all probabilities and expectations, ie. set them to NULL */
static void reset_probabilities(void)
{
    int i, j, k;
    for (i = 0; i<11; i++) {
        for (k = 0; k<24; k++) {
            prob_dealer[i][k] = 0.0;
            prob_single_bet[i][k] = 0.0;
            prob_double_bet[i][k] = 0.0;
            expect_stay[i][k] = 0.0;
            expect_buy[i][k] = 0.0;
            expect_pair[i][k] = 0.0;
            expect_split[i][k] = 0.0;
        }
        expect_player[i] = 0.0;
    }
    for (i = 0; i<22; i++)
        for (k = 0; k<24; k++) {
            prob_stay_buy[i][k] = 0.0;
            for (j = 0; j<22; j++)
                prob_pair_split[i][j][k] = 0.0;
        }
}


static void standarization(void)
{
  int i, j, k;
  for (i = 1; i<=10; i++)
      if (prob_dealer[i][0]!=0)
          for (k = 1; k<24; k++)
          {
              prob_dealer[0][k] += prob_dealer[i][k];
              prob_dealer[i][k] /= prob_dealer[i][0];
          }
  for (i = 9; i<=18; i++)
      if (prob_stay_buy[i][0]!=0)
          for (k = 11; k<=BUST; k++)
              prob_stay_buy[i][k] /= prob_stay_buy[i][0];
  for (j = 1; j<=10; j++)
      if (prob_pair_split[j][0][0]!=0) {
          for (i = 12; i<=18; i++) {
              if (prob_pair_split[j][i][0]!=0)
                  for (k = 12; k<=BUST; k++)
                      prob_pair_split[j][i][k] /= prob_pair_split[j][i][0];
              if (prob_pair_split[j+10][i][0]!=0)
                  for (k = 12; k<=BUST; k++)
                      prob_pair_split[j+10][i][k] /= prob_pair_split[j+10][i][0];
              prob_pair_split[j][i][0] /= prob_pair_split[j][0][0];
              prob_pair_split[j][i][1] /= prob_pair_split[j][0][0];
              prob_pair_split[j+10][i][0] /= prob_pair_split[j][0][0];
              prob_pair_split[j+10][i][1] /= prob_pair_split[j][0][0];
          }
          for (k = 17; k<=BUST; k++) {
              prob_pair_split[j][0][k] /= prob_pair_split[j][0][0];
              prob_pair_split[j+10][0][k] /= prob_pair_split[j][0][0];
          }
      }
}


/* main function to calculate expectations and probailities */
static void play(double prob_prev, int sum_prev, int soft_prev, int card_prev, int split_prev)
{
    int sum_now, soft_now, card_now, split_now;
    double prob_now;
    depth++;
    for (card_now = 1; card_now<=10; card_now++) {
        prob_now = ((float) cards[card_now]/cards[0])*prob_prev;
        split_now = split_prev;
        if (card_now==ACE) {
            sum_now = sum_prev+11;
            soft_now = soft_prev+1;
        }
        else {
            sum_now = sum_prev+card_now;
            soft_now = soft_prev;
        }
        if (depth==1) {
            dealer_card = card_now;
            prob_dealer[dealer_card][0] = prob_now;
            cards[card_now]--;
            cards[0]--;
            play(prob_now, sum_now, soft_now, card_now, split_now);
            cards[0]++;
            cards[card_now]++;
        }
        else {
            if (sum_now>21) {
                if (soft_now>0) {
                    sum_now -= 10;
                    soft_now--;
	        }
                else
                    sum_now = BUST;
            }
            if (depth==2) {
                if (card_prev==card_now) {
                    /* split */
                    split_now = card_now;
                    prob_pair_split[split_now][0][0] += prob_now;
                    if (sum_now>=17)
                        prob_pair_split[split_now][0][sum_now] += prob_now;
                    else
                        if (sum_now>=12 && soft_now==0)
                            prob_pair_split[split_now][sum_now][1] += prob_now;
                    cards[card_now]--;
                    cards[0]--;
                    play(prob_now, sum_now-card_now, 0, 0, split_now+10);
                    cards[0]++;
                    cards[card_now]++;
                }
                else
                    if (sum_now==21) /* BlackJack */
                        sum_now = BLACKJACK;
            }
            if (split_prev<=10) {
                if (sum_now>=17 && card_prev!=0)
                    prob_dealer[dealer_card][sum_now] += prob_now;
                if (((sum_prev>=12 && soft_prev==0) || sum_prev>=17)
                    || (depth==3 && sum_prev>=9 && sum_prev<=11)) {
                    prob_stay_buy[sum_prev][sum_now] += prob_now;
                    prob_stay_buy[sum_prev][0] += prob_now;
                }
                else
                    if ((depth!=4 || sum_prev!=11 || card_prev!=2) && split_now==0) {
                        if ((sum_now>=17 && soft_now==0) || sum_now>=19)
                            prob_stay_buy[0][sum_now] += prob_now;
                        else
                            if (soft_now==0 || sum_now>=17)
                                prob_stay_buy[sum_now][1] += prob_now;
                    }
            }
            if (split_prev!=0) {
                if ((sum_prev>=12 && soft_prev==0) || sum_prev>=17) {
                    prob_pair_split[split_prev][sum_prev][sum_now] += prob_now;
                    prob_pair_split[split_prev][sum_prev][0] += prob_now;
                }
                else {
                    if ((sum_now>=17 && soft_now==0) || sum_now>=19)
                        prob_pair_split[split_prev][0][sum_now] += prob_now;
                    else
                        if (soft_now==0 || sum_now>=17)
                            prob_pair_split[split_prev][sum_now][1] += prob_now;
                }
            }
            if (depth<DEPTH && prob_now>0.0 && split_prev!=11) {
                cards[card_now]--;
                cards[0]--;
                if (sum_now<17)
                    if (card_prev!=0)
                        play(prob_now, sum_now, soft_now, card_prev, split_now);
                    else
                        play(prob_now, sum_now, soft_now, 0, split_now);
                else
                    if (sum_now<19 && soft_now>0)
                        play(prob_now, sum_now, soft_now, 0, split_now);
                cards[0]++;
                cards[card_now]++;
            }
        }
    }
    depth--;
}


static void buy_contra_stay_double(void)
{
    int player, i, k;
    float prob_temp[22][24];
    /* hardhands 12-16 */
    for (player = 16; player>=9; player--) {
        for (k = 11; k<=BUST; k++)
            prob_temp[player][k]=prob_stay_buy[player][k];
        for (i = player+1; i<17; i++) {
            if (expect_buy[dealer_card][i]>expect_stay[dealer_card][i]
                || (player==9 && i==11)) {
                for (k = i; k<=BUST; k++)
                    prob_temp[player][k] += prob_temp[i][k]*prob_stay_buy[player][i];
                prob_temp[player][i] = 0.0;
            }
        }
        expect_stay[dealer_card][player] = prob_dealer[dealer_card][BUST]; /* dealer busted */
        for (i = 17; i<=BLACKJACK; i++)
            expect_stay[dealer_card][player] -= prob_dealer[dealer_card][i];
        expect_buy[dealer_card][player] = -prob_temp[player][BUST]; /* player busted */
        for (k = player+1; k<BLACKJACK; k++) {
            expect_buy[dealer_card][player] += prob_temp[player][k]*prob_dealer[dealer_card][BUST];
            for (i = 17; i<=BLACKJACK; i++) {
                if (k>i) /* player is higher */
                    expect_buy[dealer_card][player] += prob_temp[player][k]*prob_dealer[dealer_card][i];
                if (k<i) /* dealer is higher */
                    expect_buy[dealer_card][player] -= prob_temp[player][k]*prob_dealer[dealer_card][i];
            }
        }
    }
    /* softhands 7/17-8/18 */
    for (player = 18; player>=17; player--) {
        for (k = 12; k<=BUST; k++)
            prob_temp[player][k] = prob_stay_buy[player][k];
        for (i = 12; i<17; i++)
            if (expect_buy[dealer_card][i]>expect_stay[dealer_card][i]) {
                for (k = 12; k<=BUST; k++)
                    prob_temp[player][k] += prob_stay_buy[player][i]*prob_temp[i][k];
                prob_temp[player][i] = 0.0;
            }
        expect_stay[dealer_card][player] = prob_dealer[dealer_card][BUST]; /* dealer busted */
        for (i = 17; i<=BLACKJACK; i++) {
            if (i<player)
                expect_stay[dealer_card][player] += prob_dealer[dealer_card][i];
            if (i>player)
                expect_stay[dealer_card][player] -= prob_dealer[dealer_card][i];
        }
        expect_buy[dealer_card][player] = -prob_temp[player][BUST]; /* player busted */
        for (k = 12; k<BLACKJACK; k++) {
            expect_buy[dealer_card][player]
                += prob_temp[player][k]*prob_dealer[dealer_card][BUST];
            for (i = 17; i<=BLACKJACK; i++) {
                if (k>i) /* players score is higher */
                    expect_buy[dealer_card][player]
                      += prob_temp[player][k]*prob_dealer[dealer_card][i];
                if (k<i) /* dealers score is higher */
                    expect_buy[dealer_card][player]
                      -= prob_temp[player][k]*prob_dealer[dealer_card][i];
            }
        }
    }
    /* double */
    for (player = 9; player<=11; player++) {
        expect_stay[dealer_card][player] = prob_dealer[dealer_card][BUST];
        for (i = 17; i<=BLACKJACK; i++)
            for (k = 11; k<BLACKJACK; k++) {
                if (k<i)
                    expect_stay[dealer_card][player]
                      -= prob_stay_buy[player][k]*prob_dealer[dealer_card][i];
                if (k>i)
                    expect_stay[dealer_card][player]
                      += prob_stay_buy[player][k]*prob_dealer[dealer_card][i];
            }
    }
    for (k = 17; k<=BLACKJACK; k++) /* immediate */
        prob_single_bet[dealer_card][k] = prob_stay_buy[0][k];
    for (i = 12; i<=18; i++) {
        /* buy or stay */
        if (expect_buy[dealer_card][i]>expect_stay[dealer_card][i])
            for (k = 12; k<=BUST; k++)
                prob_single_bet[dealer_card][k] += prob_temp[i][k]*prob_stay_buy[i][1];
        else
            prob_single_bet[dealer_card][i] += prob_stay_buy[i][1];
    }
    for (i = 9; i<=11; i++) {
        /* double or buy */
        if (2*expect_stay[dealer_card][i]>expect_buy[dealer_card][i])
            for (k = 11; k<=21; k++)
                prob_double_bet[dealer_card][k] += prob_stay_buy[i][k]*prob_stay_buy[i][0];
        else
            for (k = 12; k<=BUST; k++)
                prob_single_bet[dealer_card][k] += prob_temp[i][k]*prob_stay_buy[i][0];
    }
}


static void split_contra_non_split(void)
{
    int player, i, j, k;
    float prob_pair[22][24], prob_split[22][24];

    for (player = 1; player<=10; player++) {
        for (i = 16; i>=12; i--) {
            for (k = 12; k<=BUST; k++) {
                prob_pair[i][k] = prob_pair_split[player][i][k];
                prob_split[i][k] = prob_pair_split[player+10][i][k];
            }
            for (j = i+1; j<17; j++)
                if (expect_buy[dealer_card][j]>expect_stay[dealer_card][j]) {
                    for (k = j; k<=BUST; k++)
                    {
                        prob_pair[i][k] += prob_pair[i][j]*prob_pair[j][k];
                        prob_split[i][k] += prob_split[i][j]*prob_split[j][k];
                    }
                    prob_pair[i][j] = 0.0;
                    prob_split[i][j] = 0.0;
                }
        }
        for (i = 18; i>=17; i--) {
            for (k = 12; k<=BUST; k++) {
                prob_pair[i][k] = prob_pair_split[player][i][k];
                prob_split[i][k] = prob_pair_split[player+10][i][k];
            }
            for (j = 12; j<17; j++)
                if (expect_buy[dealer_card][j]>expect_stay[dealer_card][j]) {
                    for (k = j; k<=BUST; k++) {
                        prob_pair[i][k] += prob_pair[i][j]*prob_pair[j][k];
                        prob_split[i][k] += prob_split[i][j]*prob_split[j][k];
                    }
                    prob_pair[i][j] = 0.0;
                    prob_split[i][j] = 0.0;
                }
	}
             for (k = 12; k<=BUST; k++) {
                 prob_pair[0][k] = prob_pair_split[player][0][k];
                 prob_split[0][k] = prob_pair_split[player+10][0][k];
             }
             if (player==ACE) {
                 for (k = 12; k<17; k++)
                     prob_split[0][k] = prob_pair_split[player+10][k][1];
                 for (i = 12; i<=18; i++)
                     if (expect_buy[dealer_card][i]>expect_stay[dealer_card][i])
                         for (k = 12; k<=BUST; k++)
                             prob_pair[0][k] += prob_pair[i][k]*prob_pair_split[player][i][1];
                     else
                         prob_pair[0][i] += prob_pair_split[player][i][1];
	}
             else {
                 for (i = 12; i<=18; i++) {
                     if (expect_buy[dealer_card][i]>expect_stay[dealer_card][i])
                         for (k = 12; k<=BUST; k++) {
                             prob_pair[0][k] += prob_pair[i][k]*prob_pair_split[player][i][1];
                             prob_split[0][k] += prob_split[i][k]*prob_pair_split[player+10][i][1];
                         }
                     else {
                         prob_pair[0][i] += prob_pair_split[player][i][1];
                         prob_split[0][i] += prob_pair_split[player+10][i][1];
                     }
                 }
             }
             expect_pair[dealer_card][player] -= prob_pair[0][BUST];
             expect_split[dealer_card][player] -= prob_split[0][BUST];
             for (k = 12; k<BLACKJACK; k++) {
                 expect_pair[dealer_card][player]
                   += prob_pair[0][k]*prob_dealer[dealer_card][BUST];
                 expect_split[dealer_card][player]
                   += prob_split[0][k]*prob_dealer[dealer_card][BUST];
	    for (i = 17; i<=BLACKJACK; i++) {
	        if (k>i) {
                         expect_pair[dealer_card][player]
                           += prob_pair[0][k]*prob_dealer[dealer_card][i];
                         expect_split[dealer_card][player]
                           += prob_split[0][k]*prob_dealer[dealer_card][i];
                     }
                     if (k<i) {
                         expect_pair[dealer_card][player]
                           -= prob_pair[0][k]*prob_dealer[dealer_card][i];
                         expect_split[dealer_card][player]
                           -= prob_split[0][k]*prob_dealer[dealer_card][i];
	        }
	    }
	}
	if (player!=5) {
                 /* never split 5-5, double instead */
	    if (2*expect_split[dealer_card][player]>expect_pair[dealer_card][player])
	        for (k = 12; k<=BUST; k++)
	            prob_double_bet[dealer_card][k]
                           += prob_split[0][k]*prob_pair_split[player][0][0];
	    else
	        for (k = 12; k<=BUST; k++)
                         prob_single_bet[dealer_card][k]
                           += prob_pair[0][k]*prob_pair_split[player][0][0];
	}
    }
}


static void expectation(void)
{
    int i,k;
    expect_player[dealer_card] = -prob_single_bet[dealer_card][BUST]
      -2*prob_double_bet[dealer_card][BUST];
    for (i = 11; i<=BLACKJACK; i++) {
        if (i==BLACKJACK)
            expect_player[dealer_card]
              += 1.5f*prob_single_bet[dealer_card][i]*prob_dealer[dealer_card][BUST];
        else
            expect_player[dealer_card]
              += prob_single_bet[dealer_card][i]*prob_dealer[dealer_card][BUST];
        expect_player[dealer_card]
          += 2*prob_double_bet[dealer_card][i]*prob_dealer[dealer_card][BUST];
        for (k = 17; k<=BLACKJACK; k++) {
            if (i>k)
            {
                if (i==BLACKJACK)
                    expect_player[dealer_card]
                      += 1.5f*prob_single_bet[dealer_card][i]*prob_dealer[dealer_card][k];
                else
                    expect_player[dealer_card]
                      += prob_single_bet[dealer_card][i]*prob_dealer[dealer_card][k];
                expect_player[dealer_card]
                  += 2*prob_double_bet[dealer_card][i]*prob_dealer[dealer_card][k];
            }
            if (i<k) {
                expect_player[dealer_card]
                  -= prob_single_bet[dealer_card][i]*prob_dealer[dealer_card][k];
                expect_player[dealer_card]
                  -= 2*prob_double_bet[dealer_card][i]*prob_dealer[dealer_card][k];
            }
        }
    }
}


void calc_distribution(void)
{
    reset_probabilities();
    depth = 0;
    play(1.0, 0, 0, 0, 0);
    standarization();
    for (dealer_card = 1; dealer_card<=10; dealer_card++) {
        buy_contra_stay_double();
        split_contra_non_split();
        expectation();
        expect_player[0] += expect_player[dealer_card]*prob_dealer[dealer_card][0];
    }
}
