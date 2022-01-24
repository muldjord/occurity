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

echo "Syncing files to USBPEN..." >> $LOGFILE
if rsync --files-from=$LOCALDIR/scripts/include.txt -va $LOCALDIR/ $USBDIR/ >> $LOGFILE; then
    echo "Synced main files successfully!" >> $LOGFILE
else
    echo "Sync failed, quitting..." >> $LOGFILE
    exit 1
fi

if rsync -va $LOCALDIR/icons $USBDIR/icons >> $LOGFILE; then
    echo "Synced icons successfully!" >> $LOGFILE
else
    echo "Sync failed, quitting..." >> $LOGFILE
    exit 1
fi

if rsync -va $LOCALDIR/optotypes $USBDIR/optotypes >> $LOGFILE; then
    echo "Synced optotypes files successfully!" >> $LOGFILE
else
    echo "Sync failed, quitting..." >> $LOGFILE
    exit 1
fi

if rsync -va $LOCALDIR/scripts $USBDIR/scripts >> $LOGFILE; then
    echo "Synced scripts files successfully!" >> $LOGFILE
else
    echo "Sync failed, quitting..." >> $LOGFILE
    exit 1
fi

if rsync -va $LOCALDIR/svg $USBDIR/svg >> $LOGFILE; then
    echo "Synced svg files successfully!" >> $LOGFILE
else
    echo "Sync failed, quitting..." >> $LOGFILE
    exit 1
fi

echo "Release created succesfully!" >> $LOGFILE
xmessage -center "Release created succesfully!"
