TOP=../..

# Name each subdir to build before making the filesystem
SUBDIRS=\
	tetris \
	snake

.PHONY: drive.img all

# make 'all' be the default build target...
all::

include Make.rules

# ...but don't try to make the drive image until after the subdirs are done!
all:: drive.img



# make sure all subdirs are built before we make the drive image
drive.img:: $(ALL_DIRS)
	rm -f $@
	mkfs.cpm -f $(DISKDEF) $@
	cpmcp -f $(DISKDEF) $@ */*.com 0:


burn: drive.img
	@ if [ `hostname` != "$(SD_HOSTNAME)" -o ! -b "$(SD_DEV)" ]; then\
		echo "\nWARNING: You are either NOT logged into $(SD_HOSTNAME) or there is no $(SD_DEV) mounted!\n"; \
		false; \
	fi
	sudo dd if=$< of=$(SD_DEV) bs=512 seek=$(DISK_SLOT)x16384 conv=fsync


# Erase ALL the CP/M drives on the SD card.  *** VERY DANGEROUS ***
# Write 16*8M of 0xE5 bytes into the CP/M partition on the SD card
blank-all-sd: 
	@ if [ `hostname` != "$(SD_HOSTNAME)" -o ! -b "$(SD_DEV)" ]; then\
		echo "\nWARNING: You are either NOT logged into $(SD_HOSTNAME) or there is no $(SD_DEV) mounted!\n"; \
		false; \
	fi
	LC_CTYPE=C tr '\0' '\345' < /dev/zero | sudo dd of=$(SD_DEV) bs=8M count=16 conv=fsync iflag=fullblock


ls:: drive.img 
	cpmls -f $(DISKDEF) $^


clean::
	rm -f drive.img
