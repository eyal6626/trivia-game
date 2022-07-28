#pragma once
#include <iostream>
#include "Libraries/sqlite3/sqlite3.h"
#include <string>
#include <io.h>
#include <list>
#include <vector>
#include <fstream>
#include "Question.h"
#include <algorithm>

#define USERNAME_NOT_EXIST false
#define USERNAME_EXIST true

#define PASSWORD_NOT_MATCH false
#define PASSWORD_MATCH true

class Database {
public:
	Database();//c'tor
	~Database();//d'tor

	bool isUserExist(std::string username);//Check if the username is already exist in the database 
	bool addNewUser(std::string username, std::string password, std::string mail);//Added new user to the database
	bool dosePasswordMatch(std::string username, std::string password);//Check if the password and the username matching
	std::vector<Question*> initQuestion(int numberOfQuestions);
	int insertNewGame();
	bool addAnswerToPlayer(int gameId, std::string username,int questionId,std::string answer,bool isCorrect, int answerTime);
	bool updateGameStatus(int id);
	std::vector<std::string> getPersonalStatus(std::string username);
	bool addNewQuestion(std::string question, std::string correctAnswer, std::string secondAnswer, std::string thirdAnswer, std::string fourthAnswer);//Add new question to the database
	void addNewQuestions(std::string questionFile, std::string answerFile);
	std::vector<std::string> getBestScores();
private:
	static int callBackQuestions(void* NotUsed, int argc, char** argv, char** szColName);
	static int callBackCheckIfUserExist(void* NotUsed, int argc, char** argv, char** szColName);
	static int callBackCheckIfPasswordMatch(void* NotUsed, int argc, char** argv, char** szColName);
	static int callBackGetLastGameId(void* NotUsed, int argc, char** argv, char** szColName);
	static int callBackgetPersonalStatus(void* NotUsed, int argc, char** argv, char** szColName);
	static int callBackGetNamesList(void* NotUsed, int argc, char** argv, char** szColName);

	sqlite3* db;

};