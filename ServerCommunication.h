#ifndef SERVERCOMMUNICATION_H
#define SERVERCOMMUNICATION_H
///////////////////////////////////////////////////////////////////
// Server App.h   -       Server Applcation receives open message//
//                    from different clients and process them    //
//                    and sends acknowledgement to the respective//
//                    clients.                                   //
// Language: standard C++                                        //
// Platform: Toshiba Satellite Windows 8                         //
// Application: Message Passing Communication                    //
// Author:      Ganesh Thiagarajan, Syracuse University          //
//              gthiagar@syr.edu                                 //
///////////////////////////////////////////////////////////////////
// Module Operations:                                            //
// -------------------                                           //
// Server App:        - Creates a open receiver to receive       //
//                      input message from the different clients //
//                    - Process the requests and receives data   //
//                      from different clients.                  //
//                    - Sends Acknowledegement to each client    //
///////////////////////////////////////////////////////////////////
//                              
#include "../Sockets/Sockets.h"
#include "../Threads/Threads.h"
#include "../Threads/Locks.h"
#include "../BlockingQueue/BlockingQueue.h"
#include "../HTTPMessage/HTTPMessage.h"
#include "Base_64.h"
#include <string>
#include <iostream>
#include <fstream>
#include <sstream>

using namespace std;

///////////////////////////////////////////////////
// ClientHandlerThread thread

class ServerHandlerThread : public tthreadBase
{
public:
	ServerHandlerThread(Socket s, BlockingQueue<std::string>& q) : s_(s), q_(q) {}
private:
	void run()
	{
		std::string msg;
		do
		{
			doLog("receive thread reading line");
			msg = s_.readLine();
			if (msg == "")
				break;
			q_.enQ(msg);
		} while (msg != "quit");
	}
	Socket s_;
	BlockingQueue<std::string>& q_;
};

///////////////////////////////////////////////////
// listenThread thread
class ListenThread : public threadBase
{
public:
	ListenThread(int port, BlockingQueue<std::string>& q) : listener_(port), q_(q), stop_(false) {}
	void stop(bool end) { stop_ = end; }
private:
	void run()
	{
		while (!stop_)
		{
			SOCKET s = listener_.waitForConnect();
			ServerHandlerThread* pCh = new ServerHandlerThread(s, q_);
			pCh->start();
		}
	}
	bool stop_;
	BlockingQueue<std::string>& q_;
	SocketListener listener_;
};

/////////////////////////////////////////////////////////////////////////////////
// Sender Classes below
/////////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////
// SendThread thread

class SendThread : public threadBase
{
public:
	SendThread(Socket s, BlockingQueue<std::string>& q) : s_(s), q_(q) {}
	std::string& status() { return status_; }
private:
	void run()
	{
		status_ = "good";
		doLog("send thread running");
		std::string msg;
		do
		{
			doLog("send thread enqing msg");
			msg = q_.deQ();
			if (!s_.writeLine(msg))
			{
				sout << "\n  bad status in sending thread";
				status_ = "bad";
				break;
			}
		} while (msg != "stop");
		s_.disconnect();
	}
	std::string status_;
	Socket s_;
	BlockingQueue<std::string>& q_;
};

std::string ToString(int i)
{
	std::ostringstream conv;
	conv << i;
	return conv.str();
}

class Sender
{
public:
	Sender(HTTPMessage& msg) :msg_(msg){
		port = msg.getClientPort();
	};

	void start()
	{
		pSt = new SendThread(s_, q_);
		pSt->start();
		if (!s_.connect(ip, port))
		{
			sout << locker << "\n  couldn't connect to " << ip << ":" << port << "\n\n" << unlocker;
			delete pSt;
			return;
		}
		else
		{
			sout << locker << "\n Connected to:  " << ip << ":" << port << "\n\n" << unlocker;
			std::string logMsg = "\n  connected to " + ip + ":" + ToString(port);
			doLog(logMsg.c_str());
		}
		doLog("starting Sender");
		q_.enQ(msg_.BuiltBody());
		q_.enQ("stop");
		pSt->join();
		delete pSt;
	}
private:
	Socket s_;
	BlockingQueue<std::string> q_;
	SendThread* pSt;
	HTTPMessage& msg_;
	std::string ip ="127.0.0.1";
	std::string filepath;
	int port;
};

///////////////////////////////////////////////////
// DemoThread is used to get two or more senders
// running concurrently from a single process, to
// make testing easier.

class DemoThread : public threadBase
{
public:
	DemoThread(Sender sndr) : sndr_(sndr) {}
private:
	void run()
	{
		sndr_.start();
	}
	Sender sndr_;
};

////////////////////////////////////////////////////
// Receiver class below
////////////////////////////////////////////////////
class Receiver
{
public:
	void start(int port)
	{
		sout << "\n Server Initializing in port:"<< port;
		pLt = new ListenThread(port, q_);
		try
		{
			pLt->start();
			std::string msg;
			int count = 0;
			HTTPMessage Ack;
			do
			{
				msg = q_.deQ();
				if (msg.find("BINARY") != string::npos)
				{
					msg = q_.deQ();
					std::vector<char> vdecoded = Base64::decode(msg);
					std::string decodedMsg(vdecoded.begin(), vdecoded.end());
					sout << "\n\tDecoding Binary File received...";
					// Commenting out the Binary File data printing to make the output Good.
					// sout << msg.c_str();
					msg = q_.deQ();
				}
								
				if (msg.find("Client IP:") != string::npos)
				{
					std::string IPAddress, clientPort, ClientIP;
					IPAddress.assign(msg.begin() + 11, msg.end());

					int port = IPAddress.find(":");
					ClientIP.assign(IPAddress, 0, port);
					int cIP = atoi(ClientIP.c_str());

					clientPort.assign(IPAddress, port + 1, 4);
					int cport = atoi(clientPort.c_str());

					Ack.setPort(cport);
					Ack.setIP(cIP);
				}
				if (msg.find("stop") != string::npos)
				{
					Ack.BuildBody("Acknowledgement", "Connection establihed successfully");
					std::cout << "\n\nSending Acknowledgement to Client...";

					Sender sndr(Ack);
					sndr.start();
				}
				if (msg != "stop\n")
				sout << msg.c_str();
			} while (true);
			pLt->join();
		}
		catch (std::exception& exception)
		{
			sout << "\n\n  " << exception.what();
			delete pLt;
		}
		catch (...)
		{
			delete pLt;
		}
	}
private:
	BlockingQueue<std::string> q_;
	ListenThread* pLt;
};
#endif