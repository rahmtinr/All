draw_freq_coeff <- function(dataset, coeff) {
    t = read.table(paste("~/Desktop/InnovationWords/", dataset,"/", dataset, "_words_start_burst_freq_coeff_",coeff, ".txt", sep=""), header=FALSE)
    colnames(t) = c("Word", "Week", "Freq")
    name = paste("~/Pictures/today/", dataset, "_freq_coeff_", coeff, ".png"  ,sep="")
    png(file = name)
    t = t[order(t$Freq,decreasing=TRUE), ]
    head(t)
    rownames(t) = 1:nrow(t)
    head(t)
    plot(log(t$Freq), type="o", col="blue", xlab="log(Frequency)")
    legend("topright", col = c("blue"), legend = c("log Frequency"), lty = c(1,1,1))
    dev.off()
}

draw_freq <- function(dataset) {
    draw_freq_coeff(dataset, "3.0")
    draw_freq_coeff(dataset, "3.1")
    draw_freq_coeff(dataset, "3.2")
    draw_freq_coeff(dataset, "3.3")
    draw_freq_coeff(dataset, "3.4")
    draw_freq_coeff(dataset, "3.5")
    draw_freq_coeff(dataset, "3.6")
    draw_freq_coeff(dataset, "3.7")
    draw_freq_coeff(dataset, "3.8")
    draw_freq_coeff(dataset, "3.9")
    draw_freq_coeff(dataset, "4.0")
    draw_freq_coeff(dataset, "4.1")
    draw_freq_coeff(dataset, "4.2")
    draw_freq_coeff(dataset, "4.3")
    draw_freq_coeff(dataset, "4.4")
    draw_freq_coeff(dataset, "4.5")
}

draw_all <-function() {
    draw_freq("Music")
    draw_freq("Movies_and_TV")
    draw_freq("Books")
    draw_freq("Electronics")
    draw_freq("Rate_Beer")
    draw_freq("Beer_Advocate")
}


