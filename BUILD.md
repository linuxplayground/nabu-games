# Build Instructions

First lets assume that you don't have all the tools in precisely the same place as me on your system.  I am using Windows Subsystem For Linux (WSL2) to build and maintian my code.  The NABU Internet Adapter and MAME environments are on my windows system - OUTSIDE of WSL2.

The easiest thing to do is, make a copy of `Make.default` and name it `Make.local`  Then override anything you want to override so that the variables have values that match your system.

To build Tetris and Snake, you can just run `make` from this directory.  The build will create .COM Files and .NABU files in the `build` directory under a seperate folder for each.  It will also create a CPM Disk image called `c.dsk` and copy it to your `Store` folder ready for use by the NABU Internet Adapter.

If you want to work on just one of the projects, you can CD into that project and run `make one` which will clean your build and disk folders out, compile the code for that project into a CPM COM file, create a c.dsk image, copy the COM file to it and then copy the image over to the `Store` folder ready for the NABU Internet Adapter.
