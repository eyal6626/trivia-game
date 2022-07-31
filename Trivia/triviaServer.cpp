#include "triviaServer.h"
#include <exception>
#include <iostream>
#include <string>
#include <thread>

int triviaServer::_roomIdSequence = 0;
int triviaServer::_loginCounter = 0;

triviaServer::triviaServer()
{
	//Create server socket
	_serverSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

	if (_serverSocket == INVALID_SOCKET)
		throw std::exception(__FUNCTION__ " - socket");
}

triviaServer::~triviaServer()
{
	try
	{
		closesocket(_serverSocket);
	}
	catch (...) {}
}

void triviaServer::serve(int port)
{
	
	bindAndListen();
	//Create a thread the heandle the messages 
	//std::thread messageHeandler(&triviaServer::heandleMessages,this);

	std::thread tr(&triviaServer::handleReceivedMessages, this);

	std::cout << "Listening on port " << port << std::endl;
	while (true)
	{
		std::cout << "Waiting for client connection request" << std::endl;
		accept();
	}
}

void triviaServer::bindAndListen()
{
	struct sockaddr_in sa = { 0 };

	sa.sin_port = htons(PORT); //set the port
	sa.sin_family = AF_INET;
	sa.sin_addr.s_addr = INADDR_ANY; //set ip    



	if (bind(_serverSocket, (struct sockaddr*)&sa, sizeof(sa)) == SOCKET_ERROR)
		throw std::exception(__FUNCTION__ " - bind");

	// Start listening 
	if (listen(_serverSocket, SOMAXCONN) == SOCKET_ERROR)
		throw std::exception(__FUNCTION__ " - listen");
}

void triviaServer::accept()
{
	//std::thread countLoginRequests(&triviaServer::printLoginCounter,this);
	//countLoginRequests.detach();
	while (1) {
		//Accept the client
	    //put his data in client_socket
		SOCKET client_socket = ::accept(_serverSocket, NULL, NULL);

		if (client_socket == INVALID_SOCKET)
			throw std::exception(__FUNCTION__);

		std::cout << "Client is connected!" << std::endl;

		// Create a thread for the new client and use the handler for the client 
		std::thread newClient(&triviaServer::clientHandler, this, client_socket);
		newClient.detach();
	
	}
}

void triviaServer::clientHandler(SOCKET clientSocket)
{
	RecvMessages* currRcvMsg = nullptr;
	try
	{
		// get the first message code
		int msgCode = Helper::getMessageTypeCode(clientSocket);
		//Check the msgCode of the request
		while (msgCode == LOGOUT_REQUEST || (msgCode == LOG_IN_REQUEST || msgCode == CREATE_USER_REQUEST || msgCode == CREATE_ROOM || msgCode == JOIN_ROOM || msgCode == GET_LIST_OF_ROOMS || msgCode == GET_USERS_LIST || msgCode == LEAVE_ROOM || msgCode == CLOSE_ROOM || msgCode == START_GAME_REQUEST || msgCode == GAME_ANSWER || msgCode == LEAVE_GAME || msgCode == PERSONAL_STATUS || msgCode == BEST_SOCRE))
		{
			std::cout << "Users: " << _connectedUsers.size();
			std::cout << "\nUser massage code:" << msgCode << "\n";
			currRcvMsg = build_receive_message(clientSocket, msgCode);
			addReceivedMessage(currRcvMsg);

			msgCode = Helper::getMessageTypeCode(clientSocket);
			
		}
		if (msgCode == EXIT) {
			currRcvMsg = build_receive_message(clientSocket, msgCode);
			if (getUserBySocket(clientSocket) != nullptr) {
				hendleSigout(currRcvMsg);
			}
			closesocket(clientSocket);
			delete currRcvMsg;
			std::cout << "User is out!\n\n";
		}


    }
	catch (const std::exception& e)
	{
		std::cout << "Exception was catch in function clientHandler. socket=" << clientSocket << ", what=" << e.what() << std::endl;
		currRcvMsg = build_receive_message(clientSocket, LOGOUT_REQUEST);
		if (getUserBySocket(clientSocket) != nullptr) {
			hendleSigout(currRcvMsg);
		}
		delete currRcvMsg;
		closesocket(clientSocket);
	}
	
}

