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
#include<pthread.h> //for threading , link with lpthread
using namespace std;
int islogin=0;
char* regist="REGISTER#";
char* list1="List";
int p2ping=0;
bool sendtop2p=0;
int fd;
char* exit1="Exit";
const char sep[3]="#\n";
struct db
{	
	char* name;
	char* cname;
	char* cip;
	char* cport;
	char* amount;
};
struct pair1
{
	char ip[20];
	int sd;
};
vector<pair1> sd2ip;
db sending;
void *send_handler(void *);
void *listen_handler(void *);
void *connection_handler(void *);
int sendmessage(int type,char* name,char* port,char* payeename)
{
	char response[2000]="";
	int bytes_count=0;
	char message[2000]="";//register
	if(type==1)
	{
		strcpy(message,regist);
		strcat(message,name);
		strcat(message,"\n");
		send(fd,message,strlen(message)+1,0);
		sleep(1);
		bytes_count= recv(fd,response,sizeof response,0);
		cout<<response;
	}
	else if(type==2)//login
	{
		strcpy(message,name);
		strcat(message,"#");
		strcat(message,port);
		strcat(message,"\n");

		send(fd,message,strlen(message)+1,0);
		sleep(1);
		bytes_count= recv(fd,response,sizeof response,0);
		printf("%s",response);
		if(!islogin)// listen other client
		{
			islogin=1;
			pthread_t thread_id;
			int socket_desc ;
    			struct sockaddr_in server;
          
    			socket_desc = socket(AF_INET , SOCK_STREAM , 0);
    			if (socket_desc == -1)
    			{
        			printf("Could not create socket");
    			}
    			//Prepare the sockaddr_in structure
    			server.sin_family = AF_INET;
    			server.sin_addr.s_addr = INADDR_ANY;
    			server.sin_port = htons( atoi(port) );
     	
    		//Bind
    			if( bind(socket_desc,(struct sockaddr *)&server , sizeof(server)) < 0)
    			{
   				    //print the error message
    	    		perror("bind failed. Error");
    	    		return 1;
    		}
    		puts("bind done");
    	 
   	 		//Listen
   		 	pthread_create( &thread_id , NULL ,  listen_handler , (void*) &socket_desc );
   		 	
		}
	}
	else if(type==3)//印出
	{
		strcpy(message,list1);
		strcat(message,"\n");
		send(fd,message,strlen(message)+1,0);
		sleep(1);
		bytes_count= recv(fd,response,sizeof response,0);
		printf("%s",response);
	}
	else if(type==4)//登出
	{
		strcpy(message,exit1);
		strcat(message,"\n");
		send(fd,message,strlen(message)+1,0);
		sleep(1);
		bytes_count= recv(fd,response,sizeof response,0);
		
		cout<<response;
		return 0;
	}
	else if(type==5)//micro
	{
		strcpy(message,name);
		strcat(message,"#");
		strcat(message,port);
		strcat(message,"\n");
		send(fd,message,strlen(message)+1,0);
		sleep(1);
		bytes_count= recv(fd,response,sizeof response,0);
		printf("%s",response);
	}
	else if(type==6)//p2p
	{
		strcpy(message,list1);
		strcat(message,"\n");
		send(fd,message,strlen(message)+1,0);
		sleep(1);
		bytes_count= recv(fd,response,sizeof response,0);
		string a(response);
		string cname;
		string cport;
		string cip;
		bool find=0;
		int current=0;
		int next;
		int i=0;
		next=a.find_first_of("\n",current);//balance
		current=next+1;
		next=a.find_first_of("\n",current);//num of account
		current=next+1;
		while(1)
		{
			next=a.find_first_of("#\n",current);
			if(next!=current)
				cname=a.substr(current,next-current);
			cout<<cname<<"\n";
			current=next+1;
			if(strcmp(cname.c_str(),payeename)==0)
			{
				find=1;
				break;
			}
			if(next==string::npos)
				break;
		}
		if(find!=1)
			cout<<"error"<<payeename<<"\n";
		else
		{
			next=a.find_first_of("#",current);
			if(next!=current)
				cip=a.substr(current,next-current);
			current=next+1;
			next=a.find_first_of("\n",current);
			if(next!=current)
				cport=a.substr(current,next-current);
			sending.cname=const_cast<char*>(cname.c_str());
			sending.cport=const_cast<char*>(cport.c_str());
			sending.cip=const_cast<char*>(cip.c_str());
			sending.name=name;
			sending.amount=port;
			pthread_t thread_id;
			pthread_create( &thread_id , NULL ,  send_handler ,NULL );
		}
		
	}
	return 1;
}
int main()
{
	char trash[200];
	int isexit=1;	
	int len=0;
	char temp_addr[2500];
	cin>>temp_addr;
	char ipstr[100];
	int status;
	struct addrinfo hints;
	struct addrinfo *res; // 將指向結果
	char addr[2049];
	char port[16];
	char name[2000]="";
	int post=0;
	for(int j=strlen(temp_addr);j>0;j--)
	{
		if(temp_addr[j-1]==':')
		{
			post=j-1;
			break;
		}
	}
	for (int j = 0; j < post; ++j)
	{
		addr[j]=temp_addr[j];
	}
	for (int j = 0; j < strlen(temp_addr)-post-1; ++j)
	{
		port[j]=temp_addr[j+post+1];
	}
	memset(&hints, 0, sizeof hints); // 確保 struct 為空
	hints.ai_family = AF_UNSPEC; // 不用管是 IPv4 或 IPv6
	hints.ai_socktype = SOCK_STREAM; // TCP stream sockets
	// 準備好連線
	status = getaddrinfo(addr,port , &hints, &res);
	fd=socket(res->ai_family, res->ai_socktype, res->ai_protocol);
	fcntl(fd, F_SETFL, O_NONBLOCK);
//	bind(fd, res->ai_addr, res->ai_addrlen);
	int c;
	c=connect(fd, res->ai_addr, res->ai_addrlen);
	sleep(0.1);
	char payeename[2000]="";
	while(isexit)
	{
		char temp[10];
		int n=0;
		int oargptr=1;
		char portnum[20];
		struct sockaddr_in svr;
		int p_type=0;
		cin>>temp;
		if(strcmp(temp,"r")==0)//註冊
		{
			if(islogin)
				continue;
			p_type=1;
			cin>>name;
		}
		else if(strcmp(temp,"l")==0)//登入
		{
			if(islogin)
				continue;
			p_type=2;
			cin>>name;
			cin>>portnum;
		}
		else if(strcmp(temp,"p")==0)//印出資料
		{
			p_type=3;
		}
		else if(strcmp(temp,"e")==0)//登出
		{
			p_type=4;
		}
		else if(strcmp(temp,"m")==0)//micropayment
		{	
			p_type=5;
		}
		else if(strcmp(temp,"t")==0)//p2p
		{
			p_type=6;
			cin>>payeename;
			cin>>portnum;//amount
		}
		isexit=sendmessage(p_type,name,portnum,payeename);
	}
	close(fd);
	return 0;
};
void *listen_handler(void *socket_desc)
{
	pthread_t thread_id;
	struct sockaddr_in  client;
    int  client_sock , c;
	c = sizeof(struct sockaddr_in);
	int sock = *(int*)socket_desc;
	listen(sock , 10);
    while( (client_sock = accept(sock, (struct sockaddr *)&client, (socklen_t*)&c)) )
    {
		p2ping+=1;
        char* cip=inet_ntoa(client.sin_addr);
      	struct  pair1 a;
		strcpy(a.ip,cip);
        puts("Connection accepted");
		a.sd=client_sock;
		sd2ip.push_back(a);
        if( pthread_create( &thread_id , NULL ,  connection_handler , (void*) &client_sock) < 0)
        {
            perror("could not create thread");
            break;
        }
        //Now join the thread , so that we dont terminate before the thread
        //pthread_join( thread_id , NULL);
    }
    return 0;
}	

