library("gplots")
library("reshape2")
library("ggplot2")
library("scales")

draw_heat_map <- function(dataset, final) {
    mini = 1000
    a_b = read.table(paste("~/Documents/Amazon/Output_All/",dataset, "_bursts/RealTime/MaxBenefit/",dataset,"_", final, "_[a,b]_top_500_innovations_coeff3.txt", sep=""), header=TRUE)
    mat = matrix(rep(c(0), 201 * 201), nrow=201, ncol=201)
    for(i in 1:nrow(a_b)) { 
        x = c(0, 0.25, 0.5, 0.75, 1) 
        y = c(0, a_b[i, ]$cq1, a_b[i, ]$cq2, a_b[i, ]$cq3, a_b[i, ]$cq4)
        maxi = 0
        maxi = max(y)
        y = y / maxi
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
    ggplot(dat, aes(x=a,y=b)) + 
    geom_tile(aes(fill = value),colour='black') +
    scale_fill_gradientn(colours=c("green","yellow","red")) +  
    coord_equal() 
#    dev.off()
}

draw_heat_map("Music", "final")
draw_heat_map("Movies_and_TV", "final")
draw_heat_map("Books", "final")

