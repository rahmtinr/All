
draw_relative_year_comparison <- function(dataset, final, coeff, bucket_size) {
    if(coeff == "0.05"){
        return(NULL)
    }
    print(paste(dataset, coeff,  final, bucket_size, sep=" "))
#    t = read.table(paste("~/Documents/Amazon/Output_All/", dataset, "_bursts/RealTime/MaxBenefit/", 
#                dataset, "_mlf_bigrams_", final, "_relative_year_usage_bucketed_", bucket_size ,"_median_comparison_500_innovations_coeff", coeff, ".txt", sep=""), header=TRUE)

    t = read.table(paste("~/Documents/Amazon/Output_All/", dataset, "_bursts/RealTime/MaxBenefit/", 
                dataset, "_bigrams_empty2009_", final, "_relative_year_usage_bucketed_", bucket_size ,"_median_comparison_500_innovations_coeff", coeff, ".txt", sep=""), header=TRUE)
    t = t[-1, ]
    print("Hello")
    if(nrow(t) == 0) {
        print("WTF")
        return(NULL)
    }
    print("HOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOORAY")
    t$Start_week = t$Start_week - 1100
#   name = paste("~/Pictures/today/", dataset, "_min_life_span_bigrams_", final, "_relative_year_usage_bucketed_", bucket_size,"_median_comparison_innovation_coeff", coeff, ".png"  ,sep="")
    name = paste("~/Pictures/today/", dataset, "_bigrams_empty2009_", final, "_relative_year_usage_bucketed_", bucket_size,"_median_comparison_innovation_coeff", coeff, ".png"  ,sep="")
    png(name)
    plot(t$Start_week, t$Fraction, type ="o", col = "black", ylim=c(0,1), xlab = "relative week to burst", ylab="Fraction of all reviews below median of the bucket")
    abline(h = 0.5, untf = FALSE, col="red")
    abline(v = 0, untf = FALSE, col="red")
    title(main=paste(dataset , " ", final, " rel year, bucketed(", bucket_size, ") median comparison, coeff ", coeff , sep=""))
    legend("topleft", col = c("black"),
        legend = c("fraction of all data below the median experience of each bucket"),  
            lty = c(1,1,1), cex = 0.75)
    dev.off()
}

