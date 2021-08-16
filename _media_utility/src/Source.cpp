#include "FAT32.h"
#include "Format.h"
#include <iostream>
#include <string>
#include <time.h>
#include <stdlib.h>
#include <windows.h>

const char* title = 
	" || This media_utility  will format a Physical  Drive  and ||\n"
	" || install  Sore-OS on it.  Be careful  with selecting a  ||\n"
	" || Physical Drive, as this can cause permanent data loss. ||\n\n";

const char* words[12] = {
	"original", "crystal", "power", "difference",
	"reception", "intensify", "allowance", "particle",
	"meeting", "module", "compact", "translate"
};

int main()
{
	int ID;
	std::cout<< title << "\tPhysical Drive Number: ";
	std::cin >> ID;

	std::string driveName = "\\\\.\\PhysicalDrive"+std::to_string(ID);
	std::cout<< "\t" << driveName << "\n";

	int check = safetyCheck(driveName.c_str(), 0x1B6, "Sore-OS");
	if(check == -2)
	{
		srand((unsigned int)time(NULL));
		int wordIndex = rand() % 12;
		std::string input;

		std::cout<< "\n This Drive has never been formatted by this utility!";
		std::cout<< "\n Type in the following word to proceed";
		std::cout<< "\n\tType \"" << words[wordIndex] << "\": ";
		std::cin >> input;

		check = words[wordIndex] == input ? 0 : -1;
	}

	if(check ==0)
	{	int res = build(driveName.c_str(), ID);
		std::cout << "\n ~~~ Build "<< (res == 0 ? "Successful" : "Unsuccessful") << " ~~~\n ";
	}
	else
	{	std::cout<< "\nFailed to select Physical Drive!\n";
	}

	system("PAUSE");
}