library("gplots")
library("reshape2")
library("ggplot2")
library("scales")

draw_heat_map <- function(dataset) {
    a_b = read.table(paste("~/Documents/Amazon/Output_All/",dataset, "_bursts/RealTime/MaxBenefit/",dataset,"_final_a_b_top_500_innovations.txt", sep=""), header=TRUE)
    mat = matrix(rep(c(0), 201 * 201), nrow=201, ncol=201)
    for(i in 1:nrow(a_b)) { 
        x = c(0, 0.25, 0.5, 0.75, 1) 
        y = c(0, a_b[i, ]$cq1, a_b[i, ]$cq2, a_b[i, ]$cq3, a_b[i, ]$cq4)
        K = lm(y ~ x)
        a = a_b[i, ]$a + 100
        b = a_b[i, ]$b + 100
        mat[a, b] = K$coefficients[2]
    }
    print("a_b_final_count")

    print("________________________________")
    smoothed = mat 
    print(paste("~/Pictures/", dataset, "_a_b_top_500_innovations.png", sep=""))
#    png(file=paste("~/Pictures/", dataset, "_a_b_top_500_innovations.png", sep=""), height = 1600, width = 1600, units="px", res = 150)
    dat <- melt(smoothed)
    colnames(dat) = c("a", "b", "value")
    dat[dat$a >= dat$b, "value"] = -2 
    ggplot(dat, aes(x=a,y=b)) + 
    geom_tile(aes(fill = value),colour='black') +
    scale_fill_gradientn(colours=c("green","yellow","red")) +  
    coord_equal() 
#    dev.off()
}

draw_heat_map("Music")
draw_heat_map("Movies_and_TV")
draw_heat_map("Books")

