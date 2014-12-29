///////////////////////////////////////////////////////////////////
// HTTPMessage.cpp -     TO create HTTP Message and retrieve     //
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
// -------------------                                           //
// HTTPMessage :     Sets IP Address Port number of for the HTTP //
//                 message. Retrieves values of IP Address and   //
//                 port numbers of the sender and client.        //
//                    Contains a body and Type of content.       //
///////////////////////////////////////////////////////////////////
#include "HTTPMessage.h"
// Test Stub -----
#ifdef TEST_HTTPMESSAGE
int main(){
	HTTPMessage msg("text","Test Message");
	msg.BuildHeader(sourceip, Destip);
	msg.BuildBody("Sample","Text")''
	msg.BuiltBody();
	msg.getServerIP()
	msg.getClientIP()
	msg.getPath()
	msg.getServerPort()
	msg.getClientPort()
	msg.setPort(8050)
	msg.setIP(127.0.0.1)
	std::cout << "\n\n";
}
#endif