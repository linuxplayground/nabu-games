# How to load and play on the Z80-Retro SBC

A fork of the NABU-LIB library that's been altered to support the Z80-Retro is required to compile these games.  See the BUILD.md for details on how to configure your build environment.

When you run `make world`, binaries for both the Nabu and the Z80-Retro will be made.  You will find them in the `build/retro` directory.

## Loading the games onto the Z80 Retro

There are actually a couple of different ways to do this.  

### Burn to SD Card

The recommended way is to copy the `drive.img` file into one of the "slots" on your SD card.  For details on how this works, see the documentation here: [https://github.com/Z80-Retro/example-filesystem/tree/main](https://github.com/Z80-Retro/example-filesystem/tree/main)

Something like this will put the drive image on to slot 1 of the SD Card.  This will appear inside CP/M as drive B:

This assumes you have compiled the Retro CP/M project to support multiple drives.  At the time of writing this document, this support is only available in one of the development branches.  JW has made a video on how this works.

```
cd build/retro
sudo dd if=drive.img of=<DEVICE NAME OF YOUR SD CARD PARTITION> bs=512 seek=01x16384 conv=fsync
```

### Using XMODEM

If you have the XMODEM utility installed, you can use it to transfer the files directly.

```
a> xr snake.com
```
Then use Minicom or Terraterm or whatever you would use to send a file using Xmodem.

You can find XMODEM here: [https://github.com/Z80-Retro/xmodem80](https://github.com/Z80-Retro/xmodem80) and it's also linked to on the home page of the Z80-Retro org in GitHub [https://github.com/Z80-Retro](https://github.com/Z80-Retro)
