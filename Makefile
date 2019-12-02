###############################################################################
# Makefile to build Binaries for the ASPLOS'20 Artifact Evaluation
#
# Paper: Mitosis - Mitosis: Transparently Self-Replicating Page-Tables 
#                  for Large-Memory Machines
# Authors: Reto Achermann, Jayneel Gandhi, Timothy Roscoe, 
#          Abhishek Bhattacharjee, and Ashish Panwar
###############################################################################




all: bin/page-table-dump.ko bin/page-table-dump bin/page-table-dump-periodic



###############################################################################
# Linux Kernel Module
###############################################################################

install: bin/page-table-dump.ko
	insmod bin/page-table-dump.ko

uninstall: 
	rmmod bin/page-table-dump.ko

update: bin/page-table-dump.ko
	rmmod bin/page-table-dump.ko
	insmod bin/page-table-dump.ko

bin/page-table-dump.ko: kernel-module/page-table-dump.c bin Makefile
	(cd kernel-module && $(MAKE) page-table-dump.ko)
	cp kernel-module/page-table-dump.ko bin/page-table-dump.ko



###############################################################################
# Page-Table Dump Tools
###############################################################################

bin/page-table-dump: page-table-dump/dodump.c
	gcc -static -O3 $^ -o $@ -I./include -lnuma

bin/page-table-dump-periodic: page-table-dump/dodump-periodic.c
	gcc -static -O3 $^ -o $@ -I./include -lnuma



###############################################################################
# Common Rules
###############################################################################

bin:
	mkdir -p bin

clean:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD)/kernel-module clean
	rm -rf bin

