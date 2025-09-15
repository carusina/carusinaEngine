#include <iostream>
#include <memory>
#include <windows.h>

#include "ExampleApp.h"

using namespace std;

int main() {
	ExampleApp exampleApp;

	if (!exampleApp.Initialize())
	{
		cout << "Initialization failed.\n";
		return -1;
	}

	return exampleApp.Run();
}