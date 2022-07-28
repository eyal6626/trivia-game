#pragma once

#define PORT 8787

//Client to Server - login request
#define LOG_IN_REQUEST 200

//Server to Client - login request
#define LOGIN_SUCCESS "1020"
#define LOGIN_WRONG_DETAILS "1021"
#define LOGIN_USER_IS_ALREADY_CONNECTED "1022"

//Client to Server - signup request
#define CREATE_USER_REQUEST 203

//Server to Client - signup request
#define SIGNUP_SUCCESS "1040"
#define SIGNUP_PASS_ILLEGAL "1041"
#define SIGNUP_USERNAME_IS_ALREADY_EXISTS "1042"
#define SIGNUP_USERNAME_IS_ILLEGAL "1043"
#define SIGNUP_OTHER "1044"

//Client to Server - signout
#define LOGOUT_REQUEST 201

//Server to Client - signout request
#define LOGOUT_SUCCESS "201"

//Client to Server - Join room
#define GET_USERS_LIST 207
#define GET_LIST_OF_ROOMS 205
#define JOIN_ROOM 209


//Server to Client - Join room
#define JOIN_ROOM_FAIL_ROOM_IS_FULL "1101"
#define JOIN_ROOM_FAIL "1102"

//Client to Server - create room
#define CREATE_ROOM 213

//Server to Client - create room
#define CREATE_ROOM_SUCCESS "1140"
#define CREATE_ROOM_FAIL "1141"

//Client to Server - leave room
#define LEAVE_ROOM 211

//Server to Client - leave room
#define LEAVE_ROOM_SUCCESS "1120"

//Client to Server - close room
#define CLOSE_ROOM 215

//Server to Client - close room
#define CLOSE_ROOM_SUCCESS "116"

//Client to Server - start game
#define START_GAME_REQUEST 217

//Server to Client - start game
#define START_GAME_SUCCESS "118"
#define START_GAME_FAIL "1180"

//Client to Server - answers
#define GAME_ANSWER 219

//Server to Client - answers
#define SERVER_ANSWER "120"
#define GAME_ANSWER 219

//Client to Server - leave game
#define LEAVE_GAME 222

//Client to Server - personal status
#define PERSONAL_STATUS 225

//Server to Client - personal status
#define PERSONAL_STATUS_ANSWER "126"

//Client to Server - best score
#define BEST_SOCRE 223

//Server to Client - best score
#define BEST_SOCRE_ANSWER "124"


//Other
#define VALID_NEW_USER true
#define INVALID_NEW_USER false
#define EXIT 299


#include <iostream>
#include <WinSock2.h>
#include <Windows.h>
#include <string>
#include <mutex>
#include <queue>
#include <map>
#include "RecvMessage.h"
#include "Database.h"


class triviaServer
{
public:
	static int _loginCounter;
	static int _roomIdSequence;
	triviaServer();
	~triviaServer();
	void serve(int port);

private:
	//SOCKET AND SERVER
	void addReceivedMessage(RecvMessages* curr);
	void bindAndListen();
	void accept();
	void clientHandler(SOCKET clientSocket);
	RecvMessages* build_receive_message(SOCKET sock, int msgCode);
	void handleReceivedMessages();

	//USER REQUESTS - signin/signout/signup
	User* hendleSignin(RecvMessages* currMessage);
	bool hendleSignup(RecvMessages* currMessage);
	void hendleSigout(RecvMessages* currMessage);


	//SERVER TO USER 
	void sendAnswerToTheUser(std::string msgcode, SOCKET sock);

	//USER REQUESTS - ROOMS
	bool handleCreateRoom(RecvMessages* currMessage);
	bool handleJoinRoom(RecvMessages* currMessage);
	void handleGetRooms(RecvMessages* currMessage);
	void handleGetUsersInRoom(RecvMessages* currMessage);
	bool handleLeaveRoom(RecvMessages* currMessage);
	bool handleCloseRoom(RecvMessages* currMessage);

	//USER REQUESTS - GAME
	void handleStartGame(RecvMessages* currMessage);
	void handlePlayerAnswer(RecvMessages* currMessage);
	bool handleLeaveGame(RecvMessages* currMessage);

	//USER REQUESTS - SCORE
	void handleGetPersonalStatus(RecvMessages* currMessage);
	void handleGetBestScores(RecvMessages* currMessage);

	//HELPERS
	User* getUserBySocket(SOCKET sock);
	User* getUserByName(std::string username);
	Room* getRoomById(int id);
	
	void countLogin();
	void printLoginCounter();

	Database _db;
	std::mutex _mtxRecievedMessages;
	SOCKET _serverSocket;
	std::queue<RecvMessages*> _messages;
	std::mutex _messageLock;
	std::condition_variable _messagesCV;
	std::map<SOCKET, User*> _connectedUsers;

	std::map<int,Room*> _roomsList;

};

