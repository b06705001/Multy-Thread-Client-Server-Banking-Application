
#include<iostream>
#include<string.h>    //strlen
#include<string>
#include<vector>
#include<ctype.h>
#include<utility>
#include<stdlib.h>    //strlen
#include<sys/socket.h>
#include<arpa/inet.h> //inet_addr
#include<unistd.h>    //write
#include<pthread.h> //for threading , link with lpthread
using namespace std;
/*

        
*/


const int default_balance=10000;
char* message_0="100 OK";
char* message_1="210 FAIL";
char* message_2="220 AUTH_FAIL";
char* message_3="PLEASE LOGIN FIRST";
char* message_4="Bye";
char* message_5="HAS BEEN LOGIN ON OTHER DEVICE, PLEASE LOGOUT FIRST";
char* message_6="ILEGAL COMMAND";
char* message_7="ILEGAL PORT NUM";

class account
{
    public:
        int balance;
        bool login;
        string name;
        string ip;
        string port;
        account();
        account(char* nam);
        bool isregister(char* nam);
        bool isonline();
        void log(char* ip,char* port);
        int showbalance();
        string print();
		string show_name();
        void logout();
		
};
account::account()
{
    balance=default_balance;
    login=false;
}
account::account(char* nam)
{
    string n="";
	n+=nam;
    balance=default_balance;
    login=false;
    this->name=n;
}
bool account::isregister(char* nam)
{
	string n;
	n+=nam;
    if (this->name==n)
    {
        return true;
    }
    return false;
}
bool account::isonline()
{
    if (this->login)
    {
        return true;
    }
    return false;
}
void account::log(char* ip,char* port)
{
    this->login=true;
	string p="";
	p+=port;
	string i="";
	i+=ip;
    this->ip=i;
    this->port=p;
}
int account::showbalance()
{
    return this->balance;
}
string account::print()
{
    string message;
    message+=this->name;
	message+="#";
    message+=this->ip;
    message+="#";
    message+=this->port;
    message+="\n";
    return message;
}
string account::show_name()
{
	return this->name;
}
void account::logout()
{
    this->ip="";
    this->port="";
    this->login=false;
}

struct pair1
{
	char ip[20];
	int sd;
};

vector<account> database;
vector<account> online;
vector<pair1> sd2ip;
void balancechange(char* name,int num)
{
	for(int i=0;i<online.size();i++)
	{
		if(online[i].show_name()==name)
		{
			online[i].balance+=num;
			break;
		}
	} 
	for (int i = 0; i < database.size(); ++i)
    {
        if(database[i].isregister(name))
        {
            if(database[i].isonline())
                database[i].balance+=num;
            return;
        }
    }
}


bool isregistered(char* name)
{
    for (int i = 0; i < database.size(); ++i)
    {
        if(database[i].isregister(name))
            return true;
    }
    return false;
}
int isloginable(char* name)
{
    for (int i = 0; i < database.size(); ++i)
    {
        if(database[i].isregister(name))
        {
            if(database[i].isonline())
                return -i-1;
            return i+1;
        }
    }
    return 0;
}
void remove(string name)
{
	for(int i=0;i<online.size();i++)
	{
		if(online[i].show_name()==name)
		{
			online.erase(online.begin()+i);
			return;
		}
	}
}
void removeip(int s)
{
	for(int i=0;i<sd2ip.size();i++)
	{
		if(sd2ip[i].sd==s)
		{
			sd2ip.erase(sd2ip.begin()+i);
			return;
		}
	}
	
}


void *connection_handler(void *);
int main(int argc , char *argv[])
{
	
    int port=0;
    cout<<"please input the port\n";
    cin>>port;
    int socket_desc , client_sock , c;
    struct sockaddr_in server , client;
          
    socket_desc = socket(AF_INET , SOCK_STREAM , 0);
    if (socket_desc == -1)
    {
        printf("Could not create socket");
    }
    //Prepare the sockaddr_in structure
    server.sin_family = AF_INET;
    server.sin_addr.s_addr = INADDR_ANY;
    server.sin_port = htons( port );
     
    //Bind
    if( bind(socket_desc,(struct sockaddr *)&server , sizeof(server)) < 0)
    {
        //print the error message
        perror("bind failed. Error");
        return 1;
    }
    puts("bind done");
     
    //Listen
    listen(socket_desc , 10);
     
     
    //Accept and incoming connection
    puts("Waiting for incoming connections...");
    c = sizeof(struct sockaddr_in);
	pthread_t thread_id;
	
    while( (client_sock = accept(socket_desc, (struct sockaddr *)&client, (socklen_t*)&c)) )
    {

        char* cip=inet_ntoa(client.sin_addr);
      struct  pair1 a;
		strcpy(a.ip,cip);
        puts("Connection accepted");
		a.sd=client_sock;
	sd2ip.push_back(a);
        if( pthread_create( &thread_id , NULL ,  connection_handler , (void*) &client_sock) < 0)
        {
            perror("could not create thread");
            return 1;
        }
        //Now join the thread , so that we dont terminate before the thread
        //pthread_join( thread_id , NULL);
    }
     
    if (client_sock < 0)
    {
        perror("accept failed");
        return 1;
    }
     
    return 0;
}
 
/*
 * This will handle connection for each client
 * */


