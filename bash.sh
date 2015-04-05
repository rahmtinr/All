make main
if [ $? -ne 0 ]
then
    echo "Compilation error!"
    exit 0
fi

make AmazonAnalysis
if [ $? -ne 0 ]
then
    echo "Compilation error!"
    exit 0
fi
input_directory=$1
InputFiles=("Software")
#InputFiles=("Software" "Music" "Books" "Movies_and_TV")
#InputFiles=("Books" "Movies_and_TV")
TimeMode=("RealTime")
BurstMode=("MaxBenefit")
StateMachine=("DocRatio")
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
#            rm $output_directory* -rf
#            mkdir $output_directory"BurstPlots"
#            mkdir $output_directory"AggregationPlots"
#           ./main $input $burst_mode $time_mode $StateMachine
           echo "Done With finding the innovations"
           ./amazonAnalysis $input $burst_mode $time_mode $StateMachine
#            time_line_txt=$output_directory$x"_timeline.txt"
#            awk '{print $1 }' $time_line_txt | sort | uniq > $output_directory$x"/words"
#            Rscript "RScripts/word_timeline_plot.R" $time_line_txt $x
#            mv *.jpg $output_directory"BurstPlots"
#            Rscript "RScripts/innovators_distribution.R" $output_directory "../Output_All/"$x"_bursts/" $x
#            Rscript "RScripts/innovators_cdf.R" $output_directory $x
#            if [[ "$time_mode" == "RealTime" ]]
#            then
#                Rscript "RScripts/usage_after_innovation_pdf.R" $output_directory $x
#            fi
#            Rscript "RScripts/repeated_innovator.R" $output_directory $x
#            Rscript "RScripts/baseline_boxplot.R" $output_directory $x
#            Rscript "RScripts/xp_showing_up_on_timeline.R" $output_directory $x
            echo "________________________________"
        done
    done
#    Rscript "RScripts/compare_timing.R" "../Output_All/"$x"_bursts/" $x
done
