// BitOrderConverter.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <fstream>
#include <iostream>

class bo_sm {
	enum state
	{
		STANDBY,	//	waiting for the next input
		CONTINUE,	//	waiting for the next hex input, caller should not write
		COMPLETE	//	ready for the next input
	};
	unsigned char	data;	//	hold the binary data
	std::ofstream& ofs;		
public:
	bo_sm(std::ofstream& ofs_in): ofs(ofs_in);
	put(unsigned char input);
};


int main(int argc, char** argv)
{
	char* pfiledot;
	const char* file_ext = ".txt";	//	target file extenion
	char target_file[_MAX_PATH];	//	tearget file path

	if (argv[1] == NULL) {
		std::cout << "Input file required";
		return EXIT_SUCCESS;
	}

	//	construct the target file path
	strcpy_s(target_file, argv[1]);
	pfiledot = strchr(target_file, '.');
	if (pfiledot) {
		*pfiledot = '\0';
	}
	strcat_s(target_file, file_ext);

	//	open src and dst files
	std::ifstream ifs(argv[1], std::ifstream::in);
	std::ofstream ofs(target_file, std::ifstream::out);

	//	read char one by one
	char c = ifs.get();

	while (ifs.good()) {

		//	enter the state machine
		ofs.put(c);
		if (!ofs.good()) {
			std::cout << "Ouput file failed";
			ofs.close();
			ifs.close();
			return EXIT_FAILURE;
		}
		c = ifs.get();
	}

	if (!ifs.eof()) {
		std::cout << "input file error";
	}

	ifs.close();
	ofs.close();
	return EXIT_SUCCESS;
}

bo_sm::put(unsigned char input)
{

}
