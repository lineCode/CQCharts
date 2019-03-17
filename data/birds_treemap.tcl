# TreeMap Plot

set model [load_charts_model -csv data/birds.csv -first_line_header]

set plot [create_charts_plot -model $model -type treemap -columns "names=10 12 13 0,value=5,color=9"]

set_charts_property -plot $plot -name color.map.enabled -value 1
