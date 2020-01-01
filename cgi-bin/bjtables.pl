##!/usr/bin/perl
# $Id: bjtables.pl,v 1.2 2000/01/10 23:13:10 jacob Exp $

use bbjd;
my $width1=84;
my $width2=50;

read(STDIN, $data, $ENV{'CONTENT_LENGTH'});
print "Content-type: text/html\n\n";
print "<html><head><title>Beat the Blackjack dealer (Results)</title></head>\n";
print "<body bgcolor=#F8F8FF text=#000000 vlink=#400040 alink=#000080>";

@formfields = split(/&/, $data);
foreach $found (@formfields) {
    ($name, $value) = split(/=/, $found);
    $value =~ tr/+/ /;
    $value =~ s/%([a-fA-F0-9][a-fA-F0-9])/pack("C", hex($1))/eg;
    $value =~ s/<!--(.|\n)*-->//g;
    $field{$name} = $value;
}

sub debug_input
{
    print "\ndata:--->$data<---<br>\n";
    foreach $key (keys %field) {
        print " (:", $key, ":", $field{$key}, ":)<br>\n";
    }
}
#debug_input;

sub table_pdlh
{
    print "<a href=\"/f_table.html#dlh\" target=\"upper\"><h4>Probability of DEALERS last hand</h4></a>";
    print "<table border=0 cellpadding=2 cellspacing=3>";
    print "<tr bgcolor=#C0C0C0 align=center>";
    print "<th width=$width1 align=right>p(DFH)</th>";
    for ($i = 2; $i<=10; $i++) {
        printf("<td width=$width2>%5.1f</td>", bbjd::prob_dlh($i, 0)*100);
    }
    printf "<td width=$width2>%5.1f</td><td width=$width2>%5.1f</td>", bbjd::prob_dlh(1, 0)*100, bbjd::prob_dlh(0, 0)*100;
    print "</tr>";
    for ($k = 23; $k>=17; $k--) {
        if ($k%2==0) {
            print "<tr bgcolor=#C0C0C0 align=center>";
        } else {
            print "<tr bgcolor=#E0E0E0 align=center>";
        }
        if ($k==23) {
            print "<th align=right>p(bust)</th>";
        } else {
            if ($k==22) {
                print "<th align=right>p(BJ)</th>";
            } else {  
                print "<th align=right>p($k)</th>";
            }
        }
        for ($i = 2; $i<=10; $i++) {
            printf("<td>%5.1f</td>", bbjd::prob_dlh($i, $k)*100);
        }
        printf "<td>%5.1f</td><td>%5.1f</td>", bbjd::prob_dlh(1, $k)*100, bbjd::prob_dlh(0, $k)*100;
        print "</tr>";
    }
    print "</table>\n";
}


sub table_pplh
{
    print "<a href=\"/f_table.html#plh\" target=\"upper\"><h4>Probability of PLAYERS last hand</h4></a>";
    print "<table border=0 cellpadding=2 cellspacing=3>";
    for ($k = 23; $k>=11; $k--) {
        if ($k%2==1) {
            print "<tr bgcolor=#C0C0C0 align=center>";
        } else {
            print "<tr bgcolor=#E0E0E0 align=center>";
        }
        if ($k==23) {
            print "<th width=$width1 align=right>p(bust)</th>";
        } else {
            if ($k==22) {
                print "<th align=right>p(BJ)</th>";
            } else {  
                print "<th align=right>p($k)</th>";
            }
        }
        for ($i = 2; $i<=10; $i++) {
            printf("<td width=$width2>%5.1f</td>", bbjd::prob_player_bet($i, $k)*100);
        }
        printf "<td width=$width2>%5.1f</td><td width=$width2>%5.1f</td>", bbjd::prob_player_bet(1, $k)*100, bbjd::prob_player_bet(0, $k)*100;
        print "</tr>";
    }
    print "</table>\n";
}


