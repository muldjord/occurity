# VisuTest

## What it is
VisuTest is a visual acuity test software designed for use on the RaspBerry Pi hardware platform. It enables you to built an affordable visual acuity test system using a few pieces of hardware and basically any remote control. If you do not have the option to buy the infrared reciever, it can also be used with a keyboard.

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
* The height of the actual physical monitor displayable area (do not measure it with the plastic bezels included, just the vertical pixel monitor area) must be correctly entered into the VisuTest configuration (Press 'p' on the keyboard and use arrow keys to set it).
* The physical distance from the patients eyes to the monitor must be set correctly in the VisuTest configuration (Press 'p' on the keyboard and use arrow keys to set it).

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
* Latest Raspbian (https://www.raspberrypi.org/downloads/raspbian/)
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
Run 'raspi-config' in a terminal and set the following options:
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
Open a terminal on the Pi and run the following commands. This will fetch the VisuTest source code and compile it. When it's compiled, you need to install the fonts you wish to use (read below).
```
$ cd
$ git clone https://github.com/muldjord/visutest.git
$ cd visutest
$ qmake
$ make
```

### Fonts
For licensing reasons VisuTest is provided without fonts. You need to license these yourself.

### charts.xml
All of the charts displayed in the VisuTest software are customizable. Edit the '/home/pi/visutest/charts.xml' file in any basic text/xml editor. The format is as follows:

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
* numkey="1" (required) <-- The number key assigned to charts in this group. Activating this number key on the keyboard, or a flirc supported remote control, will then switch between the defined charts

#### 'chart' nodes
```
<chart name="Chart 1" type="font" fontfamily="Sloan" bgcolor="white" sizelock="true">
  ...
</chart>
```
Must be nedsted in the 'charts' node. This defines a single chart to be used by the software. It supports the following attributes, some of which are required:
* caption="Caption" <-- The caption that is shown on-screen in VisuTest
* type="font" (required) <-- The chart type. Currently supports 'font' and 'svg'
* bgcolor="white" <-- Sets the background color of the chart (can be 'black', 'white' or hex as '#AABBCC')
* sizelock="true" <-- If this is set to true and you switch to another chart with the same attribute within the same group, it will, if possible, inherit the size of the previous chart, giving a consistent size between chart changes

##### 'font' chart type specific
* fontfamily="Sloan" (required) <-- Which font family is used by this chart
* startsize="0.1" <-- Sets font size on initialization. Size must match with a size from a 'row' below.

###### 'row' node
Must be nested inside a 'chart' node. A single 'row' node defines a row in the chart. It has the following format:
```
<row size="0.1">NCKZO</row>
```
Size tells which size is needed for the row (required). This is defined as 1.0 being equal to 5 arc-minutes at a distance of 6 meters. The text of the node are which letters are to be used by the row. You can use as many or few as you'd like.

##### 'svg' chart type specific
* source="filename.svg" <-- The filename containing the SVG you want to use

###### 'layer' node
Must be nested inside a 'chart' node. Defines a layer from inside an SVG to be displayed on the chart. It has the following format:
```
<layer id="layername"/>
```
You can add as many layer nodes as you'd like. VisuTest can then switch between them with the left/right arrow keys.

### config.ini
You can configure several options of VisuTest to fit your needs. The first time VisuTest is started, the preferences dialog will appear on screen. Remember to set everything up appropriately. To change these setting later, either press the 'p' key on the keyboard and change the values using the arrow keys, or simply edit the 'config.ini' file in any editor.
* physDistance=310 <-- The distance from the monitor to the patient in centimeters.
* physHeight=281 <-- The physical height of the actual screen area in millimeters. Be aware that this is ONLY the area where actual pixels are present on the screen. You should NOT include the plastic bezel.
* chartsXml="charts.xml" <-- Allows you to override the default xml file that defines the charts. Default is 'charts.xml'.
* fontDir="your/font/folder" <-- Defines the font folder to load chart fonts from. Defaults is './fonts'
* redValue=210 <-- Sets the red color value or SVG charts. To make this work, the initial red color of the SVG elements has to be '#d20000'.
* greenValue=210 <-- Sets the green color value or SVG charts. To make this work, the initial green color of the SVG elements has to be '#00d200'.
* sizeResetTime=240 <-- After this many seconds of inactivity, the charts will reset back to the initial size as set in charts.xml.
* hibernateTime=140 <-- After this many minutes of inactivity, the monitor will turn off to avoid burn-in.

## Releases

#### Version x.x.x (unimplemented)
* Add 'startingChart' to config to allow setting initial displayed chart
* Add all charts to combo in preferences to allow setting 'startingChart' config variable

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
