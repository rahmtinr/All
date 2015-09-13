CC = g++ 
CFLAGS = -g -std=c++11 -Wall -O2
LDFLAGS = -lm


all : main.o MyUtility.o Reviews.o AmazonUtility.o ReviewReader.o Innovations.o Videos.o ProductRelated.o UserRelated.o Aggregation.o Seasonal.o
	${CC} -o main main.o ${CFLAGS}


empty2009 : empty2009.o MyUtility.o Reviews.o AmazonUtility.o ReviewReader.o Innovations.o Videos.o ProductRelated.o UserRelated.o Aggregation.o Seasonal.o
	${CC} -o empty2009 Empty2009.o ${CFLAGS}

DBLPFindInnovation : DBLPFindInnovation.o MyUtility.o Reviews.o AmazonUtility.o ReviewReader.o Innovations.o Videos.o ProductRelated.o UserRelated.o Aggregation.o Seasonal.o
	${CC} -o dblpFindInnovation DBLPFindInnovationMain.o ${CFLAGS}

DBLPAnalysis: DBLPAnalysis.o MyUtility.o Reviews.o AmazonUtility.o ReviewReader.o Innovations.o Videos.o ProductRelated.o UserRelated.o Aggregation.o Seasonal.o
	${CC} -o dblpAnalysis DBLPAnalysisMain.o ${CFLAGS}

AmazonAnalysis: AmazonAnalysis.o MyUtility.o Reviews.o AmazonUtility.o ReviewReader.o Innovations.o Videos.o ProductRelated.o UserRelated.o Aggregation.o Seasonal.o
	${CC} -o amazonAnalysis AmazonAnalysisMain.o ${CFLAGS}

minLife : minLife.o MyUtility.o Reviews.o AmazonUtility.o ReviewReader.o Innovations.o Videos.o ProductRelated.o UserRelated.o Aggregation.o Seasonal.o
	${CC} -o minLife AmazonLifeSpanWords.o ${CFLAGS}

main.o : main.cpp MyUtility.o Reviews.o AmazonUtility.o ReviewReader.o Innovations.o Videos.o
	${CC} -c main.cpp ${CFLAGS}

empty2009.o : Empty2009.cpp MyUtility.o Reviews.o AmazonUtility.o ReviewReader.o Innovations.o Videos.o
	${CC} -c Empty2009.cpp ${CFLAGS}
 
AmazonAnalysis.o : AmazonAnalysisMain.cpp MyUtility.o Reviews.o AmazonUtility.o ReviewReader.o Innovations.o Videos.o
	${CC} -c AmazonAnalysisMain.cpp ${CFLAGS}

DBLPFindInnovation.o : DBLPFindInnovationMain.cpp MyUtility.o Reviews.o AmazonUtility.o ReviewReader.o Innovations.o Videos.o
	${CC} -c DBLPFindInnovationMain.cpp ${CFLAGS}

DBLPAnalysis.o : DBLPAnalysisMain.cpp MyUtility.o Reviews.o AmazonUtility.o ReviewReader.o Innovations.o Videos.o
	${CC} -c DBLPAnalysisMain.cpp ${CFLAGS}


minLife.o : AmazonLifeSpanWords.cpp MyUtility.o Reviews.o AmazonUtility.o ReviewReader.o Innovations.o Videos.o
	${CC} -c AmazonLifeSpanWords.cpp ${CFLAGS}

ReviewReader.o : ReviewReader.h AmazonUtility.h
	${CC} -c ReviewReader.h ${CFLAGS}

AmazonUtility.o : AmazonUtility.h Reviews.h
	${CC} -c AmazonUtility.h ${CFLAGS} 

MyUtility.o : MyUtility.h
	${CC} -c MyUtility.h ${CFLAGS} 


Reviews.o : Reviews.h MyUtility.h
	${CC} -c Reviews.h ${CFLAGS} 

Innovations.o : Innovations.h Reviews.h MyUtility.h UserRelated.h
	${CC} -c Innovations.h ${CFLAGS}

Videos.o : Videos.h Reviews.h
	${CC} -c Videos.h ${CFLAGS}

ProductRelated.o : ProductRelated.h Reviews.h MyUtility.h
	${CC} -c ProductRelated.h ${CFLAGS}

UserRelated.o : UserRelated.h Reviews.h MyUtility.h
	${CC} -c UserRelated.h ${CFLAGS}
	
Aggregation.o : Aggregation.h Reviews.h MyUtility.h
	${CC} -c Aggregation.h ${CFLAGS}
	
Seasonal.o : Seasonal.h Reviews.h MyUtility.h
	${CC} -c  Seasonal.h ${CFLAGS}
