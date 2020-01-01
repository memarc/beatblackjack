#!/usr/bin/perl
# $Id: playbj.pl,v 1.2 2000/01/10 23:13:10 jacob Exp $

use basicsimbj;

@card_name = (
  "ca", "c2", "c3", "c4", "c5", "c6", "c7", "c8", "c9", "c10", "cb", "cd", "ck",
  "ha", "h2", "h3", "h4", "h5", "h6", "h7", "h8", "h9", "h10", "hb", "hd", "hk",
  "ka", "k2", "k3", "k4", "k5", "k6", "k7", "k8", "k9", "k10", "kb", "kd", "kk",
  "pa", "p2", "p3", "p4", "p5", "p6", "p7", "p8", "p9", "p10", "pb", "pd", "pk"
);


sub place_cards
{
    my $i, $k, $s, $card, $offset[5], $n = 0;

    # pure html part
    print "<table border=0 cellpadding=0 cellspacing=3>\n";
    print "<th align=center><td>Dealer</td></th>\n";

    # dealer's row
    print "<tr align=center>\n";
    $card = $card_name[basicsimbj::get_dealer_card(0)];
    print "<td><img src=\"cards/$card.gif\" width=74 heigth=98 alt=\"Playing according to the basic strategy\"></td>\n";
    for ($i = 1; basicsimbj::get_dealer_card($i)>=0; $i++) {
        print "<td><img src=\"cards/empty.gif\" width=74 heigth=98></td>";
    }
    print "<td><img src=\"cards/empty.gif\" width=74 heigth=98></td></tr>\n";
    $offset[0] = $i+1;

    # player's row
    print "<th align=center><td>Player</td></th>\n";
    for ($s = 0; basicsimbj::get_player_card($s, 0)>=0; $s++) {
        print "<tr>";
        for ($i = 0; basicsimbj::get_player_card($s, $i)>=0; $i++) {
            print "<td><img src=\"cards/empty.gif\" width=74 heigth=98></td>";
        }
        print "<td><img src=\"cards/empty.gif\" width=74 heigth=98></td></tr>\n";
        $offset[$s+1] = $offset[$s]+$i+1;
    }
    print "</table><br>\n";

    # Javascript part
    print "<script language=\"JavaScript\">\n";
    # print "<!--\n";
    print "i = 0;\n";
    print "activ = window.setInterval(\"next_card()\", 2222);\n";
    print "function next_card() {\n";
    print "  switch (i) {\n";
    for ($s = 0; ($k = basicsimbj::get_player_card($s, 0))>=0; $s++) {
        $card = $card_name[$k];
        $k = $offset[$s];
        print "    case $n:\n";
        print "      new_img=new Image();\n";
        print "      new_img.src=\"cards/$card.gif\";\n";
        print "      document.images[$k].src=new_img.src;\n";
        print "      break;\n";
        $n++;
    }
    for ($s = 0; basicsimbj::get_player_card($s, 0)>=0; $s++) {
        for ($i = 1; ($k = basicsimbj::get_player_card($s, $i))>=0; $i++) {
            $card = $card_name[$k];
            $k = $offset[$s]+$i;
            if ($i==2 && basicsimbj::get_player_doubled($s)) {
                print "    case $n:\n";
                print "      new_img=new Image();\n";
                print "      new_img.src=\"cards/doubled.gif\";\n";
                print "      document.images[$k].src=new_img.src;\n";
                print "      break;\n";
                $n++;
            }
            print "    case $n:\n";
            print "      new_img=new Image();\n";
            print "      new_img.src=\"cards/$card.gif\";\n";
            print "      document.images[$k].src=new_img.src;\n";
            print "      break;\n";
            $n++;
        }
        $k = $offset[$s]+$i;
        my $ps = basicsimbj::get_player_score($s);
        if ($ps==22) {
            $card = "blackjack";
        } else {
            if ($ps==23) {
                $card = "busted";
            } else {
                $card = sprintf("score%s", $ps);
            }
        }
        print "    case $n:\n";
        print "      new_img=new Image();\n";
        print "      new_img.src=\"cards/$card.gif\";\n";
        print "      document.images[$k].src=new_img.src;\n";
        print "      break;\n";
        $n++;
    }
    for ($i = 1; ($k = basicsimbj::get_dealer_card($i))>=0; $i++) {
        $card = $card_name[$k];
        print "    case $n:\n";
        print "      new_img=new Image();\n";
        print "      new_img.src=\"cards/$card.gif\";\n";
        print "      document.images[$i].src=new_img.src;\n";
        print "      break;\n";
        $n++;
    }
    $ps = basicsimbj::get_dealer_score();
    if ($ps==22) {
        $card = "blackjack";
    } else {
          if ($ps==23) {
            $card = "busted";
          } else {
            $card = sprintf("score%s", $ps);
          }
    }
    print "    case $n:\n";
    print "      new_img=new Image();\n";
    print "      new_img.src=\"cards/$card.gif\";\n";
    print "      document.images[$i].src=new_img.src;\n";
    print "      break;\n";
    $n++;
    print "    case $n:\n";
    print "      window.clearInterval(activ);\n";
    print "      break;\n";
    print "  }\n";
    print "  i = i+1;\n";
    print "}\n";
    # print "-->\n";
    print "</script>\n";
}


print "\n";
basicsimbj::setup_cards(4);
basicsimbj::shuffle_cards_permutation(3);
basicsimbj::simulate_game(0, 0);
place_cards;

