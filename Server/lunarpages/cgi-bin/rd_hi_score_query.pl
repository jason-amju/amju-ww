#!/usr/bin/perl -w

#############################################
## RD REUSABLE HI SCORE QUERY 
#############################################

sub ret_hiscores()
{
  # NB We return up to 100 scores -- check that parsing this many is OK
  my $sql = "SELECT hi_score, hi_level, hi_depth, hi_x, hi_z, hi_y, hi_nick FROM hiscore ORDER BY hi_score DESC LIMIT 100";

  my $query = $dbh->prepare($sql) or die "Query prepare failed for this query: $sql\n";

  $query->execute or die "Failed to execute query!?!";

  print("<hiscores>");

  while (my ($score, $level, $depth, $x, $y, $z, $nick) = $query->fetchrow_array)
  {
    print("<hi>");
    print("<score>$score</score> <level>$level</level> <depth>$depth</depth> <pos><x>$x</x> <y>$y</y> <z>$z</z></pos> <nick>$nick</nick>");
    print("</hi>");
  }

  print("</hiscores>");
}
1;

