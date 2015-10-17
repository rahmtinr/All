library(ggplot2)
library(grid)
library(gridExtra)
draw_relative_year_comparison <- function(dataset, final, coeff, bucket_size, bigram, myrange) {
    if(coeff == "0.05"){
        return(NULL)
    }
    print(paste(dataset, coeff,  final, bucket_size, bigram, myrange,  sep=" "))
    t = read.table(paste("~/Documents/Amazon/Output_All/", dataset, "_bursts/RealTime/MaxBenefit/", 
                dataset, "_mlf_",bigram, "_", final, "_relative_year_usage_bucketed_", bucket_size ,"_median_comparison_500_innovations_coeff", coeff, ".txt", sep=""), header=TRUE)
#    t = read.table(paste("~/Documents/Amazon/Output_All/", dataset, "_bursts/RealTime/MaxBenefit/", 
#                dataset, "_bigrams_empty2009_", final, "_relative_year_usage_bucketed_", bucket_size ,"_median_comparison_500_innovations_coeff", coeff, ".txt", sep=""), header=TRUE)
    t = t[-1, ]
    print("Hello")
    if(nrow(t) == 0) {
        print("WTF")
        return(NULL)
    }
    print("HOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOORAY")
    linesize=1
    if(final=="final") {
        t$Start_week = t$Start_week - 1100
    } else {
        colnames(t)=c("Start_week", "Average", "Fraction")
    }
    t = t[t$Start_week > -myrange & t$Start_week < myrange, ]
   name = paste("~/Pictures/today/", final, "/", bigram, "/", myrange, "/", dataset, "_min_life_span_relative_year_usage_bucketed_", bucket_size,"_median_comparison_innovation_coeff", coeff, ".png"  ,sep="")
#    name = paste("~/Pictures/today/", dataset, "_bigrams_empty2009_", final, "_relative_year_usage_bucketed_", bucket_size,"_median_comparison_innovation_coeff", coeff, ".png"  ,sep="")
    print(name)
    png(name)
    p <- ggplot() + 
         theme(  legend.title=element_blank(), legend.position = "none",
                 axis.text.x=element_text(color="black", size=12),  axis.text.y=element_text(color="black", size=12)) +
         geom_line(data=t, aes(x=Start_week, y = Fraction, colour = "Innovation Median"), size=linesize) +
         xlab("Relative Week") +
         ylab("Median Fraction Score") + ggtitle(dataset) +
         ylim(0.20,0.80) +
         scale_color_manual(values=c("Innovation Median"="black")) +
         geom_hline(aes(yintercept=0.5), color="red") + geom_vline(aes(xintercept=0), color="red")
    print(p)
    dev.off()
    return(p)
}

