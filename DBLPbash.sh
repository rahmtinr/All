make DBLPFindInnovation
if [ $? -ne 0 ]
then
    echo "Compilation error!"
    exit 0
fi
echo "________________________________________"
make DBLPAnalysis
if [ $? -ne 0 ]
then
    echo "Compilation error!"
    exit 0
fi

input_directory="./DBLPparser/"
InputFiles=("DBLP")
TimeMode=("RealTime")
BurstMode=("MaxBenefit")
StateMachine=("DocRatio")
#StateMachine=("Appearance")
#"Longest" "All")
for x in ${InputFiles[*]}; do
    input=$input_directory$x.txt;
    echo $input
    echo $input_directory
    for y in ${TimeMode[*]}; do
        time_mode=$y;
        for z in ${BurstMode[*]}; do
            burst_mode=$z;
            echo $burst_mode
            echo $time_mode
            output_directory="../Output_All/DBLP/Bursts/"$StateMachine"/"
#            rm $output_directory* -rf
#            mkdir $output_directory"BurstPlots"
#            mkdir $output_directory"AggregationPlots"
#           ./dblpFindInnovation $input $burst_mode $StateMachine #always real time
           echo "Done with finding innovations, Starting Analysis ..." 
          ./dblpAnalysis $input $burst_mode $StateMachine 2>temp #always real time
           time_line_txt=$output_directory$x"_timeline.txt"
           awk '{print $1 }' $time_line_txt | sort | uniq > $output_directory"words"
#           Rscript "RScripts/innovation_correlation_exp.R" $output_directory$x"_innovation_final_exp.txt"
#           Rscript "RScripts/innovation_correlation_exp.R" $output_directory$x"_innovation_current_exp.txt"
#            Rscript "RScripts/word_timeline_plot.R" $time_line_txt "DBLP"
#            mv *.jpg $output_directory"BurstPlots"
#            Rscript "RScripts/innovators_distribution.R" $output_directory $output_directory "DBLP"
#            Rscript "RScripts/innovators_cdf.R" $output_directory "DBLP"
#            if [[ "$time_mode" == "RealTime" ]]
#            then
#                Rscript "RScripts/usage_after_innovation_pdf.R" $output_directory "DBLP"
#            fi
#            Rscript "RScripts/repeated_innovator.R" $output_directory "DBLP"
#           Rscript "RScripts/baseline_boxplot.R" $output_directory "DBLP"
#           Rscript "RScripts/xp_showing_up_on_timeline.R" $output_directory "DBLP"
            echo "________________________________"
        done
    done
#    Rscript "RScripts/compare_timing.R" $output_directory "DBLP"
done
