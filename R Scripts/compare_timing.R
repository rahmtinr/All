library(ggplot2)
directory = commandArgs(TRUE)[1]
real_time_distribution = read.table(paste(directory,"RealTime/MaxBenefit/innovation_timing.txt", sep=""), header=FALSE)
review_time_distribution = read.table(paste(directory,"ReviewTime/MaxBenefit/innovation_timing.txt", sep=""), header=FALSE)
colnames(real_time_distribution) = c("Time" , "Freq" )
colnames(review_time_distribution) = c("Time" , "Freq" )
real_time_distribution$time = "Real"
review_time_distribution$time = "Review"
total = rbind(real_time_distribution, review_time_distribution) 


png(paste(directory, "innovation_timing.png", sep=""),height=800,width=1300)
ggplot(data = total, aes(x = Time, y = Freq, fill = time)) + 
    geom_bar(stat="identity", position=position_dodge(), colour="black")
dev.off()

