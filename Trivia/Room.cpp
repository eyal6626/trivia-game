#include "Room.h"

Room::Room(int id, User* admin, std::string name, int maxUsers, int questionsNo, int questionTime)
{
	this->_id = id;
	this->_admin = admin;
	this->_users.push_back(admin);
	this->_name = name;
	this->_maxUsers = maxUsers;
	this->_questionsNo = questionsNo;
	this->_questionTime = questionTime;
}

bool Room::joinRoom(User* user)
{
	std::string msg;
	if (this->_users.size() <= this->_maxUsers)
	{
		this->_users.push_back(user);
		msg = getUsersListMessage();
		sendMessage(user,msg);

		return true;
	}
	return false;
}

std::string Room::getUsersListMessage()
{
	std::vector<User*>::iterator it;
	std::string msg;
	msg = "108" + std::to_string(this->_users.size());
	for (auto & it : this->_users) {
		msg = msg + Helper::getPaddedNumber(it->getUsername().size(),2) + it->getUsername();
	}
	return msg;
}

void Room::sendMessage(User* excludeUser, std::string message)
{
	try
	{
		for (auto& it : this->_users)
			if (excludeUser != NULL) {
				if (excludeUser->getUsername() != it->getUsername()) {
					it->send(message);
				}
			}
			else {
				it->send(message);
			}
	}
	catch (const std::exception& e)
	{
		std::cout << "\nERROR: " << e.what() << "\n";
	}
}

void Room::sendMessage(std::string message)
{
	sendMessage(NULL,message);
}

Room* Room::createRoomAndReturnPointer(int id, User* admin, std::string name, int maxUsers, int questionsNo, int questionTime)
{
	Room* p = new Room(id, admin, name, maxUsers, questionsNo, questionTime);
	return p;
}

bool Room::isRoomIsFull()
{

	return this->_maxUsers == this->_users.size();
}

int Room::getQuestionTime()
{
	return this->_questionTime;
}

int Room::getQuestionsNo()
{
	return this->_questionsNo;
}

std::string Room::getRoomName()
{
	return this->_name;
}

int Room::getRoomId()
{
	return this->_id;
}

void Room::leaveRoom(User* user)
{   
	std::vector<User*>::iterator it;
	//Search for the room
	for (auto it : this->_users)
	{
		if (it->getUsername() == user->getUsername())
		{
			this->_users.erase(std::find(this->_users.begin(), this->_users.end(), user));
			break;
		}
		
	}
	//if the room was not found
	sendMessage(user, getUsersListMessage());
}

int Room::closeRoom(User* user)
{
	if (this->_admin->getUsername() == user->getUsername()) {
		this->clearRoom();
		sendMessage("116");
		return user->getRoom()->getRoomId();
	}
	return -1;
}

void Room::clearRoom()
{
	std::vector<User*>::iterator it;
	//Search for the room
	for (auto it : this->_users)
	{
		if (it->getUsername() != this->_admin->getUsername()) {
			it->setRoom(nullptr);
		}

	}
}

std::vector<User*> Room::getUsers()
{
	return this->_users;
}
