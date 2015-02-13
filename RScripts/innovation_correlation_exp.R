input_file = commandArgs(TRUE)[1]
all = read.table(paste(input_file, sep=""), header=FALSE)
#jpeg('random_shuffle.jpg')
#plot(all)
#dev.off()
for (k in 0:498) {
	cat("Correlation k: ", cor(all$V1[all$V3==k], all$V2[all$V3==k]), "\n")
}
