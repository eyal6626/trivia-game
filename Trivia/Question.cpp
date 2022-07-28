#include "Question.h"
Question::Question(int id, std::string question, std::string correctAnswer, std::string secondAnswer, std::string thirdAnswer, std::string fourthAnswer)
{

	this->_id = id;
	this->_question = question;
	this->_correctAnswerIndex = rand() % 4;

	this->_answers[0] = "";
	this->_answers[1] = "";
	this->_answers[2] = "";
	this->_answers[3] = "";

	this->_answers[this->_correctAnswerIndex] = correctAnswer;
	std::string textAnswer[3] = { secondAnswer,thirdAnswer,fourthAnswer };
	int counter = 0;
	//Set answers
	for (int i = 0; i < 4; i++) {
		if (i != this->_correctAnswerIndex) {
			if (_answers[i] == "") {
				this->_answers[i] = textAnswer[counter];
				counter++;
			}
		}
	}
}

std::string Question::getQuestion()
{
	return this->_question;
}

std::string Question::getAnswer()
{
	return this->_answers[this->_correctAnswerIndex];
}
std::string* Question::getAnswers()
{
	return this->_answers;
}

int Question::getCorrectAnswerIndex()
{
	return this->_correctAnswerIndex;
}

int Question::getId()
{
	return this->_id;
}


void Question::setId(int id)
{
	this->_id = id;
}




Question* Question::createQuestion(int id, std::string question, std::string correctAnswer, std::string secondAnswer, std::string thirdAnswer, std::string fourthAnswer)
{
	return new Question(id, question, correctAnswer, secondAnswer, thirdAnswer, fourthAnswer);
}


