directory = commandArgs(TRUE)[1]
review_present_cdf = read.table(paste(directory,"all_present_cdf.txt", sep=""), header=FALSE)
review_final_cdf = read.table(paste(directory,"all_final_cdf.txt", sep=""), header=FALSE)
innovator_present_cdf = read.table(paste(directory,"innovator_present_cdf.txt", sep=""), header=FALSE)
innovator_final_cdf = read.table(paste(directory,"innovator_final_cdf.txt", sep=""), header=FALSE)
colnames(review_present_cdf) = c("Num", "Probability")
colnames(review_final_cdf) = c("Num", "Probability")
colnames(innovator_present_cdf) = c("Num", "Probability")
colnames(innovator_final_cdf) = c("Num", "Probability")


review_present_cdf$Probability = log(review_present_cdf$Probability)
review_final_cdf$Probability = log(review_final_cdf$Probability)
innovator_present_cdf$Probability = log(innovator_present_cdf$Probability)
innovator_final_cdf$Probability = log(innovator_final_cdf$Probability)


jpeg(paste(directory,"AggregationPlots/cdf_ending_experience_level.jpg",sep=""))
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

jpeg(paste(directory,"AggregationPlots/cdf_present_experience_level.jpg",sep=""))
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

