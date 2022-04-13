# Occurity

## What it is
Occurity is a patient visual acuity software designed for use on the RaspBerry Pi hardware platform. It enables you to built an affordable visual acuity test system using a few pieces of hardware and basically any remote control. If you do not have the option to buy the infrared receiver, it can also be used with a keyboard.

## License
Occurity is free software; you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation; either version 2 of the License, or (at your option) any later version.

Occurity is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.

## What you need

### Hardware
In order to install a system using Occurity you need the following hardware
* A Raspberry Pi (preferably a 3B+, but older models should work just fine)
  * At least an 8 GB SD flash card
  * A Raspberry Pi case (recommended, but you might have plans to built it into a monitor)
  * A Raspberry Pi power supply
* A [Flirc infrared reciever](https://flirc.tv/)
  * A remote control that works with flirc
* A monitor with at least 300 cd/m² brightness (preferably 350 cd/m²)

#### Monitor

##### Resolution
Occurity should display correctly on any monitor using any resolution provided the following two requirements are met:
* The physical length of the ruler must be set correctly in the Occurity configuration (Press `p` on the keyboard and use arrow keys to set it).
* The physical distance from the patients eyes to the monitor must be set correctly in the Occurity configuration (Press `p` on the keyboard and use arrow keys to set it).

To test if the monitor you are using reports its resolution correctly to the Occurity software, please set the width of the ruler as described above, temporarily set the patient distance to 600 cm and measure the exact width of the Sloan optotype letters at chart size 0.1 and 0.25. At 0.1 the width must be exactly 87 mm and at 0.25 the width must be exactly 35 mm. If this is correct your monitor will work with Occurity. To test if your monitor scales correctly, you can also check the sizes at 400 cm patient distance. Here the width of a Sloan optotype letter must be 58 mm at 0.1 and 23 at 0.25.

##### Brightness calibration
DISCLAIMER!!! This is only a guideline: Using a lux meter pushed up against the monitor surface, you should have a readout of about 277 lux. Hence the need for a monitor that is capable of a high brightness level.

##### Tested working with the following monitors
* Asus VG248QE

#### Remote controls

##### Tested working with the following remote controls
* LG AKB75095344
* Samsung AA59-00818A

In theory most remote controls you might have lying around should work with the disclaimer that there are some remotes that use dual-output signals which makes them hard to use with the flirc interface. Only way to know is to just try.

## Software and configuration

### Operating system
* [Latest Raspberry Pi OS](https://www.raspberrypi.org/downloads/raspberry-pi-os)
Install it on your Pi SD card and plug it into your Pi.

#### Software prerequisites
Run the following commands in a terminal on the Pi to install the prerequisites.
```
$ sudo apt update
$ sudo apt install git qtbase5-dev libqt5svg5-dev qtmultimedia5-dev libqt5multimedia5-plugins
$ sudo apt remove gstreamer1.0-plugins-bad
```

#### System configurations
I recommend applying the following settings on your Pi system to optimize it for use with Occurity.

##### /etc/sudoers.d/pi
To allow the use of the `apt*` commands from the Occurity updater scripting language (updater activated with `j` on the keyboard), it is necessary to allow the `pi` (or whichever user will run Occurity) user to manipulate packages without having to enter a password. This can be achieved by creating `/etc/sudoers.d/pi` and inserting the following:
```
pi ALL = NOPASSWD : /usr/bin/apt-get
```

##### raspi-config
Run `sudo raspi-config` in a terminal and set the following options:
* 1 System Options->S5 Boot / Auto login->B4 Desktop Autologin
* 1 System Options->S6 Network at Boot->No
* 1 System Options->S7 Splash Screen->Yes
* 2 Display Options->D2 Underscan->No
* 2 Display Options->D3 Screen Blanking->No
* 6 Advanced Options->A2 GL Driver->Enable
* 6 Advanced Options->A8 Glamor->Enable

##### /etc/xdg/lxsession/LXDE-pi/autostart
Add the following line to the bottom of the file:
```
@/home/pi/occurity/Occurity
```
This will autostart Occurity when the system is logged in.

##### Appearance Settings
* Raspberry menu->Preferences->Appearance Settings
  * Desktop
    * Layout: No Image
    * Color: Black
    * Remove marking in "Wastebasket"
  * Menu bar
    * Size: Small
    * Position: Bottom
    * Color: Black
    * Test color: White 

##### pcmanfm
* Run the command `pcmanfm` which will open the File Manager. In the Edit->Preferences->Disk Management remove checkmark from "Show available options for removable media"

##### Update notifications
* Right-click panel and choose `Panel Settings`. Go to `Notifications` and remove checkmark in `Show notifications`.

#### Getting and compiling Occurity
Open a terminal on the Pi and run the following commands. This will fetch the Occurity source code and compile it.
```
$ cd
$ mkdir programming
$ cd programming
$ git clone https://github.com/muldjord/occurity.git
$ cd occurity
$ qmake
$ make
```

### Optotypes
Occurity comes with an optotype that was created from the ground up to adhere to the design characteristics of the original Sloan optotype created by Louise Sloan in 1959. Landolt C and tumbling E optotypes are also available. Licenses are designated in the `optotypes` subdirectories.

## Keyboard controls
The following keyboard keys are in use when running Occurity.

### General
* `q`: Enable / disable standby
* `j`: Open Job Runner dialog (requires pin-code, documented [here](docs/CONFIGINI.md))
* `p`: Open Preferences dialog (requires pin-code, documented [here](docs/CONFIGINI.md))
* `s`: Restart current attention video
* `d`: Start attention video
* `f`: Stop attention video
* `g`: Next attention video

### Optotype chart specific keyboard functions
* `up`: Change to next line with bigger size
* `down`: Change to next line with smaller size
* `left`: Move entire row to the left
* `right`: Move entire row to the right
* `PgUp`: Skip n number of lines bigger (configure n in preferences)
* `PgDn`: Skip n number of lines smaller (configure n in preferences)
* `c`: Enable / disable crowding
* `m`: Switch between single and multi-optotype from the selected row. Use left / right to move between them
* `r`: Randomize optotypes from selected line
* `a`: Enable / disable attention animation (if one is defined)

### SVG chart specific keyboard functions
* `left / right`: Switch between layers defined in `charts.xml`

## Overall configuration
Most important Occurity settings can be configured through the Preferences dialog. The dialog is opened by pressing `p` on a connected keyboard. Note that a pin-code must be entered to open Preferences. This is to avoid users inadvertently changing settings that could compromise the quality of the visual acuity results. The pin-code is configured in `config.ini`.

Options that aren't available in the Preferences dialog can be changed by opening the `config.ini` file in a text editor. For a complete description of all available options go [here](docs/CONFIGINI.md).

## Charts customization
Occurity comes with a number of default charts. All charts can easily be customized through the `charts.xml` file. Go [here](docs/CHARTSXML.md) for a complete description of the format.

## Releases

#### Version x.x.x (unimplemented)
* Add all charts to combo in preferences to allow setting `startingChart` config variable

#### Version 1.0.1 (In progress, unreleased)
* Added `startingChart` config variable to allow setting initial displayed chart
* Added `fadetimings` and `fadelevels` attributes to `<chart>` xml node
* Splash screen now fades in
* Optotype symbols now fade in and out for better clinician visual clues
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
* Added `HOTV` charts to `charts.xml` on numkey 7
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
