# The Preferences dialog
Press `p` on the keyboard to open it. A pin-code is required. Default is `4242`. Use arrow keys to configure the available options. Checkbox options and options in comboboxes are changed using the left / right arrow keys. Options are saved as they are changed. Press `Enter` to close the dialog.

## Settings
* `Physical length of ruler`: The physical width of the blue ruler shown in the dialog in millimeters. This MUST be set correctly in order for Occurity to show the optotypes at their correct sizes.
* `Patient distance to monitor`: The distance from the monitor to the patient in centimeters. This MUST be set correctly in order for Occurity to show the optotypes at their correct sizes.
* `Starting chart`: The initial chart that is shown when Occurity starts.
* `Idle time before size reset`: After this many minutes of inactivity the charts will reset back to the `startsize` as defined in [charts.xml](CHARTSXML.md).
* `Idle time before putting the monitor to sleep`: After this many minutes of inactivity the monitor will be put to sleep. Wake it back up by pressing `q` or any other key.
* `Red color value`: Sets the red color value of SVG charts. To make this work the initial red color of the SVG elements must be `#d20000`.
* `Green color value`: Sets the green color value of SVG charts. To make this work the initial green color of the SVG elements must be `#00d200`.
* `Skip this many lines when using multiline row skipping`: The number of lines skipped when pressing `PgUp` and `PgDn` on optotype charts.
* `Use row captions for optotype charts`: Enable optotype chart row captions as defined per row in [charts.xml](CHARTSXML.md). By default the captions are set to show the 20 ft notation captions related to the decimal row sizes.
* `Show mouse cursor`: When enabled the mouse cursor will be shown. Otherwise it is hidden. The mouse can be used to click the touch controls.
* `Show touch controls`: Enables the on-screen touch controls. Changing this setting requires a restart of Occurity.
* `Left handed operator`: Moves the touch controls to the opposite side of the screen.
