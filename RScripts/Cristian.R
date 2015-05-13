draw_cristian_plot <- function(dataset, coeff, binary) {
    t = read.table(paste("~/Documents/Amazon/Output_All/",dataset,"_bursts/RealTime/MaxBenefit/", dataset, "_cristian_probability_", binary, "_comparison_top500_innovations_20_buckets_coeff", coeff, ".txt", sep=""), header=TRUE)    
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
    name = paste("~/Pictures/today/", dataset, "_cristian_probability_", binary, "_comparison_top500_innovations_20_buckets_coeff", coeff, ".png"  ,sep="")
    png(file = name)
    plot(t1$V2, t1$V3, type ="l", col = "blue", xlab="current xp", ylab="# innovations used", xlim =c(0,200), ylim = c(0, maxi))
    lines(t2$V2, t2$V3, type ="l", col = "green", xlab="current xp", ylab="# innovations used")
    lines(t3$V2, t3$V3, type ="l", col = "red", xlab="current xp", ylab="# innovations used")
    lines(t4$V2, t4$V3, type ="l", col = "cyan", xlab="current xp", ylab="# innovations used")
    legend("topright", col = c("blue", "green","red","cyan"), legend = c("First quantile", "Second quantile", "Third quantile", "Forth quantile"), lty = c(1,1,1))
    dev.off()
}
draw_all <-function() {
     draw_cristian_plot("Music",  "0.05", "count")
    draw_cristian_plot("Music",  "3.8", "count")
    draw_cristian_plot("Music",  "0.05", "count")
    draw_cristian_plot("Music",  "3.8", "count")

    draw_cristian_plot("Movies_and_TV",  "0.05", "count")
    draw_cristian_plot("Movies_and_TV",  "9.0", "count")
    draw_cristian_plot("Movies_and_TV",  "12", "count")
    draw_cristian_plot("Movies_and_TV",  "0.05", "count")
    draw_cristian_plot("Movies_and_TV",  "9.0", "count")
    draw_cristian_plot("Movies_and_TV",  "12", "count")

    draw_cristian_plot("Books",  "0.05", "count")
    draw_cristian_plot("Books",  "9.0", "count")
    draw_cristian_plot("Books",  "12", "count")
    draw_cristian_plot("Books",  "0.05", "count")
    draw_cristian_plot("Books",  "9.0", "count")
    draw_cristian_plot("Books",  "12", "count")

    draw_cristian_plot("Electronics",  "0.05", "count")
    draw_cristian_plot("Electronics",  "12", "count")
    draw_cristian_plot("Electronics",  "0.05", "count")
    draw_cristian_plot("Electronics",  "12", "count")

    draw_cristian_plot("Beer_Advocate",  "0.05", "count")
    draw_cristian_plot("Beer_Advocate",  "4.2", "count")
    draw_cristian_plot("Beer_Advocate",  "12", "count")
    draw_cristian_plot("Beer_Advocate",  "0.05", "count")
    draw_cristian_plot("Beer_Advocate",  "4.2", "count")
    draw_cristian_plot("Beer_Advocate",  "12", "count")

    draw_cristian_plot("Rate_Beer",  "0.05", "count")
    draw_cristian_plot("Rate_Beer",  "6.0", "count")
    draw_cristian_plot("Rate_Beer",  "12", "count")
    draw_cristian_plot("Rate_Beer",  "0.05", "count")
    draw_cristian_plot("Rate_Beer",  "6.0", "count")
    draw_cristian_plot("Rate_Beer",  "12", "count")

    draw_cristian_plot("reddit_funny",  "0.05", "count")
    draw_cristian_plot("reddit_funny",  "3.6", "count")
    draw_cristian_plot("reddit_funny",  "4.5", "count")
    draw_cristian_plot("reddit_funny",  "0.05", "count")
    draw_cristian_plot("reddit_funny",  "3.6", "count")
    draw_cristian_plot("reddit_funny",  "4.5", "count")

    draw_cristian_plot("reddit_AskReddit",  "0.05", "count")
    draw_cristian_plot("reddit_AskReddit",  "9.0", "count")
    draw_cristian_plot("reddit_AskReddit",  "12", "count")
    draw_cristian_plot("reddit_AskReddit",  "0.05", "count")
    draw_cristian_plot("reddit_AskReddit",  "9.0", "count")
    draw_cristian_plot("reddit_AskReddit",  "12", "count")
    
    draw_cristian_plot("reddit_pics",  "0.05", "count")
    draw_cristian_plot("reddit_pics",  "2.6", "count")
    draw_cristian_plot("reddit_pics",  "4.5", "count")
    draw_cristian_plot("reddit_pics",  "0.05", "count")
    draw_cristian_plot("reddit_pics",  "2.6", "count")
    draw_cristian_plot("reddit_pics",  "4.5", "count")

    draw_cristian_plot("reddit_AdviceAnimals",  "0.05", "count")
    draw_cristian_plot("reddit_AdviceAnimals",  "4.5", "count")
    draw_cristian_plot("reddit_AdviceAnimals",  "6.0", "count")
    draw_cristian_plot("reddit_AdviceAnimals",  "0.05", "count")
    draw_cristian_plot("reddit_AdviceAnimals",  "4.5", "count")
    draw_cristian_plot("reddit_AdviceAnimals",  "6.0", "count")
    
    draw_cristian_plot("reddit_gaming",  "0.05", "count")
    draw_cristian_plot("reddit_gaming",  "4.5", "count")
    draw_cristian_plot("reddit_gaming",  "6.0", "count")
    draw_cristian_plot("reddit_gaming",  "0.05", "count")
    draw_cristian_plot("reddit_gaming",  "4.5", "count")
    draw_cristian_plot("reddit_gaming",  "6.0", "count")

    draw_cristian_plot("reddit_videos",  "0.05", "count")
    draw_cristian_plot("reddit_videos",  "3.6", "count")
    draw_cristian_plot("reddit_videos",  "0.05", "count")
    draw_cristian_plot("reddit_videos",  "3.6", "count")

    draw_cristian_plot("reddit_politics",  "0.05", "count")
    draw_cristian_plot("reddit_politics",  "2.6", "count")
    draw_cristian_plot("reddit_politics",  "9.0", "count")
    draw_cristian_plot("reddit_politics",  "0.05", "count")
    draw_cristian_plot("reddit_politics",  "2.6", "count")
    draw_cristian_plot("reddit_politics",  "9.0", "count")
    
    draw_cristian_plot("reddit_trees",  "0.05", "count")
    draw_cristian_plot("reddit_trees",  "4.5", "count")
    draw_cristian_plot("reddit_trees",  "6.0", "count")
    draw_cristian_plot("reddit_trees",  "0.05", "count")
    draw_cristian_plot("reddit_trees",  "4.5", "count")
    draw_cristian_plot("reddit_trees",  "6.0", "count")

    draw_cristian_plot("reddit_WTF",  "0.05", "count")
    draw_cristian_plot("reddit_WTF",  "6.0", "count")
    draw_cristian_plot("reddit_WTF",  "0.05", "count")
    draw_cristian_plot("reddit_WTF",  "6.0", "count")
    
    draw_cristian_plot("reddit_aww",  "0.05", "count")
    draw_cristian_plot("reddit_aww",  "4.0", "count")
    draw_cristian_plot("reddit_aww",  "0.05", "count")
    draw_cristian_plot("reddit_aww",  "4.0", "count")

    draw_cristian_plot("reddit_fffffffuuuuuuuuuuuu",  "0.05", "count")
    draw_cristian_plot("reddit_fffffffuuuuuuuuuuuu",  "3.4", "count")
    draw_cristian_plot("reddit_fffffffuuuuuuuuuuuu",  "0.05", "count")
    draw_cristian_plot("reddit_fffffffuuuuuuuuuuuu",  "3.4", "count")

    draw_cristian_plot("reddit_Music",  "0.05", "count")
    draw_cristian_plot("reddit_Music",  "3.8", "count")
    draw_cristian_plot("reddit_Music",  "0.05", "count")
    draw_cristian_plot("reddit_Music",  "3.8", "count")
    
    draw_cristian_plot("reddit_worldnews",  "0.05", "count")
    draw_cristian_plot("reddit_worldnews",  "6.0", "count")
    draw_cristian_plot("reddit_worldnews",  "0.05", "count")
    draw_cristian_plot("reddit_worldnews",  "6.0", "count")
    
    draw_cristian_plot("reddit_leagueoflegends",  "0.05", "count")
    draw_cristian_plot("reddit_leagueoflegends",  "4.0", "count")
    draw_cristian_plot("reddit_leagueoflegends",  "9.0", "count")
    draw_cristian_plot("reddit_leagueoflegends",  "0.05", "count")
    draw_cristian_plot("reddit_leagueoflegends",  "4.0", "count")
    draw_cristian_plot("reddit_leagueoflegends",  "9.0", "count")


    draw_cristian_plot("reddit_technology",  "0.05", "count")
    draw_cristian_plot("reddit_technology",  "12", "count")
    draw_cristian_plot("reddit_technology",  "0.05", "count")
    draw_cristian_plot("reddit_technology",  "12", "count")
}


