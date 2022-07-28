#pragma once
#include <iostream>
#include <string>
#include "Helper.h"
#include <WinSock2.h>
#include <Windows.h>
#include "Room.h"
#include "Game.h"

#define NOT_IN_ANY_ROOM nullptr

class Game;
class Room;
class User
{
public:
	User(std::string username, SOCKET sock); //C'tor
	~User();//D'tor

	void send(std::string message);//Send message to the user
	std::string getUsername();//Get the username of the user
	SOCKET getSocket();//Get the socket of the user
	static User* createUserAndReturnPointer(std::string username,SOCKET sock);
	Room* getRoom();
	Game* getGame();
	void setGame(Game* game);
	//void clearRoom();
	bool createRoom(int roomId,std::string roomName,int maxUsers,int questionsNo,int questionsTime); 
	bool joinRoom(Room* room);
	void leaveRoom();
	void setRoom(Room* room);
	int closeRoom();
	bool leaveGame();

private:
	std::string _username;
	Room* _currRoom;
	Game* _currGame;
	SOCKET _sock;
	int* p;
};
