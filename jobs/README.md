# Occurity job scripting help

## What is contained in this folder
This folder contains `.job` files which consist of commands to be executed for custom maintenance jobs. The `.job` files within this folder are read and parsed through the integrated Occurity job runner dialog (initiated with `j` on the keyboard).

Below follows a complete description of the available commands and syntax of the VisuText update scripting language.

## Scripting language overview
A single line of the scripting language is made up of the command type, followed by a `:`, followed by the required arguments. If more arguments are allowed / required, they are separated by `;`.

## Available commands

### title:TITLE
Sets the title that will be used to select and execute this particular `.upd` entry in the Occurity updater dialog.

* Example 1: `title:This is a title`

### category:CATEGORY
This defines the category applied to the job when adding it to the Job runner dialog. More jobs can share the same category.

* Example 1: `category:Tools`
* Example 2: `category:Update jobs`

### version:x.x.x
This command is optional. If set, the `x.x.x` will be shown before the title when displaying the update entry in the Occurity updater dialog. The format of the version is undefined. A `x.x.x` format is suggested, but it can just as well be `May 2022`.

* Example 1: `version:1.2.0`
* Example 2: `version:May 2022`

### pretend:TRUE|FALSE
Setting this to true will ensure that no files or directories are manipulated while executing the script lines following this line. Default is `false`. Useful while testing / debugging a script.

* Example 1: `pretend:true`

### message:MESSAGE
Add a message to the debug output of the Occurity updater dialog. The message will be white and is useful for providing status / progress messages while processing a script.

* Example 1: `pretend:This is a message!`

### setvar:VARIABLE;VALUE
Sets a variable for use in the `.upd` script. All variables will have a `%` prepended and appended to it.

* Example 1: `setvar:MYVAR;My value` <-- This allows you to use `%MYVAR%` throughout the script command arguments / parameters where it will be replaced with `My value`.

### loadvars:FILENAME
Loads variables from a file and makes them available to the script for use in command parameters.

* Example 1: `loadvars:myvars.txt`

#### myvars.txt
```
THIS=Something
THAT=Other
```
These two variables can now be used in the script command arguments / parameters using `%THIS%` and `%THAT%` which will be replaced with the corresponding text.

* Example 1: `cpfile:path1/%THIS%/somefile.dat` becomes `cpfile:path1/Something/somefile.dat`

#### Hardcoded variables
Some hardcoded variables are available. These can be used without setting them with `setvar` or `loadvars`.

* %HOME%: The current user home directory (eg. `/home/pi`).
* %WORKDIR%: The Occurity working directory (eg. `/home/pi/occurity`).
* %USER%: The user name of the currently logged in user (eg. `pi`).

### addexclude:FILE|PATH
Adds either a file or path to the exclude list used by the cp* commands. This is useful if you want to copy an entire path but want to leave out a subdirectory or file from within the path.

Note! Both relative and non-relative file and path are allowed. If relative it will match with files or paths located relative to the defined `srcpath`.

