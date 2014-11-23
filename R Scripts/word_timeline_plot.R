input_file = commandArgs(TRUE)[1]
all = read.table(paste(input_file, sep=""), header=FALSE)
#first is x,second is y last is the state! If the state is 1 make it red
all$mycolor ="black"
all$mycolor[all$V4=="1"]="red"
words = unique(all$V1)
head(words)
for(i in 1:length(words)) {
	word = words[i];
	jpeg(paste(word,"_timeline.jpg",sep=""))
	word_all = subset(all, all$V1 == word)
	plot(word_all$V5, word_all$V3, main=paste(word, "occurances time line"),xlab="day", ylab="time that appears", pch=5, col=word_all$mycolor) 
	dev.off()
}
