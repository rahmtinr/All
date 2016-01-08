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

make minLife
if [ $? -ne 0 ]
then
    echo "Compilation error!"
    exit 0
fi

#make empty2009
if [ $? -ne 0 ]
then
    echo "Compilation error!"
    exit 0
fi

#rm largest_bucket.txt
#touch largest_bucket.txt
input_directory=$1

#######InputFiles=("reddit_funny" "reddit_AskReddit" "reddit_pics" "reddit_AdviceAnimals" "reddit_gaming" "reddit_videos" "reddit_politics" "reddit_trees" "reddit_WTF" "reddit_aww" "reddit_fffffffuuuuuuuuuuuu" "reddit_Music" "reddit_POLITIC" "reddit_worldnews" "reddit_leagueoflegends" "reddit_technology" "Music" "Books" "Movies_and_TV" "Electronics" "Beer_Advocate" "Rate_Beer")

#InputFiles=("Books" "Movies_and_TV" "Music" "reddit_books" "reddit_movies" "reddit_Music" "reddit_funny" "reddit_gaming" "reddit_politics" "reddit_leagueoflegends" "Rate_Beer" "Beer_Advocate")

#InputFiles=("reddit_posts_books" "reddit_posts_movies" "reddit_posts_Music" "reddit_posts_funny" "reddit_posts_gaming" "reddit_posts_politics" "reddit_posts_leagueoflegends")
InputFiles=("Amazon_Music" "Beer_Advocate" "Amazon_Books" "reddit_gaming") # "Beer_AdvocateWithBrand")  #"ElectronicsWithBrand"  "Movies_and_TVWithBrand" "MusicWithBrand")
TimeMode=("RealTime")
BurstMode=("MaxBenefit")
StateMachine=("DocRatio")
Denominators=(4)
ExperienceState=("current")
for x in ${InputFiles[*]}; do
    input=$input_directory$x.txt;
    if [[ $x == "reddit_funny" ]]
    then
        StateMachineCoeff=("6.0")
    fi

    if [[ $x == "reddit_movies" ]]
    then
        StateMachineCoeff=("6.3")
    fi

    if [[ $x == "reddit_books" ]]
    then
        StateMachineCoeff=("6.5")
    fi

    if [[ $x == "reddit_gaming" ]]
    then
        StateMachineCoeff=("7.0")
    fi
    if [[ $x == "reddit_politics" ]]
    then
        StateMachineCoeff=("9.0")
    fi
    if [[ $x == "reddit_Music" ]]
    then
        StateMachineCoeff=("4.5")
    fi
    if [[ $x == "reddit_leagueoflegends" ]]
    then
        StateMachineCoeff=("9.0")
    fi
    if [[ $x == "reddit_posts_funny" ]]
    then
        StateMachineCoeff=("6.0")
    fi

    if [[ $x == "reddit_posts_movies" ]]
    then
        StateMachineCoeff=("6.3")
    fi

    if [[ $x == "reddit_posts_books" ]]
    then
        StateMachineCoeff=("6.5")
    fi

    if [[ $x == "reddit_posts_gaming" ]]
    then
        StateMachineCoeff=("7.0")
    fi
    if [[ $x == "reddit_posts_politics" ]]
    then
        StateMachineCoeff=("9.0")
    fi
    if [[ $x == "reddit_posts_Music" ]]
    then
        StateMachineCoeff=("4.5")
    fi
    if [[ $x == "reddit_posts_leagueoflegends" ]]
    then
        StateMachineCoeff=("9.0")
    fi
    if [[ $x == "reddit_worldnews" ]]
    then
        StateMachineCoeff=("6.0" "7.5" "9.0")
    fi
    if [[ $x == "Music" ]]
    then
        StateMachineCoeff=("5.0")
    fi
    if [[ $x == "Movies_and_TV" ]]
    then
        StateMachineCoeff=("12.0")
    fi
    if [[ $x == "Books" ]]
    then
        StateMachineCoeff=("9.0")
    fi
     if [[ $x == "Electronics" ]]
    then
        StateMachineCoeff=("6.0")
    fi
    if [[ $x == "Beer_Advocate" ]]
    then
        StateMachineCoeff=("4.2")
    fi
    if [[ $x == "Rate_Beer" ]]
    then
        StateMachineCoeff=("6.0")
    fi 

    if [[ $x == "ElectronicsWithBrand" ]]
    then
        StateMachineCoeff=("6.0")
    fi
    
    if [[ $x == "MusicWithBrand" ]]
    then
        StateMachineCoeff=("5.0")
    fi
    if [[ $x == "Movies_and_TVWithBrand" ]]
    then
        StateMachineCoeff=("12.0")
    fi
    if [[ $x == "Beer_AdvocateWithBrand" ]]
    then
        StateMachineCoeff=("4.2")
    fi
    if [[ $x == "Rate_BeerWithBrand" ]]
    then
        StateMachineCoeff=("6.0")
    fi 
     if [[ $x == "reddit_worldnewsWithBrand" ]]
    then
        StateMachineCoeff=("7.5")
    fi
    if [[ $x == "BooksWithBrand" ]]
    then
        StateMachineCoeff=("9.0")
    fi

