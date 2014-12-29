#ifndef CLIENTCOMMUNICATION_H
#define CLIENTCOMMUNICATION_H
///////////////////////////////////////////////////////////////////
// Client Communication.h - Acts as a communcation channel to    //
//                          send and receive messages through    //
//                          sockets to server.                   //
// ver 1.1                                                       //
// Language: standard C++                                        //
// Platform: Toshiba Satellite Windows 8                         //
// Application: Message Passing Communication                    //
// Author:      Ganesh Thiagarajan, Syracuse University          //
//              gthiagar@syr.edu                                 //
////////////////////////////////////////////////////////////////////
/*                                                                //
* Package Operations:                                             //
* ===================                                             //
// Sender - Used to send blocks of data with a header to the other//
//          Server                                                //
// Receiver - Receives Acknowledgements from the Server that      //
//          the data is received or not                           //
////////////////////////////////////////////////////////////////////
*/
#pragma once
#include "../../sockets/Sockets.h"
#include "../../Threads/Threads.h"
#include "../../Threads/Locks.h"
#include "../../BlockingQueue/BlockingQueue.h"
#include "../../HTTPMessage/HTTPMessage.h"
#include "Base_64.h"
#include <string>
#include <iostream>
#include <fstream>
#include <sstream>

using namespace std;

///////////////////////////////////////////////////
// ClientHandlerThread thread

class ClientHandlerThread : public tthreadBase
{
public:
	ClientHandlerThread(Socket s, BlockingQueue<std::string>& q) : s_(s), q_(q) {}
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
			ClientHandlerThread* pCh = new ClientHandlerThread(s, q_);
			pCh->start();
		}
	}
	bool stop_;
	BlockingQueue<std::string>& q_;
	SocketListener listener_;
};
///////////////////////////////////////////////////
// Receiver classes below 
///////////////////////////////////////////////////

class Receiver
{
public:
	void start(int port)
	{
		sout << "\n Listening for incoming messages...";
		pLt = new ListenThread(port, q_);
		try
		{
			pLt->start();
			std::string msg;
			int count = 0;
			do
			{
				msg = q_.deQ();
				doLog((std::string("fetching message ") + msg).c_str());
				if (msg != "stop\n")
				sout << msg.c_str();
			} while (msg != "stop\n");
			//pLt->selfTerminate();
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

/////////////////////////////////////////////////////////////////////////////////
// Sender Classes below
/////////////////////////////////////////////////////////////////////////////////
// SendThread thread
///////////////////////////////////////////////////////
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
////////////////////////////////////////////////////
// Sender classes below
////////////////////////////////////////////////////
class Sender
{
public:
	Sender(HTTPMessage& msg):msg_ (msg){
		int cport = msg.getClientPort();
		ip = msg.getServerIP();
		port = msg.getServerPort();
		filepath = msg.getPath();
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
		q_.enQ(msg_.showHeader());
		ifstream fin(filepath, ios_base::in | std::ios::binary|std::ios::ate);
		if (filepath.find(".txt") != string::npos || filepath.find(".doc") != string::npos)
		{
			if (fin.is_open() && !fin.fail())
			{		
				fin.seekg(0, std::ios::end);
				std::streamoff size = fin.tellg();
				int block = 1024;
				if (size > 0)
				{
					char* memblock = new char[block];
					fin.seekg(0, std::ios::beg);
					while (fin.read(memblock, block))
					{
						memblock[block - 1] = '\0';
						q_.enQ(memblock);
					}
					std::streamsize t = fin.gcount();
					string s(memblock, int (fin.gcount()));
					memblock[t - 1] = '\0';
					q_.enQ(memblock);
					delete[] memblock;
					fin.close();
					q_.enQ("stop");
				}
			}
			else
			{
				cerr << "Cannot open file:" << filepath << endl;
			}
		}
		else
		{
			std::string encodedString = Base64::encodeFromFile(filepath);
			q_.enQ("BINARY");
			q_.enQ(encodedString);
			q_.enQ("stop");
		}
		pSt->join();
		delete pSt;
	}
private:
	Socket s_;
	BlockingQueue<std::string> q_;
	SendThread* pSt;
	HTTPMessage& msg_;
	std::string ip;
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

#endif