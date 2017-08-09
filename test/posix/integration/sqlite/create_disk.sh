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

  echo "PRAGMA journal_mode=OFF;" | sqlite3 -vfs unix-none blob.db
  echo "create table large (a);" | sqlite3 -vfs unix-none blob.db
  for i in `seq 1 64`
  do
    echo "insert into large values (zeroblob(1024*1024));" | sqlite3 -vfs unix-none blob.db
  done
  echo "drop table large;" | sqlite3 -vfs unix-none blob.db
  #echo "CREATE TABLE company(id INT PRIMARY KEY NOT NULL, name TEXT NOT NULL, age INT NOT NULL, address CHAR(50), salary REAL);" | sqlite3 -vfs unix-none blob.db
  echo ".q" | sqlite3 -vfs unix-none blob.db

  cp blob.db $MOUNTDIR/db/company/my.db

  sync # Mui Importante
  sudo umount $MOUNTDIR/
  rmdir $MOUNTDIR

# If "clean" is supplied, clean up
elif [ $1 = "clean" ]
then
  echo "> Cleaning up TEST DISK: $DISK"
  rm -f $DISK
fi
