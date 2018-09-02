# Y1Y2 Plots

set model1 [load_model -tsv data/multi_series.tsv -comment_header -column_type "time:format=%Y%m%d"]

set plot1 [create_plot -model $model1 -type xy -columns "x=0,y=1" -title "multiple y axis"]
set plot2 [create_plot -model $model1 -type xy -columns "x=0,y=2"]

group_plots -overlay -y1y2 [list $plot1 $plot2]

#set model2 [load_model -csv data/pareto.csv -comment_header]

#set plot3 [create_plot -model $model2 -type barchart -y1y2]
#set plot4 [create_plot -model $model2 -type xy -y1y2 -xmin -0.5 -xmax 5.5 -ymin 2.0 -title pareto]

#set_property -plot $plot4 -name options.cumulative -value 1
#set_property -plot $plot4 -name xaxis.integral -value 1