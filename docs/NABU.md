# How to load and play on the Nabu

These games are built to run on the Nabu Personal Computer.  Generally, the latest releases are hosted on the following internet adapters:

* nabu.ca Internet Adapter
    * as standalone games selectable on the main menu under the Games section.
    * within Cloud CP/M on A drive user area 3.
* nabunetwork.com Internet Adapter
    * Selectable in the drop down menus as standalone games
    * within Ishkur CP/M on A drive user area 3.

## Manually loading the ame into Cloud CP/M

If you decide to compile the gmaes yourself, you can load them into the Internet Adapter by creating a C: disk image and using cpmtools to copy `snake.com` and `tetris.com` to the image.  In fact, there is a Makefile recipe that does this automatically and copies the c.dsk image into the Internet Adapter Store folder.  You will need to configure the path to your Store folder by creating a `Make.local` file in the root of this project.

Here is how mine looks - I use Windows Sub System For Linux for development, and I am running the Windows version of the internet adapter.

```
cat Make.local 
STOREDIR = "/mnt/c/dev/nabu/Nabu Internet Adapter/Store/"
```

```text
cd build/nabu
make clean
make store
...
... compilation output ...
...
rm -f c.dsk
LC_CTYPE=C tr '\0' '\345' < /dev/zero | dd of=c.dsk bs=8M count=1 conv=fsync iflag=fullblock
1+0 records in
1+0 records out
8388608 bytes (8.4 MB, 8.0 MiB) copied, 0.0331076 s, 253 MB/s
cpmcp -f naburn c.dsk */*.com 0:
cpmls -f naburn c.dsk
0:
cpmkey.com
framebuf.com
snake.com
tetris.com
cp c.dsk "/mnt/c/dev/nabu/Nabu Internet Adapter/Store/"
```

In the above listing, you can see how am 8mb disk image is created using `dd` and filled up with `e5` data which is required by CP/M.  It then copies the COM files to it using cpmtools and finally copies the c.dsk image file over to the configured internet adapter Store folder.

Once this step is complete, you should be able to navigate to drive C user area 0 and do a directory listing with `dir`.  To play the games just type `snake` or `tetris`
