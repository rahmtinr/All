attach(mtcars)
par(mfrow=c(4,3))
myTable <- 1:720
dim(myTable) <- c(20,3,12)
temp1 = read.table("C:/cygwin64/home/rahmtin/research/SecondYear/Amazon/Output_All/Jan_star_rating_Software_has_error.txt",head=FALSE)
temp2 = read.table("C:/cygwin64/home/rahmtin/research/SecondYear/Amazon/Output_All/Feb_star_rating_Software_has_error.txt",head=FALSE)
temp3 = read.table("C:/cygwin64/home/rahmtin/research/SecondYear/Amazon/Output_All/Mar_star_rating_Software_has_error.txt",head=FALSE)
temp4 = read.table("C:/cygwin64/home/rahmtin/research/SecondYear/Amazon/Output_All/Apr_star_rating_Software_has_error.txt",head=FALSE)
temp5 = read.table("C:/cygwin64/home/rahmtin/research/SecondYear/Amazon/Output_All/May_star_rating_Software_has_error.txt",head=FALSE)
temp6 = read.table("C:/cygwin64/home/rahmtin/research/SecondYear/Amazon/Output_All/June_star_rating_Software_has_error.txt",head=FALSE)
temp7 = read.table("C:/cygwin64/home/rahmtin/research/SecondYear/Amazon/Output_All/July_star_rating_Software_has_error.txt",head=FALSE)
temp8 = read.table("C:/cygwin64/home/rahmtin/research/SecondYear/Amazon/Output_All/Aug_star_rating_Software_has_error.txt",head=FALSE)
temp9 = read.table("C:/cygwin64/home/rahmtin/research/SecondYear/Amazon/Output_All/Sep_star_rating_Software_has_error.txt",head=FALSE)
temp10 = read.table("C:/cygwin64/home/rahmtin/research/SecondYear/Amazon/Output_All/Oct_star_rating_Software_has_error.txt",head=FALSE)
temp11 = read.table("C:/cygwin64/home/rahmtin/research/SecondYear/Amazon/Output_All/Nov_star_rating_Software_has_error.txt",head=FALSE)
temp12 = read.table("C:/cygwin64/home/rahmtin/research/SecondYear/Amazon/Output_All/Dec_star_rating_Software_has_error.txt",head=FALSE)

temp = temp1
min_bound = 2.5
max_bound = 5
plot(temp$V2, main="Star rating", xlab="Month", ylab="Star rating average", ylim=c(min_bound,max_bound), xaxt='n')
axis(side = 1, at = 1:dim(temp)[1],labels = temp$V1)
arrows(1:dim(temp)[1],temp$V3+temp$V2,1:dim(temp)[1],temp$V2-temp$V3,angle=90,length=0.1,code=3)
#lines(temp$V2, main="Overall monthly accumulated star rating Software has error", xlab="Month", ylab="Star rating average", ylim=c(min(-temp$V3+temp$V2),max(temp$V2+temp$V3)), xaxt='n')

drawformonth <- function(monthname){
	temp = read.table(paste("C:/cygwin64/home/rahmtin/research/SecondYear/Amazon/Output_All/",monthname,"_star_rating_Software_has_error.txt", sep=""),head=FALSE)
	min_bound = 2.5
	max_bound = 5
	plot(temp$V2, main="Star rating", xlab="Month", ylab="Star rating average", ylim=c(min_bound,max_bound), xaxt='n')
	axis(side = 1, at = 1:dim(temp)[1],labels = temp$V1)
	arrows(1:dim(temp)[1],temp$V3+temp$V2,1:dim(temp)[1],temp$V2-temp$V3,angle=90,length=0.1,code=3)
	#lines(temp$V2, main="Overall monthly accumulated star rating Software has error", xlab="Month", ylab="Star rating average", ylim=c(min(-temp$V3+temp$V2),max(temp$V2+temp$V3)), xaxt='n')
}


 