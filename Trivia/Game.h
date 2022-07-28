#pragma once
#include <map>
#include <vector> 
#include "Question.h"
#include "User.h"
#include "Database.h"

class User;
class Game {
public:
    Game(const std::vector<User*>& players, int questionsNo, Database& db);
    ~Game();
    void sendQuestionToAllUsers(void);
    void sendFirstQuestion(void);
    int getCurrentTurnAnswer();
    bool handleAnswerFromUser(User* user, int answerNo, int time);
    bool handleNextTurn();
    int getId();
    bool isRoomIsEmpty();
    bool didEveryoneAnswer();
    void handleFinishGame();
    bool leaveGame(User* currUser);
private:
    int _gameId;
    std::vector<Question*> _questions;
    std::vector<User*> _players;
    int _questions_no;
    int _currQuestionIndex;
    Database& _db;
    std::map<std::string, int> _result;
    int _currentTurnAnswer;

};