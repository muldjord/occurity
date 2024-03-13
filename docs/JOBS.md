# The Job Runner dialog
In order to maintain and update Occurity files (videos, charts, optotypes and so on) an integrated Job Runner has been implemented. It is a powerful tool that can help perform any number of defined tasks.

All available jobs are located in the [jobs folder](CONFIGINI.md#folders). The default jobs folder is `./jobs`. If a USB pen with a FAT32 partition named `USBPEN` is inserted which includes job files located at `%USBPATH%/occurity/jobs` those jobs will always take priority over the jobs found at the configured jobs folder through, so keep that in mind.

To create a new job create a new `.job` file in the jobs folder. Use the available commands as documented below.

## Command format
A single line of the scripting language is made up of the command type, followed by a `:`, followed by the required arguments. If more arguments are allowed / required they are separated by `;`.

### Available commands

#### title:TITLE
Sets the title that will be used to select and execute this particular `.job` entry in the Occurity Job Runner dialog.

* Example 1: `title:This is a title`

#### category:CATEGORY
This defines the category applied to the job when adding it to the Job Runner dialog. More jobs can share the same category.

* Example 1: `category:Tools`
* Example 2: `category:Update jobs`

#### version:x.x.x (Optional)
If set, the `x.x.x` will be shown before the title when displaying the job entry in the Occurity Job Runner dialog. The format of the version is undefined. A `x.x.x` format is suggested, but it can just as well be `May 2022`.

* Example 1: `version:1.2.0`
* Example 2: `version:May 2022`

#### pretend:TRUE|FALSE
Setting this to true will ensure that no files or directories are manipulated while executing the script lines following this line. Default is `false`. Useful while testing / debugging a script.

Note! Setting `pretend:true` does come with caveats. Since all file operations are not actually carried out it will be impossible to copy a directory and then remove files from it afterwards since those files aren't actually there. This will result in an error.

* Example 1: `pretend:true`

#### message:MESSAGE
Add a message to the debug output of the Occurity Job Runner dialog. The message will be white and is useful for providing status / progress messages while processing a script.

* Example 1: `pretend:This is a message!`

#### setvar:VARIABLE;VALUE
Sets a variable for use in the `.job` script. All variables will have a `%` prepended and appended to it.

* Example 1: `setvar:MYVAR;My value`: This allows you to use `%MYVAR%` throughout the script command arguments / parameters where it will be replaced with `My value`.

#### loadvars:FILENAME
Loads variables from a file and makes them available to the script for use in command parameters.

* Example 1: `loadvars:myvars.txt`

##### myvars.txt
```
THIS=Something
THAT=Other
```
These two variables can now be used in the script command arguments / parameters using `%THIS%` and `%THAT%` which will be replaced with the corresponding text.

* Example 1: `cpfile:path1/%THIS%/somefile.dat` becomes `cpfile:path1/Something/somefile.dat`

##### Hardcoded variables
Some hardcoded variables are available. These can be used without setting them with `setvar` or `loadvars`.

* %HOME%: The current user home directory (eg. `/home/pi`).
* %WORKDIR%: The Occurity working directory (eg. `/home/pi/occurity`).
* %USER%: The user name of the currently logged in user (eg. `pi`).
* %ARCH%: The currently running CPU architecture (eg. `aarch64` or 'x86_64').
* %HOSTNAME%: The currently assigned hostname (with Yocto this is eg. `raspberrypi4-64` or 'raspberrypi3').
* %USBPATH%: When a USB pendrive is inserted this variable will be set to the root path of the drive but ONLY if 'USBPEN' is contained within the partition label.

#### addexclude:FILE|PATH
Adds either a FILE or PATH to the global exclude list. This list is used by several of the available file and path manipulation commands. If a match is found the file or path will be ignored by those commands. Any subdirectories of an excluded path will also be excluded.

Adding excludes is useful if you want to copy an entire path but want to leave out a subdirectory or file from within that path.
Same goes when removing a path. Excluded paths (including subdirectories) and / or files will remain after removal.

Note 1: `mvpath` does not adhere to the exclude list!

Note 2: Both relative and non-relative FILE and PATH are allowed. If relative it will match with files or paths located relative to the defined `srcpath`.

* Example 1: `addexclude:optotypes/sloan
* Example 2: `addexclude:optotypes/README.md
* Example 3: `addexclude:%WORKDIR%/README.md

#### setexec:FILENAME
Runs `chmod +x FILENAME` in order to make the file executable. This is important to apply to files moved or copied from a FAT32 usb pendrive, as they are often not executable. The main executable `Occurity` and all files from the `scripts` subfolder should all have this command run on them after an update to ensure Occurity works as expected. Otherwise Occurity might not start at all or the standby functions might not work as expected.

* Example 1: `setexec:%HOME%/occurity/Occurity`
* Example 2: `setexec:%HOME%/occurity/scripts/*.sh`

#### cpfile:SRCFILE;DSTFILE
Copies a file from `SRCFILE` to `DSTFILE`. If `DSTFILE` is left out it will copy the file using the filename from `SRCFILE`. Both are relative to the corresponding source and destination directiories set with `srcpath` and `dstpath` (documented elsewhere in this document) unless the file path starts with a `/`.

* Example 1: `cpfile:Occurity`: This copies the source file `Occurity` from the path defined by `srcpath` to a destination file of the same name located at the path defined by `dstpath`.
* Example 2: `cpfile:Occurity;OtherName`: This copies the source file `Occurity` from the path defined by `srcpath` to a destination file called `OtherName` located at the path defined by `dstpath`.
* Example 3: `cpfile:/tmp/somefilename;/home/pi/someotherfilename`: This copies the source file `/tmp/somefilename` to the destination file `/home/pi/someotherfilename` ignoring the previously defined `srcpath` and `dstpath`.
* Example 4: `cpfile:somefilename;/home/pi/someotherfilename`: This copies the source file `somefilename` to the destination file `/home/pi/someotherfilename`. The source file is relative to the path defined by `srcpath` while `dstpath` is ignored.

#### mvfile:SRCFILE[;DSTFILE]
Moves `SRCFILE` to `DSTFILE`. If `DSTFILE` is left out `SRCFILE` will be used with a 4-digit autoincremented serial added (eg `SRCFILE0000`). If `DSTFILE` already exists a 4-digit autoincremented serial will be added (eg. `DSTFILE0000`).

Note! This command DOES NOT adhere to files added using `addexclude`!

* Example 1: `mvfile:/media/%USER%/USBPEN/occurity/test.file`: Moves to `/media/%USER%/USBPEN/occurity/test.file0000`
* Example 2: `mvfile:%HOME%/occurity/charts.xml;%HOME/occurity/charts.xml.old`: If `%HOME/occurity/charts.xml.old` already exists it will move to `%HOME/occurity/charts.xml.old0000`

#### rmfile:FILEPATH[;ask]
Removes a single file. A non-relative file path is required. If `;ask` is added after the path, it will ask before deleting the file.

Note! Files added using `addexclude` will NOT be removed!

* Example 1: `rmfile:%HOME%/occurity/config.ini;ask`
* Example 2: `rmfile:%USBPATH%/occurity/config.ini`

#### cppath:SRCPATH;DSTPATH
Copies a path, including subdirectories, from `sourcepath` to `destinationpath`. If `destinationpath` is left out it will copy the path using the path name from `sourcepath`. Both are relative to the corresponding source and destination directiories set with `srcpath` and `dstpath` (documented elsewhere in this document) unless the path name starts with a `/`.

Note! Paths added using `addexclude` will NOT be copied!

* Example 1: `cppath:optotypes`: This copies the source path `optotypes` from the path defined by `srcpath` to a destination path of the same name located at the path defined by `dstpath`.
* Example 2: `cppath:optotypes;mypath`: This copies the source path `optotypes` from the path defined by `srcpath` to a destination path called `mypath` located at the path defined by `dstpath`.
* Example 3: `cppath:/tmp/somepath;/home/pi/someotherpath`: This copies the source path `/tmp/somepath` to the destination path `/home/pi/someotherpath` ignoring the previously defined `srcpath` and `dstpath`.
* Example 4: `cppath:somepath;/home/pi/someotherpath`: This copies the source path `somepath` to the destination path `/home/pi/someotherpath`. The source path is relative to the path defined by `srcpath` while `dstpath` is ignored.

#### mvpath:SRCPATH[;DSTPATH]
Moves `SRCPATH` to `DSTPATH`. If `DSTPATH` is left out `SRCPATH` will be used with a 4-digit autoincremented serial added (eg `SRCPATH0000`). If `DSTPATH` already exists a 4-digit autoincremented serial will be added (eg. `DSTPATH0000`).

Note! This command DOES NOT adhere to paths added using `addexclude`!

* Example 1: `mvpath:/media/%USER%/USBPEN/occurity/testpath`: Moves to `/media/%USER%/USBPEN/occurity/testpath0000`
* Example 2: `mvpath:%HOME%/occurity/testpath;%HOME/occurity/testpathnew`: If `%HOME/occurity/testpathnew` already exists it will move to `%HOME/occurity/testpathnew0000`

#### rmpath:PATH[;ask]
Recursively removes an entire non-relative path. If `;ask` is added after the path, it will ask before deleting each subdirectory.

Note! Paths added using `addexclude` will NOT be removed!

WARNING! This command can be dangerous to use! It removes ENTIRE directory trees, including all files and subdirectories. So BEWARE! Consider running it with `pretend:true` (documented elsewhere in this document) to test it.

* Example 1: `rmpath:%HOME%/occurity/optotypes`
* Example 2: `rmpath:/tmp/tempdir`

#### aptinstall:PACKAGE1;PACKAGE2;...
In some cases it can be necessary to install further packages using the `sudo apt-get install` command. This command allows just that. Be aware that in order for this command to work, you need to do the following.

To allow the use of the `apt` commands, it is necessary to let the user running Occurity run these commands without having to provide a password. Go [here](../README.md#etcsudoersdpi-optional) to read more.

Note! `sudo apt-get update` will automatically be run before installing any new packages.

* Example 1: `aptinstall:qtmultimedia5-dev;libqt5multimedia5-plugins`

#### aptremove:PACKAGE1;PACKAGE2;...
In some cases it can be necessary to remove packages using the `sudo apt-get remove` command. This command allows just that. Be aware that in order for this command to work, you need to do the following.

To allow the use of the `apt` commands, it is necessary to let the user running Occurity run these commands without having to provide a password. Go [here](../README.md#etcsudoersdpi-optional) to read more.

Note! `sudo apt-get update` will automatically be run before removing any packages.

* Example 1: `aptremove:gstreamer1.0-plugins-bad`

#### reboot:force|ask
Reboots the computer. `force` will restart without asking the user. `ask` will allow the user to cancel the reboot through a dialog.

* Example 1: `reboot:force`
* Example 2: `reboot:ask`

#### shutdown:force|ask
Shuts down the computer. `force` will shutdown without asking the user. `ask` will allow the user to cancel the shutdown through a dialog.

* Example 1: `shutdown:force`
* Example 2: `shutdown:ask`

#### sync:
Synchronizes any cached writes to persistent storage. This command is auto-added to the end of all jobs even when it is not in the job file.

* Example 1: `sync:`

#### exit:MESSAGE
Exits the running job with MESSAGE. This is useful while testing a job where you want a job to run until a specific command and then exit without having to comment all remaining commands.

* Example 1: `exit:Jobs done!`
