library(ggplot2)
library(scales)
library(gridExtra)
library(grid)

draw_cristian_plot <- function(dataset_name, dataset, coeff) {
    t = read.table(paste("~/Documents/Amazon/Output_All/",dataset,"_bursts/RealTime/MaxBenefit/",dataset, "_mlf_cristian_comparison_coeff_", coeff, ".txt", sep=""), header=TRUE)    
    colnames(t) = c("V1", "V2", "V3")

    t1 = t[(t$V1 == 1),]
    t2 = t[(t$V1 == 2),]
    t3 = t[(t$V1 == 3),]
    t4 = t[(t$V1 == 4),]

    maxi = 0
    maxi = max(t1$V3, maxi)
    maxi = max(t2$V3, maxi)
    maxi = max(t3$V3, maxi)
    maxi = max(t4$V3, maxi)
    linesize = 1
    p <- ggplot() +
            theme(legend.position = "none", axis.text.x=element_text(color="black"),  axis.text.y=element_text(color="black")) +
            geom_line(data=t1, aes(x=V2, y = V3, colour = "First quantile"), size=linesize) +
            geom_line(data=t2, aes(x=V2, y = V3, colour = "Second quantile"), size=linesize) +
            geom_line(data=t3, aes(x=V2, y = V3, colour = "Third quantile"), size=linesize) +
            geom_line(data=t4, aes(x=V2, y = V3, colour = "Fourth quantile"), size=linesize) +
            xlab("Current Experience") +
            ylab("Innovation Per Document") + ggtitle(dataset_name) +
            scale_color_manual(values=c("First quantile"="blue", "Second quantile"="green", "Third quantile"="red",
                                "Fourth quantile"="cyan"))
    name = paste("~/Pictures/today/Cristian/", dataset, "_mlf_cristian_comparison_coeff_", coeff, ".png"  ,sep="")
    png(file = name, width=400, height=400, res=60)
    print(p)
    dev.off()
    return(p)
}


get_legend=function(myplot) {
    tmp <- ggplot_gtable(ggplot_build(myplot))
        print("DAFUCK")
    leg <- which(sapply(tmp$grobs, function(x) x$name) == "guide-box")
        print("DAFUCK")
    legend <- tmp$grobs[[leg]]
        print("DAFUCK")
    return (legend)
}
draw_all <-function() {
    p1=draw_cristian_plot("Amazon Music", "Music",  "5.0") +
        theme(legend.title=element_blank(),
              legend.position = "right",
              legend.text = element_text(size=14),
              legend.background = element_rect(fill=alpha(0.7)))

#    p2=draw_cristian_plot("Movies_and_TV",  "12.0")

    p3=draw_cristian_plot("Amazon Books", "Books",  "9.0")

#    p4=draw_cristian_plot("Beer_Advocate",  "4.2")

#    p5=draw_cristian_plot("Rate_Beer",  "6.0")

#   p6=draw_cristian_plot("reddit_funny",  "6.0")

    p7=draw_cristian_plot("Reddit gaming", "reddit_gaming",  "7.0")

#    p8=draw_cristian_plot("reddit_politics",  "9.0")
    
#    p9=draw_cristian_plot("reddit_Music",  "4.5")
    
#    p10=draw_cristian_plot("reddit_leagueoflegends",  "9.0")

#    p11=draw_cristian_plot("reddit_books",  "6.5")

#    p12=draw_cristian_plot("reddit_movies",  "6.3")


    print("KIR")
    my_legend = get_legend(p1)
    print("KIR")
    p1 = p1 + theme(legend.position="none")
    print("KIR")
    png("~/Pictures/ICWSM2016/Cristian.png", width= 1100, height = 300, res = 60)
    print("KIR")
    grid.arrange(arrangeGrob(p1, p3, p7,
                             left = "", nrow=1), my_legend, 
                widths=unit(c(1000,175),"pt"), nrow=1)
    dev.off()
}

draw_all()

