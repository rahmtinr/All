temp = read.table("../../Output_All/innovators_distribution_4_3_3_Music.txt",header=TRUE)
head(temp)
names(temp)
jpeg('rplot-music.jpg')
plot(temp$num.of.rev, temp$entire.data, main="Scatterplot Example",xlab="Number of reviewers", ylab="distribution", pch=5, col="blue") 
points(temp$num.of.rev, temp$num.of.reviews, pch=3, col="green") 
points(temp$num.of.rev, temp$num.of.xp.level, pch=2, col="red") 
dev.off()

