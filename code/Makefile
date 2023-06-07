ALL: main
main: main.o
	@g++ -o cli network_part1.o -lpthread 
	@g++ -o serv server.o -lpthread 
main.o: network_part1.cpp
	@g++ -c network_part1.cpp
	@g++ -c server.cpp
