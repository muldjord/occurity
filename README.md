# Occurity

## What it is
Occurity is a visual acuity test software designed for use on the RaspBerry Pi hardware platform. It enables you to built an affordable visual acuity test system using a few pieces of hardware and basically any remote control. If you do not have the option to buy the infrared receiver, it can also be used with a keyboard.

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
* A Flirc infrared reciever (https://flirc.tv/)
  * A remote control that works with flirc
* A monitor with at least 300 cd/m² brightness (preferably 350 cd/m²)

### Monitor

#### Resolution
Occurity should display correctly on any monitor using any resolution provided the following two requirements are met:
* The physical length of the ruler must be set correctly in the Occurity configuration (Press `p` on the keyboard and use arrow keys to set it).
* The physical distance from the patients eyes to the monitor must be set correctly in the Occurity configuration (Press `p` on the keyboard and use arrow keys to set it).

To test if the monitor you are using reports its resolution correctly to the Occurity software, please set the width of the ruler as described above, temporarily set the patient distance to 600 cm and measure the exact width of the Sloan optotype letters at chart size 0.1 and 0.25. At 0.1 the width must be exactly 87 mm and at 0.25 the width must be exactly 35 mm. If this is correct your monitor will work with Occurity. To test if your monitor scales correctly, you can also check the sizes at 400 cm patient distance. Here the width of a Sloan optotype letter must be 58 mm at 0.1 and 23 at 0.25.

#### Brightness calibration
DISCLAIMER!!! This is only a guideline: Using a lux meter pushed up against the monitor surface, you should have a readout of about 277 lux. Hence the need for a monitor that is capable of a high brightness level.

#### Tested working with the following monitors
* Asus VG248QE

### Remote controls

#### Tested working with the following remote controls
* LG AKB75095344
* Samsung AA59-00818A

In theory most remote controls you might have lying around should work with the disclaimer that there are some remotes that use dual-output signals which makes them hard to use with the flirc interface. Only way to know is to just try.

### Operating system
* [Latest Raspberry Pi OS](https://www.raspberrypi.org/downloads/raspberry-pi-os)
Install it on your Pi SD card and plug it into your Pi.

### Software prerequisites
Run the following commands in a terminal on the Pi to install the prerequisites.
```
$ sudo apt update
$ sudo apt install git qtbase5-dev libqt5svg5-dev qtmultimedia5-dev libqt5multimedia5-plugins
$ sudo apt remove gstreamer1.0-plugins-bad
```

### System configurations
I recommend applying the following settings on your Pi system to optimize it for use with Occurity.

#### /etc/sudoers/sudoers.d/pi
To allow the use of the `apt*` commands from the Occurity updater scripting language (updater activated with `j` on the keyboard), it is necessary to allow the `pi` (or whichever user will run Occurity) user to manipulate packages without having to enter a password. This can be achieved by creating `/etc/sudoers/sudoers.d/pi` and inserting the following:
```
pi ALL = NOPASSWD : /usr/bin/apt-get
```

#### raspi-config
Run `sudo raspi-config` in a terminal and set the following options:
* 1 System Options->S5 Boot / Auto login->B4 Desktop Autologin
* 1 System Options->S6 Network at Boot->No
* 1 System Options->S7 Splash Screen->Yes
* 2 Display Options->D2 Underscan->No
* 2 Display Options->D3 Screen Blanking->No
* 6 Advanced Options->A2 GL Driver->Enable
* 6 Advanced Options->A8 Glamor->Enable

#### /etc/xdg/lxsession/LXDE-pi/autostart
Add the following line to the bottom of the file:
```
@/home/pi/occurity/Occurity
```
This will autostart Occurity when the system is logged in.

#### Appearance Settings
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

#### pcmanfm
* Run the command `pcmanfm` which will open the File Manager. In the Edit->Preferences->Disk Management remove checkmark from "Show available options for removable media"

#### Update notifications
* Right-click panel and choose `Panel Settings`. Go to `Notifications` and remove checkmark in `Show notifications`.

### Getting and compiling Occurity
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

### charts.xml
All of the charts displayed in the Occurity software are customizable. Edit the `charts.xml` file in any basic text/xml editor. The format is as follows:

#### 'xml' node
```
<?xml version="1.0" encoding="UTF-8"?>
```
This node tells the xml format. Don't change it.

#### 'charts' node
```
<charts>
  ...
</charts>
```
This node is the parent node containing all charts. Don't change or remove it.

#### 'group' node
```
<group numkey="1">
  ...
</group>
```
This groups several charts together on the same number key. It supports the following attributes, some of which are required:
* numkey="1" (required) <-- This is the number key assigned to charts in this group. Activating this number key on the keyboard, or a flirc supported remote control, will then switch between the charts contained in the group.

#### 'chart' nodes
```
<chart name="Chart 1" type="optotype" optotype="sloan" bgcolor="white" sizelock="true">
  ...
</chart>
```
Must be nested in the `group` node. This defines a single chart to be used by the software. It supports the following attributes, some of which are required:
* caption="Caption" <-- The caption that is shown on-screen in Occurity.
* type="optotype" (required) <-- The chart type. Currently supports `optotype` and `svg`.
* bgcolor="white" <-- Sets the background color of the chart (can be `black`, `white` or hex as `#AABBCC`).
* sizelock="true" <-- If this is set to true and you switch to another chart with the same attribute, it will, if possible, inherit the size of the previous chart, giving a consistent size between chart changes.

##### 'optotype' chart type specific
* optotype="sloan" (required) <-- Which optotype is used by this chart. This MUST correspond to the name of a subdirectory located in the `optotypes` subfolder. In this case `optotypes/sloan`.
* startsize="0.1" <-- Sets optotype size on initialization. Size must match with a size from a `row` below.
* crowdingspan="2.5" <-- Sets optotype crowding span in size relative arc minutes. If left out the default value of 2.5 is used.
* animation="file.gif" <-- Sets the attention GIF animation for this chart. Activate with `a`.

###### 'row' node
Must be nested inside a `chart` node. A single `row` node defines a row in the chart. It has the following format:
```
<row size="0.1">NCKZO</row>
<row size="0.1">one;two;three;four;five</row>
```
Size defines the size used by the row (required). This is defined as 0.1 being equal to 5 arc-minutes at a distance of 6 meters.

You can fill in the contents of a row in two different ways. Either by simply entering the letters that should be used. These letters MUST correspond to the filenames of the optotype specific subdirectory located in the `optotypes` subdirectory. In the first examples above the files must be called `N.svg`, `C.svg` and so on.

The second way is by using semicolons to separate the optotypes, which correspond to the filenames of the optotype specific subdirectory located in the `optotypes` subdirectory. In the second examples this would require files to be named `one.svg`, `two.svg` and so on.

##### 'svg' chart type specific
* source="filename.svg" <-- The filename containing the SVG you want to use.
* scaling="distance" <-- Can be `width`, `height` or `distance`. `distance` scales according to patient distance. The SVG must be scaled as 100 pixels being equal to 1 arc minute at a distance of 6 meters. `width` and `height` simply scales the SVG to fit the width or height of the screen.

###### 'layer' node
Must be nested inside a `chart` node. Defines a layer from inside an SVG to be displayed on the chart. It has the following format:
```
<layer id="layername"/>
```
You can add as many layer nodes as you'd like. Occurity can then switch between them with the left/right arrow keys. SVG's and their layers can be created with the open source software [Inkscape](https://inkscape.org/).

### config.ini
You can configure several options of Occurity to fit your needs. The first time Occurity is started the preferences dialog will appear on screen. Remember to set everything up appropriately. To change these setting later, either press the `p` key on the keyboard and change the values using the arrow keys, or simply edit the `config.ini` file in any editor.
* `physDistance=310`: The distance from the monitor to the patient in centimeters. This MUST be set correctly in order for Occurity to show the optotypes at their correct sizes.
* `rulerWidth=125`: The physical width of the ruler shown in the preferences dialog in millimeters. This MUST be set correctly in order for Occurity to show the optotypes at their correct sizes.
* `chartsXml="charts.xml"`: Allows you to override the default xml file that defines the charts. Default is `charts.xml`.
* `optotypesDir="your/optotypes/folder"`: Defines the optotypes folder to load chart optotypes from. Default is `optotypes`.
* `redValue=210`: Sets the red color value or SVG charts. To make this work, the initial red color of the SVG elements has to be `#d20000`.
* `greenValue=210`: Sets the green color value or SVG charts. To make this work, the initial green color of the SVG elements has to be `#00d200`.
* `sizeResetTime=240`: After this many seconds of inactivity, the charts will reset back to the startsize as defined in `charts.xml`.
* `hibernateTime=140`: After this many minutes of inactivity, the monitor will turn off to avoid burn-in. You can turn it back on by pressing `q`.
* `rowSkipDelta=4`: The number of lines skipped when pressing `w` and `s`.
* `pinCode=4242`: Sets the pincode to be entered in order to unlock the Preferences and Updater dialogs. It can be any length as long as it only contains numbers. Default is `4242`.

## Keyboard control
* q: Hibernate / turn monitor off
* u: Launch updater (requires pin-code)
* p: Launch preferences (requires pin-code)
* z: Start attention video
* x: Stop attention video

### Optotype chart specific keyboard functions
* up: Change to next line with bigger size
* down: Change to next line with smaller size
* left: Move entire row to the left
* right: Move entire row to the right
* PgUp: Skip n number of lines bigger (configure n in preferences)
* PgDn: Skip n number of lines smaller (configure n in preferences)
* c: Enable / disable crowding
* m: Switch between single and multi-optotype from the selected row. Use left / right to move between them
* r: Randomize optotypes from selected line
* a: Enable / disable attention animation

### SVG chart specific keyboard functions
* left / right: Switch between layers defined in charts.xml

## Releases

#### Version x.x.x (unimplemented)
* Add `startingChart` to config to allow setting initial displayed chart
* Add all charts to combo in preferences to allow setting `startingChart` config variable

#### Version 1.0.0 (25mar2022)
* Rename VisuTest to Occurity
* Added category to jobs
* Changed `update` to `job` everywhere
* Added custom MessageBox that works well with the remote control and keyboard controls
* Preferences dialog can now be closed with `p`
* Updater dialog can now be closed with `j`
* Fixed bug where `updateSrcPath` and `updateDstPath` weren't cleared when running two updates one after the other
* Fixed bug where an excluded path copy would result in all subsequent paths not being investigated
* Added attention gif animation option to `optotype` chart. Show / hide with `a`. Set with `animation="file.gif"`
* Added fullscreen attention video. Play / stop with `z` and `x`. Currently hardcoded to load and play `./video.mp4`. Be aware that the Raspberry Pi might struggle with Full-HD videos, so lower resolutions are recommended
* Added option to skip multiple lines on `optotype` charts using `w` and `s`. Configure number of lines in Preferences
* Added pincode dialog to preferences. Pincode can be set in config.ini with `pinCode=0000`
* Changed mainSettings to reference instead of pointer
* Added scriptable updater allowing more update types than one

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
