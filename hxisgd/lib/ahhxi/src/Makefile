CVSRCS_ID           := "$$Name:  $$ $$Id: Makefile,v 1.11 2012/08/17 21:08:14 dhon Exp $$"

#$(info $$Name:  $$ $$Id: Makefile,v 1.11 2012/08/17 21:08:14 dhon Exp $$)

HD_COMPONENT_NAME	= astroh

HD_COMPONENT_VERS	= 

HD_LIBRARY_ROOT		= ahhxi

HD_LIBRARY_SRC_cxx	= doHXI.cxx doHXIsim.cxx doHXItest.cxx HXItemplate.cxx ioHXI.cxx lutHXI.cxx

HD_CXXFLAGS		=  -g -O00 -I./ -I../ -I../../ahtlm -I../../ahsgd -I../../../tasks/hxisgdsff ${HD_STD_CXXFLAGS} \
                 -I../../../mission/lib/ahmapp -I../../../mission/lib/common \
                 -I../../../gen/lib/ahapp -I../../../gen/lib/ahfits -I../../../gen/lib/ahmath \
                 -I../../../gen/lib/ahgen -I../../../gen/lib/ahlog -I../../../gen/lib/st_stream

HD_INSTALL_LIBRARIES	= ${HD_LIBRARY_ROOT}

include ${HD_STD_MAKEFILE}
