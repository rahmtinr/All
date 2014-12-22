library(ggplot2)
directory = commandArgs(TRUE)[1]
dataset = commandArgs(TRUE)[2]
baseline = read.table(paste(directory,dataset,"_random_baseline_innovation", ".txt", sep=""), header=FALSE)

#first is x,second is y last is the state! If the state is 1 make it red
colnames(baseline) = c("Group", "CurrentExp", "FinishingExp")

#        jpeg(paste(directory,"random_baseline_present_exp",sep=""))
#        boxplot(CurFreq$Freq)
#        dev.off()
innovator_distribution = read.table(paste(directory,dataset, "_distribution.txt", sep=""), header=FALSE)

colnames(innovator_distribution) = c("CurrentExp", "FinishingExp")
innovator_distribution$Group = "innovation"

png(paste(directory, "AggregationPlots/",dataset,"_random_baseline_ending_exp.png", sep=""),height=800,width=1300)
total = rbind(innovator_distribution, baseline) 
ggplot(total, aes(x = factor(Group), y = FinishingExp), height=600, width=800) + geom_boxplot() + coord_cartesian(ylim = c(0, 5000))
dev.off()

png(paste(directory, "AggregationPlots/",dataset,"_random_baseline_present_exp.png", sep=""),height=800,width=1300)
total = rbind(innovator_distribution, baseline) 
ggplot(total, aes(x = factor(Group), y = CurrentExp), height=600, width=800) + geom_boxplot() + coord_cartesian(ylim = c(0, 5000))

dev.off()
