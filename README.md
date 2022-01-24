# VisuTest

## What it is
VisuTest is a visual acuity test software designed for use on the RaspBerry Pi hardware platform. It enables you to built an affordable visual acuity test system using a few pieces of hardware and basically any remote control. If you do not have the option to buy the infrared receiver, it can also be used with a keyboard.

## License
VisuTest is free software; you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation; either version 2 of the License, or (at your option) any later version.

VisuTest is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.

## What you need

### Hardware
In order to install a system using VisuTest you need the following hardware
* A Raspberry Pi (preferably a 3B+, but older models should work just fine)
  * At least an 8 GB SD flash card
  * A Raspberry Pi case (recommended, but you might have plans to built it into a monitor)
  * A Raspberry Pi power supply
* A Flirc infrared reciever (https://flirc.tv/)
  * A remote control that works with flirc
* A monitor with at least 300 cd/m² brightness (preferably 350 cd/m²)

### Monitor

#### Resolution
VisuTest should display correctly on any monitor using any resolution provided the following two requirements are met:
* The physical length of the ruler must be set correctly in the VisuTest configuration (Press `p` on the keyboard and use arrow keys to set it).
* The physical distance from the patients eyes to the monitor must be set correctly in the VisuTest configuration (Press `p` on the keyboard and use arrow keys to set it).

To test if the monitor you are using reports its resolution correctly to the VisuTest software, please set the height as described above and temporarily set the patient distance to 600 cm and measure the exact height of the letters at chart size 0.05 and 0.25. At 0.05 the height must be exactly 175 mm and at 0.25 the height must be exactly 35 mm. If this is correct your monitor will work with VisuTest.

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
$ sudo apt install qt5-default libqt5svg5-dev
```

### System configurations
I recommend applying the following settings on your Pi system to optimize it for use with VisuTest.

#### raspi-config
Run `raspi-config` in a terminal and set the following options:
* Boot Options->B1 Desktop / CLI->B4 Desktop Autologin
* Boot Options->B2 Wait for Network at Boot->No
* Boot Options->B3 Splash Screen->Yes
* Localization Options->Change Timezone->CONTINENT->CITY
* Localization Options->Change Locale->Check only your locale (for instance en_DK.UTF-8)
* Advanced Options->A2 Overscan->No (IMPORTANT!!! Otherwise the screen will have black bars and acuity test will be faulty)

Some, if not all, of the above can also be set in the ui when the Pi boots for the first time.

#### /etc/xdg/lxsession/LXDE-pi/autostart
Add the following line to the bottom of the file:
```
@/home/pi/visutest/VisuTest
```
This will autostart VisuTest when the system is logged in.

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

### Getting and compiling VisuTest
Open a terminal on the Pi and run the following commands. This will fetch the VisuTest source code and compile it.
```
$ cd
$ git clone https://github.com/muldjord/visutest.git
$ cd visutest
$ qmake
$ make
```

### Optotypes
VisuTest comes with an optotype that was created from the ground up to adhere to the design characteristics of the original Sloan optotype created by Louise Sloan in 1959. Landolt C and tumbling E optotypes are also available. Licenses are designated in the `optotypes` subdirectories.

### charts.xml
All of the charts displayed in the VisuTest software are customizable. Edit the `charts.xml` file in any basic text/xml editor. The format is as follows:

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
* caption="Caption" <-- The caption that is shown on-screen in VisuTest.
* type="optotype" (required) <-- The chart type. Currently supports `optotype` and `svg`.
* bgcolor="white" <-- Sets the background color of the chart (can be `black`, `white` or hex as `#AABBCC`).
* sizelock="true" <-- If this is set to true and you switch to another chart with the same attribute, it will, if possible, inherit the size of the previous chart, giving a consistent size between chart changes.

##### 'optotype' chart type specific
* optotype="sloan" (required) <-- Which optotype is used by this chart. This MUST correspond to the name of a subdirectory located in the `optotypes` subfolder. In this case `optotypes/sloan`.
* startsize="0.1" <-- Sets optotype size on initialization. Size must match with a size from a `row` below.
* crowdingspan="2.5" <-- Sets optotype crowding span in size relative arc minutes. If left out the default value of 2.5 is used.

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
You can add as many layer nodes as you'd like. VisuTest can then switch between them with the left/right arrow keys. SVG's and their layers can be created with the open source software [Inkscape](https://inkscape.org/).

### config.ini
You can configure several options of VisuTest to fit your needs. The first time VisuTest is started the preferences dialog will appear on screen. Remember to set everything up appropriately. To change these setting later, either press the `p` key on the keyboard and change the values using the arrow keys, or simply edit the `config.ini` file in any editor.
* physDistance=310 <-- The distance from the monitor to the patient in centimeters. This MUST be set correctly in order for VisuTest to show the optotypes at their correct sizes.
* rulerWidth=125 <-- The physical width of the ruler shown in the preferences dialog in millimeters. This MUST be set correctly in order for VisuTest to show the optotypes at their correct sizes.
* chartsXml="charts.xml" <-- Allows you to override the default xml file that defines the charts. Default is `charts.xml`.
* optotypesDir="your/optotypes/folder" <-- Defines the optotypes folder to load chart optotypes from. Default is `optotypes`.
* redValue=210 <-- Sets the red color value or SVG charts. To make this work, the initial red color of the SVG elements has to be `#d20000`.
* greenValue=210 <-- Sets the green color value or SVG charts. To make this work, the initial green color of the SVG elements has to be `#00d200`.
* sizeResetTime=240 <-- After this many seconds of inactivity, the charts will reset back to the startsize as defined in `charts.xml`.
* hibernateTime=140 <-- After this many minutes of inactivity, the monitor will turn off to avoid burn-in. You can turn it back on by pressing `q`.

## Releases

#### Version x.x.x (unimplemented)
* Add `startingChart` to config to allow setting initial displayed chart
* Add all charts to combo in preferences to allow setting `startingChart` config variable

#### Version 0.7.0 (In progress, unreleased)
* MAJOR: Added `optotype` chart type. This chart type obsoletes the old `font` chart and uses SVG's directly instead of requiring a ttf font. All SVG's should be calibrated as 100 pixels = 1 arc minute.
* Added crowding rectangle for all optotype charts using `c` key.
* Now uses a 500 pixel width ruler in preferences to determine pixel to mm ratio.
* Now sizelocks between number keys instead of just in the same group

##### Known issues
* `svgchart` type doesn't scale in a meaningful manner. Considering changing `scale` attribute to a `mode` attribute that can be set as `fittowidth`, `static`, `scaled` and perhaps a few others. `static` should just place the SVG at whatever pixel size it has been made with. `scaled` should scale according to patient distance (otherwise similar to `static`)
* SVG optotype files has to be named as single letter filenames to work wellwith the current XML row design. This hould probably be expanded to allow any filename where the XML could split them using `;`

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
