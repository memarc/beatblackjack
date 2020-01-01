// Copyright 1999, Jacob Rief
// $Id: shoe.js,v 1.2 2000/01/10 23:11:49 jacob Exp $

function normalize()
{
  var npks4 = window.document.Shoe.Decks.value*4;
  if (window.document.Shoe.C1.value<0) window.document.Shoe.C1.value = 0;
  if (window.document.Shoe.C1.value>npks4) window.document.Shoe.C1.value = npks4;
  if (window.document.Shoe.C2.value<0) window.document.Shoe.C2.value = 0;
  if (window.document.Shoe.C2.value>npks4) window.document.Shoe.C2.value = npks4;
  if (window.document.Shoe.C3.value<0) window.document.Shoe.C3.value = 0;
  if (window.document.Shoe.C3.value>npks4) window.document.Shoe.C3.value = npks4;
  if (window.document.Shoe.C4.value<0) window.document.Shoe.C4.value = 0;
  if (window.document.Shoe.C4.value>npks4) window.document.Shoe.C4.value = npks4;
  if (window.document.Shoe.C5.value<0) window.document.Shoe.C5.value = 0;
  if (window.document.Shoe.C5.value>npks4) window.document.Shoe.C5.value = npks4;
  if (window.document.Shoe.C6.value<0) window.document.Shoe.C6.value = 0;
  if (window.document.Shoe.C6.value>npks4) window.document.Shoe.C6.value = npks4;
  if (window.document.Shoe.C7.value<0) window.document.Shoe.C7.value = 0;
  if (window.document.Shoe.C7.value>npks4) window.document.Shoe.C7.value = npks4;
  if (window.document.Shoe.C8.value<0) window.document.Shoe.C8.value = 0;
  if (window.document.Shoe.C8.value>npks4) window.document.Shoe.C8.value = npks4;
  if (window.document.Shoe.C9.value<0) window.document.Shoe.C9.value = 0;
  if (window.document.Shoe.C9.value>npks4) window.document.Shoe.C9.value = npks4;
  if (window.document.Shoe.C10.value<0) window.document.Shoe.C10.value = 0;
  if (window.document.Shoe.C10.value>npks4*4) window.document.Shoe.C10.value = npks4*4;
  window.document.Shoe.C0.value = eval(window.document.Shoe.C1.value)+eval(window.document.Shoe.C2.value)+eval(window.document.Shoe.C3.value)+eval(window.document.Shoe.C4.value)+eval(window.document.Shoe.C5.value)+eval(window.document.Shoe.C6.value)+eval(window.document.Shoe.C7.value)+eval(window.document.Shoe.C8.value)+eval(window.document.Shoe.C9.value)+eval(window.document.Shoe.C10.value);
  var nc = eval(window.document.Shoe.C0.value/100);
  window.document.Shoe.P1.value = window.document.Shoe.C1.value/nc;
  window.document.Shoe.P2.value = window.document.Shoe.C2.value/nc;
  window.document.Shoe.P3.value = window.document.Shoe.C3.value/nc;
  window.document.Shoe.P4.value = window.document.Shoe.C4.value/nc;
  window.document.Shoe.P5.value = window.document.Shoe.C5.value/nc;
  window.document.Shoe.P6.value = window.document.Shoe.C6.value/nc;
  window.document.Shoe.P7.value = window.document.Shoe.C7.value/nc;
  window.document.Shoe.P8.value = window.document.Shoe.C8.value/nc;
  window.document.Shoe.P9.value = window.document.Shoe.C9.value/nc;
  window.document.Shoe.P10.value = window.document.Shoe.C10.value/nc;
}

function init_shoe()
{
  if (window.document.Shoe.Decks.value<1)
    window.document.Shoe.Decks.value = 1;
  if (window.document.Shoe.Decks.value>10)
    window.document.Shoe.Decks.value = 10;
  var npks4 = window.document.Shoe.Decks.value*4;
  window.document.Shoe.C1.value = npks4;
  window.document.Shoe.C2.value = npks4;
  window.document.Shoe.C3.value = npks4;
  window.document.Shoe.C4.value = npks4;
  window.document.Shoe.C5.value = npks4;
  window.document.Shoe.C6.value = npks4;
  window.document.Shoe.C7.value = npks4;
  window.document.Shoe.C8.value = npks4;
  window.document.Shoe.C9.value = npks4;
  window.document.Shoe.C10.value = npks4*4;
  normalize();
}

function dec_A_in_shoe() { window.document.Shoe.C1.value--; normalize(); }
function inc_A_in_shoe() { window.document.Shoe.C1.value++; normalize(); }
function dec_2_in_shoe() { window.document.Shoe.C2.value--; normalize(); }
function inc_2_in_shoe() { window.document.Shoe.C2.value++; normalize(); }
function dec_3_in_shoe() { window.document.Shoe.C3.value--; normalize(); }
function inc_3_in_shoe() { window.document.Shoe.C3.value++; normalize(); }
function dec_4_in_shoe() { window.document.Shoe.C4.value--; normalize(); }
function inc_4_in_shoe() { window.document.Shoe.C4.value++; normalize(); }
function dec_5_in_shoe() { window.document.Shoe.C5.value--; normalize(); }
function inc_5_in_shoe() { window.document.Shoe.C5.value++; normalize(); }
function dec_6_in_shoe() { window.document.Shoe.C6.value--; normalize(); }
function inc_6_in_shoe() { window.document.Shoe.C6.value++; normalize(); }
function dec_7_in_shoe() { window.document.Shoe.C7.value--; normalize(); }
function inc_7_in_shoe() { window.document.Shoe.C7.value++; normalize(); }
function dec_8_in_shoe() { window.document.Shoe.C8.value--; normalize(); }
function inc_8_in_shoe() { window.document.Shoe.C8.value++; normalize(); }
function dec_9_in_shoe() { window.document.Shoe.C9.value--; normalize(); }
function inc_9_in_shoe() { window.document.Shoe.C9.value++; normalize(); }
function dec_10_in_shoe() { window.document.Shoe.C10.value--; normalize(); }
function inc_10_in_shoe() { window.document.Shoe.C10.value++; normalize(); }
