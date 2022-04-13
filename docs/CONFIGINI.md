## config.ini
You can configure several options of Occurity to fit your needs. The first time Occurity is started the preferences dialog will appear on screen. Remember to set everything up appropriately. To change these setting later, either press the `p` key on the keyboard (pincode required, see below) and change the values using the arrow keys, or simply edit the `config.ini` file in any editor.

### [General]
* `chartsXml="charts.xml"`: Allows you to override the default xml file that defines the charts. Default is `charts.xml`.
* `physDistance=310`: The distance from the monitor to the patient in centimeters. This MUST be set correctly in order for Occurity to show the optotypes at their correct sizes.
* `rulerWidth=125`: The physical width of the ruler shown in the preferences dialog in millimeters. This MUST be set correctly in order for Occurity to show the optotypes at their correct sizes.
* `redValue=210`: Sets the red color value or SVG charts. To make this work, the initial red color of the SVG elements has to be `#d20000`.
* `greenValue=210`: Sets the green color value or SVG charts. To make this work, the initial green color of the SVG elements has to be `#00d200`.
* `sizeResetTime=240`: After this many seconds of inactivity, the charts will reset back to the startsize as defined in `charts.xml`.
* `hibernateTime=140`: After this many minutes of inactivity, the monitor will turn off to avoid burn-in. You can turn it back on by pressing `q`.
* `rowSkipDelta=4`: The number of lines skipped when pressing `PgUp` and `PgDn`.
* `pinCode=4242`: Sets the pincode to be entered in order to unlock the Preferences and Updater dialogs. It can be any length as long as it only contains numbers. Default is `4242`.
* `enableVideoPlauer=true`: Enables or disables the integrated attention video player.

### [folders]
* `optotypes="your/optotypes/folder"`: Defines the optotypes folder to load chart optotypes from. Default is `./optotypes`.
* `videos="your/videos/folder"`: Defines the attention videos folder. Only supports mp4 video container format. Default is `./videos`.
* `jobs="your/jobs/folder"`: Defines the jobs folder. All `.job` files within this folder will be accessible through the Job Runner dialog (open with `j`). Default is `./jobs`.

### [network]
The following variables are used when determining if a network connection exists when running the `aptinstall` and `aptremove` job commands. Occurity DOES NOT require a network connection for anything else than this. If the host can't be contacted, a dialog box will appear which asks whether the `apt*` command is critical for the job to function properly. If not, it will continue executing the job. Otherwise it will end the job.
* `host="www.somesite.org"`
* `port=80`
