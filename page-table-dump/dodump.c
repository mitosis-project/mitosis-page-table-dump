/* 
    Copyright (C) 2018-2019 VMware, Inc.
    SPDX-License-Identifier: GPL-2.0

    Linux kernel module to dump process page-tables.
    The kernel-module is free software; you can redistribute it and/or
    modify it under the terms of the GNU General Public
    License as published by the Free Software Foundation; version 2.

    The kernel-module  is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    General Public License for more details.
    You should find a copy of v2 of the GNU General Public License somewhere
    on your Linux system; if not, write to the Free Software Foundation,
    Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA 
*/

#include <stdio.h>
#include <fcntl.h>      /* open */ 
#include <unistd.h>     /* exit */
#include <sys/ioctl.h>  /* ioctl */
#include <stdlib.h>
#include <limits.h>
#include <numa.h>

#include <page-table-dump.h>


#define BUF_SIZE_BITS 24
#define PARAM(ptr, sz) ((unsigned long) sz << 48 | (unsigned long)ptr)
#define PARAM_GET_PTR(p) (void *)(p & 0xffffffffffff)
#define PARAM_GET_BITS(p) (p >> 48)

#define PTABLE_BASE_MASK(x) ((x) & 0xfffffffff000UL)


int main(int argc, char *argv[])
{
    if (argc != 2) {
        printf("Usage: dodump <pid>\n");
        return -1;
    }
    
    long pid = strtol(argv[1], NULL, 10);

    if (pid == 0) {
        pid = getpid();
    }

    printf("Dumping page tables for process %ld\n", pid);

    printf("Opening '/proc/ptdump'\n");
    int f = open("/proc/ptdump", 0);
    if (f < 0) {
        printf ("Can't open device file: %s\n", "/proc/ptdump");
        return -1;
    }

    struct ptdump *result = calloc(1, sizeof(*result));

    printf("do ioctl '/proc/ptdump'\n");
    long c = 0;
    struct ptranges ranges = PTRANGES_INIT;
    ranges.processid = pid;
    
    c = ioctl(f, PTDUMP_IOCTL_MKCMD(PTDUMP_IOCTL_CMD_RANGES, 0, 256), 
                 PTDUMP_IOCTL_MKARGBUF(&ranges, 0));
    if (c == 0) {
        printf("<ptranges>\n"
            "<l4>0x%012lx..0x%012lx</l4>\n"
            "<table>0x%012lx..0x%012lx</table>\n"
            "<data>0x%012lx..0x%012lx</data>\n"
            "<code>0x%012lx..0x%012lx</code>\n"
            "<kernel>0x%012lx..0x%012lx</kernel>\n"
            "</ptranges>\n",
            ranges.pml4_base, ranges.pml4_limit, 
            ranges.pt_base, ranges.pt_limit, 
            ranges.data_base, ranges.data_limit,
            ranges.code_base, ranges.code_limit,
            ranges.kernel_base, ranges.kernel_limit);
    }


    result->processid = pid; 
    c = ioctl(f, PTDUMP_IOCTL_MKCMD(PTDUMP_IOCTL_CMD_DUMP, 0, 256), 
                 PTDUMP_IOCTL_MKARGBUF(result, 0));
    if (c == 0) {

        FILE *opt_file_out = stdout;

        printf("Obtained %lu tables\n", result->num_tables);
        fprintf(opt_file_out, "<ptables>");
        for (unsigned long i = 0; i < result->num_tables; i++) {
            fprintf(opt_file_out, "%09lx ", PTABLE_BASE_MASK(result->table[i].base) >> 12);
        }
        fprintf(opt_file_out, "</ptables>\n");

        fprintf(opt_file_out, "<codeframes4k>");
        for (unsigned long i = 0; i < result->num_tables; i++) {
            if (PTDUMP_TABLE_EXLEVEL(result->table[i].base) != 1) {
                continue;
            }
            for (int j = 0; j < 512; j++) {
                if ((result->table[i].entries[j] & 0x1)) {
                    if (!(result->table[i].entries[j] & (0x1UL << 63))) {
                        fprintf(opt_file_out, "%09lx ", PTABLE_BASE_MASK(result->table[i].entries[j]) >> 12);
                    }
                }
            }
        }
        fprintf(opt_file_out, "</codeframes4k>\n");

        fprintf(opt_file_out, "<dataframes4k>");
        for (unsigned long i = 0; i < result->num_tables; i++) {
            if (PTDUMP_TABLE_EXLEVEL(result->table[i].base) != 1) {
                continue;
            }
            for (int j = 0; j < 512; j++) {
                if ((result->table[i].entries[j] & 0x1)) {
                    if ((result->table[i].entries[j] & (0x1UL << 63))) {
                        fprintf(opt_file_out, "%09lx ", PTABLE_BASE_MASK(result->table[i].entries[j]) >> 12);
                    }
                }
            }            
        }
        fprintf(opt_file_out, "</dataframes4k>\n");

        fprintf(opt_file_out, "<codeframes2M>");
        for (unsigned long i = 0; i < result->num_tables; i++) {
            if (PTDUMP_TABLE_EXLEVEL(result->table[i].base) != 2) {
                continue;
            }
            for (int j = 0; j < 512; j++) {
                if ((result->table[i].entries[j] & 0x1) && (result->table[i].entries[j] & (0x1 << 7))) {
                    if (!(result->table[i].entries[j] & (0x1UL << 63))) {
                        fprintf(opt_file_out, "%07lx ", PTABLE_BASE_MASK(result->table[i].entries[j]) >> 21);
                    }
                }
            }
        }
        fprintf(opt_file_out, "</codeframes2M>\n");

        fprintf(opt_file_out, "<dataframes2M>");
        for (unsigned long i = 0; i < result->num_tables; i++) {
            if (PTDUMP_TABLE_EXLEVEL(result->table[i].base) != 2) {
                continue;
            }
            for (int j = 0; j < 512; j++) {
                if ((result->table[i].entries[j] & 0x1) && (result->table[i].entries[j] & (0x1 << 7))) {
                    if ((result->table[i].entries[j] & (0x1UL << 63))) {
                        fprintf(opt_file_out, "%07lx ", PTABLE_BASE_MASK(result->table[i].entries[j]) >> 21);
                    }
                }
            }            
        }
        fprintf(opt_file_out, "</dataframes2M>\n");
    }
    free(result);
    close(f); 

    return 0;
}
