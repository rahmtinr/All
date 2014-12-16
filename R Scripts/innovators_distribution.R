directory = commandArgs(TRUE)[1]
commandArgs(TRUE)[2]
review_distribution = read.table(paste(commandArgs(TRUE)[2],"distribution.txt", sep=""), header=FALSE)
innovator_distribution = read.table(paste(directory,"distribution.txt", sep=""), header=FALSE)
#first is x,second is y last is the state! If the state is 1 make it red
colnames(innovator_distribution) = c("CurrentExp", "FinishingExp")
colnames(review_distribution) = c("CurrentExp", "FinishingExp")
#All the unknown users will be counted as a first comer 
innovator_distribution$CurrentExp[innovator_distribution$FinishingExp>10000] = 0
innovator_distribution$FinishingExp[innovator_distribution$FinishingExp>10000] = 1
review_distribution$CurrentExp[review_distribution$FinishingExp>10000] = 0
review_distribution$FinishingExp[review_distribution$FinishingExp>10000] = 1

CurFreq = as.data.frame(table(innovator_distribution$CurrentExp))
FinishFreq = as.data.frame(table(innovator_distribution$FinishingExp))
ReviewCurFreq = as.data.frame(table(review_distribution$CurrentExp))
ReviewFinishFreq = as.data.frame(table(review_distribution$FinishingExp))

CurFreq$Freq = CurFreq$Freq / nrow(innovator_distribution)
FinishFreq$Freq = FinishFreq$Freq / nrow(innovator_distribution)
ReviewCurFreq$Freq = ReviewCurFreq$Freq / nrow(review_distribution)
ReviewFinishFreq$Freq = ReviewFinishFreq$Freq / nrow(review_distribution)

jpeg(paste(directory,"AggregationPlots/distribution_ending_experience_level_linear_linear",sep=""))
plot(ReviewFinishFreq$Freq, col = "blue", xlab = "Experience level", ylab = "Frequency", type="l")
lines(FinishFreq$Freq, col = "red")
legend("topright", 
    col = c("blue", "red"), 
    legend = c("All reviews", "Innovations"),
    lty=c(1,1,1)
)
dev.off()

jpeg(paste(directory,"AggregationPlots/distribution_present_experience_level_linear_linear",sep=""))
plot(ReviewCurFreq$Freq, col = "blue", xlab = "Experience level", ylab = "Frequency", type="l")
lines(CurFreq$Freq , col = "red")
legend("topright", 
    col = c("blue", "red"), 
    legend = c("All reviews", "Innovations"),
    lty=c(1,1,1)
)
dev.off()

jpeg(paste(directory,"AggregationPlots/distribution_ending_experience_level_log_linear",sep=""))
plot(log(ReviewFinishFreq$Freq), col = "blue", xlab = "Experience level", ylab = "Frequency", type="l")
lines(FinishFreq$Freq, col = "red")
legend("topright", 
    col = c("blue", "red"), 
    legend = c("All reviews", "Innovations"),
    lty=c(1,1,1)
)
dev.off()

jpeg(paste(directory,"AggregationPlots/distribution_present_experience_level_log_linear",sep=""))
plot(log(ReviewCurFreq$Freq), col = "blue", xlab = "Experience level", ylab = "Frequency", type="l")
lines(CurFreq$Freq , col = "red")
legend("topright", 
    col = c("blue", "red"), 
    legend = c("All reviews", "Innovations"),
    lty=c(1,1,1)
)
dev.off()

jpeg(paste(directory,"AggregationPlots/distribution_ending_experience_level_log_log",sep=""))
plot(log(as.numeric(ReviewFinishFreq$Var1)), log(ReviewFinishFreq$Freq), col = "blue", xlab = "Experience level", ylab = "Frequency", type="l")
lines(FinishFreq$Freq, col = "red")
legend("topright", 
    col = c("blue", "red"), 
    legend = c("All reviews", "Innovations"),
    lty=c(1,1,1)
)
dev.off()
head(log(as.numeric(ReviewCurFreq$Var1)))
jpeg(paste(directory,"AggregationPlots/distribution_present_experience_level_log_log",sep=""))
plot(log(as.numeric(ReviewCurFreq$Var1)), log(ReviewCurFreq$Freq), col = "blue", xlab = "Experience level", ylab = "Frequency", type="l")
lines(CurFreq$Freq , col = "red")
legend("topright", 
    col = c("blue", "red"), 
    legend = c("All reviews", "Innovations"),
    lty=c(1,1,1)
)
dev.off()

