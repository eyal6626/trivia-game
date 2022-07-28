#include "User.h"

User::User(std::string username, SOCKET sock)
{
	this->_username = username;
	this->_sock = sock;
	this->_currRoom = NOT_IN_ANY_ROOM;
	this->_currGame = nullptr;
}

User::~User()
{
}

void User::send(std::string message)
{
	//Send the message to the user
	Helper::sendData(getSocket(),message);
}

std::string User::getUsername()
{
	return this->_username;
}

SOCKET User::getSocket()
{
	return this->_sock;
}

User* User::createUserAndReturnPointer(std::string username, SOCKET sock)
{
	
	User* user = new User(username, sock);
	return user;
}

Room* User::getRoom()
{
	return this->_currRoom;
}

Game* User::getGame()
{
	return this->_currGame;
}

void User::setGame(Game* game)
{
	this->_currGame = game;
}

bool User::createRoom(int roomId, std::string roomName, int maxUsers, int questionsNo, int questionsTime)
{
	if (this->_currRoom != NOT_IN_ANY_ROOM)
	{
		return false;
	}

	//Create a new room for the user
	this->_currRoom = Room::createRoomAndReturnPointer(roomId, this, roomName, maxUsers, questionsNo, questionsTime);
	return true;
}

bool User::joinRoom(Room* room)
{
	
	if (this->_currRoom != NOT_IN_ANY_ROOM)
	{
		return false;
	}
	//If the user is not in a room already try to add the user to the room and return if 
	//the user entered the room successfully(true) or not(false)
	return room->joinRoom(this);
}

void User::leaveRoom()
{
	getRoom()->leaveRoom(this);
	this->_currRoom = nullptr;
}

void User::setRoom(Room* room)
{
	this->_currRoom = room;
}

int User::closeRoom()
{
	int id = 0;
	if (this->getRoom() == nullptr) {
		return -1;
	}
	else
	{
		id = this->getRoom()->closeRoom(this);
	}
	return id;
}

bool User::leaveGame()
{
	if (this->getGame() != nullptr) {
		bool ans = this->getGame()->leaveGame(this);
		this->setRoom(nullptr);
		this->setGame(nullptr);
		return ans;
	}
	return false;
}



