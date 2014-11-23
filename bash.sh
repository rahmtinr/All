make main
if [ $? -ne 0 ]
then
    echo "Compilation error!"
    exit 0
fi
input_directory="/media/rahmtin/1E3655063654E077/cygwin64/home/rahmtin/research/SecondYear/Amazon/All/inputs/"
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
            ./main $input $burst_mode $time_mode
            rm $output_directory"words"
            time_line_txt=$output_directory"timeline.txt"
            awk '{print $1 }' $time_line_txt | sort | unique $output_directory"/words"
            Rscript "R Scripts/word_timeline_plot.R" $time_line_txt
            mv *.jpg $output_directory
            echo "________________________________"
        done
    done
done
