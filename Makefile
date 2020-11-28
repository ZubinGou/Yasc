.PHONY: rc
.PHONY: ll1

rc: RC# build & run
	./RC

RC:	Lexer.cpp RC.cpp # build
	g++ Lexer.cpp RC.cpp -o RC

ll1: LL1
	./ll1

LL1: LL1.cpp LL1_driver.cpp Lexer.cpp
	g++ LL1.cpp LL1_driver.cpp Lexer.cpp -o LL1

