#!/bin/csh -f
rm -rf doxydoc/*
doxygen doxyfile.vcslicer
if ( $? != 0 ) then
  echo doxygen exit stat failure ...
  exit
endif
if( ! -e doxydoc/html ) then
  echo no doxygen html
  exit
endif
# 
rm -rf ~/public_html/ahvcslicer/*
rm -rf ~/public_html/ahvcslicer-viz/*
cp -a doxydoc/html/* ~/public_html/ahvcslicer
pushd ~/public_html/ahvcslicer-viz
foreach i ( ../ahvcslicer/*.png )
  \ln -s $i
end
popd

