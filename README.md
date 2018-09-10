# CQCharts

CQCharts is a Qt based charting library to support the display of an interactive
chart from a 'QAbstractItemModel' data model with support for cross selection using
a 'QItemSelectionModel' selection model.

Charts are live in that changes to the data model automatically update
the chart which allows the programmer to build interactive charts where
the user can select items from the model using a table/tree view and/or
the displayed chart data.

The library comes with a test program which supports scripting of charts using the
'tcl' programming language. A number of example plot scripts are available in the
data directory.

Chart types supported 
  + Adjacency
    + tabular connectivity table (needs special column value syntax for connections)
    ![adjacency](adjacency.png "Adjacency Plot")
  + BarChart
    + 1d bar chart for column values
    ![barchart](barchart.png "Bar Chart")
  + Box
    + Box plot of aggregated values from value and group columns
    ![boxplot](boxplot.png "Box Plot")
    ![connected_boxplot](boxplot_connected.png "Connected Box Plot")
  + Bubble
    + Bubble plot where circle is size of column value
    ![bubble](bubbleplot.png "Bubble Plot")
  + Chord
    + Circlular connectivity chart (needs special column value syntax for connections)
    ![chord](chordplot.png "Chord Plot")
  + Delaunay
    + Delaunay/Voronoi geometric connectivity chart
    ![delaunay](delaunay.png "Delaunay Plot")
  + Distribution
    + Distribution count of range based column values
    ![distribution](distribution.png "Distribution Plot")
  + ForceDirected
    + Force directed connectivity chart
    ![forcedirected](forcedirected.png "Force Directed Plot")
  + Geometry
    + General polygon geometry colored by value
    ![geometry](geometryplot.png "Geometry Plot")
  + HierBubble
    + Hierarchical bubble plot where circle is size of column value
    ![hierbubble](hierbubble.png "Hierarchical Bubble Plot")
  + Image
    + Image/regular grid plot (x, y, color)
    ![image](imageplot.png "Image Plot")
  + Parallel
    + Parallel lines plot using multiple connected value sets
    ![parallel](parallelplot.png "Parallel Plot")
  + Pie
    + Pie chart
    ![piechart](piechart.png "Pie Chart")
  + Radar
    + Radar (polygon pie chart)
    ![radar](radar.png "Radar Plot")
  + Sankey
    + Sankey plot of connected values
    ![sankey](sankey.png "Sankey Plot")
  + Scatter
    + Scatter plot of disparate x/y column values
    ![scatterplot](scatterplot.png "Scatter Plot")
  + Sunburst
    + Hierarchical pie chart plot
    ![sunburst](sunburst.png "Sunburst Plot")
  + TreeMap
    + Hierarchical tree map
    ![treemap](treemap.png "Treemap Plot")
  + XY
    + x/y monotonic value plot
    ![xychart](xychart.png "XY Chart")

## Adjacency

Uses table cell coloring to show number of connections between two items.

Items names are displayed in the row and column headers and the number of connections
is used to color the cell (row/column intersection).

Cell colors are derived from the blend of the colors for each item's group
and the color intensity is calculated from the relative value.

The following values can be customized:
 + table background color
 + empty cell color
 + margin
 + cell border color, alpha and corner size
 + header text color and font

Input data model can contain either node data (name, id, group) and a list of
connections (id, count) [old format] or a '/' separated connection data
(from name/to name, value, count).

## BarChart

1d bar chart for column value or values.

Bars can be grouped using a group/category column.

Custom bar colors and data labels can be provided in additional columns.

Bars can be stacked next to each other or on top of each other and can be
drawn vertically (default) or horizontally.

## Box

Box plot of aggregated values from value and group columns.

Input data is a set of y values for an associated common x value.

Data can also be additional grouped by an extra column.

Values can be displayed as a candlestick bar or connected into a single
solid bar of the value range.

## Bubble

Bubble plot where circle is size of column value.

Circles are packed in minimum enclosing circle.

## Chord

Circlular connectivity chart.

Input data model can contain either node data (name, id, group) and a list of
connections (id, count) (old format) or '/' separated connection data
(from name/to name, value, count).

## Delaunay

Delaunay/Voronoi geometric connectivity chart

## Distribution
Distribution count of range based values

## ForceDirected
Force directed connectivity chart

## Geometry
General polygon geometry colored by value

## HierBubble

Hierarchical bubble plot where circle is size of column value.

Hierarchical circles are packed in minimum enclosing circle.

## Image
Image plot (x, y, color)

## Parallel
Parallel lines plots multiple overlaid value sets

## Pie
Pie chart

## Radar
Radar (polygon pie chart)

## Sankey
Sankey plot of connected values

## Scatter
Scatter plot of disparate values

## Sunburst
Hierarchical pie chart plot

## TreeMap
Hierarchical tree map

## XY
Plot of x,y values. x values should be monotonic.

Multiple y columns can be supplied to produce multi line plot.

Binariate plot using two y columns.

Display of connecing lines and individual points can be customized.
