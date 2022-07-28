#pragma comment (lib, "ws2_32.lib")

#include "WSAInitializer.h"
#include "triviaServer.h"
#include "Database.h"
#include <iostream>
#include <exception>

int main()
{
	

	try
	{
		srand(time(NULL));
		WSAInitializer wsaInit;
		triviaServer myServer;
		

		myServer.serve(8787);
	}
	catch (std::exception& e)
	{
		std::cout << "Error occured: " << e.what() << std::endl;
	}

	system("PAUSE");
	return 0;
}