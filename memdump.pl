#!/usr/bin/perl


#cherche le pid de dummy
#cherche ses addresses dans /proc/<pid>/maps
#appel memdump avec les couples debut d'adresses-end addresses
#affiche le resultat

#cherche une string pour l'instant

my $numArgs = $#ARGV + 1;
my $programme = $ARGV[0];

#print "$programme \n";

my $psdummy=`ps -fu nap | grep $programme | grep -v grep |grep -v memdump| cut -d' ' -f8`;
chomp($pid=$psdummy);


#print "Pid: $pid \n";

my $cmd="cat /proc/".$pid."/maps";
#print "Cmd: $cmd \n";
my @table_mem=`$cmd`;

#print @table_mem;

$couples = "";

foreach (@table_mem){
    $line = $_;
    if($line =~ /lib/){
	#nothing
    }else{
	#pas de lib donc on garde
	@part = split(/-/,$line);
	$part1 = $part[0];
	#print "Part1 = $part1 \n";

	@minipart = split(/ /,$part[1]);
	$part2 = $minipart[0];
	#print "Part2 = $part2 \n";
	
	$couples .= " 0x$part1 0x$part2";
    }
    
} 

#print "$pid$couples";

@toOptimise = split(/ /,$couples);

#print "Debut Optimise \n";
#print @toOptimise;
$len = @toOptimise - 1;
#print "Taille = $len \n";

$newcouple = "";

$i = 0;
for($i=1;$i<=$len;$i++){
    $part = @toOptimise[$i];
    #print "$i: $part \n";
    
    #on passe le 1 et le dernier
    if($i !=1 && $i !=$len){
	if($part eq @toOptimise[$i+1] || $part eq @toOptimise[$i -1]){
	    print "";
	}else{
	    $newcouple .= " $part";
	}
    }else{#on les rajoute de toute maniere le 1 et len
	      $newcouple .= " $part ";
	  }
}

print "$pid $newcouple";
