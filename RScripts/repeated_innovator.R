directory = commandArgs(TRUE)[1]
dataset = commandArgs(TRUE)[2]
innovator_distribution = read.table(paste(directory, dataset, "_innovation_for_each_user.txt", sep=""), header=FALSE)
#first is x,second is y last is the state! If the state is 1 make it red
colnames(innovator_distribution) = c("UserId", "Num")

Freq = as.data.frame(table(innovator_distribution$Num))

jpeg(paste(directory,"AggregationPlots/", dataset, "_reinvention.jpg",sep=""))
hist(innovator_distribution$Num, main = "Innovators VS Innovations made", col = "blue", xlab = "Number of Inventions", ylab = "Number of Inventors")
dev.off()

