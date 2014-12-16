make main
if [ $? -ne 0 ]
then
    echo "Compilation error!"
    exit 0
fi
input_directory=$1
InputFiles=("Software")
TimeMode=("RealTime" "ReviewTime")
BurstMode=("MaxBenefit")
#"Longest" "All")
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
           rm $output_directory* -rf
           mkdir $output_directory"BurstPlots"
           mkdir $output_directory"AggregationPlots"
           ./main $input $burst_mode $time_mode
            time_line_txt=$output_directory"timeline.txt"
            awk '{print $1 }' $time_line_txt | sort | uniq > $output_directory"/words"
            Rscript "R Scripts/word_timeline_plot.R" $time_line_txt
            mv *.jpg $output_directory"BurstPlots"
            Rscript "R Scripts/innovators_distribution.R" $output_directory "../Output_All/"$x"_bursts/"
            Rscript "R Scripts/innovators_cdf.R" $output_directory
            if [[ "$time_mode" == "RealTime" ]]
            then
                Rscript "R Scripts/usage_after_innovation_pdf.R" $output_directory
            fi
            Rscript "R Scripts/repeated_innovator.R" $output_directory
            Rscript "R Scripts/baseline_boxplot.R" $output_directory
            echo "________________________________"
        done
        Rscript "R Scripts/compare_timing.R" "../Output_All/"$x"_bursts/"
    done
done