RecvMessages* triviaServer::build_receive_message(SOCKET sock, int msgCode)
{
	RecvMessages* msg = nullptr;
	std::vector<std::string> values;
	if (msgCode == LOG_IN_REQUEST || msgCode == CREATE_USER_REQUEST) {
		
		int userNameSize = Helper::getIntPartFromSocket(sock, 2);
		std::string username = Helper::getStringPartFromSocket(sock, userNameSize);

		int passSize = Helper::getIntPartFromSocket(sock, 2);
		std::string pass = Helper::getStringPartFromSocket(sock, passSize);

		
		values.push_back(username);
		values.push_back(pass);

		if (msgCode == CREATE_USER_REQUEST) {
			int mailSize = Helper::getIntPartFromSocket(sock, 2);
			std::string mail = Helper::getStringPartFromSocket(sock, mailSize);
			values.push_back(mail);
		}
		msg = new RecvMessages(sock,msgCode,values);
	}
	
	else if (msgCode == LOGOUT_REQUEST) {
		msg = new RecvMessages(sock, msgCode, values);
	}
	
	else if (msgCode == CREATE_ROOM) {
		int roomNameSize = Helper::getIntPartFromSocket(sock, 2);
		std::string roomName = Helper::getStringPartFromSocket(sock, roomNameSize);
		std::cout << "Room name: " << roomName ;

        int playerNumber = Helper::getIntPartFromSocket(sock, 1);
		std::cout << "\nPlayer number: " << std::to_string(playerNumber);

		int questionsNumber = Helper::getIntPartFromSocket(sock, 2);
		std::cout << "\nquestionsNumber: " << std::to_string(questionsNumber);

		int	questionTimeInSec = Helper::getIntPartFromSocket(sock, 2);
		std::cout << "\nQuestion Time: " << std::to_string(questionTimeInSec);

		values.push_back(roomName);
		values.push_back(std::to_string(playerNumber));
		values.push_back(std::to_string(questionsNumber));
		values.push_back(std::to_string(questionTimeInSec));
		msg = new RecvMessages(sock, msgCode, values);
	}

	else if (msgCode == JOIN_ROOM || msgCode == GET_USERS_LIST) {
		int roomId = Helper::getIntPartFromSocket(sock, 4);
		values.push_back(std::to_string(roomId));
		msg = new RecvMessages(sock, msgCode, values);
		
	}

	else if (msgCode == GET_LIST_OF_ROOMS || msgCode == LEAVE_ROOM || msgCode == CLOSE_ROOM || msgCode == START_GAME_REQUEST || msgCode == LEAVE_GAME || msgCode == PERSONAL_STATUS || msgCode == BEST_SOCRE) {
		msg = new RecvMessages(sock, msgCode, values);
	}

	else if (msgCode == GAME_ANSWER) {
		std::string answerNumber = Helper::getStringPartFromSocket(sock, 1);
		std::cout << "\nPlayer - " << getUserBySocket(sock)->getUsername() << ":\n";
		std::cout << "Answer: " << answerNumber;
		int time = Helper::getIntPartFromSocket(sock, 2);
		std::cout << "\nTime: " << std::to_string(time);
		values.push_back(answerNumber);
		values.push_back(std::to_string(time));
		std::cout << "\n";
		msg = new RecvMessages(sock, msgCode, values);
	}
	

	return msg;
}

