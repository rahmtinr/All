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

CurFreq$Freq = log(CurFreq$Freq / nrow(innovator_distribution))
FinishFreq$Freq = log(FinishFreq$Freq / nrow(innovator_distribution))
ReviewCurFreq$Freq = log(ReviewCurFreq$Freq / nrow(review_distribution))
ReviewFinishFreq$Freq = log(ReviewFinishFreq$Freq / nrow(review_distribution))

jpeg(paste(directory,"distribution_ending_experience_level",sep=""))
plot(ReviewFinishFreq$Freq, col = "blue", xlab = "Experience level", ylab = "Frequency")
points(FinishFreq$Freq, col = "red")
dev.off()

jpeg(paste(directory,"distribution_present_experience_level",sep=""))
plot(ReviewCurFreq, col = "blue", xlab = "Experience level", ylab = "Frequency")
points(CurFreq , col = "red")
dev.off()

