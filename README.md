Page Table Dump Tools for Linux
===============================

Authors
-------
 
 * Reto Achermann
 * Ashish Panwar
 * Jayneel Gandhi


License
-------

See LICENSE file.

Dependencies
------------

```
sudo apt-get install build-essential libnuma-dev git kernel-package
```                  

Building
--------

 * `make` - builds the kernel module and binaries
 * `make clean` - removes all the compiled binaries 
 * `make install` - installs the kernel module
 * `make uninstall` - removes the kernel module
 * `make update` - updates the kernel module (remove & reinstall)


Running
-------

 * `bin/page-table-dump <PID>` - dump the page-tables of a running proccess with pid <PID>
 * `bin/page-table-dump-periodic <PID>` - dumps the page-tables of a running process every 30s.

Publication
-----------

This repository contains the page-table dump tools and scripts for the 
ASPLOS'20 artifact evaluation of the paper **Mitosis - Mitosis: Transparently 
Self-Replicating Page-Tables for Large-Memory Machines** by Reto Achermann, 
Jayneel Gandhi, Timothy Roscoe, Abhishek Bhattacharjee, and Ashish Panwar.
