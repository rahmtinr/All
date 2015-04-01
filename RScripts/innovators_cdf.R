directory = commandArgs(TRUE)[1]
dataset = commandArgs(TRUE)[2]
review_present_cdf = read.table(paste(directory, dataset, "_all_present_cdf.txt", sep=""), header=FALSE)
review_final_cdf = read.table(paste(directory, dataset, "_all_final_cdf.txt", sep=""), header=FALSE)
innovator_present_cdf = read.table(paste(directory, dataset, "_innovator_present_cdf.txt", sep=""), header=FALSE)
innovator_final_cdf = read.table(paste(directory, dataset, "_innovator_final_cdf.txt", sep=""), header=FALSE)
colnames(review_present_cdf) = c("Num", "Probability")
colnames(review_final_cdf) = c("Num", "Probability")
colnames(innovator_present_cdf) = c("Num", "Probability")
colnames(innovator_final_cdf) = c("Num", "Probability")


head(innovator_final_cdf)
review_present_cdf$Probability = log(review_present_cdf$Probability)
review_final_cdf$Probability = log(review_final_cdf$Probability)
innovator_present_cdf$Probability = log(innovator_present_cdf$Probability)
innovator_final_cdf$Probability = log(innovator_final_cdf$Probability)
head(innovator_final_cdf)
head(review_final_cdf)
jpeg(paste(directory,"AggregationPlots/", dataset, "_cdf_present_experience_level.jpg",sep=""))
plot(review_present_cdf, col = "blue", xlab = "Experience level", ylab = "Frequency", type="l")
#lines(review_present_cdf, col = "blue")
#points(innovator_present_cdf, col = "red")
lines(innovator_present_cdf, col = "red")
legend("topright", 
    col = c("blue", "red"), 
    legend = c("All reviews", "innovation reviews"),
    lty=c(1,1,1)
)
dev.off()

jpeg(paste(directory,"AggregationPlots/", dataset, "_cdf_final_experience_level.jpg",sep=""))
plot(review_final_cdf, col = "blue", xlab = "Experience level", ylab = "Frequency", type="l")
#lines(review_final_cdf , col = "blue")
#points(innovator_final_cdf , col = "red")
lines(innovator_final_cdf , col = "red")
legend("topright", 
    col = c("blue", "red"), 
    legend = c("All reviews", "innovation reviews"),
    lty=c(1,1,1)
)
dev.off()

