P3.out : Project3.o SetLimits.o LexicalAnalyzer.o SyntacticalAnalyzer.o CodeGenerator.o
	g++ -g -o P3.out Project3.o SetLimits.o LexicalAnalyzer.o SyntacticalAnalyzer.o CodeGenerator.o

test.out : test.o Object.o
	g++ -g -o test.out test.o Object.o

Project3.o : Project3.cpp SetLimits.h SyntacticalAnalyzer.h CodeGenerator.h
	g++ -g -c -std=c++11 Project3.cpp

Object.o : Object.cpp Object.h
	g++ -g -c -std=c++11 Object.cpp

SetLimits.o : SetLimits.cpp SetLimits.h
	g++ -g -c -std=c++11 SetLimits.cpp

LexicalAnalyzer.o : LexicalAnalyzer.cpp LexicalAnalyzer.h
	g++ -g -c -std=c++11 LexicalAnalyzer.cpp

CodeGenerator.o : CodeGenerator.cpp CodeGenerator.h LexicalAnalyzer.h Object.h
	g++ -g -c -std=c++11 CodeGenerator.cpp

SyntacticalAnalyzer.o : SyntacticalAnalyzer.cpp SyntacticalAnalyzer.h LexicalAnalyzer.h
	g++ -g -c -std=c++11 SyntacticalAnalyzer.cpp

clean : 
	rm -rf *.o *.out *.gch *.p1 *.p2 *.lst *.dbg

submit : Project3.cpp LexicalAnalyzer.h LexicalAnalyzer.cpp SyntacticalAnalyzer.h SyntacticalAnalyzer.cpp makefile SetLimits.h SetLimits.cpp CodeGenerator.h CodeGenerator.cpp Object.h Object.cpp
	rm -rf Team6P3
	mkdir Team6P3
	cp Project3.cpp Team6P3
	cp LexicalAnalyzer.h Team6P3
	cp LexicalAnalyzer.cpp Team6P3
	cp SyntacticalAnalyzer.h Team6P3
	cp SyntacticalAnalyzer.cpp Team6P3
	cp Object.h Team6P3
	cp Object.cpp Team6P3
	cp SetLimits.h Team6P3
	cp SetLimits.cpp Team6P3
	cp CodeGenerator.h Team6P3
	cp CodeGenerator.cpp Team6P3
	cp makefile Team6P3
	tar cfvz Team6P3.tgz Team6P3
	cp Team6P3.tgz ~tiawatts/cs460drop
