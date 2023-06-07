# Multy-Thread-Client-Server-Banking-Application
Developed Linux-based Multy-Thread client-server banking application using C++ and socket

compile: 
         
         g++ -o cli network_part1.cpp -lpthread 

         g++ -o serv server.cpp -lpthread 
         
execute: 
         
         ./serv then enter the port.

         ./cli then enter ip and port (ie: 0.0.0.0:6666)
         
then input the option (r=register, l =login, p= list, e= exit, t= p2p) #list means print your account information, t means connect with other clients and trandfer money to them.

after r option you need to input your name

after l option you need to input your name then input your port

after t option you need to input payeename and the amount

others don't need to input argument

linux environment

#include<ctype.h>

#include<iostream>
         
#include<stdlib.h>
         
#include<string.h>

#include<string>
         
#include<utility>
         
#include<vector>
         
#include <sys/select.h>
         
#include <sys/types.h>
         
#include <sys/socket.h>
         
#include <netdb.h>
         
#include<arpa/inet.h> //inet_addr
         
#include<sys/fcntl.h>
         
#include <unistd.h>
         
#include <math.h>
         
#include<pthread.h>
