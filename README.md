# Occurity

## What it is
Occurity is a fully-featured patient visual acuity test software designed for use on the RaspBerry Pi hardware platform. It enables you to built an affordable visual acuity test system using a few pieces of hardware and basically any remote control. If you do not have the option to buy the infrared receiver, it can also be used with a keyboard.

## License
Occurity is free software; you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation; either version 2 of the License, or (at your option) any later version.

Occurity is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.

## What you need

### Hardware
In order to install a system using Occurity you need the following hardware:
* A Raspberry Pi (3B+ or later is recommended)
  * At least an 8 GB SD flash card
  * A Raspberry Pi case (Recommended, but you might have plans to built it into a monitor)
  * A Raspberry Pi power supply
* A monitor with at least 300 cd/m² brightness (preferably 350 cd/m²)
* A [Flirc infrared reciever](https://flirc.tv/more/flirc-usb) (Optional)
  * A remote control that works with flirc. Most do (Optional)

Note that Occurity can be installed on any ordinary Debian-derived PC as well (Ubuntu is recommended as this is the OS used to develop Occurity).

#### Monitor

##### Resolution
Occurity displays correctly on any monitor using any resolution provided the following two requirements are met:
* The physical length of the ruler must be set correctly in the Occurity [Preferences](docs/PREFERENCES.md) dialog.
* The physical distance from the patients eyes to the monitor must be set correctly in the Occurity [Preferences](docs/PREFERENCES.md) dialog.

To test if the monitor you are using reports its resolution correctly to the Occurity software, please set the width of the ruler as described above, temporarily set the patient distance to 600 cm and measure the exact width of the Sloan optotype sumbols at chart size 0.1 and 0.25. At 0.1 the width must be exactly 87 mm and at 0.25 the width must be exactly 35 mm. If this is correct your monitor will work with Occurity.

To test if your monitor scales the distance correctly, you should also check the sizes at 400 cm patient distance. Here the width of a Sloan optotype symbol must be 58 mm at 0.1 and 23 at 0.25.

##### Brightness calibration
DISCLAIMER!!! This is only a guideline: Using a lux meter pushed up against the monitor surface, you should have a readout of about 277 lux. Hence the need for a monitor that is capable of a high brightness level.

##### Tested working with the following monitors
* Asus VG248QE

#### Remote controls

##### Tested working with the following remote controls
* LG AKB75095344
* Samsung AA59-00818A

In theory most remote controls you might have lying around should work with the disclaimer that there are some remotes that use dual-output signals which makes them hard to use with the flirc interface. Only way to know is to just try.

Note! The [Flirc infrared reciever](https://flirc.tv/more/flirc-usb) emulates a keyboard. For the remote to work, you need to install the Flirc software on a supported OS and configure the buttons on your remote to correspond with [these](README.md#keyboard-controls) keyboard keys.

## Software and configuration

### Operating system
* [Latest Raspberry Pi OS](https://www.raspberrypi.org/downloads/raspberry-pi-os)
Install it on your Pi SD card and plug it into your Pi.

#### System configurations
I recommend applying the following settings on your Pi system to optimize it for use with Occurity.

##### Appearance Settings
* Raspberry menu->Preferences->Appearance Settings
  * Desktop
    * Layout: No Image
    * Color: Black
    * Remove marking in "Wastebasket"
  * Taskbar
    * Size: Small
    * Position: Bottom
    * Color: Black
    * Test color: White 

##### Disable removable media pop-up
Open a File Manager. In Edit->Preferences->Volume Management remove checkmark from "Show available options for removable media"

##### Update notifications
* Right-click the panel and choose `Panel Settings`. Go to `Notifications` and remove checkmark in `Show notifications`.

##### raspi-config
Run `sudo raspi-config` in a terminal and set the following options:
* 1 System Options->S5 Boot / Auto login->B4 Desktop Autologin
* 1 System Options->S6 Network at Boot->No
* 1 System Options->S7 Splash Screen->Yes (You can set this to 'No' if you don't want to expose your support for the Raspberry Pi platform)
* 2 Display Options->D2 Underscan->No
* 2 Display Options->D3 Screen Blanking->No
* 6 Advanced Options->A2 GL Driver->Enable (Not relevant on RPi4)
* 6 Advanced Options->A8 Glamor->Enable (Not relevant on RPi4)

##### /etc/xdg/lxsession/LXDE-pi/autostart
Add the following line to the bottom of the file:
```
@/home/pi/occurity/Occurity
```
Remove the following line to stop the screensaver from starting:
```
@xscreensaver -no-splash
```
Occurity will now autostart when the user logs in. You should also consider commenting the `lxpanel` line to stop the panel from being loaded. But be aware that this will make it harder to launch software. To remedy this it is recommended to first copy the `lxterminal.desktop` shortcut to `/home/USER/Desktop` before rebooting. This will place a terminal icon on the desktop for easy access.

Note that disabling the panel will also complicate wifi connections as the tray icon for connecting to access points will no longer be available. You can of course always re-enable the panel by re-adding the `lxpanel` command to `/etc/xdg/lxsession/LXDE-pi/autostart`.

##### /etc/sudoers.d/pi (Optional)
To allow the use of the `apt*` commands through the Occurity Job Runner scripting language (activated with `j` on the keyboard), it is necessary to allow the `pi` (or whichever user will run Occurity) user to manipulate packages without having to enter a password. This can be achieved by creating `/etc/sudoers.d/pi` (or whichever username will be running Occurity) and inserting the following:
```
pi ALL = NOPASSWD : /usr/bin/apt-get
```

#### Installing Occurity

##### Software prerequisites
Run the following commands in a terminal on the Pi to install the prerequisites needed for Occurity to function correctly.
```
$ sudo apt update
$ sudo apt install qtbase5-dev libqt5svg5-dev qtmultimedia5-dev libqt5multimedia5-plugins
$ sudo apt remove gstreamer1.0-plugins-bad
```
The `gstreamer1.0-plugins-bad` package might not be installed already. But try removing it to be sure. Having it installed is known to break h.264 video playback needed by the Occurity attention video player.

##### Download and compile
Open a terminal on the Pi and run the following commands. Be sure to substitute `LATEST` with the version number of the latest Occurity release (eg. `0.7.2`). Check [here](https://github.com/muldjord/occurity/releases) for the latest version.
```
$ cd
$ mkdir occurity
$ cd occurity
$ wget -N https://github.com/muldjord/occurity/archive/LATEST.tar.gz
$ tar xvzf LATEST.tar.gz --strip-components 1 --overwrite
$ qmake
$ make
```

##### Running Occurity
You should now have a `/home/USER/occurity/Occurity` executable ready to run. Provided that you added Occurity to autostart as described previously, you should now be able to simply restart, and Occurity will run automatically. Note that the first time Occurity runs it has no `config.ini`. It will therefore try to open up the Preferences dialog. The default pin-code is `4242`.

### Optotypes
Occurity comes with an optotype that was created from the ground up to adhere to the design characteristics of the original Sloan optotype created by Louise Sloan in 1959. Landolt C and tumbling E optotypes are also available. Licenses are designated in the `optotypes` subdirectories.

## Keyboard controls
The following keyboard keys are in use when running Occurity.

### General
* `q`: Enable / disable standby
* `j`: Open Job Runner dialog (requires pin-code, documented [here](docs/CONFIGINI.md))
* `p`: Open Preferences dialog (requires pin-code, documented [here](docs/CONFIGINI.md))
* `s`: Restart current attention video
* `d`: Play / pause current attention video
* `f`: Stop attention video
* `g`: Play next attention video

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

#### Version 1.1.0 (In progress, unreleased)
* Added on-screen touch controls

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
