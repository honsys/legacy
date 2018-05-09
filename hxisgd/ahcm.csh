#!/bin/csh -f
set cwd = `pwd`
echo $cwd
set cwd = $cwd:t
if( $cwd != 'astroh' ) then
  echo this script must run within the astroh top-level src-tree directory
  exit
endif
set newdirs = "hxisgd/tasks/hxisgdsff hxisgd/lib hxisgd/lib/ahtlm hxisgd/lib/ahtlm/ahtlm hxisgd/lib/ahtlm/src hxisgd/lib/ahhxi hxisgd/lib/ahhxi/ahhxi hxisgd/lib/ahhxi/src hxisgd/lib/ahsgd hxisgd/lib/ahsgd/ahsgd hxisgd/lib/ahsgd/src"
#
foreach d ($newdirs)
  mkdir $d; touch $d/{Makefile,README}
  cvs add $d
  cvs commit -m"added new directory $d"
  cvs add $d/[MR]* $d/*.[ch]*
  cvs ci -m"initial checkin" $d/*.[ch]*
end
# for good measure?
#$cvs commit -m"added new directories $newdirs"
#
# and if anything was removed, update with prune
# note that prune removes any empty directory, even
# if that directory has not been removed from the cvs repos.!
cvs up -d -P
exit

