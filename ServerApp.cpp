///////////////////////////////////////////////////////////////////
// Server App.cpp -       Server Applcation receives open message//
//  Ver 1.0           from different clients and process them    //
//                    and sends acknowledgement to the respective//
//                    clients.                                   //
// Language: standard C++                                        //
// Platform: Toshiba Satellite Windows 8                         //
// Application: Message Passing Communication                    //
// Author:      Ganesh Thiagarajan, Syracuse University          //
//              gthiagar@syr.edu                                 //
///////////////////////////////////////////////////////////////////
// Package Operations:                                           //
// -------------------                                           //
// Server App:        - Creates a open receiver to receive       //
//                      input message from the different clients //
//                    - Process the requests and receives data   //
//                      from different clients.                  //
//                    - Sends Acknowledegement to each client    //
///////////////////////////////////////////////////////////////////
#include "../../Sockets/Sockets.h"
#include "../../Threads/Threads.h"
#include "../../Threads/Locks.h"
#include "../../BlockingQueue/BlockingQueue.h"
#include "../../MPC - Server/ServerCommunication.h"
#include "../../HTTPMessage/HTTPMessage.h"
#include <string>


int main()
{

  std::cout << "\n\t\tTo Demonstrate Message Passing Communication" << std::endl;
  std::cout << "\t\t============================================\n\n";

  std::cout << "\t\t\t\t-- Server Console --\n";

  int ServerPort;
  std::cout << "  Initializing Server...\n\n  Enter Port Number to be used for Server:";
  std::cin >> ServerPort;
  try
  {
	  Receiver rcvr;

	  rcvr.start(ServerPort);
  }
  catch(std::exception& ex)
  {
    std::cout << "\n\n  " << ex.what();
    return -1;
  }
  catch(...)
  {
    sout << "\n  something bad happened";
    return -1;
  }
  sout << "\n\n";
  return 0;
}
