draw_cristian_plot <- function(dataset, coeff, binary) {
    t = read.table(paste("~/Documents/Amazon/Output_All/",dataset,"_bursts/RealTime/MaxBenefit/", dataset, "_cristian_probability_", binary, "_comparison_top500_innovations_coeff", coeff, ".txt", sep=""), header=TRUE)    
    colnames(t) = c("V1", "V2", "V3")
    t1 = t[(t$V1 == 1) & (t$V3 > 0),]
    t2 = t[(t$V1 == 2) & (t$V3 > 0),]
    t3 = t[(t$V1 == 3) & (t$V3 > 0),]
    t4 = t[(t$V1 == 4) & (t$V3 > 0),]
    maxi = 0
    maxi = max(t1$V3, maxi)
    maxi = max(t2$V3, maxi)
    maxi = max(t3$V3, maxi)
    maxi = max(t4$V3, maxi)
    name = paste("~/Pictures/", dataset, "_cristian_probability_", binary, "_comparison_top500_innovations_coeff", coeff, ".png"  ,sep="")
    png(file = name)
    plot(t1$V2, t1$V3, type ="l", col = "blue", xlab="current xp", ylab="# innovations used", xlim =c(0,200), ylim = c(0, maxi))
    lines(t2$V2, t2$V3, type ="l", col = "green", xlab="current xp", ylab="# innovations used")
    lines(t3$V2, t3$V3, type ="l", col = "red", xlab="current xp", ylab="# innovations used")
    lines(t4$V2, t4$V3, type ="l", col = "cyan", xlab="current xp", ylab="# innovations used")
    legend("topright", col = c("blue", "green","red","cyan"), legend = c("First quantile", "Second quantile", "Third quantile", "Forth quantile"), lty = c(1,1,1))
    dev.off()
}
draw_all <-function() {
    draw_cristian_plot("Music", "3", "count")
    draw_cristian_plot("Music", "6", "count")
    draw_cristian_plot("Music", "12", "count")
    draw_cristian_plot("Music", "24", "count")

    draw_cristian_plot("Music", "3", "binary")
    draw_cristian_plot("Music", "6", "binary")
    draw_cristian_plot("Music", "12", "binary")
    draw_cristian_plot("Music", "24", "binary")


    draw_cristian_plot("Movies_and_TV", "3", "count")
    draw_cristian_plot("Movies_and_TV", "6", "count")
    draw_cristian_plot("Movies_and_TV", "12", "count")
    draw_cristian_plot("Movies_and_TV", "24", "count")

    draw_cristian_plot("Movies_and_TV", "3", "binary")
    draw_cristian_plot("Movies_and_TV", "6", "binary")
    draw_cristian_plot("Movies_and_TV", "12", "binary")
    draw_cristian_plot("Movies_and_TV", "24", "binary")



    draw_cristian_plot("Books", "3", "count")
    draw_cristian_plot("Books", "6", "count")
    draw_cristian_plot("Books", "12", "count")
    draw_cristian_plot("Books", "24", "count")
    
    draw_cristian_plot("Books", "3", "binary")
    draw_cristian_plot("Books", "6", "binary")
    draw_cristian_plot("Books", "12", "binary")
    draw_cristian_plot("Books", "24", "binary")
}


