
draw_relative_year_comparison <- function(dataset, final, coeff) {
    t = read.table(paste("/home/rahmtin/workspace/Output_All/", dataset, "/Bursts/DocRatio/", 
                dataset, "_", final, "_relative_year_usage_bucketed_median_comparison_500_innovations_coeff", coeff, ".txt", sep=""), header=TRUE)
    t$Start_week = t$Start_week - 100
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
    draw_relative_year_comparison("DBLP", "final", "3.0")
    draw_relative_year_comparison("DBLP", "current", "3.0")
}

draw_all()