draw_all <- function() {
       bucket_sizes = c("100", "500", "1000", "2000", "5000", "10000", "20000")
    for(i in 1:7) {
        print(bucket_sizes[i])
        draw_relative_year_comparison("Music", "final", "0.05", bucket_sizes[i])
        draw_relative_year_comparison("Music", "final", "3.8", bucket_sizes[i])
        draw_relative_year_comparison("Music", "current", "0.05", bucket_sizes[i])
        draw_relative_year_comparison("Music", "current", "3.8", bucket_sizes[i])
        print("1")

        draw_relative_year_comparison("Movies_and_TV", "final", "0.05", bucket_sizes[i])
        draw_relative_year_comparison("Movies_and_TV", "final", "9.0", bucket_sizes[i])
        draw_relative_year_comparison("Movies_and_TV", "final", "12", bucket_sizes[i])
        draw_relative_year_comparison("Movies_and_TV", "current", "0.05", bucket_sizes[i])
        draw_relative_year_comparison("Movies_and_TV", "current", "9.0", bucket_sizes[i])
        draw_relative_year_comparison("Movies_and_TV", "current", "12", bucket_sizes[i])
        print("2")

        draw_relative_year_comparison("Books", "final", "0.05", bucket_sizes[i])
        draw_relative_year_comparison("Books", "final", "9.0", bucket_sizes[i])
        draw_relative_year_comparison("Books", "final", "12", bucket_sizes[i])
        draw_relative_year_comparison("Books", "current", "0.05", bucket_sizes[i])
        draw_relative_year_comparison("Books", "current", "9.0", bucket_sizes[i])
        draw_relative_year_comparison("Books", "current", "12", bucket_sizes[i])
        print("3")

        draw_relative_year_comparison("Electronics", "final", "0.05", bucket_sizes[i])
        draw_relative_year_comparison("Electronics", "final", "12", bucket_sizes[i])
        draw_relative_year_comparison("Electronics", "current", "0.05", bucket_sizes[i])
        draw_relative_year_comparison("Electronics", "current", "12", bucket_sizes[i])
        print("4")

        draw_relative_year_comparison("Beer_Advocate", "final", "0.05", bucket_sizes[i])
        draw_relative_year_comparison("Beer_Advocate", "final", "4.2", bucket_sizes[i])
        draw_relative_year_comparison("Beer_Advocate", "final", "12", bucket_sizes[i])
        draw_relative_year_comparison("Beer_Advocate", "current", "0.05", bucket_sizes[i])
        draw_relative_year_comparison("Beer_Advocate", "current", "4.2", bucket_sizes[i])
        draw_relative_year_comparison("Beer_Advocate", "current", "12", bucket_sizes[i])
        print("5")

        draw_relative_year_comparison("Rate_Beer", "final", "0.05", bucket_sizes[i])
        draw_relative_year_comparison("Rate_Beer", "final", "6.0", bucket_sizes[i])
        draw_relative_year_comparison("Rate_Beer", "final", "12", bucket_sizes[i])
        draw_relative_year_comparison("Rate_Beer", "current", "0.05", bucket_sizes[i])
        draw_relative_year_comparison("Rate_Beer", "current", "6.0", bucket_sizes[i])
        draw_relative_year_comparison("Rate_Beer", "current", "12", bucket_sizes[i])
        print("6")

        draw_relative_year_comparison("reddit_funny", "final", "0.05", bucket_sizes[i])
        draw_relative_year_comparison("reddit_funny", "final", "3.6", bucket_sizes[i])
        draw_relative_year_comparison("reddit_funny", "final", "4.5", bucket_sizes[i])
        draw_relative_year_comparison("reddit_funny", "current", "0.05", bucket_sizes[i])
        draw_relative_year_comparison("reddit_funny", "current", "3.6", bucket_sizes[i])
        draw_relative_year_comparison("reddit_funny", "current", "4.5", bucket_sizes[i])
        print("7")

        draw_relative_year_comparison("reddit_AskReddit", "final", "0.05", bucket_sizes[i])
        draw_relative_year_comparison("reddit_AskReddit", "final", "9.0", bucket_sizes[i])
        draw_relative_year_comparison("reddit_AskReddit", "final", "12", bucket_sizes[i])
        draw_relative_year_comparison("reddit_AskReddit", "current", "0.05", bucket_sizes[i])
        draw_relative_year_comparison("reddit_AskReddit", "current", "9.0", bucket_sizes[i])
        draw_relative_year_comparison("reddit_AskReddit", "current", "12", bucket_sizes[i])
        print("8")
        
        draw_relative_year_comparison("reddit_pics", "final", "0.05", bucket_sizes[i])
        draw_relative_year_comparison("reddit_pics", "final", "2.6", bucket_sizes[i])
        draw_relative_year_comparison("reddit_pics", "final", "4.5", bucket_sizes[i])
        draw_relative_year_comparison("reddit_pics", "current", "0.05", bucket_sizes[i])
        draw_relative_year_comparison("reddit_pics", "current", "2.6", bucket_sizes[i])
        draw_relative_year_comparison("reddit_pics", "current", "4.5", bucket_sizes[i])
        print("9")

        draw_relative_year_comparison("reddit_AdviceAnimals", "final", "0.05", bucket_sizes[i])
        draw_relative_year_comparison("reddit_AdviceAnimals", "final", "4.5", bucket_sizes[i])
        draw_relative_year_comparison("reddit_AdviceAnimals", "final", "6.0", bucket_sizes[i])
        draw_relative_year_comparison("reddit_AdviceAnimals", "current", "0.05", bucket_sizes[i])
        draw_relative_year_comparison("reddit_AdviceAnimals", "current", "4.5", bucket_sizes[i])
        draw_relative_year_comparison("reddit_AdviceAnimals", "current", "6.0", bucket_sizes[i])
        print("10")
        
        draw_relative_year_comparison("reddit_gaming", "final", "0.05", bucket_sizes[i])
        draw_relative_year_comparison("reddit_gaming", "final", "4.5", bucket_sizes[i])
        draw_relative_year_comparison("reddit_gaming", "final", "6.0", bucket_sizes[i])
        draw_relative_year_comparison("reddit_gaming", "current", "0.05", bucket_sizes[i])
        draw_relative_year_comparison("reddit_gaming", "current", "4.5", bucket_sizes[i])
        draw_relative_year_comparison("reddit_gaming", "current", "6.0", bucket_sizes[i])
        print("11")

        draw_relative_year_comparison("reddit_videos", "final", "0.05", bucket_sizes[i])
        draw_relative_year_comparison("reddit_videos", "final", "3.6", bucket_sizes[i])
        draw_relative_year_comparison("reddit_videos", "current", "0.05", bucket_sizes[i])
        draw_relative_year_comparison("reddit_videos", "current", "3.6", bucket_sizes[i])
        print("12")

        draw_relative_year_comparison("reddit_politics", "final", "0.05", bucket_sizes[i])
        draw_relative_year_comparison("reddit_politics", "final", "2.6", bucket_sizes[i])
        draw_relative_year_comparison("reddit_politics", "final", "9.0", bucket_sizes[i])
        draw_relative_year_comparison("reddit_politics", "current", "0.05", bucket_sizes[i])
        draw_relative_year_comparison("reddit_politics", "current", "2.6", bucket_sizes[i])
        draw_relative_year_comparison("reddit_politics", "current", "9.0", bucket_sizes[i])
        print("13")
        
        draw_relative_year_comparison("reddit_trees", "final", "0.05", bucket_sizes[i])
        draw_relative_year_comparison("reddit_trees", "final", "4.5", bucket_sizes[i])
        draw_relative_year_comparison("reddit_trees", "final", "6.0", bucket_sizes[i])
        draw_relative_year_comparison("reddit_trees", "current", "0.05", bucket_sizes[i])
        draw_relative_year_comparison("reddit_trees", "current", "4.5", bucket_sizes[i])
        draw_relative_year_comparison("reddit_trees", "current", "6.0", bucket_sizes[i])
        print("14")

        draw_relative_year_comparison("reddit_WTF", "final", "0.05", bucket_sizes[i])
        draw_relative_year_comparison("reddit_WTF", "final", "6.0", bucket_sizes[i])
        draw_relative_year_comparison("reddit_WTF", "current", "0.05", bucket_sizes[i])
        draw_relative_year_comparison("reddit_WTF", "current", "6.0", bucket_sizes[i])
        print("15")
        
        draw_relative_year_comparison("reddit_aww", "final", "0.05", bucket_sizes[i])
        print("---1")
        draw_relative_year_comparison("reddit_aww", "final", "4.0", bucket_sizes[i])
        print("---2")
        draw_relative_year_comparison("reddit_aww", "current", "0.05", bucket_sizes[i])
        print("---3")
        draw_relative_year_comparison("reddit_aww", "current", "4.0", bucket_sizes[i])
        print("16")

        draw_relative_year_comparison("reddit_fffffffuuuuuuuuuuuu", "final", "0.05", bucket_sizes[i])
        draw_relative_year_comparison("reddit_fffffffuuuuuuuuuuuu", "final", "3.4", bucket_sizes[i])
        draw_relative_year_comparison("reddit_fffffffuuuuuuuuuuuu", "current", "0.05", bucket_sizes[i])
        draw_relative_year_comparison("reddit_fffffffuuuuuuuuuuuu", "current", "3.4", bucket_sizes[i])
        print("17")

        draw_relative_year_comparison("reddit_Music", "final", "0.05", bucket_sizes[i])
        draw_relative_year_comparison("reddit_Music", "final", "3.8", bucket_sizes[i])
        draw_relative_year_comparison("reddit_Music", "current", "0.05", bucket_sizes[i])
        draw_relative_year_comparison("reddit_Music", "current", "3.8", bucket_sizes[i])
        print("18")
        
        draw_relative_year_comparison("reddit_worldnews", "final", "0.05", bucket_sizes[i])
        draw_relative_year_comparison("reddit_worldnews", "final", "6.0", bucket_sizes[i])
        draw_relative_year_comparison("reddit_worldnews", "current", "0.05", bucket_sizes[i])
        draw_relative_year_comparison("reddit_worldnews", "current", "6.0", bucket_sizes[i])
        print("19")
        
        draw_relative_year_comparison("reddit_leagueoflegends", "final", "0.05", bucket_sizes[i])
        draw_relative_year_comparison("reddit_leagueoflegends", "final", "4.0", bucket_sizes[i])
        draw_relative_year_comparison("reddit_leagueoflegends", "final", "9.0", bucket_sizes[i])
        draw_relative_year_comparison("reddit_leagueoflegends", "current", "0.05", bucket_sizes[i])
        draw_relative_year_comparison("reddit_leagueoflegends", "current", "4.0", bucket_sizes[i])
        draw_relative_year_comparison("reddit_leagueoflegends", "current", "9.0", bucket_sizes[i])
        print("20")

        draw_relative_year_comparison("reddit_technology", "final", "0.05", bucket_sizes[i])
        draw_relative_year_comparison("reddit_technology", "final", "12", bucket_sizes[i])
        draw_relative_year_comparison("reddit_technology", "current", "0.05", bucket_sizes[i])
        draw_relative_year_comparison("reddit_technology", "current", "12", bucket_sizes[i])
    }
print("done")
    draw_relative_year_comparison("Music", "final", "3.8", "1710000" )
    draw_relative_year_comparison("Music", "current", "3.8","1290000" )

    draw_relative_year_comparison("Movies_and_TV", "final", "9.0", "270000")
    draw_relative_year_comparison("Movies_and_TV", "final", "12", "210000")
    draw_relative_year_comparison("Movies_and_TV", "current", "9.0", "240000")
    draw_relative_year_comparison("Movies_and_TV", "current", "12", "150000")

    draw_relative_year_comparison("Books", "final", "9.0", "150000")
    draw_relative_year_comparison("Books", "final", "12", "90000")
    draw_relative_year_comparison("Books", "current", "9.0", "120000")
    draw_relative_year_comparison("Books", "current", "12", "90000")

    draw_relative_year_comparison("Electronics", "final", "12", "30000" )
    draw_relative_year_comparison("Electronics", "current", "12", "30000")

    draw_relative_year_comparison("Beer_Advocate", "final", "4.2", "810000")
    draw_relative_year_comparison("Beer_Advocate", "final", "12", "60000")
    draw_relative_year_comparison("Beer_Advocate", "current", "4.2", "750000" )
    draw_relative_year_comparison("Beer_Advocate", "current", "12", "60000")

    draw_relative_year_comparison("Rate_Beer", "final", "6.0", "180000")
    draw_relative_year_comparison("Rate_Beer", "final", "12", "60000")
    draw_relative_year_comparison("Rate_Beer", "current", "6.0", "180000")
    draw_relative_year_comparison("Rate_Beer", "current", "12", "60000")

    draw_relative_year_comparison("reddit_funny", "final", "3.6", "90000")
    draw_relative_year_comparison("reddit_funny", "final", "4.5", "60000")
    draw_relative_year_comparison("reddit_funny", "current", "3.6", "60000" )
    draw_relative_year_comparison("reddit_funny", "current", "4.5", "30000")

    draw_relative_year_comparison("reddit_AskReddit", "final", "9.0", "60000")
    draw_relative_year_comparison("reddit_AskReddit", "final", "12", "30000")
    draw_relative_year_comparison("reddit_AskReddit", "current", "9.0", "30000")
    draw_relative_year_comparison("reddit_AskReddit", "current", "12", "30000")
    
    draw_relative_year_comparison("reddit_pics", "final", "2.6", "90000")
    draw_relative_year_comparison("reddit_pics", "final", "4.5", "60000")
    draw_relative_year_comparison("reddit_pics", "current", "2.6", "60000")
    draw_relative_year_comparison("reddit_pics", "current", "4.5", "30000")

    draw_relative_year_comparison("reddit_AdviceAnimals", "final", "4.5",  "60000")
    draw_relative_year_comparison("reddit_AdviceAnimals", "final", "6.0", "30000")
    draw_relative_year_comparison("reddit_AdviceAnimals", "current", "4.5", "30000")
    draw_relative_year_comparison("reddit_AdviceAnimals", "current", "6.0", "30000" )
    
    draw_relative_year_comparison("reddit_gaming", "final", "4.5", "90000")
    draw_relative_year_comparison("reddit_gaming", "final", "6.0", "60000")
    draw_relative_year_comparison("reddit_gaming", "current", "4.5", "60000")
    draw_relative_year_comparison("reddit_gaming", "current", "6.0", "30000")

    draw_relative_year_comparison("reddit_videos", "final", "3.6", "30000")
    draw_relative_year_comparison("reddit_videos", "current", "3.6", "30000")

    draw_relative_year_comparison("reddit_politics", "final", "2.6", "120000")
    draw_relative_year_comparison("reddit_politics", "final", "9.0", "60000")
    draw_relative_year_comparison("reddit_politics", "current", "2.6", "120000")
    draw_relative_year_comparison("reddit_politics", "current", "9.0", "60000")
    
    draw_relative_year_comparison("reddit_trees", "final", "4.5", "150000")
    draw_relative_year_comparison("reddit_trees", "final", "6.0", "90000")
    draw_relative_year_comparison("reddit_trees", "current", "4.5", "90000")
    draw_relative_year_comparison("reddit_trees", "current", "6.0", "60000")

    draw_relative_year_comparison("reddit_WTF", "final", "6.0", "30000")
    draw_relative_year_comparison("reddit_WTF", "current", "6.0", "30000")
    
    draw_relative_year_comparison("reddit_aww", "final", "4.0", "30000")
    draw_relative_year_comparison("reddit_aww", "current", "4.0", "30000")

    draw_relative_year_comparison("reddit_fffffffuuuuuuuuuuuu", "final", "3.4", "30000")
    draw_relative_year_comparison("reddit_fffffffuuuuuuuuuuuu", "current", "3.4", "30000")

    draw_relative_year_comparison("reddit_Music", "final", "3.8", "30000")
    draw_relative_year_comparison("reddit_Music", "current", "3.8", "30000")
    
    draw_relative_year_comparison("reddit_worldnews", "final", "6.0", "30000")
    draw_relative_year_comparison("reddit_worldnews", "current", "6.0", "30000")
    
    draw_relative_year_comparison("reddit_leagueoflegends", "final", "4.0", "120000")
    draw_relative_year_comparison("reddit_leagueoflegends", "final", "9.0", "30000")
    draw_relative_year_comparison("reddit_leagueoflegends", "current", "4.0", "60000")
    draw_relative_year_comparison("reddit_leagueoflegends", "current", "9.0", "30000")

    draw_relative_year_comparison("reddit_technology", "final", "12", "30000")
    draw_relative_year_comparison("reddit_technology", "current", "12", "30000")
}

draw_all()


