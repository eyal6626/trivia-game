#include "RecvMessage.h"

RecvMessages::RecvMessages(SOCKET sock, int messageCode)
{
    this->_sock = sock;
    this->_messageCode = messageCode;
}

RecvMessages::RecvMessages(SOCKET sock, int messageCode, std::vector<std::string> values)
{
    this->_sock = sock;
    this->_messageCode = messageCode;
    _values = values;
}

SOCKET RecvMessages::getSock()
{
    return this->_sock;
}

User* RecvMessages::getUser()
{
    return this->_user;
}

void RecvMessages::setUser(User* user)
{
    this->_user = user;
}

int RecvMessages::getMessageCode()
{
    return this->_messageCode;
}

std::vector<std::string> RecvMessages::getValues()
{
    return this->_values;
}