void *connection_handler(void *socket_desc)
{
    //Get the socket descriptor
    char cip[20];
    int num;
    int sock = *(int*)socket_desc;
    int read_size;
    char message[2000] ;
	char client_message[2000];
    for(int i=0;i<sd2ip.size();++i)
    {
    	if(sd2ip[i].sd==sock)
    	{
    		strcpy(cip,sd2ip[i].ip);
    	}
    }
    //Receive a message from client
	memset(client_message, 0, 2000);
    while( (read_size = recv(sock , client_message , sizeof client_message , 0)) > 0 )
    {
        char sendmessage[2000];
        memset(sendmessage, 0, strlen(sendmessage));
        cout<<client_message;
        send(fd,client_message,strlen(client_message)+1,0);
		memset(client_message, 0, 2000);
    }
     
    if(read_size == 0)
    {
        puts("Client disconnected");
        fflush(stdout);
    }
    else if(read_size == -1)
    {
        perror("recv failed");
    }
    p2ping--;
    return 0;
} 
void *send_handler(void * n)
{
char* trash;

	struct addrinfo hints;
	struct addrinfo *res; // 將指向結果
	memset(&hints, 0, sizeof hints); // 確保 struct 為空
	hints.ai_family = AF_UNSPEC; // 不用管是 IPv4 或 IPv6
	hints.ai_socktype = SOCK_STREAM; // TCP stream sockets
	// 準備好連線
	int fd;
	int status = getaddrinfo(sending.cip,sending.cport , &hints, &res);
	fd=socket(res->ai_family, res->ai_socktype, res->ai_protocol);
	fcntl(fd, F_SETFL, O_NONBLOCK);
//	bind(fd, res->ai_addr, res->ai_addrlen);
	int c;
	c=connect(fd, res->ai_addr, res->ai_addrlen);
	sleep(1);
	recv(fd,trash,sizeof trash,0);
	cout<<trash;
	char message[2000]="";
	cout<<4<<"\n";
	strcpy(message,sending.name);
	
	strcat(message,"#");
	strcat(message,sending.amount);
	strcat(message,"#");
	strcat(message,sending.cname);
	strcat(message,"\n");
	send(fd,message,strlen(message)+1,0);
	sleep(1);
	return 0;
}