void triviaServer::handleReceivedMessages()
{
	int msgCode = 0;
	SOCKET clientSock = 0;
	std::string userName;
	std::string password;
	while (true)
	{
		try
		{
			std::unique_lock<std::mutex> lck(_messageLock);

			// Wait for clients to enter the queue.
			if (_messages.empty())
				_messagesCV.wait(lck);

			// in case the queue is empty.
			if (_messages.empty())
				continue;

			RecvMessages* currMessage = _messages.front();
			_messages.pop();
			lck.unlock(); 

			// Extract the data from the tuple.
			clientSock = currMessage->getSock();
			msgCode = currMessage->getMessageCode();

			if (msgCode == LOG_IN_REQUEST) {
				//countLogin();
				userName = currMessage->getValues()[0];
				User* loginHendleAnswer = hendleSignin(currMessage);
				//If the login request is valid add connect the user to the server
				if (loginHendleAnswer != nullptr) {
					this->_connectedUsers.insert({ clientSock, loginHendleAnswer });
				}
			}

			
			else if (msgCode == CREATE_USER_REQUEST) {
				userName = currMessage->getValues()[0];
				password = currMessage->getValues()[1];
				std::string mail = currMessage->getValues()[2];
				//Check the signup request 
				if (hendleSignup(currMessage))
				{
					//If the request is valid and passed all of the checks add the user to the database
					this->_db.addNewUser(userName, password, mail);
				}
			}

			else if (msgCode == LOGOUT_REQUEST) {
				hendleSigout(currMessage);
			}

			else if (msgCode == CREATE_ROOM) {
				bool validName = true;
				for (auto it : this->_roomsList) {
					if (it.second->getRoomName() == currMessage->getValues()[0]) {
						validName = false;
					}
				}

				if (validName) {
					if (handleCreateRoom(currMessage)) {
						//Add the room to the list of the rooms
						this->_roomsList.insert({ getUserBySocket(currMessage->getSock())->getRoom()->getRoomId(), getUserBySocket(currMessage->getSock())->getRoom() });
					}
				}
				else
				{
					sendAnswerToTheUser(CREATE_ROOM_FAIL, currMessage->getSock());
				}
			}

		    else if (msgCode == JOIN_ROOM) {
				if (handleJoinRoom(currMessage) == true) {
					int roomId = stoi(currMessage->getValues()[0]);
					
					std::string answer = "1100" + Helper::getPaddedNumber(getRoomById(roomId)->getQuestionsNo(),2) + Helper::getPaddedNumber(getRoomById(roomId)->getQuestionTime(),2);
					sendAnswerToTheUser(answer, currMessage->getSock());
					getUserBySocket(currMessage->getSock())->setRoom(getRoomById(roomId));
				}
				
			}

			else if (msgCode == GET_LIST_OF_ROOMS) {
				handleGetRooms(currMessage);
			}

			else if (msgCode == GET_USERS_LIST) {
				handleGetUsersInRoom(currMessage);
			}

			else if (msgCode == LEAVE_ROOM) {
				if (handleLeaveRoom(currMessage)) {
					sendAnswerToTheUser(LEAVE_ROOM_SUCCESS,currMessage->getSock());
				}
			}

			else if (msgCode == CLOSE_ROOM) {
				bool ans = handleCloseRoom(currMessage);
				if (ans) {
					this->_roomsList.erase(getUserBySocket(currMessage->getSock())->getRoom()->getRoomId());
				}
				getUserBySocket(currMessage->getSock())->setRoom(nullptr);
			}

			else if(msgCode == START_GAME_REQUEST)
			{
				handleStartGame(currMessage);
			}

			else if (msgCode == GAME_ANSWER) {
				handlePlayerAnswer(currMessage);
			}

			else if (msgCode == LEAVE_GAME) {
				if (handleLeaveGame(currMessage)) {
					delete  getUserBySocket(currMessage->getSock())->getGame();
				}
			}

			else if (msgCode == PERSONAL_STATUS) {
				handleGetPersonalStatus(currMessage);
			}

			else if (msgCode == BEST_SOCRE) {
				handleGetBestScores(currMessage);
			}
			delete currMessage;
		}
		catch (...)
		{

		}
	}
}

