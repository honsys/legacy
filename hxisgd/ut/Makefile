HD_COMPONENT_NAME		= astroh

HD_COMPONENT_VERS		= 

HD_SUBDIRS			=

HD_TEST_SUBDIRS			= hxisgdsff

include ${HD_STD_MAKEFILE}

run-test:
	@for dir in ${HD_TEST_SUBDIRS}; do \
	  ${HD_MAKE} subdir HD_SUBDIR=$$dir HD_TARGET=$@; \
	  status=$$?; if test $$status -ne 0; then exit $$status; fi; \
	done