/*########################################################333*/
void *connection_handler(void *socket_desc)
{
    //Get the socket descriptor
    char cip[20];
    int num;
    int sock = *(int*)socket_desc;
    int read_size;
    char message[2000] ;
	char client_message[2000];
    bool islogin=false;
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
        int cut=-1;
        char command[2000]="";
        char context[2000]="";
        //end of string marker
        if (strcmp(client_message,"Exit\n")==0)
        {
            if (islogin)
            {
                remove(database[num-1].show_name());
		removeip(sock);
                database[num-1].logout();
                strcpy(sendmessage,message_4);
                strcat(sendmessage,"\n");
                write(sock , sendmessage , strlen(sendmessage));
	        puts("Client disconnected");
                break;
            }
            else
            {
                strcpy(sendmessage,message_4);
                strcat(sendmessage,"\n");
                write(sock , sendmessage , strlen(sendmessage));
	        puts("Client disconnected");
                break;
            }
        }
        else if (strcmp(client_message,"List\n")==0)
        {
            if(islogin)
            {
                int balance=database[num-1].showbalance();
                char str[333];
                sprintf(str,"%d",balance);
                strcpy(sendmessage,str);
                strcat(sendmessage,"\n");
		int len=online.size();
		char strl[100];
                sprintf(strl,"%d",len);
                strcat(sendmessage,strl);
                strcat(sendmessage,"\n");
                for (int i = 0; i < len; ++i)
                {
                    strcat(sendmessage,online[i].print().c_str());
                        
                }
                write(sock , sendmessage , strlen(sendmessage));
            }
            else
            {
                strcpy(sendmessage,message_3);
                strcat(sendmessage,"\n");
                write(sock , sendmessage , strlen(sendmessage));
            }
        }
        else if(islogin!=true)
        {
            for(int i=0; i<read_size-1;++i)
            {
                if(client_message[i]=='#')
                {
                    cut=i;

                    break;
                }
            }
            if (cut==-1)
            {
                strcpy(sendmessage,message_6);
                strcat(sendmessage,"\n");
                write(sock , sendmessage , strlen(sendmessage));
                memset(sendmessage, 0, strlen(sendmessage));
                memset(client_message, 0, 2000);
                continue;
            }
            for (int i = 0; i < cut; ++i)
            {
                command[i]=client_message[i];
            }
            for (int i = cut+1; i < read_size-2; ++i)
            {
                context[i-cut-1]=client_message[i];
            }
		cout<<client_message<<strlen(client_message)<<read_size<<"\n";
			
            if(strcmp(command,"REGISTER")==0)
            {
                if (isregistered(context))
                {
                    strcpy(sendmessage,message_1);
                    strcat(sendmessage,"\n");
                    write(sock , sendmessage , strlen(sendmessage));
                    memset(sendmessage, 0, strlen(sendmessage));
                    memset(client_message, 0, 2000);
                    continue;
                }
                account a(context);
				
                database.push_back(a);
	
                strcpy(sendmessage,message_0);
                strcat(sendmessage,"\n");
                send(sock , sendmessage , strlen(sendmessage)+1,0);
            }
            else 
            {
                num=isloginable(command);
                if(num>0)
                {
			islogin=true;
                    database[num-1].log(cip,context);
                    online.push_back(database[num-1]);
                    int balance=database[num-1].showbalance();
                    char str[333];
                    sprintf(str,"%d",balance);
                    strcpy(sendmessage,str);
                    strcat(sendmessage,"\n");
				int len=online.size();
				char strl[100];
                sprintf(strl,"%d",len);
                strcat(sendmessage,strl);
                    strcat(sendmessage,"\n");
                    for (int i = 0; i < online.size(); ++i)
                    {
                        strcat(sendmessage,online[i].print().c_str());
                        
                    }
                    write(sock , sendmessage , strlen(sendmessage));
                }
                else if(num==0)
                {
                    strcpy(sendmessage,message_2);
                    strcat(sendmessage,"\n");
                    write(sock , sendmessage , strlen(sendmessage));
                }
                else
                {
                    strcpy(sendmessage, command);
                    strcat(sendmessage," ");
                    strcat(sendmessage,message_5);
                    strcat(sendmessage,"\n");
                    write(sock , sendmessage , strlen(sendmessage));
                }
            }
        }
        else
        {
		int cut[2]={-1,-1};
        	char command[2000]="";
        	char amount[200]="";
        	char context[2000]="";
        	//end of string marker
        	int fin=0;
        	for(int i=0; i<read_size-1;++i)
        	{
        	    if(client_message[i]=='#')
        	    {
        	        cut[fin]=i;
        	        if(fin==1)
        	        	break;
        	    	fin++;
        	    }
        	}
        	for (int i = 0; i < cut[0]; ++i)
        	{
        	    command[i]=client_message[i];
        	}
        	for (int i = cut[0]+1; i < cut[1]; ++i)
        	{
       		     amount[i-cut[0]-1]=client_message[i];
        	}
        	for (int i = cut[1]+1; i < read_size-2; ++i)
        	{
        	    context[i-cut[1]-1]=client_message[i];
       		}
       		balancechange(command,atoi(amount)*(-1));
       		balancechange(context,atoi(amount));
       	}
	
        memset(sendmessage, 0, strlen(sendmessage));
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
         
    return 0;
} 
