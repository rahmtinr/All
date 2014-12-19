directory = commandArgs(TRUE)[1]
dataset = commandArgs(TRUE)[2]
after_innovation_pdf = read.table(paste(directory, dataset, "_pdf_after_innovation.txt", sep=""), header=FALSE)
colnames(after_innovation_pdf) = c("Num", "Probability")
after_innovation_pdf$group = as.integer(after_innovation_pdf$Num / 10 + 0.00001)
temp = aggregate(after_innovation_pdf[,1:2], by=list(after_innovation_pdf$group), sum)

jpeg(paste(directory,"AggregationPlots/", dataset, "_pdf_after_innovation_linear_linear.jpg",sep=""))
plot(temp$Group.1, temp$Probability, col = "blue", xlab = "10 day bucket", ylab = "Frequency", type="l")
legend("topright", 
    col = c("blue"), 
    legend = c("Buckets of size 10 days"),
    lty=c(1,1,1)
)
dev.off()

jpeg(paste(directory,"AggregationPlots/", dataset, "_pdf_after_innovation_log_log.jpg",sep=""))
plot(log(temp$Group.1),log(temp$Probability), col = "blue", xlab = "10 day bucket", ylab = "Frequency", type="l")
legend("topright", 
    col = c("blue"), 
    legend = c("Buckets of size 10 days"),
    lty=c(1,1,1)
)
dev.off()

jpeg(paste(directory,"AggregationPlots/", dataset, "_pdf_after_innovation_log_linear.jpg",sep=""))
plot(temp$Group.1,log(temp$Probability), col = "blue", xlab = "10 day bucket", ylab = "Frequency", type="l")
legend("topright", 
    col = c("blue"), 
    legend = c("Buckets of size 10 days"),
    lty=c(1,1,1)
)
dev.off()


