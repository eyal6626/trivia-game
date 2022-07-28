#include "Database.h"
#include <map>

Database::Database()
{
	std::string dbFileName = "DB.sqlite";
	int doesFileExist = _access(dbFileName.c_str(), 0);
	int res = sqlite3_open(dbFileName.c_str(), &db);
	if (res != SQLITE_OK) {
		db = nullptr;
		std::cout << "Failed to open DB" << std::endl;
		exit(1);
	}

	if (doesFileExist == -1) {
		//USERS
		const char* q = "CREATE TABLE USERS (USERNAME TEXT NOT NULL,PASSWORD TEXT NOT NULL, MAIL TEXT NOT NULL);";
		char** errMessage = nullptr;
		res = sqlite3_exec(db, q, nullptr, nullptr, errMessage);
		if (res != SQLITE_OK) {
			std::cout << "Can't create users table" << std::endl;
			exit(1);
		}

		//QUESTIONS
		q = "CREATE TABLE QUESTIONS (QUESTION_ID INTEGER PRIMARY KEY AUTOINCREMENT,QUESTION TEXT NOT NULL, CORRECT_ANS TEXT NOT NULL, ANS1 TEXT NOT NULL, ANS2 TEXT NOT NULL, ANS3 TEXT NOT NULL);";
		errMessage = nullptr;
		res = sqlite3_exec(db, q, nullptr, nullptr, errMessage);
		if (res != SQLITE_OK) {
			std::cout << "Can't create questions table" << std::endl;
			exit(1);
		}

		//GAME
		q = "CREATE TABLE GAME (GAME_ID INTEGER PRIMARY KEY AUTOINCREMENT,STATUS INTEGER NOT NULL,START_GAME DATATIME NOT_NULL,END_GAME DATATIME NOT NULL);";
		errMessage = nullptr;
		res = sqlite3_exec(db, q, nullptr, nullptr, errMessage);
		if (res != SQLITE_OK) {
			std::cout << "Can't create game table" << std::endl;
			exit(1);
		}

		//PLAYER ANSWERS
		q = "CREATE TABLE PLAYERS_ANSWERS (GAME_ID INTEGER REFERENCES GAME(GAME_ID),USERNAME TEXT REFERENCES USERS(USERNAME),QUESTION_ID INTEGER REFERENCES QUESTIONS(QUESTION_ID),PLAYER_ANSWER TEXT NOT NULL, IS_CORRECT INTEGER NOT NULL,ANSWER_TIME INTEGER NOT NULL);";
		errMessage = nullptr;
		res = sqlite3_exec(db, q, nullptr, nullptr, errMessage);
		if (res != SQLITE_OK) {
			std::cout << "Can't create players answers table" << std::endl;
			exit(1);
		}
	}
}

Database::~Database()
{
	sqlite3_close(this->db);
	db = nullptr;
}

bool Database::isUserExist(std::string username)
{

	int res = 0;
	std::list<std::string> usernameList;
	std::list<std::string>::iterator it;

	//The query
	const char* q = "SELECT * FROM USERS;";
	char** errMessage = nullptr;

	//Get list usernames from the database
	res = sqlite3_exec(db, q, callBackCheckIfUserExist, &usernameList, errMessage);

	//Check for an error
	if (res != SQLITE_OK) {
		std::cout << "ERROR:Can't get data from the database!" << std::endl;
	}

	else
	{
		//Check if the user is exits 
		for (it = usernameList.begin(); it != usernameList.end(); ++it) {
			if (*it == username)
			{
				//std::cout << "User is existing!\n";
				return USERNAME_EXIST;
			}
		}
	}

	//std::cout << "User is not existing!\n";
	return USERNAME_NOT_EXIST;
}

bool Database::addNewUser(std::string username, std::string password, std::string mail)
{
	int res = 0;
	//The query
	std::string q = "INSERT INTO USERS VALUES(\"" + username + "\", " + "\"" + password + "\", " + "\"" + mail + "\"" ");";
	char** errMessage = nullptr;

	//Add new user into the database
	res = sqlite3_exec(db, q.c_str(), nullptr, nullptr, errMessage);

	//Check for an error
	if (res != SQLITE_OK) {
		std::cout << "ERROR:Can't add user to the database!" << std::endl;
	}
	else
	{
		std::cout << "User successfully added:\n| Username:" << username << " | Password:" << password << " | Mail:" << mail << " | \n";
		return true;
	}
	return false;
}

