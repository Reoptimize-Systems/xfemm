#------------------------------------------------------------------------------#
# Makefile for creating the libraries necessary for mfemm mex files
#------------------------------------------------------------------------------#


WRKDIR = $(CURDIR)
MAKE = make

all: pfemm_pfemm_libs

pfemm_pfemm_libs: 
	$(MAKE) -C pfemm all -f Makefile_pfemm_libs MAKE=$(MAKE)

clean: clean_pfemm_pfemm_libs

clean_pfemm_pfemm_libs: 
	$(MAKE) -C pfemm clean -f Makefile_pfemm_libs MAKE=$(MAKE)

.PHONY: clean_pfemm_pfemm_libs

