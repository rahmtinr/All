t1000 = read.table("/home/rahmtin/workspace/Output_All/Software_bursts/RealTime/MaxBenefit/Software_final_relative_year_usage_all_exp_top1000_innovations.txt")
colnames(t1000) = c("Week", "AverageExp", "NumOfInnovations")
t1000$AverageExp = t1000$AverageExp / max(t1000$AverageExp)
t1000$NumOfInnovations = t1000$NumOfInnovations / max(t1000$NumOfInnovations)
plot(t1000$Week, t1000$AverageExp, type ="l", col = "black", xlab = "relative year to burst", ylab="fraction of Y")
lines(t1000$Week, t1000$NumOfInnovations, col = "blue")


t500 = read.table("/home/rahmtin/workspace/Output_All/Software_bursts/RealTime/MaxBenefit/Software_final_relative_year_usage_all_exp_top500_innovations.txt")
colnames(t500) = c("Week", "AverageExp", "NumOfInnovations")
t500$AverageExp = t500$AverageExp / max(t500$AverageExp)
t500$NumOfInnovations = t500$NumOfInnovations / max(t500$NumOfInnovations)
lines(t500$Week, t500$AverageExp, type ="l", col = "red")
lines(t500$Week, t500$NumOfInnovations, col = "green")

t2000 = read.table("/home/rahmtin/workspace/Output_All/Software_bursts/RealTime/MaxBenefit/Software_final_relative_year_usage_all_exp_top2000_innovations.txt")
colnames(t2000) = c("Week", "AverageExp", "NumOfInnovations")
t2000$AverageExp = t2000$AverageExp / max(t2000$AverageExp)
t2000$NumOfInnovations = t2000$NumOfInnovations / max(t2000$NumOfInnovations)
lines(t2000$Week, t2000$AverageExp, type ="l", col = "yellow")
lines(t2000$Week, t2000$NumOfInnovations, col = "cyan")

legend("topleft", col = c("black", "blue", "red", "green", "yellow", "cyan"),
        legend = c("Aggregated average experience normalized, top 1000", "Aggreagated # innovation used normalized, top 1000",
            "Aggregated average experience normalized, top 500", "Aggreagated # innovation used normalized, top 500",
            "Aggregated average experience normalized, top 2000", "Aggreagated # innovation used normalized, top 2000"),  
            lty = c(1,1,1), cex = 0.75)