bool Database::dosePasswordMatch(std::string username, std::string password)
{

	int res = 0;
	std::vector<std::string> usernameAndPasswordList;
	std::vector<std::string>::iterator it;

	//The query
	const char* q = "SELECT * FROM USERS;";
	char** errMessage = nullptr;

	//Get a list of usernames and passwords 
	res = sqlite3_exec(db, q, callBackCheckIfPasswordMatch, &usernameAndPasswordList, errMessage);

	//Check for an error
	if (res != SQLITE_OK) {
		std::cout << "ERROR:Can't get data from the database!" << std::endl;
	}

	else
	{
		//Check if the user is exits 
		for (it = usernameAndPasswordList.begin(); it != usernameAndPasswordList.end(); ++it) {
			if (it[0] == username && it[1] == password)
			{
				//std::cout << "Password is matching the username!\n";
				return PASSWORD_MATCH;
			}
		}
	}

	//std::cout << "Password is not matching the username!\n";
	return PASSWORD_NOT_MATCH;
}

std::vector<Question*> Database::initQuestion(int numberOfQuestions)
{

	std::vector<Question*> questions;
	char** errMessage = nullptr;
	int res = 0;

	//The query
	std::string q = "SELECT * FROM QUESTIONS ORDER BY RANDOM() LIMIT " + std::to_string(numberOfQuestions) + ";";
	

	//Add new question into the database
	res = sqlite3_exec(db, q.c_str(), callBackQuestions, &questions, errMessage);
	
	//Check for an error
	return questions;
}

int Database::insertNewGame()
{
	int res = 0;
	//The query
	std::string q = "INSERT INTO GAME VALUES(NULL,0,DateTime('now'),0);";;
	char** errMessage = nullptr;

	//Add new user into the database
	res = sqlite3_exec(db, q.c_str(), nullptr, nullptr, errMessage);

	//Check for an error
	if (res != SQLITE_OK) {
		std::cout << "ERROR:Can't add game to the database!" << std::endl;
	}
	else
	{

		int gameId = 0;

		//The query
		q = "SELECT * FROM GAME ORDER BY GAME_ID DESC LIMIT 1;";
		errMessage = nullptr;

		//Get the last game id
		res = sqlite3_exec(db, q.c_str(), callBackGetLastGameId, &gameId, errMessage);

		//Check for an error
		if (res != SQLITE_OK) {
			std::cout << "ERROR:Can't get data from the database!" << std::endl;
			return -1;
		}
		else
		{
			return gameId;
		}
	}
	return -1;
}

bool Database::addAnswerToPlayer(int gameId, std::string username, int questionId, std::string answer, bool isCorrect, int answerTime)
{
	if (stoi(answer) == 5) {
		answer = "";
	}
	int res = 0;
	int isCorrectInInt = 0;
	if (isCorrect == false) {
		isCorrectInInt = 0;
	}
	else {
		isCorrectInInt = 1;
	}
	//The query
	std::string q = "INSERT INTO PLAYERS_ANSWERS VALUES(" + std::to_string(gameId) + ",\"" + username + "\"," + std::to_string(questionId) + ",\"" + answer + "\"," + std::to_string(isCorrectInInt) + "," + std::to_string(answerTime) + ");";;
	char** errMessage = nullptr;

	//Add answer to the database
	res = sqlite3_exec(db, q.c_str(), nullptr, nullptr, errMessage);

	//Check for an error
	if (res != SQLITE_OK) {
		std::cout << "ERROR:Can't add answer to the database!" << std::endl;
		return false;
	}
	
	return true;
}

bool Database::updateGameStatus(int id)
{
	int res = 0;
	std::list<std::string> usernameList;
	std::list<std::string>::iterator it;

	//The query
	std::string q = "UPDATE GAME SET STATUS = 1 WHERE GAME_ID = " + std::to_string(id) + ";";
	char** errMessage = nullptr;

	res = sqlite3_exec(db, q.c_str(), nullptr, nullptr, errMessage);

	//Check for an error
	if (res != SQLITE_OK) {
		std::cout << "ERROR:Can't set data!" << std::endl;
		return false;
	}

	return true;

	
}

