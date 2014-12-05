directory = commandArgs(TRUE)[1]
after_innovation_pdf = read.table(paste(directory,"pdf_after_innovation.txt", sep=""), header=FALSE)
colnames(after_innovation_pdf) = c("Num", "Probability")
after_innovation_pdf$group = as.integer(after_innovation_pdf$Num / 30 + 0.00001)
temp = aggregate(after_innovation_pdf[,1:2], by=list(after_innovation_pdf$group), sum)

jpeg(paste(directory,"pdf_after_innovation",sep=""))
plot(temp$Group.1, temp$Probability, col = "blue", xlab = "Experience level", ylab = "Frequency", type="l")
legend("topright", 
    col = c("blue"), 
    legend = c("Buckets of size 30 days"),
    lty=c(1,1,1)
)
dev.off()


