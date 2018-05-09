#!/bin/csh -f
echo $0 $1
# support debugging local binary exec. and lib(s). prior to install...
# hopefully the aht setup will "do the right thing", and not polute the
# the env. vars with duplicate entries:
#set setup = '../../../gen/aht/etc/setup.csh'
set setup = './heasetup.csh'
if ( -e $setup ) source $setup
#
# again, avoid duplicates by first checking the current env. setting:
echo $LD_LIBRARY_PATH | grep 'lib/ahvbslice/src'
if ( $? ) setenv LD_LIBRARY_PATH ./lib/ahvbslice/src':'$LD_LIBRARY_PATH
#
set ftool = ./tasks/hxisgdsff/hxisgdsff
set fpar = $ftool:t'.par'
set nnfpar = $ftool:t'_nn.par'
set yyfpar = $ftool:t'_yy.par'
alias aldd "if ( -e $ftool ) ldd $ftool"
# althogh hmake clean works (too?) well by visiting all subdirs,
# currently the hmake dependency fails to pre-compile the lib,
# before the compile & link of the main -- so use alias(s):
alias abackup 'set bnow = ./backup/`date "+%Y/%j/%H/%M"` ; if ( ! -e ${bnow} ) mkdir -p ${bnow} ; mv ./{cache,input,output,expected_output} ${bnow} >& /dev/null'
alias amake 'pushd lib/ahtlm ; hmake ; cd ../ahhxi; hmake ; cd ../ahsgd; hmake ; popd ; hmake'
alias bmake 'hmake clean ; amake ; rm *.o *~ ; abackup ; aldd'
alias cmake 'hmake clean ; bmake'
alias nnpar 'if ( -e $fpar ) unlink $fpar ; \ln -s $nnfpar $fpar ; \ls -l *.par'
alias yypar 'if ( -e ./ahvbslicer.par ) unlink $fpar ; \ln -s $yypar $fpar ; \ls -l *.par'
#
# also make sure input and output sub-directories exist ?
# to all testing/debuging here:
#mkdir ./input ; pushd ./input ; ln -s ../lib/fits_templates/ahhxisgd_fits_template_4col.tpl >& /dev/null; popd
#mkdir ./output >& /dev/null
# check that all lib deps are resolved:
#

