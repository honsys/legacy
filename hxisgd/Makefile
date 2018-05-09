CVSRCS_ID           := "$$Name:  $$ $$Id: Makefile,v 1.6 2012/07/28 02:20:17 peachey Exp $$"

#$(info $$Name:  $$ $$Id: Makefile,v 1.6 2012/07/28 02:20:17 peachey Exp $$)

HD_COMPONENT_NAME   = astroh

HD_COMPONENT_VERS   = 

HD_SUBDIRS          = lib tasks 

HD_TEST_SUBDIRS     = lib ut

include ${HD_STD_MAKEFILE}

run-test:
	@for dir in ${HD_TEST_SUBDIRS}; do \
	  ${HD_MAKE} subdir HD_SUBDIR=$$dir HD_TARGET=$@; \
	  status=$$?; if test $$status -ne 0; then exit $$status; fi; \
	done
