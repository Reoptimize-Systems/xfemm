#------------------------------------------------------------------------------#
# Makefile for creating the libraries necessary for mfemm mex files
#------------------------------------------------------------------------------#


WRKDIR = $(shell pwd)
MAKE = make

all: pfemm_pfemm_libs

pfemm_pfemm_libs: 
	$(MAKE) -C pfemm all -f Makefile_pfemm_libs

clean: clean_pfemm_pfemm_libs

clean_pfemm_pfemm_libs: 
	$(MAKE) -C pfemm clean -f Makefile_pfemm_libs

.PHONY: clean_pfemm_pfemm_libs

