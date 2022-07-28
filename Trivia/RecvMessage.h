#pragma once

#include "User.h"
#include <string>
#include <iostream>
#include <vector>
#include <queue>
#include <Windows.h>
#include <mutex>

class RecvMessages {
public:
	RecvMessages(SOCKET sock, int messageCode);

	RecvMessages(SOCKET sock, int messageCode, std::vector<std::string> values);
	
	SOCKET getSock();
	User* getUser();
	void setUser(User* user);
	int getMessageCode();
	std::vector<std::string> getValues();

private:
	SOCKET _sock;
	User* _user;
	int _messageCode;
	std::vector<std::string> _values;

	
};