User* triviaServer::hendleSignin(RecvMessages* currMessage)
{
	//Get current user socket
	SOCKET clientSock = currMessage->getSock();
	//Get current user username
	std::string userName = currMessage->getValues()[0];
	//Get current user password
	std::string password = currMessage->getValues()[1];

	//Check if the username is valid or not
	if (this->_db.isUserExist(userName) == USERNAME_EXIST) {
		//Check if the password is valid or not
		if (this->_db.dosePasswordMatch(userName, password) == PASSWORD_MATCH) {
			//Check if the user is already connected into the server or  not
			if (getUserByName(userName) == nullptr) {
				//If the user data is valid and he is not connected already send the user msg code that says, the user is login successful
				sendAnswerToTheUser(LOGIN_SUCCESS, clientSock);
				std::cout << "User is in\n\n";
				return User::createUserAndReturnPointer(userName, clientSock);
			}
			//If the user is connected to the server already send error code to the user
			else
			{
				sendAnswerToTheUser(LOGIN_USER_IS_ALREADY_CONNECTED, clientSock);
				std::cout << "User can not login\nReason:User is connected already!\n\n";

			}
		}
		//If the password is not valid send error code to the user 
		else {
			sendAnswerToTheUser(LOGIN_WRONG_DETAILS, clientSock);
			std::cout << "User can not login\nReason:Wrong details(password)!\n\n";
		}
	}
	//If the username is not valid send error code to the user 
	else
	{
		sendAnswerToTheUser(LOGIN_WRONG_DETAILS, clientSock);
		std::cout << "User can not login\nReason:Wrong details(Username)!\n\n";

	}
	return nullptr;
}

bool triviaServer::hendleSignup(RecvMessages* currMessage)
{
	SOCKET clientSock = currMessage->getSock();
	std::string userName = currMessage->getValues()[0];
	std::string password = currMessage->getValues()[1];
	std::string mail = currMessage->getValues()[2];

	//Check if the user is already exists in the database
	if (this->_db.isUserExist(userName) == true) {
		//if the user exists send error msg code to the client
		sendAnswerToTheUser(SIGNUP_USERNAME_IS_ALREADY_EXISTS, clientSock);
		std::cout << "User can not be add\nReason:User is already exists!\n\n";
		return INVALID_NEW_USER;
	}
	//If the user is not exists send success msg code to the client
	sendAnswerToTheUser(SIGNUP_SUCCESS, clientSock);
	return VALID_NEW_USER;
}

void triviaServer::hendleSigout(RecvMessages* currMessage)
{
	SOCKET clientSocket = currMessage->getSock();
	std::map<SOCKET, User*>::iterator it = this->_connectedUsers.find(clientSocket);
	if (it->second->getRoom() != nullptr) {
		it->second->leaveGame();
	}
	if (it->second->getRoom() != nullptr) {
		it->second->leaveRoom();
	}
	delete it->second;
	this->_connectedUsers.erase(it);
	sendAnswerToTheUser(LOGOUT_SUCCESS, clientSocket);
	std::cout << "User is out!\n\n";
}

void triviaServer::sendAnswerToTheUser(std::string msgcode,SOCKET sock)
{
	try { 
		std::string s = msgcode;
		send(sock, s.c_str(), s.size(), 0);  
		std::cout << "\nServer: " << s << "\n";;
	}
	catch(...){
		std::cout << "Can't send message to the client!";
	}
}

User* triviaServer::getUserByName(std::string username)
{
	std::map<SOCKET, User*>::iterator it;

	//Search for the user
	for (it = this->_connectedUsers.begin(); it != this->_connectedUsers.end(); ++it)
	{
		if (it->second->getUsername() == username)
		{
			//std::cout << "User is connected already!\n";
			//return the user
			return it->second;
		}
	}
	//std::cout << "User is not connected already!\n";
	//if the user was not found
	return nullptr;
}