#    StateMachineCoeff+=("0.05")
#    StateMachineCoeff=("4.2" "4.5" "6.0" "9.0" "12.0")
#    StateMachineCoeff=("2.5" "3" "4")
#    StateMachineCoeff=("5.0")
    echo $input_directory
    for y in ${TimeMode[*]}; do
        time_mode=$y;
        for z in ${BurstMode[*]}; do
            burst_mode=$z;
            for t in ${StateMachineCoeff[*]}; do
                state_machine_coeff=$t;
                echo $burst_mode
                echo $time_mode
                echo $state_machine_coeff
                output_directory="../Output_All/"$x"_bursts/"$time_mode"/"$burst_mode"/"
#   ####            rm $output_directory* -rf
#               mkdir $output_directory"BurstPlots"
#               mkdir $output_directory"AggregationPlots"
#               ./main $input $burst_mode $time_mode $StateMachine $state_machine_coeff Bigram Normal
#               ./empty2009 $input $burst_mode $time_mode $StateMachine $state_machine_coeff Bigram
                echo "Done With finding the innovations"
#                ./minLife $input $burst_mode $time_mode $StateMachine $state_machine_coeff final 4 Bigram
                for q in ${ExperienceState[*]}; do
                    experience_state=$q;
                    for p in ${Denominators[*]}; do
                        denominator=$p
                       ./amazonAnalysis $input $burst_mode $time_mode $StateMachine $state_machine_coeff $experience_state $denominator Unigram Normal
                    done
                done
#               time_line_txt=$output_directory$x"_timeline.txt"
#               awk '{print $1 }' $time_line_txt | sort | uniq > $output_directory$x"/words"
#               Rscript "RScripts/word_timeline_plot.R" $time_line_txt $x
#               mv *.jpg $output_directory"BurstPlots"
#               Rscript "RScripts/innovators_distribution.R" $output_directory "../Output_All/"$x"_bursts/" $x
#               Rscript "RScripts/innovators_cdf.R" $output_directory $x
#               if [[ "$time_mode" == "RealTime" ]]
#               then
#                  Rscript "RScripts/usage_after_innovation_pdf.R" $output_directory $x
#               fi
#               Rscript "RScripts/repeated_innovator.R" $output_directory $x
#               Rscript "RScripts/baseline_boxplot.R" $output_directory $x
#               Rscript "RScripts/xp_showing_up_on_timeline.R" $output_directory $x
                echo "________________________________"
            done
        done
    done
#    Rscript "RScripts/compare_timing.R" "../Output_All/"$x"_bursts/" $x
done


