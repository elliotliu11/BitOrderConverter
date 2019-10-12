// Filename: BitOrderConverter.cpp
// Description: To convert bit order of hex values, the rest of text is just copied
// Author(s):  Elliot Liu
// Date: Oct 12 2019

#include <fstream>
#include <iostream>

const unsigned char hex[] = "0123456789ABCDEF";

class bo_sm {
	unsigned char	data;	//	hold the binary data
	unsigned char	first_byte;
	std::ofstream& ofs;	
	enum  {READY, WAIT_FOR_X, WAIF_FOR_FIRST_BYTE, WAIT_FOR_SECOND_BYTE} stats;
public:
	bo_sm(std::ofstream& ofs_in) : ofs(ofs_in), stats(READY), data(0) {};
	bool put(unsigned char input);
};

/*
	main
		open the input file and start the bit order control loop

	Arguements:
		argv[1]	IN
			the input file name
	Return:
*/
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

	bo_sm BitOrderConverter(ofs);

	//	read char one by one
	char c = ifs.get();

	while (ifs.good()) {

		//	enter the state machine
		if (BitOrderConverter.put(c)) {
			ofs.put(c);
			if (!ofs.good()) {
				std::cout << "Ouput file failed";
				ofs.close();
				ifs.close();
				return EXIT_FAILURE;
			}
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

/*
	bo_sm::put
		get input byte by byte. If 0x is found, convert the next two bytes to binary and convert the bit order.
		Write results back to the stream when conversion completes

	Arguements:
		input	IN
			the input data in ASCII
	Return:
		bool
			return true to tell the caller to copy the input data to output
			return false to tell the caller to skip the copy action
*/
bool bo_sm::put(unsigned char input)
{
	bool ret = true;
	switch(stats)
	{
	case READY:
		if (input == '0')
			stats = WAIT_FOR_X;
		break;
	case WAIT_FOR_X:
		if (input == 'x')
			stats = WAIF_FOR_FIRST_BYTE;
		else
			stats = READY;
		break;
	case WAIF_FOR_FIRST_BYTE:
		stats = WAIT_FOR_SECOND_BYTE;
		if (input >= '0' && input <= '9') {
			data = input - '0';
			ret = false;
		}
		else if (input >= 'A' && input <= 'F') {
			data = input - 'A' + 10;
			ret = false;
		}
		else if (input >= 'a' && input <= 'f') {
			data = input - 'a' + 10;
			ret = false;
		}
		else
		{
			stats = READY;
			ret = true;
		}
		data <<= 4;
		first_byte = input;
		break;
	case WAIT_FOR_SECOND_BYTE:
		stats = READY;
		if (input >= '0' && input <= '9') {
			data += input - '0';
			ret = false;
		}
		else if (input >= 'A' && input <= 'F') {
			data += input - 'A' + 10;
			ret = false;
		}
		else if (input >= 'a' && input <= 'f') {
			data += input - 'a' + 10;
			ret = false;
		}
		else
		{	//	not valid hex value, put first byte back
			ofs.put(first_byte);
			ret = true;
		}

		if (ret == false) {
			//	convert bit order
			unsigned char new_val = 0;
			for (unsigned char mask = 0x01;; mask <<=1) {
				if (data & mask) {
					new_val |= 0x01;
				}
				if (mask == 0x80)	break;
				else {
					new_val <<= 1;
				}
			}
			//	convert data bits to hex value
			ofs.put(hex[new_val >> 4]);
			ofs.put(hex[new_val & 0x0F]);
		}
		break;
	}
	return ret;
}
