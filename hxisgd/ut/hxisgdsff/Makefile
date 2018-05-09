HD_COMPONENT_NAME		= astroh

HD_COMPONENT_VERS		= 

HD_SUBDIRS			=

# Do not use HD_TEST_SUBDIRS because that causes the build to try to
# recurse into directories that do not contain Makefiles.
AH_TEST_SUBDIRS			= ut01hxi ut01sgd ut02hxi ut02sgd ut03hxi ut03sgd

include ${HD_STD_MAKEFILE}

run-test:
	@for dir in ${AH_TEST_SUBDIRS}; do \
	  ${HD_MAKE} run-aht HD_SUBDIR=$$dir; \
	  status=$$?; if test $$status -ne 0; then exit $$status; fi; \
	done

run-aht:
	cd ${HD_SUBDIR}; ${AHT} -t
