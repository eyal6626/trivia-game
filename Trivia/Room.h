#pragma once
#include <vector>
#include "User.h"
#include <iostream>
#include <string>

class User;
class Room {
public:
	Room(int id,User* admin,std::string name,int maxUsers,int questionsNo,int questionTime);
	bool joinRoom(User* user);
	std::string getUsersListMessage();
	void sendMessage(User* excludeUser, std::string message);
	void sendMessage( std::string message);
	static Room* createRoomAndReturnPointer(int id, User* admin, std::string name, int maxUsers, int questionsNo, int questionTime);
	bool isRoomIsFull();
	int getQuestionTime();
	int getQuestionsNo();
	std::string getRoomName();
	int getRoomId();
	void leaveRoom(User* user);
	int closeRoom(User* user);
	void clearRoom();
	std::vector<User*> getUsers();

private:
	std::vector<User*> _users;
	User* _admin;
	int _maxUsers;
	int _questionTime;
	int _questionsNo;
	std::string _name;
	int _id;
};