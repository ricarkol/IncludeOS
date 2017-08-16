#!/bin/bash

### FAT32 TEST DISK ###

DISK=my.disk
MOUNTDIR=tmpdisk

# If no arg supplied, setup disk
if [ $# -eq 0 ]
then

  # Remove disk if exists
  rm -f $DISK
  # Preallocate space to a 4GB file
  truncate -s 256M $DISK
  # Create FAT32 filesystem on "my.disk"
  mkfs.fat $DISK

  # Create mountdir and mount
  mkdir -p $MOUNTDIR
  sudo mount -o rw $DISK $MOUNTDIR/
  sudo mkdir -p $MOUNTDIR/db/company

  rm -f blob.db

  cp TPC-H.db $MOUNTDIR/db/company/my.db
  #cp TPC-H-small.db $MOUNTDIR/db/company/my.db

  sync # Mui Importante
  sudo umount $MOUNTDIR/
  rmdir $MOUNTDIR

# If "clean" is supplied, clean up
elif [ $1 = "clean" ]
then
  echo "> Cleaning up TEST DISK: $DISK"
  rm -f $DISK
fi