* Example 1: `addexclude:optotypes/sloan
* Example 2: `addexclude:optotypes/README.md
* Example 3: `addexclude:%WORKDIR%/README.md

### cpfile:SOURCEFILE;DESTINATIONFILE
Copies a file from `SOURCEFILE` to `DESTINATIONFILE`. If `DESTINATIONFILE` is left out it will copy the file using the filename from `SOURCEFILE`. Both are relative to the corresponding source and destination directiories set with `srcpath` and `dstpath` (documented elsewhere in this document) unless the file path starts with a `/`.

* Example 1: `cpfile:Occurity` <-- This copies the source file `Occurity` from the path defined by `srcpath` to a destination file of the same name located at the path defined by `dstpath`.
* Example 2: `cpfile:Occurity;OtherName` <-- This copies the source file `Occurity` from the path defined by `srcpath` to a destination file called `OtherName` located at the path defined by `dstpath`.
* Example 3: `cpfile:/tmp/somefilename;/home/pi/someotherfilename` <-- This copies the source file `/tmp/somefilename` to the destination file `/home/pi/someotherfilename` ignoring the previously defined `srcpath` and `dstpath`.
* Example 4: `cpfile:somefilename;/home/pi/someotherfilename` <-- This copies the source file `somefilename` to the destination file `/home/pi/someotherfilename`. The source file is relative to the path defined by `srcpath` while `dstpath` is ignored.

### mvfile:SOURCEFILE;DESTINATIONFILE
Moves / renames a file from 'SOURCEFILE' to 'DESTINATIONFILE'. Both file paths must be non-relative. If 'DESTINATIONFILE' is left out it will automatically be set to the same as 'SOURCEFILE' with a 4-digit serial number added. This is useful for creating backups of existing files.

WARNING! If 'DESTINATIONFILE' is set to an already existing file IT WILL BE OVERWRITTEN with the contents of 'SOURCEFILE'!

* Example 1: `mvfile:/media/%USER%/USBPEN/occurity/test.file`
* Example 2: `mvfile:%HOME%/occurity/charts.xml;%HOME/occurity/charts.xml.old`

### rmfile:FILEPATH
Removes a single file. A non-relative file path is required.

* Example 1: `rmpath:%HOME%/occurity/config.ini`
* Example 2: `rmpath:/media/%USER%/USBPEN/occurity/config.ini`

### cppath:SOURCEPATH;DESTINATIONPATH
Copies a path, including subdirectories, from `sourcepath` to `destinationpath`. If `destinationpath` is left out it will copy the path using the path name from `sourcepath`. Both are relative to the corresponding source and destination directiories set with `srcpath` and `dstpath` (documented elsewhere in this document) unless the path name starts with a `/`.

* Example 1: `cppath:optotypes` <-- This copies the source path `optotypes` from the path defined by `srcpath` to a destination path of the same name located at the path defined by `dstpath`.
* Example 2: `cppath:optotypes;mypath` <-- This copies the source path `optotypes` from the path defined by `srcpath` to a destination path called `mypath` located at the path defined by `dstpath`.
* Example 3: `cppath:/tmp/somepath;/home/pi/someotherpath` <-- This copies the source path `/tmp/somepath` to the destination path `/home/pi/someotherpath` ignoring the previously defined `srcpath` and `dstpath`.
* Example 4: `cppath:somepath;/home/pi/someotherpath` <-- This copies the source path `somepath` to the destination path `/home/pi/someotherpath`. The source path is relative to the path defined by `srcpath` while `dstpath` is ignored.

### mvpath:SOURCEPATH;DESTINATIONPATH
Moves / renames a path from 'SOURCEPATH' to 'DESTINATIONPATH'. Both paths must be non-relative. If 'DESTINATIONPATH' is left out it will automatically be set to the same as 'SOURCEPATH' with a 4-digit serial number added. This is useful for creating backups of existing paths.

WARNING! If 'DESTINATIONPATH' is set to an already existing path IT WILL BE DELETED AND OVERWRITTEN with the contents of 'SOURCEPATH'!

* Example 1: `mvpath:/media/%USER%/USBPEN/occurity/test.path`
* Example 2: `mvpath:%HOME%/occurity/charts.xml;%HOME/occurity/charts.xml.old`

### rmpath:PATH[;ask]
Recursively removes an entire non-relative path. If `;ask` is added after the path, it will ask before deleting each subdirectory.

WARNING! This command can be dangerous to use! It removes ENTIRE directory trees, including all files and subdirectories. So BEWARE! Consider running it with `pretend:true` (documented elsewhere in this document) to test it.

* Example 1: `rmpath:%HOME%/occurity/optotypes`
* Example 2: `rmpath:/tmp/tempdir`

### aptinstall:PACKAGE1;PACKAGE2;...
In some cases it can be necessary to install further packages using the `sudo apt-get install` command. This command allows just that. Be aware that in order for this command to work, you need to do the following.

#### /etc/sudoers.d/pi
To allow the use of the `apt*` commands from the Occurity updater scripting language (updater activated with `j` on the keyboard), it is necessary to allow the `pi` (or whichever user will run Occurity) user to manipulate software packages without having to enter a password. This can be achieved by creating `/etc/sudoers.d/pi` and inserting the following:
```
pi ALL = NOPASSWD : /usr/bin/apt-get

```
* Example 1: `aptinstall:qtmultimedia5-dev;libqt5multimedia5-plugins`

Note! `sudo apt-get update` will automatically be run before installing any new packages.

### aptremove:PACKAGE1;PACKAGE2;...
In some cases it can be necessary to remove packages using the `sudo apt-get remove` command. This command allows just that. Be aware that in order for this command to work, you need to do the following.

#### /etc/sudoers.d/pi
To allow the use of the `apt*` commands from the Occurity updater scripting language (updater activated with `j` on the keyboard), it is necessary to allow the `pi` (or whichever user will run Occurity) user to manipulate software packages without having to enter a password. This can be achieved by creating `/etc/sudoers.d/pi` and inserting the following:
```
pi ALL = NOPASSWD : /usr/bin/apt-get

```
* Example 1: `aptremove:gstreamer1.0-plugins-bad`

Note! `sudo apt-get update` will automatically be run before removing any packages.

### reboot:force|ask
Reboots the computer. `force` will restart without asking the user. `ask` will allow the user to cancel the reboot through a dialog.

* Example 1: `reboot:force`
* Example 2: `reboot:ask`

### shutdown:force|ask
Shuts down the computer. `force` will shutdown without asking the user. `ask` will allow the user to cancel the shutdown through a dialog.

* Example 1: `shutdown:force`
* Example 2: `shutdown:ask`

### exit:MESSAGE
Exits the running job with MESSAGE. This is useful in test-cases where you want a job to run until a specific command and then exit without having to comment all remaining commands.

* Example 1: `exit:Jobs done!`
