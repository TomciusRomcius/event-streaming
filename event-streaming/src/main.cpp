#include <iostream>
#include "application/application.h"

int main()
{
	std::cout << "Event streaming application started." << std::endl;

	Application app;
	app.Start();
	return 0;
}
