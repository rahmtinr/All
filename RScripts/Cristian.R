t = read.table("/home/rahmtin/workspace/Output_All/DBLP/Bursts/DocRatio/DBLP_cristian_probability_binary_comparison_top500_innovations.txt")
t1 = t[t$V1 == 1 && t$V3 > 0,]
t1 = t[(t$V1 == 1) & (t$V3 > 0),]
t2 = t[(t$V1 == 2) & (t$V3 > 0),]
t3 = t[(t$V1 == 3) & (t$V3 > 0),]
t4 = t[(t$V1 == 4) & (t$V3 > 0),]
plot(t1$V2, t1$V3, type ="l", col = "blue", xlab="current xp", ylab="# innovations used", xlim =c(0,200), ylim = c(0, 0.5))
lines(t2$V2, t2$V3, type ="l", col = "green", xlab="current xp", ylab="# innovations used")
lines(t3$V2, t3$V3, type ="l", col = "red", xlab="current xp", ylab="# innovations used")
lines(t4$V2, t4$V3, type ="l", col = "cyan", xlab="current xp", ylab="# innovations used")
legend("topright", col = c("blue", "green","red","cyan"), legend = c("First quantile", "Second quantile", "Third quantile", "Forth quantile"), lty = c(1,1,1))

