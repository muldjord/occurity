# Optotypes
Occurity can easily be configured to use custom optotypes. Each optotype should have its own subfolder created within the [configured optotype folder](CONFIGINI.md#folders).

This folder contains all optotypes available for use with the [optotype](CHARTSXML.md#optotype-chart-type-specific) visual acuity chart type in [charts.xml](CHARTSXML.md). To create a calibrated optotype you must ensure that the size of the optotype in each SVG file adheres to the rule of 100 pixels being equal to 1 arc minute at a 6 meter distance to the patient.

Furthermore, each optotype should include an SVG named `_.svg`. The width of this SVG defines the width between each optotype letter in a chart.

See the included optotypes for examples.

The optotypes included with Occurity are all created with the open source vector graphics software [Inkscape](https://inkscape.org).