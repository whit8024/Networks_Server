// Networks_Server.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "Networks_Server.h"
#include <winsock2.h>
#include <WS2tcpip.h>
#include <stdio.h>

#pragma comment(lib, "Ws2_32.lib")

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

#define DEFAULT_BUFLEN 512
#define DEFAULT_PORT 1122 //27015
#define DEFAULT_OUT 1121

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif

// The one and only application object

CWinApp theApp;

using namespace std;

// The _tmain function is the actual entry point, but we shouldn't have code here
int _tmain(int argc, TCHAR* argv[], TCHAR* envp[])
{
	int nRetCode = 0;

	HMODULE hModule = ::GetModuleHandle(NULL);

	if (hModule != NULL)
	{
		// initialize MFC and print and error on failure
		if (!AfxWinInit(hModule, NULL, ::GetCommandLine(), 0))
		{
			// TODO: change error code to suit your needs
			_tprintf(_T("Fatal Error: MFC initialization failed\n"));
			nRetCode = 1;
		}
		else
		{
			// TODO: code your application's behavior here.
		}
	}
	else
	{
		// TODO: change error code to suit your needs
		_tprintf(_T("Fatal Error: GetModuleHandle failed\n"));
		nRetCode = 1;
	}

	return nRetCode;
}

// This is our normal entry point
int main()
{
    SOCKET s,out;
    struct sockaddr_in server, si_other, send;
    int slen , recv_len;
    char buf[DEFAULT_BUFLEN];
    WSADATA wsa;
	int counter = 0;
 
    slen = sizeof(si_other) ;
     
    //Initialise winsock
    printf("\nInitialising Winsock...");
    if (WSAStartup(MAKEWORD(2,2),&wsa) != 0)
    {
        printf("Failed. Error Code : %d",WSAGetLastError());
        exit(EXIT_FAILURE);
    }
    printf("Initialised.\n");
     
    //Create a socket
    if((s = socket(AF_INET , SOCK_DGRAM , 0 )) == INVALID_SOCKET)
    {
        printf("Could not create socket : %d" , WSAGetLastError());
    }
    printf("Socket created.\n");

	//Create a socket
    if((out = socket(AF_INET , SOCK_DGRAM , 0 )) == INVALID_SOCKET)
    {
        printf("Could not create socket : %d" , WSAGetLastError());
    }
    printf("Socket created.\n");

	send.sin_family = AF_INET;
	send.sin_addr.s_addr = htonl (INADDR_ANY);
	send.sin_port = htons( DEFAULT_OUT );

    //Prepare the sockaddr_in structure
    server.sin_family = AF_INET;
    server.sin_addr.s_addr = htonl (INADDR_ANY);
    server.sin_port = htons( DEFAULT_PORT );


     
    //Bind
    if( bind(s ,(struct sockaddr *)&server , sizeof(server)) == SOCKET_ERROR)
    {
        printf("Bind failed with error code : %d" , WSAGetLastError());
        exit(EXIT_FAILURE);
    }

	if( bind(out, (struct sockaddr *)&send, sizeof(send)) == SOCKET_ERROR)
	{
		printf("Bind to output socket failed with error code : %d", WSAGetLastError());
		exit(EXIT_FAILURE);
	}

    puts("Bind done");
 
    //keep listening for data
    while(1)
    {
        printf("Waiting for data...");
        fflush(stdout);
         
        //clear the buffer by filling null, it might have previously received data
        memset(buf,'\0', DEFAULT_BUFLEN);
         
        //try to receive some data, this is a blocking call
        if ((recv_len = recvfrom(s, buf, DEFAULT_BUFLEN, 0, (struct sockaddr *) &si_other, &slen)) == SOCKET_ERROR)
        {
            printf("recvfrom() failed with error code : %d" , WSAGetLastError());
            exit(EXIT_FAILURE);
        }
         
        //print details of the client/peer and the data received
		//if(counter++ > 5000)
		{
			printf("Received packet from %s:%d\n", inet_ntoa(si_other.sin_addr), ntohs(si_other.sin_port));
			printf("Data: %s\n" , buf);
			counter = 0;
		}
         
        //now reply to the client
	send.sin_addr.s_addr = si_other.sin_addr.s_addr;

	/*
	// Force data
        memset(buf,'\0', DEFAULT_BUFLEN);
	strcpy(buf, "f(3.3,0.00,0.06");

	if (sendto(out, buf, sizeof(buf), 0, (struct sockaddr*) &send, slen) == SOCKET_ERROR)
	{
            printf("sendto() failed with error code : %d" , WSAGetLastError());
	   exit(EXIT_FAILURE);
	}
	*/
	
	// Position data
	if (sendto(out, buf, recv_len, 0, (struct sockaddr*) &send, slen) == SOCKET_ERROR)
        {
            printf("sendto() failed with error code : %d" , WSAGetLastError());
            exit(EXIT_FAILURE);
	}
    }
 
    closesocket(s);
    WSACleanup();
     
    return 0;
}
