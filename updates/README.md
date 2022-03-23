# VisuTest update scripting help

## What is contained in this folder
This folder contains `.upd` files which contain recipe's for updates, creating updates or other maintenance jobs. The `.upd` files within this folder are read and parsed through the integrated VisuTest updater dialog (initiated with `u` on the keyboard).

Below follows a complete description of the available commands and syntax of the VisuText update scripting language.

## Scripting language overview
A single line of the scripting language is made up of the command type, followed by a `:`, followed by the required arguments. If more arguments are allowed, they are separated by `;`.

## Available commands

### title:TITLE
Sets the title that will be used to select and execute this particular `.upd` entry in the VisuTest updater dialog.

* Example 1: `title:This is a title`

### pretend:TRUE|FALSE
Setting this to true will ensure that no files or directories are manipulated while executing the script lines following this line. Default is `false`. Useful while testing / debugging a script.

* Example 1: `pretend:true`


### message:MESSAGE
Add a message to the debug output of the VisuTest updater dialog. The message will be white and is useful for providing status / progress messages while processing a script.

* Example 1: `pretend:This is a message!`

### setvars:FILENAME
Loads variables from a file and makes them available to the script for use in command parameters.

* Example 1: `setvars:myvars.txt`

#### myvars.txt
```
THIS=Something
THAT=Other
```

These two variables can now be used in the script command parameters using `%THIS%` and `%THAT%` which will be replaced with the corresponding text.

* Example 1: `cpfile:path1/%THIS%/somefile.dat` becomes `cpfile:path1/Something/somefile.dat`

#### Hardcoded variables
Some hardcoded variables are available. These can be used without setting them with `setvars`.

* %HOME%: The current user home directory (eg. `/home/pi`)

### aptinstall:PACKAGE1;PACKAGE2;...
In some cases it can be necessary to install further packages using the `sudo apt-get install` command. This command allows just that. Be aware that in order for this command to work, you need to do the following.

#### /etc/sudoers/sudoers.d/pi
To allow the use of the `apt*` commands from the VisuTest updater scripting language (updater activated with `u` on the keyboard), it is necessary to allow the `pi` (or whichever user will run VisuTest) user to manipulate software packages without having to enter a password. This can be achieved by creating `/etc/sudoers/sudoers.d/pi` and inserting the following:
```
pi ALL = NOPASSWD : /usr/bin/apt-get

```

* Example 1: `aptinstall:qtmultimedia5-dev;libqt5multimedia5-plugins`

`sudo apt-get update` will automatically be run before installing any new packages.

### aptremove:PACKAGE1;PACKAGE2;...
In some cases it can be necessary to remove packages using the `sudo apt-get remove` command. This command allows just that. Be aware that in order for this command to work, you need to do the following.

#### /etc/sudoers/sudoers.d/pi
To allow the use of the `apt*` commands from the VisuTest updater scripting language (updater activated with `u` on the keyboard), it is necessary to allow the `pi` (or whichever user will run VisuTest) user to manipulate software packages without having to enter a password. This can be achieved by creating `/etc/sudoers/sudoers.d/pi` and inserting the following:
```
pi ALL = NOPASSWD : /usr/bin/apt-get

```

* Example 1: `aptremove:gstreamer1.0-plugins-bad`

`sudo apt-get update` will automatically be run before removing any packages.

### cpfile:sourcefile;destinationfile
Copies a file from `sourcefile` to `destinationfile`. If `destinationfile` is left out it will copy the file using the filename from `sourcefile`. Both are relative to the corresponding source and destination directiories set with `srcpath` and `dstpath` (documented elsewhere in this document) unless the file path starts with a `/`.

* Example 1: `cpfile:VisuTest` <-- This copies the source file `VisuTest` from the path defined by `srcpath` to a destination file of the same name located at the path defined by `dstpath`.
* Example 2: `cpfile:VisuTest;OtherName` <-- This copies the source file `VisuTest` from the path defined by `srcpath` to a destination file called `OtherName` located at the path defined by `dstpath`.
* Example 3: `cpfile:/tmp/somefilename;/home/pi/someotherfilename` <-- This copies the source file `/tmp/somefilename` to the destination file `/home/pi/someotherfilename` ignoring the previously defined `srcpath` and `dstpath`.
* Example 4: `cpfile:somefilename;/home/pi/someotherfilename` <-- This copies the source file `somefilename` to the destination file `/home/pi/someotherfilename`. The source file is relative to the path defined by `srcpath` while `dstpath` is ignored.

### cppath:sourcepath;destinationpath
Copies a path, including subdirectories, from `sourcepath` to `destinationpath`. If `destinationpath` is left out it will copy the path using the path name from `sourcepath`. Both are relative to the corresponding source and destination directiories set with `srcpath` and `dstpath` (documented elsewhere in this document) unless the path name starts with a `/`.

* Example 1: `cppath:optotypes` <-- This copies the source path `optotypes` from the path defined by `srcpath` to a destination path of the same name located at the path defined by `dstpath`.
* Example 2: `cppath:optotypes;mypath` <-- This copies the source path `optotypes` from the path defined by `srcpath` to a destination path called `mypath` located at the path defined by `dstpath`.
* Example 3: `cppath:/tmp/somepath;/home/pi/someotherpath` <-- This copies the source path `/tmp/somepath` to the destination path `/home/pi/someotherpath` ignoring the previously defined `srcpath` and `dstpath`.
* Example 4: `cppath:somepath;/home/pi/someotherpath` <-- This copies the source path `somepath` to the destination path `/home/pi/someotherpath`. The source path is relative to the path defined by `srcpath` while `dstpath` is ignored.

### version:x.x.x
This command is optional. If set, the `x.x.x` will be shown before the title when displaying the update entry in the VisuTest updater dialog. The format of the version is undefined. A `x.x.x` format is suggested, but it can just as well be `May 2022`.

* Example 1: `version:1.2.0`
* Example 2: `version:May 2022 version`