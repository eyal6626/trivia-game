#include "Game.h"

Game::Game(const std::vector<User*>& players, int questionsNo, Database& db) : _players (players) , _db (db)
{
	
	this->_gameId = this->_db.insertNewGame();
	if (this->_gameId != -1) {
		this->_questions = this->_db.initQuestion(questionsNo);
		for (auto it : this->_players) {
			it->setGame(this);
			this->_result.insert({ it->getUsername(),0 });
		}
		this->_currentTurnAnswer = 0;
		this->_questions_no = questionsNo;
	}
	else {
		this->_currentTurnAnswer = -999;
	}
}

Game::~Game()
{
	for (auto it : this->_questions) {
		delete it;
	}
	this->_questions.clear();
	this->_players.clear();
}

void Game::sendQuestionToAllUsers(void)
{
	
	std::string question = this->_questions[this->_currQuestionIndex]->getQuestion();
	std::string* answers = this->_questions[this->_currQuestionIndex]->getAnswers();

	//Build the message
	std::string answer = "118" + Helper::getPaddedNumber(question.size(),3) + question;
	answer = answer + Helper::getPaddedNumber(answers[0].size(), 3) + answers[0];
	answer = answer + Helper::getPaddedNumber(answers[1].size(), 3) + answers[1];
	answer = answer + Helper::getPaddedNumber(answers[2].size(), 3) + answers[2];
	answer = answer + Helper::getPaddedNumber(answers[3].size(), 3) + answers[3];
	
	//Send the message
	for (auto it : this->_players) {
		std::cout << "\nServer:" << answer << "\n";
		it->send(answer);
	}
}

void Game::sendFirstQuestion(void)
{
	this->_currentTurnAnswer = 0;
	sendQuestionToAllUsers();
}

int Game::getCurrentTurnAnswer()
{
	return this->_currentTurnAnswer;
}

bool Game::handleAnswerFromUser(User* user, int answerNo, int time)
{
	bool isCorrect = true;
	
	//check if the user answer is correct or not
	if (answerNo == this->_questions[this->_currQuestionIndex]->getCorrectAnswerIndex()+1) {
		std::map<std::string, int>::iterator it = this->_result.find(user->getUsername());
		it->second = it->second++;
	}
	else {
		isCorrect = false;
	}
	
	//Add the answer to the database
	this->_db.addAnswerToPlayer(this->getId(), user->getUsername(), this->_questions[this->_currQuestionIndex]->getId(), std::to_string(answerNo), isCorrect, time);
	
	//Send answer to the user
	if (isCorrect == true) {
		user->send("1201");
	}
	else {
		user->send("1200");
	}
	this->_currentTurnAnswer++;
	return false;
}

bool Game::handleNextTurn()
{
	//If the room is not empty
	if (isRoomIsEmpty() == false) {
		if (didEveryoneAnswer() == true) {
			if (this->_questions_no == (this->_currQuestionIndex+1)) {
				handleFinishGame();
			}
			else {
				this->_currentTurnAnswer = 0;
				this->_currQuestionIndex++;
				sendQuestionToAllUsers();
			}
		}
	}
	//If the room is empty
	else {
		handleFinishGame();
	}

	return false;
}

int Game::getId()
{
	return this->_gameId;
}

bool Game::isRoomIsEmpty()
{
	int counter = 0;
	std::vector<User*>::iterator it;
	for (auto it : this->_players) {
		if (it->getRoom() != nullptr) {
			counter++;
			if (counter >= 1) {
				return false;
			}
		}
	}
	return true;
}

bool Game::didEveryoneAnswer()
{
	return this->_currentTurnAnswer == this->_players.size();
}

void Game::handleFinishGame()
{
	if (this->_db.updateGameStatus(this->_gameId)) {
		std::string answer;
		answer = "121" + Helper::getPaddedNumber(this->_players.size(), 1);
		try
		{
			for (auto it : this->_players) {
				answer = answer + Helper::getPaddedNumber(it->getUsername().size(), 2);
				answer = answer + it->getUsername() + Helper::getPaddedNumber(this->_result.find(it->getUsername())->second, 2);
				it->setRoom(nullptr);
				it->setGame(nullptr);
			}
			std::cout << "\nServer:" + answer + "\n";
			for (auto it : this->_players) {
				it->send(answer);
			}
		}
			catch (const std::exception&)
			{
				std::cout << "ERROR:Can't disconnect user from the room!\n";
			}

	}
}

bool Game::leaveGame(User* currUser)
{
	int i = 0;
	this->_result.erase(currUser->getUsername());
	for (auto it : this->_players) {
		if (it->getUsername() == currUser->getUsername()) {
			std::cout << it->getUsername();
			this->_players.erase(this->_players.begin()+i);
			i++;
		}
	}
	return handleNextTurn();;
}


