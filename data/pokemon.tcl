set model [load_model -csv data/Pokemon.csv -comment_header]

# get column numbers
set type1Column      [get_charts_data -model $model -name column_index -data {Type 1}]
set type2Column      [get_charts_data -model $model -name column_index -data {Type 2}]
set legendaryColumn  [get_charts_data -model $model -name column_index -data Legendary]
set totalColumn      [get_charts_data -model $model -name column_index -data Total]
set generationColumn [get_charts_data -model $model -name column_index -data Generation]

# create new Type 2 row with empty rows set to Type 1 value
set type2Column1 [process_model -model $model -add -expr "column($type2Column)" -header "Type 2a"]

set rows [process_model -model $model -query -expr "column($type2Column1) == {}"]

foreach row $rows {
  set type1 [get_charts_data -model $model -row $row -column 2 -name value]

  set_charts_data -model $model -row $row -column $type2Column1 -name value -value $type1
}

# create new legendary column as boolean
#set legendaryExpr "column($legendaryColumn) == {True}"

set legendaryColumn1 [process_model -model $model -add -expr "column($legendaryColumn) == {True}" -header "Legendary(bool)"]

# use box plot
if {0} {
set plot1 [create_plot -type boxplot -columns "group=$legendaryColumn1,value=$totalColumn"]

set_property -plot $plot1 -name "xaxis.label.text" -value "is Legendary"
set_property -plot $plot1 -name "yaxis.label.text" -value "Strength"
}

# create new column as combination of Type 1 and new Type 2
set type12Expr "column($type1Column) == column($type2Column1) ? column($type1Column) : concat(column($type1Column),{ - },column($type2Column1))"

set type12Column [process_model -model $model -add -expr $type12Expr -header "Type Combination"]

sort_model -model $model -column $totalColumn

filter_model -model $model -expr "column($legendaryColumn1) == 1"

set plot2 [create_plot -type distribution -columns "value=$type12Column,color=$generationColumn" -bool "horizontal=1"]

set_property -plot $plot2 -name "color.mapped" -value 1