sub table_buy
{
    print "<a href=\"/f_table.html#buy\" target=\"upper\"><h4>Expectations for BUY</h4></a>";
    print "<table border=0 cellpadding=2 cellspacing=3>";
    for ($k = 18; $k>=12; $k--) {
        if ($k%2==0) {
            print "<tr bgcolor=#C0C0C0 align=center>";
        } else {
            print "<tr bgcolor=#E0E0E0 align=center>";
        }
        if ($k<17) {
            print "<th align=right>E($k)</th>";
        } else {
            print "<th width=$width1 align=right>E(", $k-10, "/$k)</th>";
        }
        for ($i = 2; $i<=10; $i++) {
            my $ebs = bbjd::expect_buy_stay($i, $k);
            if ($ebs<0) {
                print "<td width=$width2 bgcolor=#E0A0A0>";
            } else {
                print "<td width=$width2 bgcolor=#A0E0A0>";
            }
            printf("%5.1f</td>", $ebs*100);
        }
        my $ebs = bbjd::expect_buy_stay(1, $k);
        if ($ebs<0) {
            print "<td width=$width2 bgcolor=#E0A0A0>";
        } else {
            print "<td width=$width2 bgcolor=#A0E0A0>";
        }
        printf "%5.1f</td>", $ebs*100;
        print "</tr>";
    }
    print "</table>\n";
}


sub table_double
{
    print "<a href=\"/f_table.html#double\" target=\"upper\"><h4>Expectations for DOUBLE</h4></a>";
    print "<table border=0 cellpadding=2 cellspacing=3>";
    for ($k = 11; $k>=9; $k--) {
        if ($k%2==0) {
            print "<tr bgcolor=#E0E0E0 align=center>";
        } else {
            print "<tr bgcolor=#C0C0C0 align=center>";
        }
        print "<th width=$width1 align=right>E($k)</th>";
        for ($i = 2; $i<=10; $i++) {
            my $value = bbjd::expect_double_buy($i, $k);
            if ($value<0) {
                print "<td width=$width2 bgcolor=#E0A0A0>";
            } else {
                print "<td width=$width2 bgcolor=#A0E0A0>";
            }
            printf("%5.1f</td>", $value*100);
        }
        my $value = bbjd::expect_double_buy(1, $k);
        if ($value<0) {
            print "<td width=$width2 bgcolor=#E0A0A0>";
        } else {
            print "<td width=$width2 bgcolor=#A0E0A0>";
        }
        printf "%5.1f</td>", $value*100;
        print "</tr>";
    }
    print "</table>\n";
}


sub table_split
{
    my $partner = shift;
    if (defined $partner) {
        print "<a href=\"/f_table.html#partner\" target=\"upper\"><h4>Expectation for Partner-SPLIT</h4></a>";
    } else {
        print "<a href=\"/f_table.html#split\" target=\"upper\"><h4>Expectation for SPLIT</h4></a>";
    }
    print "<table border=0 cellpadding=2 cellspacing=3>";
    for ($k = 1; $k<=10; $k++) {
        if ($k%2==0) {
            print "<tr bgcolor=#E0E0E0 align=center>";
        } else {
            print "<tr bgcolor=#C0C0C0 align=center>";
        }
        if ($k==1) {
            print "<th width=$width1 align=right>E(A-A)</th>";
        } else {
              if ($k==10) {
                  print "<th align=right>E(X-X)</th>";
              } else {
                  print "<th align=right>E($k-$k)</th>";
              }
        }
        my $esd;
        for ($i = 2; $i<=10; $i++) {
            if (defined $partner) {
                $esd = bbjd::expect_split_joined_partner($i, $k);
            } else {
                $esd = bbjd::expect_split_joined_default($i, $k);
            }
            if ($esd<0) {
                print "<td width=$width2 bgcolor=#E0A0A0>";
            } else {
                print "<td width=$width2 bgcolor=#A0E0A0>";
            }
            printf("%5.1f</td>", $esd*100);
        }
        if (defined $partner) {
            $esd = bbjd::expect_split_joined_partner(1, $k);
        } else {
            $esd = bbjd::expect_split_joined_default(1, $k);
        }
        if ($esd<0) {
            print "<td width=$width2 bgcolor=#E0A0A0>";
        } else {
            print "<td width=$width2 bgcolor=#A0E0A0>";
        }
        printf "%5.1f</td>", $esd*100;
        print "</tr>";
    }
    print "</table>\n";
}


