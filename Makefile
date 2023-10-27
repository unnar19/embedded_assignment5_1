all: part2 part3

part2: main2.cpp
	g++ -o main2 main2.cpp 

part3: main3.cpp
	g++ -o main3 main3.cpp -lpthread