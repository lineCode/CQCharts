proc objPressed { view plot id } {
  echo "$view $plot $id"

  set inds [get_charts_data -plot $plot -object $id -name inds]

  echo "$inds"
}

set model [load_model -tsv data/scatter.tsv -first_line_header]

set plot1 [create_plot -model $model -type distribution \
  -columns "value=sepalLength,group=species" \
  -properties "xaxis.userLabel=Sepal Length,yaxis.userLabel=Count"]

connect_chart -plot $plot1 -from objIdPressed -to objPressed