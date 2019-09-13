#!/bin/bash

USBDIR=/media/pi/USBPEN/visutest
LOCALDIR=/home/pi/visutest
BADCONFIG=$USBDIR/config.ini
LOGFILE=$USBDIR/update.log

echo "---" >> $LOGFILE
echo "Updating VisuTest on `date`" >> $LOGFILE

if [ -d $LOCALDIR ]; then
    echo "Destination folder found!" >> $LOGFILE
else
    echo "Destination folder not found, trying to create it..." >> $LOGFILE
    if mkdir $LOCALDIR; then
	echo "Destination folder created succesfully!" >> $LOGFILE
	echo "VERSION=0.0.0" >> $LOCALDIR/VERSION
    else
	echo "Couldn't create destination folder, quitting..." >> $LOGFILE
	exit 1
    fi
fi

if [ ! -f $LOCALDIR/VERSION ]; then
    echo "No VERSION file found in LOCALDIR, quitting..." >> $LOGFILE
    exit 1
fi
source $LOCALDIR/VERSION
echo "Old version:" $VERSION >> $LOGFILE

if [ ! -f $USBDIR/VERSION ]; then
    echo "No VERSION file found in USBDIR, quitting..." >> $LOGFILE
    exit 1
fi
source $USBDIR/VERSION
echo "New version:" $VERSION >> $LOGFILE

if [ -f $BADCONFIG ]; then
    echo "Removing bad config from update" >> $LOGFILE
    if rm $BADCONFIG; then
	echo "Config removed successfully!" >> $LOGFILE
    else
	echo "Config couldn't be removed, quitting..." >> $LOGFILE
	exit 1
    fi
fi

echo "Syncing update files..." >> $LOGFILE
if rsync -va $USBDIR/ $LOCALDIR/ >> $LOGFILE; then
    echo "Sync completed successfully!" >> $LOGFILE
else
    echo "Sync failed, quitting..." >> $LOGFILE
    exit 1
fi

echo "Update completed!" >> $LOGFILE
xmessage -center -timeout 5 "VisuTest succesfully updated, please restart!"
