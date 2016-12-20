#!/usr/bin/perl -w

#############################################
## RD LOG HI SCORE
#############################################

# Expects these params:
# device_id: unique ID for a device
# score
# level
# depth
# x
# y
# z
# nick - nickname

require "common.pl";
require "rd_hi_score_query.pl";

sub log_hi();

# Sanitise params and connect to DB
my_connect();
log_hi();
# Return new hi score table
ret_hiscores();


sub log_hi()
{
  my $device_id = param('device_id') or die "Expected device_id\n";
  my $score = param('score') or die "Expected score\n";
  my $level = param('level') or die "Expected level\n";
  my $depth = param('depth') or die "Expected depth\n";
  my $x = param('x') or die "Expected x\n";
  my $y = param('y') or die "Expected y\n";
  my $z = param('z') or die "Expected z\n";
  my $nick = param('nick') or die "Expected nick\n";

  # Decode coded params
  my $dec_device_id = pack('H*', "$device_id");
  my $dec_score = pack('H*', "$score");
  my $dec_level = pack('H*', "$level");
  my $dec_depth = pack('H*', "$depth");
  my $dec_x = pack('H*', "$x");
  my $dec_y = pack('H*', "$y");
  my $dec_z = pack('H*', "$z");
  # Store ENCODED nick in db

  my $sql = <<END;
  INSERT INTO hiscore (
    device_id, 
    hi_score, 
    hi_level, 
    hi_depth, 
    hi_x, 
    hi_y, 
    hi_z,
    hi_nick)
    VALUES (
    '$dec_device_id',
    $dec_score, 
    $dec_level, 
    $dec_depth, 
    $dec_x, 
    $dec_y, 
    $dec_z,
    '$nick'); # NB store ENCODED nick, not decoded. Return ENCODED nick to client.
END

  print "Query: $sql\n";
  insert($sql);
}


