library(ggplot2)
draw_cristian_plot <- function(dataset, coeff, binary) {
    t = read.table(paste("~/Documents/Amazon/Output_All/",dataset,"_bursts/RealTime/MaxBenefit/",dataset, "_mlf_cristian_comparison_coeff_", coeff, ".txt", sep=""), header=TRUE)    
    colnames(t) = c("V1", "V2", "V3")
#    t1 = t[(t$V1 == 1) & (t$V3 > 0),]
#    t2 = t[(t$V1 == 2) & (t$V3 > 0),]
#    t3 = t[(t$V1 == 3) & (t$V3 > 0),]
#    t4 = t[(t$V1 == 4) & (t$V3 > 0),]

    t1 = t[(t$V1 == 1),]
    t2 = t[(t$V1 == 2),]
    t3 = t[(t$V1 == 3),]
    t4 = t[(t$V1 == 4),]

    maxi = 0
    maxi = max(t1$V3, maxi)
    maxi = max(t2$V3, maxi)
    maxi = max(t3$V3, maxi)
    maxi = max(t4$V3, maxi)
    name = paste("~/Pictures/today/", dataset, "_mlf_cristian_comparison_coeff_", coeff, ".png"  ,sep="")
    png(file = name)
    plot(t1$V2, t1$V3, type ="l", col = "blue", xlab="current xp", ylab="# innovations used", xlim =c(0,200), ylim = c(0, maxi))
    lines(t2$V2, t2$V3, type ="l", col = "green", xlab="current xp", ylab="# innovations used")
    lines(t3$V2, t3$V3, type ="l", col = "red", xlab="current xp", ylab="# innovations used")
    lines(t4$V2, t4$V3, type ="l", col = "cyan", xlab="current xp", ylab="# innovations used")
    legend("topright", col = c("blue", "green","red","cyan"), legend = c("First quantile", "Second quantile", "Third quantile", "Forth quantile"), lty = c(1,1,1))
    dev.off()
}
draw_all <-function() {
     draw_cristian_plot("Music",  "5.0", "count")

    draw_cristian_plot("Movies_and_TV",  "12.0", "count")

    draw_cristian_plot("Books",  "9.0", "count")

    draw_cristian_plot("Beer_Advocate",  "4.2", "count")

    draw_cristian_plot("Rate_Beer",  "6.0", "count")

    draw_cristian_plot("reddit_funny",  "6.0", "count")

    draw_cristian_plot("reddit_gaming",  "7.0", "count")

    draw_cristian_plot("reddit_politics",  "9.0", "count")
    
    draw_cristian_plot("reddit_Music",  "4.5", "count")
    
    draw_cristian_plot("reddit_leagueoflegends",  "9.0", "count")

    draw_cristian_plot("reddit_books",  "6.5", "count")

    draw_cristian_plot("reddit_movies",  "6.3", "count")
}

draw_all()


