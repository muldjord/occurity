# charts.xml
All of the charts displayed in the Occurity software are customizable. Edit the `charts.xml` file in any basic text/xml editor. The format is as follows:

## 'xml' node
```
<?xml version="1.0" encoding="UTF-8"?>
```
This node tells the xml format. Don't change it.

## 'charts' node
```
<charts startingchart="CHARTNAME">
  ...
</charts>
```
This node is the parent node containing all charts.

## 'group' node
```
<group numkey="1">
  ...
</group>
```
This groups several charts together on the same number key. It supports the following attributes, some of which are required:
* `numkey="1"` (required): This is the number key assigned to charts in this group. Activating this number key on the keyboard, or a flirc supported remote control, will then switch between the charts contained in the group.

## 'chart' nodes
```
<chart name="Chart 1" type="optotype" optotype="sloan" bgcolor="white" sizelock="true">
  ...
</chart>
```
Must be nested in the `group` node. This defines a single chart to be used by the software. It supports the following attributes, some of which are required:
* `caption="Caption"`: The caption that is shown on-screen in Occurity.
* `type="optotype"` (required): The chart type. Currently supports `optotype` and `svg`.
* `bgcolor="white"`: Sets the background color of the chart (can be `black`, `white` or hex as `#AABBCC`).
* `sizelock="true"`: If this is set to true and you switch to another chart with the same attribute, it will, if possible, inherit the size of the previous chart, giving a consistent size between chart changes.

### 'optotype' chart type specific
* `optotype="sloan"` (required): Which optotype is used by this chart. This MUST correspond to the name of a subdirectory located in the `optotypes` subfolder. In this case `optotypes/sloan`.
* `startsize="0.1"`: Sets optotype size on initialization. Size must match with a size from a `row` below.
* `crowdingspan="2.5"`: Sets optotype crowding span in size relative arc minutes. If left out the default value of 2.5 is used.
* `animation="file.gif"`: Sets the attention GIF animation for this chart. Activate with `a`.
* `fadetimings="0;50;150;150"`: Defines the optotype fade timings. There are three relevant timings. 1: Used when fading in a symbol, 2: Used when fading out a symbol, 3 and 4: Used when momentarily showing a symbol (fade in / out). All values are in ms.
* `fadelevels="0.0;1.0;0.15"`: Defines the optotype fade levels. There are three relevant levels. 1: Opacity when hiding a symbol, 2: Opacity when showing a symbol, 3: Opacity when momentarily showing a symbol. All values can range from 0.0 to 1.0. Completely opaque is 1.0.

#### 'row' node
Must be nested inside a `chart` node. A single `row` node defines a row in the chart. It has the following format:
```
<row size="0.1">NCKZO</row>
```
or
```
<row size="0.1">one;two;three;four;five</row>
```
Size defines the size used by the row (required). This is defined as 0.1 being equal to 5 arc-minutes at a distance of 6 meters.

You can fill in the contents of a row in two different ways. Either by simply entering the letters that should be used. These letters MUST correspond to the filenames of the optotype specific subdirectory located in the `optotypes` subdirectory. In the first examples above the files must be called `N.svg`, `C.svg` and so on.

The second way is by using semicolons to separate the optotypes, which correspond to the filenames of the optotype specific subdirectory located in the `optotypes` subdirectory. In the second example this would require files to be named `one.svg`, `two.svg` and so on.

### 'svg' chart type specific
* `source="filename.svg"`: The filename containing the SVG you want to use.
* `scaling="distance"`: Can be `width`, `height` or `distance`. `distance` scales according to patient distance. The SVG must be scaled as 100 pixels being equal to 1 arc minute at a distance of 6 meters. `width` and `height` simply scales the SVG to fit the width or height of the screen.

#### 'layer' node
Must be nested inside a `chart` node. Defines a layer from inside an SVG to be displayed on the chart. It has the following format:
```
<layer id="layername"/>
```
You can add as many layer nodes as you'd like. Occurity can then switch between them with the left/right arrow keys. SVG's and their layers can be created with the open source software [Inkscape](https://inkscape.org/).
