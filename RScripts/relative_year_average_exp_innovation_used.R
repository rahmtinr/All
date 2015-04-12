
draw_relative_year <- function(dataset, final, coeff) {
    t = read.table(paste("/home/rahmtin/Documents/Amazon/Output_All/", dataset, "_bursts/RealTime/MaxBenefit/", 
                dataset, "_", final, "_relative_year_usage_top500_innovations_coeff", coeff, ".txt", sep=""))
    colnames(t) = c("Week", "AverageExp", "NumOfInnovations")
    t$AverageExp = t$AverageExp / max(t$AverageExp)
    t$NumOfInnovations = t$NumOfInnovations / max(t$NumOfInnovations)
    name = paste("~/Pictures/", dataset, "_", final, "_relative_year_usage_innovation_coeff", coeff, ".png"  ,sep="")
    png(name)
    plot(t$Week, t$AverageExp, type ="l", col = "black", xlab = "relative week to burst", ylab="fraction of Y")
    title(main=paste(dataset , final, "relative year with coeff", coeff , sep=" "))
    lines(t$Week, t$NumOfInnovations, col = "blue")
    legend("topleft", col = c("black", "blue"),
        legend = c("Aggregated average experience normalized, top 500", "Aggreagated # innovation used normalized, top 500"),  
            lty = c(1,1,1), cex = 0.75)
    dev.off()
}

draw_all <- function() {
    draw_relative_year("Music", "final", "3")
    draw_relative_year("Music", "final", "6")
    draw_relative_year("Music", "final", "12")
    draw_relative_year("Music", "final", "24")
    draw_relative_year("Music", "current", "3")
    draw_relative_year("Music", "current", "6")
    draw_relative_year("Music", "current", "12")
    draw_relative_year("Music", "current", "24")


    draw_relative_year("Movies_and_TV", "final", "3")
    draw_relative_year("Movies_and_TV", "final", "6")
    draw_relative_year("Movies_and_TV", "final", "12")
    draw_relative_year("Movies_and_TV", "final", "24")
    draw_relative_year("Movies_and_TV", "current", "3")
    draw_relative_year("Movies_and_TV", "current", "6")
    draw_relative_year("Movies_and_TV", "current", "12")
    draw_relative_year("Movies_and_TV", "current", "24")


    draw_relative_year("Books", "final", "3")
    draw_relative_year("Books", "final", "6")
    draw_relative_year("Books", "final", "12")
    draw_relative_year("Books", "final", "24")
    draw_relative_year("Books", "current", "3")
    draw_relative_year("Books", "current", "6")
    draw_relative_year("Books", "current", "12")
    draw_relative_year("Books", "current", "24")
}

draw_all()
