library("gplots")
library("reshape2")
library("ggplot2")
library("scales")

draw_heat_map <- function(dataset, final, coeff) {
    mini = 1000
    a_b = read.table(paste("~/Documents/Amazon/Output_All/",dataset, "_bursts/RealTime/MaxBenefit/",dataset,"_", final, "_[a,b]_top_500_innovations_coeff", coeff, "_4parts.txt", sep=""), header=TRUE)
    mat = matrix(rep(c(0), 200 * 199), nrow=200, ncol=199)
    for(i in 1:nrow(a_b)) { 
        x = c(0, 0.05, 0.5, 0.75, 1) 
        y = c(0, a_b[i, ]$cq1, a_b[i, ]$cq2, a_b[i, ]$cq3, a_b[i, ]$cq4)
        maxi = 0.000001
        maxi = max(max(y), maxi)
#       print(maxi)
        y = y / maxi
#       print(x)
#       print(y)
        K = lm(y ~ x)
        a = a_b[i, ]$a + 100
        b = a_b[i, ]$b + 100
        mat[a, b] = K$coefficients[2]
        mini = min(mini,K$coefficients[2])
#        cat(a," ", b, " ", K$coefficients[2],"\n")
    }
    print("a_b_final_count")

    print("________________________________")
    smoothed = mat 
#    tempname = (paste(dataset, "_a_b_top_500_innovations_coeff3.png", sep=""))
#    png(filename=tempname, height = 1600, width = 1600, units="px", res = 150)
    dat <- melt(smoothed)
    colnames(dat) = c("a", "b", "value")
    dat[dat$a >= dat$b, "value"] = mini 
    myplot <- ggplot(dat, aes(x=a,y=b)) + 
    geom_tile(aes(fill = value),colour='black') +
    scale_fill_gradientn(colours=c("green","yellow","red")) +  
    ggtitle(paste(dataset, final, "coeff", coeff, "4parts", sep=" ")) + 
    coord_equal() 
    save_address = paste("~/Pictures/today/", dataset,"_", final, "_heat_map_coeff", coeff, "_4parts.jpg", sep="")
    print(save_address)
    ggsave(filename=save_address, plot=myplot)
#    dev.off()
}

