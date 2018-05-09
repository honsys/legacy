#!/bin/csh -f
set doxy = "doxyfiles.txt"
if( -e $doxy ) mv $doxy .$doxy
touch doxyfiles.txt
find ./tasks -name '*.h' | grep -v '/\.' | sort -u >> $doxy
find ./lib -name '*.h' | grep -v '/\.' | sort -u >> $doxy
find ./tasks -name '*.cxx' | grep -v '/\.' | sort -u >> $doxy
find ./lib -name '*.cxx' | grep -v '/\.' | sort -u >> $doxy
