library("gplots")
library("reshape2")
library("ggplot2")
library("scales")

draw_heat_map <- function(dataset, final, coeff) {
    dev.new()
    mini = 1000
    a_b = read.table(paste("~/Documents/Amazon/Output_All/",dataset, "_bursts/RealTime/MaxBenefit/",dataset,"_", final, "_[a,b]_top_500_innovations_coeff", coeff, "_4parts.txt", sep=""), header=TRUE)
    mat = matrix(rep(c(0), 200 * 199), nrow=200, ncol=199)
    for(i in 1:nrow(a_b)) { 
        x = c(0, 0.25, 0.5, 0.75, 1) 
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
    ggplot(dat, aes(x=a,y=b)) + 
    geom_tile(aes(fill = value),colour='black') +
    scale_fill_gradientn(colours=c("green","yellow","red")) +  
    ggtitle(paste(dataset, final, "coeff", coeff, "4parts", sep=" ")) + 
    coord_equal() 
#    dev.off()
}

draw_heat_map("Music", "final", "3.5")
draw_heat_map("Music", "current", "3.5")

draw_heat_map("Movies_and_TV", "final", "3.3")
draw_heat_map("Movies_and_TV", "final", "4.2")
draw_heat_map("Movies_and_TV", "current", "3.3")
draw_heat_map("Movies_and_TV", "current", "4.2")

draw_heat_map("Books", "final", "3.5")
draw_heat_map("Books", "final", "4.0")
draw_heat_map("Books", "current", "3.5")
draw_heat_map("Books", "current", "4.0")

draw_heat_map("Beer_Advocate", "final", "3.7")
draw_heat_map("Beer_Advocate", "current", "3.7")


draw_heat_map("Rate_Beer", "final", "3.4")
draw_heat_map("Rate_Beer", "final", "3.5")
draw_heat_map("Rate_Beer", "current", "3.4")
draw_heat_map("Rate_Beer", "current", "3.5")

draw_heat_map("Electronics", "final", "4.2")
draw_heat_map("Electronics", "final", "4.1")
draw_heat_map("Electronics", "current", "4.2")
draw_heat_map("Electronics", "current", "4.1")

