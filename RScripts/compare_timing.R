library(ggplot2)
directory = commandArgs(TRUE)[1]
dataset = commandArgs(TRUE)[2]
real_time_distribution = read.table(paste(directory,"RealTime/MaxBenefit/",dataset,"_innovation_timing.txt", sep=""), header=FALSE)
review_time_distribution = read.table(paste(directory,"ReviewTime/MaxBenefit/",dataset,"_innovation_timing.txt", sep=""), header=FALSE)
colnames(real_time_distribution) = c("Time" , "Freq" )
colnames(review_time_distribution) = c("Time" , "Freq" )
real_time_distribution$time = "Real"
review_time_distribution$time = "Review"
total = rbind(real_time_distribution, review_time_distribution) 


png(paste(directory, dataset, "_innovation_timing.png", sep=""),height=800,width=1300)
ggplot(data = total, aes(x = floor(Time), y = Freq, fill = time)) + 
    geom_bar(stat="identity", position=position_dodge()) + scale_x_continuous(breaks=floor(total$Time))
dev.off()

