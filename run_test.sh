#!/bin/bash

data_output_dir=run_out
data_input_dir=data
exec=structured_stack
thread_count=( 1 2 4 8)

files=`ls $data_input_dir`
files=${files#"$data_input_dir"}
# files=input_example

rm -rf $data_output_dir
mkdir $data_output_dir

# for i in ${!thread_count[@]}
# do
#     mkdir $data_output_dir/$i
    for input in $files
    do
        echo ${thread_count[$i]} $data_input_dir/$input $data_output_dir/$input
        # touch $data_output_dir/$i/$input
        time ./$exec $data_input_dir/$input > $data_output_dir/$input
        cat $data_output_dir/$input | grep , > $data_output_dir/$input\_guesses.csv
    done
# done
