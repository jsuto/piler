#!/usr/bin/perl
#

@x = ('A'..'Z','a'..'z',0..9);
$iv = "";

srand;

for($i=0; $i<16; $i++){
   $iv .= @x[rand @x];
}

print $iv;
