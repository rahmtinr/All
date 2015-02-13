input_file = commandArgs(TRUE)[1]
all = read.table(paste(input_file, sep=""), header=FALSE)
#jpeg('random_shuffle.jpg')
#plot(all)
#dev.off()
cat("Correlation: ", cor(all$V1, all$V2), "\n")
