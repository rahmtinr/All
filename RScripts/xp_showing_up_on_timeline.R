library(plyr)
library(ggplot2)
directory = commandArgs(TRUE)[1]
dataset = commandArgs(TRUE)[2]
xp_distribution = read.table(paste(directory, dataset, "_experiences_showing_up_on_timeline.txt", sep=""), header=FALSE)
colnames(xp_distribution) = c("word", "CurrentExp", "FinishingExp", "Time")


xp_distribution$CurrentBucket = floor(xp_distribution$CurrentExp / 20)
xp_distribution$FinishingBucket = floor(xp_distribution$FinishingExp / 20)

my_summary  = ddply(xp_distribution, c("CurrentBucket"), summarize, N = length(Time),  mean_time = mean(Time), sd = sd(Time), se = sd / sqrt(N))
my_summary[is.na(my_summary)] = 0
pd = position_dodge(.1)
jpeg(paste(directory,"AggregationPlots/", dataset, "_mean_appereance_of_present_xp.jpg",sep=""), height=800, width=1300)
ggplot(data = my_summary, aes(x = CurrentBucket, y=mean_time, colour = "blue")) + 
    geom_errorbar(aes(ymin=mean_time-se, ymax=mean_time+se), width=.1, position=pd) +
    geom_line(position=pd) +  ggtitle("Mean time vs Experience bucket of size 20") +
    scale_colour_hue(name="Legend", # Legend label, use darker colors
                     labels=c("Mean of current experience"),
                     l=40) +                  # Use darker colors, lightness=40
    geom_point(position=pd)
dev.off()

my_summary2  = ddply(xp_distribution, c("FinishingBucket"), summarize, N = length(Time),  mean_time = mean(Time), sd = sd(Time), se = sd / sqrt(N))
my_summary2[is.na(my_summary2)] = 0
jpeg(paste(directory,"AggregationPlots/", dataset, "_mean_appereance_of_final_xp.jpg",sep=""), height=800, width=1300)
ggplot(data = my_summary2, aes(x = FinishingBucket, y=mean_time, colour = "blue")) + 
    geom_errorbar(aes(ymin=mean_time-se, ymax=mean_time+se), width=.1, position=pd) +
    geom_line(position=pd) +  ggtitle("Mean time vs Experience bucket of size 20") +
    scale_colour_hue(name="Legend", # Legend label, use darker colors
                     labels=c("Mean of Final experience"),
                     l=40) +                  # Use darker colors, lightness=40
    geom_point(position=pd)

dev.off()
