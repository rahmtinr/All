
draw_relative_year_comparison <- function(dataset, final, coeff, bucket_size) {
    t = read.table(paste("~/Documents/Amazon/Output_All/", dataset, "_bursts/RealTime/MaxBenefit/", 
                dataset, "_", final, "_relative_year_usage_bucketed_", bucket_size ,"_median_comparison_500_innovations_coeff", coeff, ".txt", sep=""), header=TRUE)
    t = t[-1, ]
    t$Start_week = t$Start_week - 1100
    name = paste("~/Pictures/today/", dataset, "_", final, "_relative_year_usage_bucketed_", bucket_size,"_median_comparison_innovation_coeff", coeff, ".png"  ,sep="")
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
    bucket_sizes = c("30", "50", "100", "200", "500")
    for(i in 1:5) {
        print(bucket_sizes[i])
        draw_relative_year_comparison("Music", "final", "3.5", bucket_sizes[i])
        draw_relative_year_comparison("Music", "current", "3.5", bucket_sizes[i])

        draw_relative_year_comparison("Movies_and_TV", "final", "3.3", bucket_sizes[i])
        draw_relative_year_comparison("Movies_and_TV", "final", "4.2", bucket_sizes[i])
        draw_relative_year_comparison("Movies_and_TV", "current", "3.3", bucket_sizes[i])
        draw_relative_year_comparison("Movies_and_TV", "current", "4.2", bucket_sizes[i])
    
        draw_relative_year_comparison("Books", "final", "3.5", bucket_sizes[i])
        draw_relative_year_comparison("Books", "final", "4.0", bucket_sizes[i])
        draw_relative_year_comparison("Books", "current", "3.5", bucket_sizes[i])
        draw_relative_year_comparison("Books", "current", "4.0", bucket_sizes[i])
    
        draw_relative_year_comparison("Beer_Advocate", "final", "3.7", bucket_sizes[i])
        draw_relative_year_comparison("Beer_Advocate", "current", "3.7", bucket_sizes[i])


        draw_relative_year_comparison("Rate_Beer", "final", "3.4", bucket_sizes[i])
        draw_relative_year_comparison("Rate_Beer", "final", "3.5", bucket_sizes[i])
        draw_relative_year_comparison("Rate_Beer", "current", "3.4", bucket_sizes[i])
        draw_relative_year_comparison("Rate_Beer", "current", "3.5", bucket_sizes[i])

        draw_relative_year_comparison("Electronics", "final", "4.2", bucket_sizes[i])
        draw_relative_year_comparison("Electronics", "final", "4.1", bucket_sizes[i])
        draw_relative_year_comparison("Electronics", "current", "4.2", bucket_sizes[i])
        draw_relative_year_comparison("Electronics", "current", "4.1", bucket_sizes[i])
    }
}

draw_all()


