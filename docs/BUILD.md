# Build Instructions

These games can be built for both John Winan's Z80_Retro board (with the VDP Daughter Board) and the Nabu.

In the case of the Nabu, it will produce binaries for both CP/M and Native Nabu (000001.nabu).

## Notes on cross compilation

Cross compilation works because we have two different libraries for the target systems.  They each support the same APIs as each other so any code that uses any of the APIs in one library will work when targeted against the other.  The difference between the two libraries is that in some cases, one target does not carry support for features in the other.  In these cases, the functions are still present but return sensible defaults that do not break the games.  The biggest example of this is Sound output on the Z80_Retro.  There is no audio hardware on the Z80_Retro.

## Prerequsites

1. Z88DK (latest) with SDCC
    * You must have the Z88DK located in the same folder you cloned this project into.  **OR** You must override the `Z88DIR` variable in a `Make.local` file saved here.
    * For detailed instructions on how to install Z88DK - see: https://github.com/z88dk/z88dk/wiki/installation
2. NABU-LIB
    * You must clone the NABU LIB Project from github (https://github.com/DJSures/NABU-LIB.git) into a folder next to where you cloned this project and z88dk.  The location of this can be overriden in: `build/nabu/Make.rules` by editing the value of `TARGET_LIB_DIR`.  For simplicity, it's recommended that you do **NOT** do this and just locate your repos in your dev environment sensibly.
3. z80retro-lib
    * You must clone the z80retro-lib project from github (https://github.com/linuxplaygrouond/z80retro-lib.git) into a folder next to where you cloned this project.  The location of this can be overren in: `build/retro/Make.rules` by editing the value of `TARGET_LIB_DIR`.  For For simplicity, it's recommended that you do **NOT** do this and just locate your repos in your dev environment sensibly.

## Example project layout

```text
development $ tree -L 1
.
├── NABU-LIB
├── nabu-games
├── z80retro-lib
└── z88dk
```

As you can see in the above directory listing, the NABU-LIB, z80retro-lib and z88dk folders are all in the same folder as this nabu-games project.

## Building

The make files are designed to be run on Linux.  They are tested to run on Ubuntu and Windows Subsystem For Linux (WSL2) and on a Raspberry Pi.

Execute the `make` command from the root of this project or from the build directory.  As you descend into the build directory, executions of Make will produce only the items from where you are and below.

For example, to build everything:

```shell
cd nabu-games
make world
```

This will delete all the binary objects, disk images and other artifacts from within the build directory structure and compile everything from scratch.

All the binary outputs will be in the relevant `build/` subfolders.

### Nabu Build Resources
For you Nabuers. the varous .nabu and .COM files along with a c.dsk for use with the nabu.ca Internet Adapter will be written out to the `build/nabu` folders.  There is a detailed description here: [NABU.md](./NABU.md)

### Z80-Retro! Build Resources
You will find a `drive.img` which you can write on to your SD Card in `build/retro/drive.img`.  See: [RETRO.md](./RETRO.md) for details.

At this point you can go ahead and use the makefile rules in `build/retro` to burn to your SD Drive.  Just make sure you have the appropriate overrides configured in `Make.local`.  For more inforamtion checkout John Winan's YouTube video: [https://www.youtube.com/watch?v=H4XypSs2wRA](https://www.youtube.com/watch?v=H4XypSs2wRA)  Again - for more detailed instructions of the Makefile in `build/retro` see: [RETRO.md](./RETRO.md)

## Z80-Retro - Download the Pre-Compiled Binaries

If you don't want to combile the sources but would rather just download the 
pre-compiled binaries, you can do the following steps:

1. Make sure you `Make.local` has the correct overrides for `SD_HOSTNAME` and 
    `SD_DEV`
2. cd into the `bulid/retro` directory.
3. `make g.img`
4. `make g_burn`

This will download the latest binaries published on the nabu-games github releases
extract them, make a g.img disk image and burn it into SLOT 7 of your SD Card.

Of course this all will only work on your retro if you are running the version
of the ROM and Filesystem that supports multiple dives.  Currently the `dev` branch
of https://github.com/Z80-Retro/2063-Z80-cpm
