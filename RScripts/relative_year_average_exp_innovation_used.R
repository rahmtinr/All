# THIS FILESHOULD BE DONE FOR EVERYTHING TOO

#t1000 = read.table("/home/rahmtin/workspace/Output_All/Software_bursts/RealTime/MaxBenefit/Software_final_relative_year_usage_all_exp_top1000_innovations.txt")
#colnames(t1000) = c("Week", "AverageExp", "NumOfInnovations")
#t1000$AverageExp = t1000$AverageExp / max(t1000$AverageExp)
#t1000$NumOfInnovations = t1000$NumOfInnovations / max(t1000$NumOfInnovations)
#plot(t1000$Week, t1000$AverageExp, type ="l", col = "black", xlab = "relative year to burst", ylab="fraction of Y")
#lines(t1000$Week, t1000$NumOfInnovations, col = "blue")

draw_relative_year <- function(dataset, final, coeff) {
    t = read.table(paste("/home/rahmtin/Documents/Output_All/", dataset, "_bursts/RealTime/MaxBenefit/", dataset, "_final_relative_year_usage_all_exp_top500_innovations_coeff.txt", sep=""))
    colnames(t) = c("Week", "AverageExp", "NumOfInnovations")
    t$AverageExp = t$AverageExp / max(t$AverageExp)
    t$NumOfInnovations = t$NumOfInnovations / max(t$NumOfInnovations)
    png()
    plot(t$Week, t$AverageExp, type ="l", col = "black", xlab = "relative year to burst", ylab="fraction of Y")
    lines(t$Week, t$NumOfInnovations, col = "blue")
    legend("topleft", col = c("red", "green"),
        legend = c("Aggregated average experience normalized, top 500", "Aggreagated # innovation used normalized, top 500"),  
            lty = c(1,1,1), cex = 0.75)
}