std::vector<std::string> Database::getPersonalStatus(std::string username)
{

	int res = 0;
	std::vector<std::string> results;
	std::map<std::string, int> info;
	info.insert({ "time",0 });
	info.insert({ "answerSum",0 });
	info.insert({ "correct",0 });
	info.insert({ "wrong",0 });
	info.insert({ "games",0 });
	info.insert({ "lastGameId",-1 });
	

	//The query
	std::string q = "SELECT * FROM PLAYERS_ANSWERS WHERE USERNAME = \"" + username + "\"" + ";";
	char* errMessage = nullptr;
	res = sqlite3_exec(db, q.c_str(), callBackgetPersonalStatus, &info, &errMessage);

	//Check for an error
	if (res != SQLITE_OK) {
		std::cout << "ERROR:Can't get data!" << std::endl;
		return results;
	}
	//Calc avrage time
	
	
	
	results.push_back(std::to_string(info.find("time")->second));
	results.push_back(std::to_string(info.find("answerSum")->second));
	results.push_back(std::to_string(info.find("wrong")->second));
	results.push_back(std::to_string(info.find("correct")->second));
	results.push_back(std::to_string(info.find("games")->second));
	return results;


}

bool Database::addNewQuestion(std::string question, std::string correctAnswer, std::string secondAnswer, std::string thirdAnswer, std::string fourthAnswer)
{
	char** errMessage = nullptr;
	int res = 0;
	
	//The query
	std::string q = "INSERT INTO QUESTIONS VALUES(NULL,\"" + question + "\", " + "\"" + correctAnswer + "\", " + "\"" + secondAnswer + "\"" + "\"" + thirdAnswer + "\"" + "\"" + fourthAnswer + "\"" ");";
	errMessage = nullptr;

	//Add new question into the database
	res = sqlite3_exec(db, q.c_str(), nullptr, nullptr, errMessage);

	//Check for an error
	if (res != SQLITE_OK) {
		return false;
	}
	else
	{
		return true;
	}
	return false;
}

void Database::addNewQuestions(std::string questionFile, std::string answerFile)
{
	std::string question;
	std::string answers[4];

	std::ifstream questionFileP(questionFile);
	std::ifstream answerFileP(answerFile);
	//Read questions
	while (std::getline(questionFileP, question)) {
		//Read answer
		for (size_t i = 0; i < 4; i++)
		{
			std::getline(answerFileP,answers[i]);
		}
		//add question to the database
		addNewQuestion(question, answers[0], answers[1], answers[2], answers[3]);
		
	}
	questionFileP.close();
	answerFileP.close();
}

std::vector<std::string> Database::getBestScores()
{
	int res = 0;
	std::vector<std::string> results;
	std::vector<std::string> names;
	std::vector<int> topScore;
	int temp = 0;
	std::string nameTemp;
	int avrageTime = 0;
	std::map<std::string, int> scores;
	int score = 0;
	scores.insert({ "time",0 });
	scores.insert({ "answerSum",0 });
	scores.insert({ "correct",0 });
	scores.insert({ "wrong",0 });
	scores.insert({ "games",0 });
	scores.insert({ "lastGameId",-1 });



	//Get names
	std::string q = "SELECT DISTINCT(USERNAME) FROM PLAYERS_ANSWERS;";
	char* errMessage = nullptr;
	res = sqlite3_exec(db, q.c_str(), callBackGetNamesList, &names, &errMessage);

	for (auto it : names) {
		//The query

		q = "SELECT * FROM PLAYERS_ANSWERS WHERE USERNAME = \"" + it + "\"" + ";";
		res = sqlite3_exec(db, q.c_str(), callBackgetPersonalStatus, &scores, &errMessage);
		if (scores.find("wrong")->second == 0) {
			scores.find("wrong")->second = 1;
		}
		if (scores.find("time")->second != 0 && scores.find("answerSum")->second != 0 && scores.find("correct")->second != 0 && scores.find("wrong")->second != 0){
		avrageTime = scores.find("time")->second / scores.find("answerSum")->second;
		score = (scores.find("correct")->second * scores.find("games")->second);
		score = score / (avrageTime * scores.find("wrong")->second);
	    }

		std::cout << "\n" << it << " points:" << score;
		topScore.push_back(score);
		score = 0;
		//check who is the biggest
		avrageTime = 0;
		scores.find("time")->second = 0;
		scores.find("answerSum")->second = 0;
		scores.find("correct")->second = 0;
		scores.find("games")->second  = 0;
		scores.find("wrong")->second = 0;
	}


	for (int i = 0; i < names.size(); i++) {
		for (int j = 0; j < names.size(); j++) {
			if (topScore[i] > topScore[j]) {
				temp = topScore[i];
				nameTemp = names[i];
				names[i] = names[j];
				topScore[i] = topScore[j];
				topScore[j] = temp;
				names[j] = nameTemp;
			}
			
		}
	}


	//Check for an error
	if (res != SQLITE_OK) {
		std::cout << "ERROR:Can't get data!" << std::endl;
		return results;
	}
	//Calc avrage time
	
	if (names.size() >= 1) {
		results.push_back(names[0]);
	}
	else {
		results.push_back("");
	}
	if (names.size() >= 2)
	{
		results.push_back(names[1]);
	}
	else {
		results.push_back("");
	}
	if (names.size() >= 3)
	{
		results.push_back(names[2]);
	}
	else {
		results.push_back("");
	}
	return results;
}


