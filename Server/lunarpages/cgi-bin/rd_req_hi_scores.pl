#!/usr/bin/perl -w

#############################################
## RD REQUEST HI SCORES 
#############################################

require "common.pl";
require "rd_hi_score_query.pl";

# Sanitise params and connect to DB
my_connect();

ret_hiscores();

