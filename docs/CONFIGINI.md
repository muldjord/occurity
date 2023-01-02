# config.ini
You can configure several options of Occurity to fit your needs. The first time Occurity is started the Preferences dialog will appear on screen. Remember to set everything up appropriately. To change these settings later, either press the `p` key on the keyboard (pincode required, see below) and change the values using the arrow keys and enter to close, or simply edit the `config.ini` file in any editor.

## [General]
* `chartsXml="charts.xml"`: Allows you to override the default xml file that defines the charts. Default is `charts.xml`. The format of `charts.xml` is documented [here](docs/CHARTSXML.md).
* `physDistance=310`: The distance from the monitor to the patient in centimeters. This MUST be set correctly in order for Occurity to show the optotypes at their correct sizes.
* `rulerWidth=125`: The physical width of the ruler shown in the Preferences dialog in millimeters. This MUST be set correctly in order for Occurity to show the optotypes at their correct sizes.
* `redValue=210`: Sets the red color value of SVG charts. To make this work, the initial red color of the SVG elements has to be `#d20000`.
* `greenValue=210`: Sets the green color value of SVG charts. To make this work, the initial green color of the SVG elements has to be `#00d200`.
* `sizeResetTime=240`: After this many seconds of inactivity, the charts will reset back to the startsize as defined in `charts.xml`.
* `hibernateTime=140`: After this many minutes of inactivity, the monitor will turn off to avoid burn-in. You can turn it back on by pressing `q`.
* `rowSkipDelta=4`: The number of lines skipped when pressing `PgUp` and `PgDn` on optotype charts.
* `pinCode=4242`: Sets the pincode to be entered in order to unlock the Preferences and Job Runner dialogs. It can be any length as long as it only contains numbers. Default is `4242`.
* `enableVideoPlayer=true`: Enables or disables the integrated attention video player.
* `touchControls=true`: Enables touch controls. Controls are configured on a per-chart basis as documented [here](CHARTSXML.md) .
* `leftHandedOperator=true`: Moves all touch controls to the right side of the screen.

## [folders]
* `optotypes="your/optotypes/folder"`: Defines the optotypes folder to load chart optotypes from. Read more about the customizable optotype format [here](OPTOTYPES.md). Default is `./optotypes`.
* `videos="your/videos/folder"`: Defines the attention videos folder. Only supports `.mp4` video files. Default is `./videos`. Recommended codec is h.264.
* `jobs="your/jobs/folder"`: Defines the jobs folder. All `.job` files within this folder will be accessible through the Job Runner dialog (open with `j`). Default is `./jobs`.

## [network] (Optional)
The following variables are used when determining if a network connection exists when running the `aptinstall` and `aptremove` job commands. Occurity DOES NOT require a network connection for anything other than this. If the host can't be contacted, a dialog box will appear which asks whether the `apt` command is critical for the job to function properly. If not, it will continue executing the job. Otherwise it will end the job. No Occurity jobs make use of these commands by default.
* `host="www.somesite.org"`
* `port=80`