draw_all <- function() {
    draw_heat_map("Music", "final", "0.05")
    draw_heat_map("Music", "final", "3.8")
    draw_heat_map("Music", "current", "0.05")
    draw_heat_map("Music", "current", "3.8")

    draw_heat_map("Movies_and_TV", "final", "0.05")
    draw_heat_map("Movies_and_TV", "final", "9.0")
    draw_heat_map("Movies_and_TV", "final", "12")
    draw_heat_map("Movies_and_TV", "current", "0.05")
    draw_heat_map("Movies_and_TV", "current", "9.0")
    draw_heat_map("Movies_and_TV", "current", "12")

    draw_heat_map("Books", "final", "0.05")
    draw_heat_map("Books", "final", "9.0")
    draw_heat_map("Books", "final", "12")
    draw_heat_map("Books", "current", "0.05")
    draw_heat_map("Books", "current", "9.0")
    draw_heat_map("Books", "current", "12")

    draw_heat_map("Electronics", "final", "0.05")
    draw_heat_map("Electronics", "final", "12")
    draw_heat_map("Electronics", "current", "0.05")
    draw_heat_map("Electronics", "current", "12")

    draw_heat_map("Beer_Advocate", "final", "0.05")
    draw_heat_map("Beer_Advocate", "final", "4.2")
    draw_heat_map("Beer_Advocate", "final", "12")
    draw_heat_map("Beer_Advocate", "current", "0.05")
    draw_heat_map("Beer_Advocate", "current", "4.2")
    draw_heat_map("Beer_Advocate", "current", "12")

    draw_heat_map("Rate_Beer", "final", "0.05")
    draw_heat_map("Rate_Beer", "final", "6.0")
    draw_heat_map("Rate_Beer", "final", "12")
    draw_heat_map("Rate_Beer", "current", "0.05")
    draw_heat_map("Rate_Beer", "current", "6.0")
    draw_heat_map("Rate_Beer", "current", "12")

    draw_heat_map("reddit_funny", "final", "0.05")
    draw_heat_map("reddit_funny", "final", "3.6")
    draw_heat_map("reddit_funny", "final", "4.5")
    draw_heat_map("reddit_funny", "current", "0.05")
    draw_heat_map("reddit_funny", "current", "3.6")
    draw_heat_map("reddit_funny", "current", "4.5")

    draw_heat_map("reddit_AskReddit", "final", "0.05")
    draw_heat_map("reddit_AskReddit", "final", "9.0")
    draw_heat_map("reddit_AskReddit", "final", "12")
    draw_heat_map("reddit_AskReddit", "current", "0.05")
    draw_heat_map("reddit_AskReddit", "current", "9.0")
    draw_heat_map("reddit_AskReddit", "current", "12")
   
    draw_heat_map("reddit_pics", "final", "0.05")
    draw_heat_map("reddit_pics", "final", "2.6")
    draw_heat_map("reddit_pics", "final", "4.5")
    draw_heat_map("reddit_pics", "current", "0.05")
    draw_heat_map("reddit_pics", "current", "2.6")
    draw_heat_map("reddit_pics", "current", "4.5")

    draw_heat_map("reddit_AdviceAnimals", "final", "0.05")
    draw_heat_map("reddit_AdviceAnimals", "final", "4.5")
    draw_heat_map("reddit_AdviceAnimals", "final", "6.0")
    draw_heat_map("reddit_AdviceAnimals", "current", "0.05")
    draw_heat_map("reddit_AdviceAnimals", "current", "4.5")
    draw_heat_map("reddit_AdviceAnimals", "current", "6.0")
   
    draw_heat_map("reddit_gaming", "final", "0.05")
    draw_heat_map("reddit_gaming", "final", "4.5")
    draw_heat_map("reddit_gaming", "final", "6.0")
    draw_heat_map("reddit_gaming", "current", "0.05")
    draw_heat_map("reddit_gaming", "current", "4.5")
    draw_heat_map("reddit_gaming", "current", "6.0")

    draw_heat_map("reddit_videos", "final", "0.05")
    draw_heat_map("reddit_videos", "final", "3.6")
    draw_heat_map("reddit_videos", "current", "0.05")
    draw_heat_map("reddit_videos", "current", "3.6")

    draw_heat_map("reddit_politics", "final", "0.05")
    draw_heat_map("reddit_politics", "final", "2.6")
    draw_heat_map("reddit_politics", "final", "9.0")
    draw_heat_map("reddit_politics", "current", "0.05")
    draw_heat_map("reddit_politics", "current", "2.6")
    draw_heat_map("reddit_politics", "current", "9.0")
   
    draw_heat_map("reddit_trees", "final", "0.05")
    draw_heat_map("reddit_trees", "final", "4.5")
    draw_heat_map("reddit_trees", "final", "6.0")
    draw_heat_map("reddit_trees", "current", "0.05")
    draw_heat_map("reddit_trees", "current", "4.5")
    draw_heat_map("reddit_trees", "current", "6.0")

    draw_heat_map("reddit_WTF", "final", "0.05")
    draw_heat_map("reddit_WTF", "final", "6.0")
    draw_heat_map("reddit_WTF", "current", "0.05")
    draw_heat_map("reddit_WTF", "current", "6.0")
    
    draw_heat_map("reddit_aww", "final", "0.05")
    draw_heat_map("reddit_aww", "final", "4.0")
    draw_heat_map("reddit_aww", "current", "0.05")
    draw_heat_map("reddit_aww", "current", "4.0")

    draw_heat_map("reddit_fffffffuuuuuuuuuuuu", "final", "0.05")
    draw_heat_map("reddit_fffffffuuuuuuuuuuuu", "final", "3.4")
    draw_heat_map("reddit_fffffffuuuuuuuuuuuu", "current", "0.05")
    draw_heat_map("reddit_fffffffuuuuuuuuuuuu", "current", "3.4")

    draw_heat_map("reddit_Music", "final", "0.05")
    draw_heat_map("reddit_Music", "final", "3.8")
    draw_heat_map("reddit_Music", "current", "0.05")
    draw_heat_map("reddit_Music", "current", "3.8")
    
    draw_heat_map("reddit_worldnews", "final", "0.05")
    draw_heat_map("reddit_worldnews", "final", "6.0")
    draw_heat_map("reddit_worldnews", "current", "0.05")
    draw_heat_map("reddit_worldnews", "current", "6.0")
    
    draw_heat_map("reddit_leagueoflegends", "final", "0.05")
    draw_heat_map("reddit_leagueoflegends", "final", "4.0")
    draw_heat_map("reddit_leagueoflegends", "final", "9.0")
    draw_heat_map("reddit_leagueoflegends", "current", "0.05")
    draw_heat_map("reddit_leagueoflegends", "current", "4.0")
    draw_heat_map("reddit_leagueoflegends", "current", "9.0")


    draw_heat_map("reddit_technology", "final", "0.05")
    draw_heat_map("reddit_technology", "final", "12")
    draw_heat_map("reddit_technology", "current", "0.05")
    draw_heat_map("reddit_technology", "current", "12")
}
draw_all()

