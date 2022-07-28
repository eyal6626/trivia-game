#pragma once

#include <iostream>


class  Question
{
public:
	Question(int id ,std::string question ,std::string correctAnswer, std::string secondAnswer, std::string thirdAnswer, std::string fourthAnswer);
	std::string getQuestion();
	std::string getAnswer();
	std::string* getAnswers();
	int getCorrectAnswerIndex();
	int getId();
	void setId(int id);
	void setWorngAnswers(std::string ans1, std::string ans2, std::string ans3);
	static Question* createQuestion(int id, std::string question, std::string correctAnswer, std::string secondAnswer, std::string thirdAnswer, std::string fourthAnswer);

private:
	std::string _question;
	std::string _answers[4];
	int _correctAnswerIndex;
	int _id;
};

