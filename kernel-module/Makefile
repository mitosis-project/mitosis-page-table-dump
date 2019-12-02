###############################################################################
# Makefile to build Binaries for the ASPLOS'20 Artifact Evaluation
#
# Paper: Mitosis - Mitosis: Transparently Self-Replicating Page-Tables 
#                  for Large-Memory Machines
# Authors: Reto Achermann, Jayneel Gandhi, Timothy Roscoe, 
#          Abhishek Bhattacharjee, and Ashish Panwar
###############################################################################

obj-m += page-table-dump.o
ccflags-y=-I$(PWD)/../include

page-table-dump.ko: page-table-dump.c Makefile ../include/page-table-dump.h
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) modules