User* triviaServer::getUserBySocket(SOCKET sock)
{
	std::map<SOCKET, User*>::iterator it;

	//Search for the user
	for (it = this->_connectedUsers.begin(); it != this->_connectedUsers.end(); ++it)
	{
		if (it->first == sock)
		{
			//return the user
			return it->second;
		}
	}
	//if the user was not found
	return nullptr;
}

Room* triviaServer::getRoomById(int id)
{
	std::map<int, Room*>::iterator it;

	//Search for the room
	for (it = this->_roomsList.begin(); it != this->_roomsList.end(); ++it)
	{
		if (it->first == id)
		{
			//return the room
			return it->second;
		}
	}
	//if the room was not found
	return nullptr;
}

void triviaServer::countLogin()
{
	this->_loginCounter++;
}

void triviaServer::printLoginCounter()
{
	while (true) {
		std::cout << "\n\nCounter: " << this->_loginCounter << "\n\n";
		Sleep(3000);
	}
}

void triviaServer::handleGetRooms(RecvMessages* currMessage)
{

	std::string msg = "106" + Helper::getPaddedNumber(this->_roomsList.size(),4);
	std::map<int, Room*>::iterator it;

	for (it = this->_roomsList.begin(); it != this->_roomsList.end(); ++it)
	{
		msg = msg + Helper::getPaddedNumber(it->first,4) + Helper::getPaddedNumber(it->second->getRoomName().size(),2) + it->second->getRoomName();
	}
	sendAnswerToTheUser(msg, currMessage->getSock());
}

bool triviaServer::handleLeaveRoom(RecvMessages* currMessage)
{
	User* user = getUserBySocket(currMessage->getSock());

	if (user != nullptr) {

		user->leaveRoom();
		return true;
	}

	return false;
}

void triviaServer::handleStartGame(RecvMessages* currMessage)
{
	Room* room = getUserBySocket(currMessage->getSock())->getRoom();
	Game* game = new Game((room->getUsers()) ,room->getQuestionsNo() ,this->_db);
	if (game->getCurrentTurnAnswer() != -999) {
		//Delete the room from the open rooms list
		this->_roomsList.erase(room->getRoomId());
		//Send the first question to the players
		game->sendFirstQuestion();

	}
	else {
		sendAnswerToTheUser(START_GAME_FAIL,currMessage->getSock());
	}
	
}

void triviaServer::handlePlayerAnswer(RecvMessages* currMessage)
{
	if (getUserBySocket(currMessage->getSock())->getGame() != nullptr) {
		int answer = stoi(currMessage->getValues()[0]);
		int time = stoi(currMessage->getValues()[1]);
	    Game* game = getUserBySocket(currMessage->getSock())->getGame();
		game->handleAnswerFromUser(getUserBySocket(currMessage->getSock()), answer, time);
		game->handleNextTurn();
	}
	
}

bool triviaServer::handleLeaveGame(RecvMessages* currMessage)
{
	return getUserBySocket(currMessage->getSock())->leaveGame();
}

void triviaServer::handleGetPersonalStatus(RecvMessages* currMessage)
{
	std::string answer;
	std::vector<std::string> playerInfo = this->_db.getPersonalStatus(getUserBySocket(currMessage->getSock())->getUsername());
	if (stoi(playerInfo[3]) > 0) {
	
		
		double avrageTime = (double)(stoi(playerInfo[0])) / stoi(playerInfo[1]);
		answer = PERSONAL_STATUS_ANSWER + Helper::getPaddedNumber(stoi(playerInfo[4]),4);
		answer = answer + Helper::getPaddedNumber(stoi(playerInfo[3]), 6);
		answer = answer + Helper::getPaddedNumber(stoi(playerInfo[2]), 6);
		answer = answer + Helper::getPaddedNumber((avrageTime - (avrageTime / 10)), 2);
		avrageTime = (avrageTime / 10)*100;
		answer = answer + Helper::getPaddedNumber(avrageTime, 2);
	}
	else
	{
		answer = PERSONAL_STATUS_ANSWER + Helper::getPaddedNumber(0, 4);
		answer = answer + Helper::getPaddedNumber(0, 6);
		answer = answer + Helper::getPaddedNumber(0, 6);
		answer = answer + Helper::getPaddedNumber(0, 2);
		answer = answer + Helper::getPaddedNumber(0, 2);
	}
	sendAnswerToTheUser(answer, currMessage->getSock());
}

