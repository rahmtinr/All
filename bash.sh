#make main
if [ $? -ne 0 ]
then
    echo "Compilation error!"
    exit 0
fi
input_directory=$1
InputFiles=("Software")
#, "Electronics.txt")
TimeMode=("RealTime" "ReviewTime")
BurstMode=("MaxBenefit" "Longest" "All")
for x in ${InputFiles[*]}; do
    input=$input_directory$x.txt;
    echo $input_directory
    for y in ${TimeMode[*]}; do
        time_mode=$y;
        for z in ${BurstMode[*]}; do
            burst_mode=$z;
            echo $burst_mode
            echo $time_mode
            output_directory="../Output_All/"$x"_bursts/"$time_mode"/"$burst_mode"/"
            rm $output_directory*
            ./main $input $burst_mode $time_mode
            time_line_txt=$output_directory"timeline.txt"
            awk '{print $1 }' $time_line_txt | sort | uniq > $output_directory"/words"
            Rscript "R Scripts/word_timeline_plot.R" $time_line_txt
            mv *.jpg $output_directory
            Rscript "R Scripts/innovators_distribution.R" $output_directory "../Output_All/"$x"_bursts/"
            echo "________________________________"
        done
    done
done