draw_all <- function() {
    ExpOption = c("final", "current")
    bigramOption= c("words", "bigrams")

    for(i in 1:2) {
       for(j in 1:2) { 
            for(k in seq(30,30,by=10)) {
                p1=draw_relative_year_comparison("Music", ExpOption[i], "5.0","200", bigramOption[j], k)

                p2=draw_relative_year_comparison("Movies_and_TV", ExpOption[i], "12.0", "200", bigramOption[j], k)

                p3=draw_relative_year_comparison("Books", ExpOption[i], "9.0", "200", bigramOption[j], k)

                p4=draw_relative_year_comparison("Beer_Advocate", ExpOption[i], "4.2", "200", bigramOption[j], k)

                p5=draw_relative_year_comparison("Rate_Beer", ExpOption[i], "6.0", "200", bigramOption[j], k)

                p6=draw_relative_year_comparison("reddit_funny", ExpOption[i], "6.0", "200", bigramOption[j], k)
                
                p7=draw_relative_year_comparison("reddit_politics", ExpOption[i], "9.0", "200", bigramOption[j], k)
                
                p8=draw_relative_year_comparison("reddit_Music", ExpOption[i], "4.5", "200", bigramOption[j], k)

                p9=draw_relative_year_comparison("reddit_leagueoflegends", ExpOption[i], "9.0", "200", bigramOption[j], k)
                p10=draw_relative_year_comparison("reddit_gaming", ExpOption[i], "7.0", "200", bigramOption[j], k)
                p11=draw_relative_year_comparison("reddit_movies", ExpOption[i], "6.3", "200", bigramOption[j], k)
                p12=draw_relative_year_comparison("reddit_books", ExpOption[i], "6.5", "200", bigramOption[j], k)
                if(k == 30) {
                    png(paste("~/Pictures/today/Finalized/", ExpOption[i], "_",bigramOption[j], "_", k,".png", sep=""), width= 1100, height = 1200, res = 60)
                    p=grid.arrange(p1, p2, p3, p4, p5, p6, p7, p8, p9, p10, p11, p12, left = "", nrow=4)
                    dev.off()
                }
                if(j==1) {
                    draw_relative_year_comparison("reddit_posts_funny", ExpOption[i], "6.0", "200", bigramOption[j], k)
                    draw_relative_year_comparison("reddit_posts_politics", ExpOption[i], "9.0", "200", bigramOption[j], k)
                    draw_relative_year_comparison("reddit_posts_Music", ExpOption[i], "4.5", "200", bigramOption[j], k)
                    draw_relative_year_comparison("reddit_posts_leagueoflegends", ExpOption[i], "9.0", "200", bigramOption[j], k)
                    draw_relative_year_comparison("reddit_posts_gaming", ExpOption[i], "7.0", "200", bigramOption[j], k)
                    draw_relative_year_comparison("reddit_posts_movies", ExpOption[i], "6.3", "200", bigramOption[j], k)
                    draw_relative_year_comparison("reddit_posts_books", ExpOption[i], "6.5", "200", bigramOption[j], k)
                }
            }

        }
    }
}
draw_all()

########################################################################################## DBLP VERSION
library(ggplot2)
library(grid)
library(gridExtra)
draw_relative_year_comparison <- function(dataset, final, coeff, bucket_size) {
    if(coeff == "0.05"){
        return(NULL)
    }
    print(paste(dataset, coeff,  final, bucket_size, sep=" "))
    t = read.table(paste("~/Documents/Amazon/Output_All/", dataset, "_bursts/RealTime/MaxBenefit/", 
                dataset, "_mlf_words_", final, "_relative_year_usage_bucketed_", bucket_size ,"_median_comparison_500_innovations_coeff", coeff, ".txt", sep=""), header=TRUE)
    t = t[-1, ]
    print("Hello")
    if(nrow(t) == 0) {
        print("WTF")
        return(NULL)
    }
    print("HOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOORAY")
    linesize=1
    if(final=="final") {
        t$Start_week = t$Start_week - 100
    } else {
        colnames(t)=c("Start_week", "Average", "Fraction")
    }
    t = t[t$Start_week > -15 & t$Start_week < 20, ]
    p <- ggplot() + 
         theme(  legend.title=element_blank(), legend.position = "none",
                 axis.text.x=element_text(color="black", size=12),  axis.text.y=element_text(color="black", size=12)) +
         geom_line(data=t, aes(x=Start_week, y = Fraction, colour = "Innovation Median"), size=linesize) +
         xlab("Relative Year") +
         ylab("Median Fraction Score") + ggtitle(paste(dataset, final)) +
         ylim(0.20,0.80) +
         scale_color_manual(values=c("Innovation Median"="black")) +
         geom_hline(aes(yintercept=0.5), color="red") + geom_vline(aes(xintercept=0), color="red")
    return(p)
}

p1=draw_relative_year_comparison("DBLP", "current", "6.0", "100")
p2=draw_relative_year_comparison("DBLP", "final", "6.0", "100")
png(paste("~/Pictures/today/Finalized/DBLP.png", sep=""), width= 650, height = 300, res = 60)
p=grid.arrange(p1, p2, left = "", nrow=1)
dev.off()
