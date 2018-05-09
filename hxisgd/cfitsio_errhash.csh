#!/bin/csh -f
set file = /tmp/${USER}_cfitsio_errcodes.html
if ( ! -e $file ) then 
  wget http://heasarc.gsfc.nasa.gov/fitsio/c/c_user/node124.html -O $file
endif
set start = `cat -n $file | grep -i '<pre>' | awk '{print $1}'`
#echo $start
set stop = `cat -n $file | grep -i '</pre>' | awk '{print $1}'`
#echo $stop
set hcnt = $stop
@ hcnt = $hcnt - 1
set tcnt = $stop
@ tcnt = $tcnt - $start
@ tcnt = $tcnt - 3
echo $hcnt $tcnt
#echo "head -225 $file | tail  -157 | awk '{print $2, $1, $3}'"
#if ( -r cfitsio_errhash.cc ) rm cfitsio_errhash.cc
head -$hcnt $file | tail -$tcnt |\
awk '{printf "%s%s%s%s ", "errhash[", $2, "] = \"", $1; for (i=3; i<NF; i++) {printf " %s", $i;}; printf "%s\n", "\";"}' 


