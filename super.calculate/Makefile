CC := clang++
CFLAGS := -std=c++17 -O2 -lm # without debug
TARGET := ./bin/compiler

all: run

src/main.lex.yy.c: src/main.lex
	 flex --noyywrap -o src/main.lex.yy.cpp  src/main.lex

src/main.tab.c: src/main.y
	bison -o src/main.tab.cpp --defines=src/main.tab.h -v src/main.y

src/pch.h.gch: src/pch.h
	$(CC) $(CFLAGS) -x c++-header -o src/pch.h.gch -c src/pch.h

lex: src/main.lex.yy.c

yacc: src/main.tab.c

main: src/pch.h.gch
	$(CC) $(CFLAGS) $(shell ls ./src/*.cpp) -I ./src/ -o $(TARGET)
	cp $(TARGET) ./x86_tests/main.out

.PHONY: all clean main run lex yacc test debug link testscope asm nasm example-code out

run: lex yacc main

test1:run
	$(TARGET) -S -o test/test.s test/test.c
	gcc test/test.c -S -o test/test_std.s -m32


test2:test1
	gcc test/test.c -o test/test_std.out -m32
	gcc test/test.s -o test/test.out -m32 
	test/test_std.out <test/test.in >test/test_std.output 
	test/test.out <test/test.in >test/test.output

arm_test1:run
	$(TARGET) -S -o test/test.s test/test.c
	arm-linux-gnueabi-gcc -o test/test_std.s -S -O0 test/test.c 

arm_test2:arm_test1
	arm-linux-gnueabi-gcc test/test.c -o test/test_std.out -static
	arm-linux-gnueabi-gcc test/test.s -o test/test.out -static
	test/test_std.out <test/test.in >test/test_std.output 
	test/test.out <test/test.in >test/test.output

test:
	arm-linux-gnueabi-gcc test/test_std.s -o test/test_std.out -static
	test/test_std.out <test/test.in >test/test_std.output 
	arm-linux-gnueabi-gcc test/test.s -o test/test.out -static
	test/test.out <test/test.in >test/test.output 

compile:
	gcc test/test.s -o test/test.out -m32 -m32
	./test/test.out

gdb:
	gcc -g test/test.s -o test/test.out -m32
	gdb test/test.out

clean:
	rm -f src/*.output src/main.lex.yy.cpp src/main.tab.cpp src/main.tab.h src/main.output src/pch.h.gch $(TARGET) *.o ./bin/*
