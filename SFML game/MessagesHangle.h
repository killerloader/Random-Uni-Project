#pragma once
#include <SFML/Graphics.hpp>
#include <SFML/Network.hpp>
#include <iostream>
#include <vector>

class WrapperClass;

class MessagesHangle
{
public:
	MessagesHangle(WrapperClass &WCR_);
	~MessagesHangle();

	void sendData(sf::Packet&, int PID);//Uses queue
	void sendData(sf::Packet& Data_, sf::TcpSocket& Sock);

	void deletePlayer(int);//Server

	WrapperClass& WCR;
	void ServerMessagesHandle();
	void ClientMessagesHandle();
};