sub table_final
{
    print "<a href=\"/f_table.html#total\" target=\"upper\"><h4>Total expectations</h4></a>";
    print "<table border=0 cellpadding=2 cellspacing=3>";
    print "<tr bgcolor=#C0C0C0 align=center>";
    print "<th width=$width1 align=right>Expect</th>";
    for ($i = 2; $i<=10; $i++) {
        printf("<td width=$width2>%5.1f</td>", bbjd::expect_player_bet($i)*100);
    }
    printf "<td width=$width2>%5.1f</td>", bbjd::expect_player_bet(1)*100;
    my $value = bbjd::expect_player_bet(0)*100;
    if ($value<0) {
        printf "<td width=$width2 bgcolor=#E0A0A0><b>%6.2f</b></td>", $value;
    } else {
        printf "<td width=$width2 bgcolor=#A0E0A0><b>%6.2f</b></td>", $value;
    }
    print "</tr>";
    print "</table>\n";
}


sub table_insure
{
    print "<a href=\"/f_table.html#insure\" target=\"upper\"><h4>Expectation for insurance</h4></a>";
    print "<table border=0 cellpadding=2 cellspacing=3>";
    print "<tr align=center bgcolor=#C0C0C0>";
    print "<th align=right width=$width1>E(insurance)</th>";
    print "<td width=$width2></td><td width=$width2></td><td width=$width2></td><td width=$width2></td>";
    print "<td width=$width2></td><td width=$width2></td><td width=$width2></td><td width=$width2></td>";
    printf "<td width=$width2></td>";
    my $eins = (3*bbjd::prob_dlh(1, 22)-1);
    if ($eins<0) {
        printf "<td width=$width2 bgcolor=#E0A0A0>%5.1f</td>", ($eins*100.0);
    } else {
        printf "<td width=$width2 bgcolor=#A0E0A0>%5.1f</td>", ($eins*100.0);
    }
    print "</tr>";
    print "</table>\n";
}


# main function
print "<table border=0 cellpadding=2 cellspacing=3><tr align=center bgcolor=#E0E0A0>";
print "<th width=$width1>Dealers First Hand</th>";
print "<th width=$width2><b>2</b></th>";
print "<th width=$width2><b>3</b></th>";
print "<th width=$width2><b>4</b></th>";
print "<th width=$width2><b>5</b></th>";
print "<th width=$width2><b>6</b></th>";
print "<th width=$width2><b>7</b></th>";
print "<th width=$width2><b>8</b></th>";
print "<th width=$width2><b>9</b></th>";
print "<th width=$width2><b>X</b></th>";
print "<th width=$width2><b>A</b></th>";
print "<th width=$width2><b>Total</b></th>";
print "</tr></table>";

if (length($data)>1) {
    bbjd::init_shoe($field{'Decks'});
    bbjd::set_cards_in_shoe(1, $field{'C1'});
    bbjd::set_cards_in_shoe(2, $field{'C2'});
    bbjd::set_cards_in_shoe(3, $field{'C3'});
    bbjd::set_cards_in_shoe(4, $field{'C4'});
    bbjd::set_cards_in_shoe(5, $field{'C5'});
    bbjd::set_cards_in_shoe(6, $field{'C6'});
    bbjd::set_cards_in_shoe(7, $field{'C7'});
    bbjd::set_cards_in_shoe(8, $field{'C8'});
    bbjd::set_cards_in_shoe(9, $field{'C9'});
    bbjd::set_cards_in_shoe(10, $field{'C10'});
    bbjd::calc_distribution;

    # print expectation and probability tables
    table_final if ($field{'etotal'} eq 'on');
    table_buy if ($field{'ebuy'} eq 'on');
    table_double if ($field{'edouble'} eq 'on');
    table_split if ($field{'esplit'} eq 'on');
    table_split(1) if ($field{'espp'} eq 'on');
    table_insure if ($field{'einsure'} eq 'on');
    table_pdlh if ($field{'pdlh'} eq 'on');
    table_pplh if ($field{'pplh'} eq 'on');

    print "<HR><BR>Copyright &copy; 2000, <A HREF=\"mailto:bbjd\@bigfoot.com\">Jacob Rief</A><BR>\n";
    my $now_string = gmtime;
    printf "<H6>Created dynamically: $now_string GMT<BR></H6>\n";
    print "</BODY>\n</HTML>\n";
} else {
    bbjd::init_shoe(6);
    bbjd::calc_distribution;
    table_final;
    table_buy;
    table_double;
    table_split;
}
