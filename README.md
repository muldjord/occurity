# Occurity

## What it is
Occurity is a fully-featured patient visual acuity test software designed for use on the RaspBerry Pi hardware platform. It enables you to built an affordable visual acuity test system using a few pieces of hardware and basically any remote control. If you do not have the option to buy the infrared receiver, it can also be used with a keyboard.

## Included tests
- ETDRS, HOTV, numbers, Landolt C and Tumbling E charts
- Astigmatism dots and astigmatism rays charts
- Coincidence, Worth 4 dots and mirrored RED / GREEN charts

## License
Occurity is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.

## What you need

### Hardware
In order to install a system running Occurity you need the following hardware:

**Required**
* A Raspberry Pi (3 or 4, 4 required for attention video support)
* At least a 2 GB SD flash card
* A monitor with at least 300 cd/m² brightness (preferably 350 cd/m²)

**Optional**
* A [Flirc infrared reciever](https://flirc.tv/more/flirc-usb)
  * A remote control that works with flirc. Most do.

#### Monitor

##### Resolution
Occurity displays correctly on any monitor using any resolution provided the following two requirements are met:
* The physical length of the blue ruler must be set correctly in the Occurity [Preferences](docs/PREFERENCES.md) dialog.
* The physical distance from the patients eyes to the monitor must be set correctly in the Occurity [Preferences](docs/PREFERENCES.md) dialog.

To test if the monitor you are using reports its resolution correctly to the Occurity software, please set the physical width of the blue ruler in the [Preferences dialog](docs/PREFERENCES.md). Then temporarily set the patient distance to 400 cm and 600 cm respectively and make sure the width of the optotype symbols match with the table below. If they do your monitor will work correctly with Occurity.
|Patient distance|Chart size|Expected symbol width|
|----------------|----------|---------------------|
|600             |0.1       |87 mm                |
|600             |0.25      |35 mm                |
|400             |0.1       |58 mm                |
|400             |0.25      |23 mm                |

##### Brightness calibration
DISCLAIMER!!! This is only a guideline: Using a lux meter pushed up against the monitor surface, you should have a readout of about 277 lux. Hence the need for a monitor that is capable of a high brightness level.

#### Remote controls

##### Tested working with the following remote controls
* LG AKB75095344
* Samsung AA59-00818A

In theory most remote controls you might have lying around should work with the disclaimer that there are some remotes that use dual-output signals which makes them hard to use with the flirc interface. Only way to know is to just try.

Note! The [Flirc infrared reciever](https://flirc.tv/more/flirc-usb) emulates a keyboard. For the remote to work, you need to install the Flirc software on a supported OS and configure the buttons on your remote to correspond with [these](README.md#keyboard-controls) keyboard keys.

## How to install Occurity
You have two options for installing and running Occurity. Either use the pre-built SDCard images or built one yourself from scratch.

## Option 1: Using a pre-built SDCard image
If you require no customization of the image (you probably don't) this is the easiest way to get up and running. Simply download the SDCard image corresponding to your Raspberry Pi from the [latest release files](https://github.com/muldjord/occurity/releases/latest). Decompress the gzipped image and flash it to an SDCard (`dd` or similar). Insert the SDCard in your Raspberry Pi and you are done! Be sure to check the rest of the documentation on how to use Occurity.

## Option 2: Building a custom Occurity SDCard image
The step-by-step procedure for building an Occurity image that can be flashed to an SDCard for the Raspberry Pi hardware platform is described in detail below. The build is currently based on the `scarthgap` release of the very popular [Yocto embedded platform](https://www.yoctoproject.org).

The only pre-requisite is a recent working Ubuntu installation with at least 150 GB available harddrive space. The final SDCard image will be about 1 GB. You might also be able to make this work on any previous or later Ubuntu LTS release. The main difference will probably be the package pre-requisites. Refer to the [official Yocto documentation](https://docs.yoctoproject.org/brief-yoctoprojectqs/index.html) for further information.

### Package pre-requisites
Install the required packages by running the following two commands:
```
$ sudo apt install gawk wget git diffstat unzip texinfo gcc build-essential chrpath socat cpio python3 python3-pip python3-pexpect xz-utils debianutils iputils-ping python3-git python3-jinja2 python3-subunit zstd liblz4-tool file locales libacl1
$ sudo locale-gen en_US.UTF-8
```

If some or more of these packages do not exist on your system they might be different for your specific version of Ubuntu. Please read the [official Yocto documentation](https://docs.yoctoproject.org/brief-yoctoprojectqs/index.html) if that is the case.

### Setting up Yocto
Run the following commands to clone Yocto and the required layers. The `source` command sets up the environment required for building the image. If you wish to rebuilt the image later you need to re-run this `source` command in order for the `bitbake` related tools to be available and configured:
```
$ cd
$ git clone https://git.yoctoproject.org/poky
$ cd poky/
$ git checkout -t origin/scarthgap -b occurity
$ git clone -b scarthgap https://git.yoctoproject.org/meta-raspberrypi
$ git clone -b scarthgap https://github.com/openembedded/meta-openembedded
$ git clone https://code.qt.io/yocto/meta-qt6
$ git clone https://github.com/muldjord/meta-occurity
$ source oe-init-build-env
$ bitbake-layers add-layer ../meta-raspberrypi
$ bitbake-layers add-layer ../meta-openembedded/meta-oe
$ bitbake-layers add-layer ../meta-openembedded/meta-python
$ bitbake-layers add-layer ../meta-qt6
$ bitbake-layers add-layer ../meta-occurity
```

### Add required Yocto configs
You should now be at the `poky/build` directory. Edit `conf/local.conf` and add the following lines to the very top of the file:
```
LICENSE_FLAGS_ACCEPTED = "synaptics-killswitch commercial"
WKS_FILE = "sdimage-raspberrypi.wks"
IMAGE_FSTYPES += "wic.gz wic.bmap"
DISABLE_OVERSCAN = "1"
MACHINE ?= "raspberrypi4-64"
```
NOTE!!! If you are building for a Raspberry Pi 3 change the MACHINE to `raspberrypi3-64`.

### Build the image
You are now ready to build the image. Depending on your host machine this can take a long time (several hours). Run the following commands to start the build. You should run this command from the `poky/build` directory:
```
$ bitbake core-image-sato
```
Bitbake is Yocto's build system. It pulls in all required source code and compiles and configures everything needed for an embedded Linux system running Occurity on a Raspberry Pi.

#### Troubleshooting
If you get an error when running the `bitbake core-image-sato` command your system might have a strict App-Armor setup that disallows Bitbake to run well in user-space (this has been observed on a clean Ubuntu 24.40 installation). You need to create a special rule for it before it will work. To fix this, you can create the file `/etc/apparmor.d/bitbake` with the following contents:
```
abi <abi/4.0>,
include <tunables/global>
profile bitbake /**/bitbake/bin/bitbake flags=(unconfined) {
        userns,
}
```
Then restart the apparmor service and you should be able to built the image.
```
$ sudo systemctl restart apparmor
```

### Flash the final image to an SDCard
If everything went well you will now have a working Occurity image at `poky/build/tmp/deploy/images/raspberrypi4-64/core-image-sato-raspberrypi4-64.rootfs.wic.gz`. Be aware that this is a symbolic link - the actual file is placed in the same directory but has a timestamp in the filename.

Decompress the gzipped image and flash it to an SDCard using your favorite SDCard flashing tool (the Raspberry Pi Imager works well for this task, just choose `Use custom` image and set it to show `All files` to choose the image). Insert the card into your Raspberry Pi and boot it up. After a little while Occurity will be automatically launched. Be sure to check the rest of the documentation on how to use Occurity.

## Building Occurity on Ubuntu 24.04
NOTE!!! If you've already downloaded or built the Raspberry Pi Yocto image as documented above you do not need to continue with these instructions. The following describes how to compile and run Occurity on Ubuntu for anyone who wishes to do so.

### Software prerequisites
Run the following commands in a terminal on Ubuntu to install the prerequisites needed for Occurity to function correctly.
```
$ sudo apt update
$ sudo apt install build-essential git cmake qt6-base-dev qt6-svg-dev qt6-multimedia-dev qt6-tools-dev
```

### Download and compile
Open a terminal on Ubuntu and run the following commands:
```
$ cd
$ git clone https://github.com/muldjord/occurity.git
$ cd occurity
$ mkdir build
$ cd build
$ cmake ..
$ make
$ make install
```

### Running Occurity
You should now have a `/home/USER/occurity/release/Occurity` executable ready to run. The `make install` command also installed all of the necessary data files into the folder.

Note that the first time Occurity runs it has no `config.ini`. It will therefore try to open up the Preferences dialog. The default pin-code is `4242`.

## Optotypes
Occurity comes with an optotype (SVG's) that was created from the ground up to adhere to the design characteristics of the original Sloan optotype created by Louise Sloan in 1959. Landolt C and tumbling E optotypes are also available. They are licensed with the Occurity software under the GPLv3.

## Keyboard controls
The following keyboard keys are in use when running Occurity.

### General
* `q`: Enable / disable standby
* `j`: Open Job Runner dialog (requires pin-code, documented [here](docs/CONFIGINI.md))
* `p`: Open Preferences dialog (requires pin-code, documented [here](docs/CONFIGINI.md))
* `s`: Play attention video (only on RPi4 and above)
* `d`: Pause / resume attention video (only on RPi4 and above)
* `f`: Stop attention video (only on RPi4 and above)
* `g`: Play next attention video (only on RPi4 and above)

### Optotype chart specific keyboard functions
* `up`: Change to next row with bigger size
* `down`: Change to next row with smaller size
* `left`: Move entire row to the left
* `right`: Move entire row to the right
* `PgUp`: Skip n number of rows bigger (configure n in preferences)
* `PgDn`: Skip n number of rows smaller (configure n in preferences)
* `c`: Enable / disable crowding
* `m`: Switch between single and multisymbol from the selected row. Use left / right to move between them
* `r`: Randomize symbols from selected row
* `a`: Enable / disable attention animation (if one is defined for the chart in [charts.xml](docs/CHARTSXML.md))

### SVG chart specific keyboard functions
* `left / right`: Switch between SVG layers defined in [charts.xml](docs/CHARTSXML.md)

## Overall configuration
The most important Occurity settings can be configured through the [Preferences dialog](docs/PREFERENCES.md). The dialog is opened by pressing `p` on a connected keyboard. Note that a pin-code must be entered to open Preferences. This is to avoid users inadvertently changing settings that could compromise the quality of the visual acuity results (default is `4242`).

Options that aren't available in the Preferences dialog can be changed by opening the `config.ini` file in a text editor. For a complete description of all available options go [here](docs/CONFIGINI.md).

## Charts customization
Occurity comes with a number of default charts. All charts can easily be customized through the [charts.xml](docs/CHARTSXML.md) file. Go [here](docs/CHARTSXML.md) for a complete description of the format.

## Releases

#### Version x.x.x (unimplemented)

#### Version 1.3.1 (29aug2025)
* MAJOR: Migrated Occurity from Qt5 to Qt6
* MAJOR: Migrated Occurity from qmake to cmake
* Switched to ffmpeg for video playback backend
* Video playback no longer supported on RPi3 due to RPi3 and Qt6 video backend limitations
* Fixed text in about box concerning video playback
* Removed documentation for the `aptinstall` and `aptremove` jobrunner commands as they are no longer supported
* Updated procedures for both the Yocto image and the Ubuntu build

This version has functionality parity with version 1.2.5. The change mainly lies in updated build procedure documentation and the switch to the Qt6 framework. An update to this version is not considered necessary unless you absolutely want to.

#### Version 1.2.5 (21aug2024)
* Now sets default size reset to 5 minutes instead of 240

#### Version 1.2.4 (16May2024)
* Added delay to sleep activated by `q` key press to avoid monitor instantly turning back on due to remote control double-presses.

#### Version 1.2.3 (08May2024)
* Improved the sleep key 'q' functionality to now work both for putting the monitor to sleep and making it wake up again. Any other key will also wake it up.
* Renamed all instances of hibernate to sleep as 'hibernate' is misleading. It only puts the monitor to sleep not the entire system.
* Added 'minutesBeforeSleep' to Preferences dialog.
* Size reset is now in minutes instead of seconds.
* Renamed 'General' config.ini section to 'main'
* Renamed 'sizeResetTime' config variable to 'minutesBeforeSizeReset' for better clarity.
* Removed obsolete 'aptinstall' and 'aptremove' jobrunner commands and all related functions and configs.
* Fixed bug in Slider where value would only be set in Preferences if config variable was in 'General' section.
* Added 'quit' command and 'Quit Occurity' job to jobrunner to allow admin to quit Occurity

#### Version 1.2.2 (06May2024)
* Added row caption support. This allows users to set a caption for each row of the optotype charts.
* Changed sleep scripts to use `xset` instead of `xrandr` for (hopefully) better support for more monitors.

#### Version 1.2.1 (02apr2024)
* Updated documentation
* Updated update job

#### Version 1.2.0 (15mar2024)
* Added 'sync:' command to jobrunner which syncs the filesystem. This command is also auto-added at the end of all jobs
* Added '%ARCH%' hardcoded variable to jobrunner variables (becomes `aarch64` when using Yocto on a Raspberry Pi 4)
* Added '%HOSTNAME%' hardcoded variable to jobrunner variables (becomes `raspberrypi4-64` when using Yocto on a Raspberry Pi 4)
* Splash screen is now closed before preferences instantiation in cases where no 'config.ini' is found when Occurity is started
* Removed 'secret' functionality as it was no longer in use
* Updated JOBS to include note about USBPEN functionality

#### Version 1.1.0 (16nov2023)
* Added on-screen touch controls
* Added astigmastism rays chart
* Fixed occasional ghosting when changing between optotype sizes on a chart

#### Version 1.0.2 (29dec2022)
* Added `startingChart` config variable to allow setting initially displayed chart
* Added all charts to combo in preferences to allow setting `startingChart` config variable
* Added `fadetimings` and `fadelevels` attributes to `<chart>` xml node
* Splash screen now fades in
* Optotype symbols can now be optionally configured to fade in and out
* Rename VisuTest to Occurity
* Added category to jobs
* Changed `update` to `job` everywhere
* Added custom MessageBox that works well with the remote control and keyboard controls
* Preferences dialog can now be closed with `p`
* Updater dialog can now be closed with `j`
* Fixed bug where `updateSrcPath` and `updateDstPath` weren't cleared when running two updates one after the other
* Fixed bug where an excluded path copy would result in all subsequent paths not being investigated
* Added attention gif animation option to `optotype` chart. Show / hide with `a`. Set with `animation="file.gif"`
* Added fullscreen attention video. Controls: Previous video `s`, Play / Pause `d`, Stop `f`, Next video `g`. Loads all .mp4 videos from the defined `videosFolder`. Default is `./videos`. Be aware that the Raspberry Pi might struggle with Full-HD videos, so lower resolutions are recommended
* Added option to skip multiple lines on `optotype` charts using `w` and `s`. Configure number of lines in Preferences
* Added pincode dialog to preferences. Pincode can be set in config.ini with `pinCode=0000`
* Changed mainSettings to reference instead of pointer
* Added scriptable updater allowing more update types than one
* Now shows job file contents in the output view
* Added a plehtora of commands that can be used in job files. All documents in `jobs/README.md`

#### Version 0.7.2 (27jan2022)
* Implemented randomize row with `r` key

#### Version 0.7.1 (26jan2022)
* Moved reset timer to mainwindow instead of each chart

#### Version 0.7.0 (26jan2022)
* MAJOR: Added `optotype` chart type. This chart type obsoletes the old `font` chart and uses SVG's directly instead of requiring a ttf font. All SVG's should be calibrated as 100 pixels = 1 arc minute at a 6 meter patient distance.
* Added crowding rectangle for all optotype charts using `c` key.
* Now uses a 500 pixel width ruler in preferences to determine pixel to mm ratio.
* Now sizelocks between all charts instead of just in the same group.
* Crowding and single mode now follows chart changes.

#### Version 0.6.2 (26may2021)
* Added `T` to Sloan font
* Added `HOTV` charts to [charts.xml](docs/CHARTSXML.md) on numkey 7
* Updated deprecated Qt5 functions

#### Version 0.6.1 (08jul2020)
* Now remembers size for all charts on same button even when switching between back and forth between charts on different buttons
* Fixed bug where chart resize would use letterrow from old size instead of switching to letters from size inherited from other chart on same button

#### Version 0.6.0 (26sep2019)
* Added auto-hibernation after x minutes of inactivity
* Improved hibernation handling overall
* Now supports size locking between charts on same number key

#### Version 0.5.6 (04jul2019)
* Added splash screen
* Updated icons

#### Version 0.5.5 (02jul2019)
* Added chart size reset timer
* Added automated release and update scripts

#### Version 0.5.0 (01apr2019)
* Initial release
* Added SVG chart type support
* Implemented red/green color customization
