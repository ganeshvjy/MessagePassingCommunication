///////////////////////////////////////////////////////////////////
// Client App.cpp -       Client Application Executive           //
//                        Creates HTTP Message and establishes   //
//                        connection with the Server to send     //
//                        data                                   //
// ver 1.1                                                       //
// Language: standard C++                                        //
// Platform: Toshiba Satellite Windows 8                         //
// Application: Message Passing Communication                    //
// Author:      Ganesh Thiagarajan, Syracuse University          //
//              gthiagar@syr.edu                                 //
///////////////////////////////////////////////////////////////////
// Package Operations:                                           //
// -------------------                                           //
// ClientApp - Creates a HTTP Header Packet and establishes      //
//             communication with the server and send the data   //
//             to the server.                                    //
//             Transports the message from Client to Server      //
//             Waits for Acknowledgement from Server about the   //
//             data transfer.                                    //
///////////////////////////////////////////////////////////////////
#pragma once
#include "..\..\MPC - Client\CLientCommunication.h"
#include "..\..\HTTPMessage\HTTPMessage.h"

int Process(std::string FilePath, int ServerPort, int ClientPort){
	
	std::string IPAddress = "127.0.0.1";
	int length =0;

	try
	{	
		//Create a test HTTP Message to be sent to server
		HTTPMessage HTTPmsg;
		HTTPmsg.BuildHeader(IPAddress, ClientPort, IPAddress, ServerPort,FilePath,length);
		std::cout << HTTPmsg.showHeader();
		Sender sndr1(HTTPmsg);

		DemoThread t1(sndr1);
		t1.start();
		t1.join();

		Receiver rcvr;
		rcvr.start(ClientPort);
		return 0;
	}
	catch (std::exception& ex)
	{
		sout << "\n\n  " << ex.what();
		return -1;
	}
	catch (...)
	{
		sout << "\n\n  something bad happend to a sender";
		return -1;
	}
}

int main()
{
	int ServerPort, ClientPort;

	std::cout << "\n\t\tTo Demonstrate Message Passing Communication" << std::endl;
	std::cout << "\t\t============================================\n\n";

	std::cout << "\t\t\t\t -- Client Console --\n";

	std::cout << "  Initializing Client...\n\n  Enter Port Number to be used for Client:";
	std::cin >> ClientPort;

	std::cout << "  Enter Port number of Server:";
	std::cin >> ServerPort;
	

	std::string UserInput;
	std::cout << "\nEnter File Path to transfer to Server:";
	std::cin >> UserInput;
	Process(UserInput, ServerPort, ClientPort);
	
  std::cout << "\n\nTerminating Client...\n\n";
  return 0;
}