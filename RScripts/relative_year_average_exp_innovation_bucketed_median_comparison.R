
draw_relative_year_comparison <- function(dataset, final, coeff) {
    t = read.table(paste("/home/rahmtin/Documents/Amazon/Output_All/", dataset, "_bursts/RealTime/MaxBenefit/", 
                dataset, "_", final, "_relative_year_usage_bucketed_median_comparison_500_innovations_coeff", coeff, ".txt", sep=""), header=TRUE)
    t$Start_week = t$Start_week - 1100
    name = paste("~/Pictures/today/", dataset, "_", final, "_relative_year_usage_bucketed_median_comparison_innovation_coeff", coeff, ".png"  ,sep="")
    png(name)
    plot(t$Start_week, t$Fraction, type ="o", col = "black", xlab = "relative week to burst", ylab="Fraction of all reviews below median of the bucket")
    title(main=paste(dataset , final, "rel year, bucketed with same innovation size(30), median comparison, coeff", coeff , sep=" "))
    legend("topleft", col = c("black"),
        legend = c("fraction of all data below median experience of each bucket, top 500"),  
            lty = c(1,1,1), cex = 0.75)
    dev.off()
}

draw_all <- function() {
    draw_relative_year_comparison("Music", "final", "3.5")
    draw_relative_year_comparison("Music", "current", "3.5")

    draw_relative_year_comparison("Movies_and_TV", "final", "3.3")
    draw_relative_year_comparison("Movies_and_TV", "final", "4.2")
    draw_relative_year_comparison("Movies_and_TV", "current", "3.3")
    draw_relative_year_comparison("Movies_and_TV", "current", "4.2")

    draw_relative_year_comparison("Books", "final", "3.5")
    draw_relative_year_comparison("Books", "final", "4.0")
    draw_relative_year_comparison("Books", "current", "3.5")
    draw_relative_year_comparison("Books", "current", "4.0")

    draw_relative_year_comparison("Beer_Advocate", "final", "3.7")
    draw_relative_year_comparison("Beer_Advocate", "current", "3.7")


    draw_relative_year_comparison("Rate_Beer", "final", "3.4")
    draw_relative_year_comparison("Rate_Beer", "final", "3.5")
    draw_relative_year_comparison("Rate_Beer", "current", "3.4")
    draw_relative_year_comparison("Rate_Beer", "current", "3.5")

    draw_relative_year_comparison("Electronics", "final", "4.2")
    draw_relative_year_comparison("Electronics", "final", "4.1")
    draw_relative_year_comparison("Electronics", "current", "4.2")
    draw_relative_year_comparison("Electronics", "current", "4.1")
}

draw_all()