void triviaServer::handleGetBestScores(RecvMessages* currMessage)
{
	std::vector<std::string> answers = this->_db.getBestScores();
	std::string answer = BEST_SOCRE_ANSWER;
	for (int i = 0; i < 3;i++)
	{
		if (answers[i] == "") {
			answer = answer + "00" + "000000";
		}
		else
		{
			answer = answer + Helper::getPaddedNumber(answers[i].size(), 2) + answers[i];
			answer = answer + Helper::getPaddedNumber(stoi(this->_db.getPersonalStatus(answers[i])[3]),6);
		}
	}
	sendAnswerToTheUser(answer, currMessage->getSock());
}

void triviaServer::handleGetUsersInRoom(RecvMessages* currMessage)
{
	std::string msg = getRoomById(stoi(currMessage->getValues()[0]))->getUsersListMessage();
	sendAnswerToTheUser(msg,currMessage->getSock());
}

bool triviaServer::handleCreateRoom(RecvMessages* currMessage)
{
	
	User* user = getUserBySocket(currMessage->getSock());
	
	if (user != nullptr) {
		//Get data 
		
		std::string roomName = currMessage->getValues()[0];
		int playerNumber = stoi(currMessage->getValues()[1]);
		int questionsNumber = stoi(currMessage->getValues()[2]);
		int questionTimeInSec = stoi(currMessage->getValues()[3]);
		
		//Get the room id 
		_roomIdSequence++;
		//Create the room 
		bool answer = user->createRoom(_roomIdSequence, roomName, playerNumber, questionsNumber, questionTimeInSec);
		if (answer == true) {
			sendAnswerToTheUser(CREATE_ROOM_SUCCESS, currMessage->getSock());
			
			return true;
		}
		else
		{
			sendAnswerToTheUser(CREATE_ROOM_FAIL, currMessage->getSock());
			return false;
		}
	}
	sendAnswerToTheUser(CREATE_ROOM_FAIL, currMessage->getSock());
	return false;
}

bool triviaServer::handleJoinRoom(RecvMessages* currMessage)
{
	User* user = getUserBySocket(currMessage->getSock());
	
	if (user != nullptr) {
		
		int roomId = stoi(currMessage->getValues()[0]);
		if (getRoomById(roomId) != nullptr) {
			if (getRoomById(roomId)->isRoomIsFull() == false)
			{
				return user->joinRoom(getRoomById(roomId));
			}
			else{
				//send error message room is full (error code: 1101)
				sendAnswerToTheUser(JOIN_ROOM_FAIL_ROOM_IS_FULL,currMessage->getSock());
			}
		}
	}
    //send error message room not found or user is not found (error code: 1102)
	sendAnswerToTheUser(JOIN_ROOM_FAIL, currMessage->getSock());
	return false;
}

bool triviaServer::handleCloseRoom(RecvMessages* currMessage)
{
	if (getUserBySocket(currMessage->getSock())->closeRoom() != -1) {
		return true;
	}
	return false;
}





void triviaServer::addReceivedMessage(RecvMessages* curr)
{
	//Lock
	std::unique_lock<std::mutex> uniqueLock(_messageLock);
	//Add request to the list
	_messages.push(curr);
	//Unlock
	uniqueLock.unlock();
	//Notify that the the unique lock is unlock now
	_messagesCV.notify_all();

}
