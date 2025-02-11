.PHONY: rc
.PHONY: ll1
.PHONY: lr1

rc: RC		# build & run
	./RC

RC:	Lexer.cpp RC.cpp # build
	g++ Lexer.cpp RC.cpp -o RC

ll1: LL1	# build & run
	./LL1

LL1: LL1.cpp Lexer.cpp	# build
	g++ LL1.cpp Lexer.cpp -o LL1

lr1: LR1	# build & run
	./LR1

LR1: LR1.cpp Lexer.cpp	# build
	g++ LR1.cpp Lexer.cpp -o LR1