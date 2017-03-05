all: main

svdcmp.o:	svdcmp.c
	gcc -c -g svdcmp.c 

svbksb.o:	svbksb.c
	gcc -c -g svbksb.c

solve_system.o:		svdcmp.o svbksb.o solve_system.c 
	gcc -c -g solve_system.c

main:	solve_system.o main.cpp
	g++ --std=c++0x -g -Wno-deprecated -o PA2 svdcmp.o svbksb.o solve_system.o main.cpp
