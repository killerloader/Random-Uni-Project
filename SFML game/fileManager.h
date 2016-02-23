#pragma once
#include "Main.h"
#include <fstream>

struct WrapperClass;

class fileManager
{
public:
	bool isOpen;
	bool openFile(bool, char[]);
	bool openFile(bool, const char*);
	void saveByte(char&);
	void saveByte(int);
	void save4Bytes(int);
	int load4Bytes();
	unsigned char loadByte();
	ofstream OStream;
	ifstream IStream;
	WrapperClass &WCR;
	fileManager(WrapperClass &WCR_);
	~fileManager();
};

