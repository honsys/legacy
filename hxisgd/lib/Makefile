CVSRCS_ID           := "$$Name:  $$ $$Id: Makefile,v 1.4 2012/07/27 20:43:09 peachey Exp $$"

#$(info $$Name:  $$ $$Id: Makefile,v 1.4 2012/07/27 20:43:09 peachey Exp $$)

HD_COMPONENT_NAME		= astroh

HD_COMPONENT_VERS		= 

HD_SUBDIRS			    = ahtlm ahhxi ahsgd

HD_TEST_SUBDIRS			= ${HD_SUBDIRS}

include ${HD_STD_MAKEFILE}

# None of these libraries have unit tests, so leave run-test target blank for now.
run-test:

real-run-test:
	@for dir in ${HD_TEST_SUBDIRS}; do \
	  ${HD_MAKE} subdir HD_SUBDIR=$$dir HD_TARGET=$@; \
	  status=$$?; if test $$status -ne 0; then exit $$status; fi; \
	done