int Database::callBackQuestions(void* NotUsed, int argc, char** argv, char** szColName)
{

	std::vector<Question*>* questions = (std::vector<Question*>*)(NotUsed);
	std::string question;
	int id = 0;
	std::string correctAnswer;
	std::string ans1;
	std::string ans2;
	std::string ans3;

	for (int i = 0; i < argc; i++)
	{
		if (std::string(szColName[i]) == "QUESTION") {

			question = argv[i];
		}
		else if (std::string(szColName[i]) == "QUESTION_ID") {
			id = (atoi(argv[i]));
		}
		else if (std::string(szColName[i]) == "CORRECT_ANS") {
			correctAnswer = argv[i];
		}
		else if (std::string(szColName[i]) == "ANS1") {
			ans1 = argv[i];
		}
		else if (std::string(szColName[i]) == "ANS2") {
			ans2 = argv[i];
		}
		else if (std::string(szColName[i]) == "ANS3") {
			ans3 = argv[i];
		}
	}
	questions->push_back(Question::createQuestion(id, question, correctAnswer, ans1, ans2, ans3));
	return 0;
}

int Database::callBackCheckIfUserExist(void* NotUsed, int argc, char** argv, char** szColName)
{
	std::list<std::string>* usernameList = (std::list<std::string>*)(NotUsed);
	for (int i = 0; i < argc; i++)
	{
		if (std::string(szColName[i]) == "USERNAME") {
			usernameList->push_back(argv[i]);
		}
	}

	return 0;
}

int Database::callBackCheckIfPasswordMatch(void* NotUsed, int argc, char** argv, char** szColName)
{
	std::vector<std::string>* usernameAndPasswordList = (std::vector<std::string>*)(NotUsed);
	for (int i = 0; i < argc; i++)
	{
		if (std::string(szColName[i]) == "USERNAME") {
			usernameAndPasswordList[0].push_back(argv[i]);
		}
		else if (std::string(szColName[i]) == "PASSWORD") {
			usernameAndPasswordList[0].push_back(argv[i]);
		}
	}
	return 0;
}

int Database::callBackGetLastGameId(void* NotUsed, int argc, char** argv, char** szColName)
{
	int* gameId = (int*)(NotUsed);
	for (int i = 0; i < argc; i++)
	{
		if (std::string(szColName[i]) == "GAME_ID") {
			*gameId = atoi(argv[i]);
		}
	}

	return 0;
}

int Database::callBackgetPersonalStatus(void* NotUsed, int argc, char** argv, char** szColName)
{
	std::map<std::string, int>* info = (std::map<std::string, int>*)(NotUsed);
	

	for (int i = 0; i < argc; i++)
	{
		
		if (std::string(szColName[i]) == "ANSWER_TIME") {
			info->find("time")->second += atoi(argv[i]);
		}
		else if (std::string(szColName[i]) == "PLAYER_ANSWER") {
			info->find("answerSum")->second += 1;
		}
		else if (std::string(szColName[i]) == "IS_CORRECT") {
			if (strcmp(argv[i],"1") == 0) {
				info->find("correct")->second += 1;
			}
			else {
				info->find("wrong")->second += 1;
			}
		}
		else if (std::string(szColName[i]) == "GAME_ID" && atoi(argv[i]) != info->find("lastGameId")->second) {
			info->find("games")->second += 1;
			info->find("lastGameId")->second = atoi(argv[i]);
		}
		
	}

	return 0;
}

int Database::callBackGetNamesList(void* NotUsed, int argc, char** argv, char** szColName)
{
	std::vector<std::string>* names = (std::vector<std::string>*)(NotUsed);
	for (int i = 0; i < argc; i++)
	{
		if (std::string(szColName[i]) == "USERNAME") {
			names->push_back(argv[i]);
			
		}
	}
	return 0;
}

