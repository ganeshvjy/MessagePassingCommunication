#ifndef HTTPMESSAGE_H
#define HTTPMESSAGE_H
///////////////////////////////////////////////////////////////////
// HTTPMessage.h -     TO create HTTP Message and retrieve       //
//                     information about the Client IP Address   //
//                     Server IP address and Port information.   //
// ver 1.1                                                       //
// Language: standard C++                                        //
// Platform: Toshiba Satellite Windows 8                         //
// Application: Message Passing Communication                    //
// Author:      Ganesh Thiagarajan, Syracuse University          //
//              gthiagar@syr.edu                                 //
///////////////////////////////////////////////////////////////////
// Package Operations:                                           //
//  HTTP Message Header: Builds a HTTP Header which has diff     //
//                       attributes like IP Address Port Info    //
//                       type of content and Content length      //
//  HTTP Body          : Usually has body of the HTTP Message    //
//                       Holds the data to be conveyed           //
///////////////////////////////////////////////////////////////////

#include <iostream>
#include <sstream>
#include <string>
#include <fstream>

using namespace std;

class HTTPMessage{
public:
	HTTPMessage(){};
	void BuildHeader(std::string Sourceip, int sport, std::string Destip, int dport, std::string FilePath, int length)
	{
		clientip = Sourceip;
		sport_ = sport;

		serverip = Destip;
		dport_ = dport;
		filepath_ = FilePath;

		if (FilePath.find(".dat") != string::npos)
			msgtype += "Data File";
		else if (FilePath.find(".exe") != string::npos)
			msgtype += "Executable";
		else if (FilePath.find(".png") != string::npos)
			msgtype += "image";
		else if (FilePath.find(".jpg") != string::npos)
			msgtype += "image";
		else if (FilePath.find(".jpeg") != string::npos)
			msgtype += "image";
		else
			msgtype += "text/html";

		//std::cout << "LEngth value parsed:" << length;
		ifstream fin(filepath_, ios_base::in | ios_base::binary);
		if (fin.is_open())
		{
			char* buffer = new char[1024];
			while (fin.read(buffer, 1024))
			{
				string s(buffer);
				length += 1024;
			}

			// if the bytes of the block are less than 1024,
			// use fin.gcount() calculate the number, put the va
			// into var s
			string s(buffer, int(fin.gcount()));
			length += int(fin.gcount());
			delete[] buffer;
			fin.close();
		}
		else
		{
			cerr << "Cannot open file:" << filepath_ << endl;
		}
		//std::cout << "calculated total length: " << length;
		msglength = length;
	}
	std::string HTTPMessage::showHeader(){
		std::string temp;
		temp += "\n\tPOST ";
		temp += filepath_;
		temp += " HTTP 1.0";
		temp += "\n\tClient IP:";
		temp += clientip;
		temp += ":";
		temp += ToString(sport_);
		temp += "\n\tServer IP:";
		temp += serverip;
		temp += ":";
		temp += ToString(dport_);
		temp += "\n\tContent-Type:";
		temp += msgtype;
		temp += "\n\tContent-Length:";
		temp += std::to_string(msglength);
		temp += "\n";
		return temp;

	}
	void HTTPMessage::BuildBody(std::string type, std::string body){
		msgtype = type;
		msgcontent = body;
	}
	std::string HTTPMessage::BuiltBody(){
		std::string temp;
		temp += "\n\tMessage Type:";
		temp += msgtype;
		temp += "\n\tMessage Body:";
		temp += msgcontent;
		return temp;
	};
	std::string HTTPMessage::getServerIP(){
		return serverip;
	}
	std::string HTTPMessage::getClientIP(){
		return clientip;
	}
	std::string HTTPMessage::getPath(){
		return filepath_;
	}
	int HTTPMessage::getServerPort(){
		return dport_;
	}
	int HTTPMessage::getClientPort(){
		return sport_;
	}
	void HTTPMessage::setPort(int port){
		sport_ = port;
	}
	void HTTPMessage::setIP(int IP){
		serverip = IP;
	}
	std::string HTTPMessage::ToString(int i){
		std::ostringstream conv;
		conv << i;
		return conv.str();
	}
private:
	std::string clientip;
	std::string serverip;
	std::string msgtype;
	std::string msgcontent;
	std::string filepath_;
	int msglength;
	int sport_;
	int dport_;
};
#endif