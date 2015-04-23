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
InputFiles=("Software" "Music" "Books" "Movies_and_TV" "Electronics" "Beer_Advocate" "Rate_Beer")
#InputFiles=("Beer_Advocate" "Rate_Beer")
TimeMode=("RealTime")
BurstMode=("MaxBenefit")
StateMachine=("DocRatio")
#StateMachineCoeff=("3" "4.5" "5.3" "6" "12")
#StateMachineCoeff=("3.0" "3.1" "3.2" "3.3" "3.4" "3.5" "3.6" "3.7" "3.8" "3.9" "4.0" "4.1" "4.2" "4.3" "4.4" "4.5")
ExperienceState=("final" "current")
for x in ${InputFiles[*]}; do
    input=$input_directory$x.txt;
    if (($x == "Music")) 
    then
        StateMachineCoeff=("3.5")
    fi
    if (($x == "Movies_and_TV")) 
    then
        StateMachineCoeff=("3.3" "4.2")
    fi
    if (($x == "Books")) 
    then
        StateMachineCoeff=("3.5" "4.0")
    fi
    if (($x == "Rate_Beer")) 
    then
        StateMachineCoeff=("3.4" "3.5")
    fi
    if (($x == "Beer_Advocate")) 
    then
        StateMachineCoeff=("3.7")
    fi
    if (($x == "Electronics")) 
    then
        StateMachineCoeff=("4.2" "4.1")
    fi
    echo $input_directory
    for y in ${TimeMode[*]}; do
        time_mode=$y;
        for z in ${BurstMode[*]}; do
            burst_mode=$z;
            for t in ${StateMachineCoeff[*]}; do
                state_machine_coeff=$t;
#               for q in ${ExperienceState[*]}; do
#                    experience_state=$q;
                    echo $burst_mode
                    echo $time_mode
                    echo $state_machine_coeff
                    output_directory="../Output_All/"$x"_bursts/"$time_mode"/"$burst_mode"/"
#                      rm $output_directory* -rf
#                      mkdir $output_directory"BurstPlots"
#                      mkdir $output_directory"AggregationPlots"
#                      ./main $input $burst_mode $time_mode $StateMachine $state_machine_coeff
                       echo "Done With finding the innovations"
                       ./amazonAnalysis $input $burst_mode $time_mode $StateMachine $state_machine_coeff $experience_state
#                      time_line_txt=$output_directory$x"_timeline.txt"
#                      awk '{print $1 }' $time_line_txt | sort | uniq > $output_directory$x"/words"
#                      Rscript "RScripts/word_timeline_plot.R" $time_line_txt $x
#                      mv *.jpg $output_directory"BurstPlots"
#                      Rscript "RScripts/innovators_distribution.R" $output_directory "../Output_All/"$x"_bursts/" $x
#                      Rscript "RScripts/innovators_cdf.R" $output_directory $x
#                      if [[ "$time_mode" == "RealTime" ]]
#                      then
#                              Rscript "RScripts/usage_after_innovation_pdf.R" $output_directory $x
#                      fi
#                      Rscript "RScripts/repeated_innovator.R" $output_directory $x
#                      Rscript "RScripts/baseline_boxplot.R" $output_directory $x
#                      Rscript "RScripts/xp_showing_up_on_timeline.R" $output_directory $x
                       echo "________________________________"
#                done
            done
        done
    done
#    Rscript "RScripts/compare_timing.R" "../Output_All/"$x"_bursts/" $x
done
