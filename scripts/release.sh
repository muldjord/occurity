#!/bin/bash

USBDIR=/media/pi/USBPEN/visutest
LOCALDIR=/home/pi/programming/visutest
LOGFILE=$LOCALDIR/release.log

echo "---" >> $LOGFILE
echo "Creating release on `date`" >> $LOGFILE

if [ -d $LOCALDIR ]; then
    echo "Local folder found!" >> $LOGFILE
else
    echo "Local folder not found, quitting..." >> $LOGFILE
    exit 1
fi

if [ ! -f $LOCALDIR/VERSION ]; then
    echo "No VERSION file found in LOCALDIR, quitting..." >> $LOGFILE
    exit 1
fi
source $LOCALDIR/VERSION
echo "Release version:" $VERSION >> $LOGFILE

echo "Syncing release files to USBPEN..." >> $LOGFILE
if rsync --files-from=$LOCALDIR/scripts/include.txt -va $LOCALDIR/ $USBDIR/ >> $LOGFILE; then
    echo "Sync completed successfully!" >> $LOGFILE
else
    echo "Sync failed, quitting..." >> $LOGFILE
    exit 1
fi

echo "Release created succesfully!" >> $LOGFILE
xmessage -center "Release created succesfully!"
