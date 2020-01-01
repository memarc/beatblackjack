// Copyright 1999, Jacob Rief
// $Id: navigate.js,v 1.1.1.1 1999/12/12 23:12:31 jacob Exp $

var but_introduction_up = new Image();
but_introduction_up.src = "but-introduction-ns.gif";
var but_introduction_dn = new Image();
but_introduction_dn.src = "but-introduction-sel.gif";
var but_theory_up = new Image();
but_theory_up.src = "but-theory-ns.gif";
var but_theory_dn = new Image();
but_theory_dn.src = "but-theory-sel.gif";
var but_table_up = new Image();
but_table_up.src = "but-tables-ns.gif";
var but_table_dn = new Image();
but_table_dn.src = "but-tables-sel.gif";
var but_counting_up = new Image();
but_counting_up.src = "but-counting-ns.gif";
var but_counting_dn = new Image();
but_counting_dn.src = "but-counting-sel.gif";
var but_download_up = new Image();
but_download_up.src = "but-download-ns.gif";
var but_download_dn = new Image();
but_download_dn.src = "but-download-sel.gif";

function over_introduction()
{
  if (document.images) {
    document.images[0].src = but_introduction_dn.src; 
  }
}

function out_introduction()
{
  if (document.images) {
    document.images[0].src = but_introduction_up.src; 
  }
}

function over_theory()
{
  if (document.images) {
    document.images[1].src = but_theory_dn.src; 
  }
}

function out_theory()
{
  if (document.images) {
    document.images[1].src = but_theory_up.src; 
  }
}

function over_tables()
{
  if (document.images) {
    document.images[2].src = but_table_dn.src; 
  }
}

function out_tables()
{
  if (document.images) {
    document.images[2].src = but_table_up.src; 
  }
}

function over_counting()
{
  if (document.images) {
    document.images[3].src = but_counting_dn.src; 
  }
}

function out_counting()
{
  if (document.images) {
    document.images[3].src = but_counting_up.src; 
  }
}

function over_download()
{
  if (document.images) {
    document.images[4].src = but_download_dn.src; 
  }
}

function out_download()
{
  if (document.images) {
    document.images[4].src = but_download_up.src; 
  }
}

