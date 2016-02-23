#include "fileManager.h"

fileManager::fileManager(WrapperClass &WCR_) : WCR(WCR_)
{
	isOpen = false;
}

bool fileManager::openFile(bool loading, char Fname[])
{
	if (!loading)
	{
		OStream.open(Fname, ios::out | ios::binary);
		isOpen = OStream.is_open();
		//OStream.fail
		return OStream.is_open();
	}
	else
	{
		IStream.open(Fname, ios::out | ios::binary);
		isOpen = IStream.is_open();
		//OStream.fail
		return IStream.is_open();
	}
}


bool fileManager::openFile(bool loading, const char* Fname)
{
	if (!loading)
	{
		OStream.open(Fname, ios::out | ios::binary);
		isOpen = OStream.is_open();
		//OStream.fail
		return OStream.is_open();
	}
	else
	{
		IStream.open(Fname, ios::out | ios::binary);
		isOpen = IStream.is_open();
		//OStream.fail
		return IStream.is_open();
	}
}

unsigned char fileManager::loadByte()
{
	if (IStream.eof())
	{
		isOpen = false;
	}
	char b;
	IStream.read(&b, 1);
	return b + 128;
}

void fileManager::saveByte(char &SByte)
{

	if (!isOpen)
		return;
	if (OStream.eof())
	{
		isOpen = false;
		return;
	}
	OStream.write(&SByte - 127, 1);
}

void fileManager::saveByte(int SByte)
{
	if (!isOpen)
		return;
	if (OStream.eof())
	{
		isOpen = false;
		return;
	}
	char AByte = (char)SByte - 128;
	OStream.write(&AByte, 1);
}

void fileManager::save4Bytes(int B4Num)
{
	if (!isOpen)
		return;
	if (OStream.eof())
	{
		isOpen = false;
		return;
	}
	char ABytes[4];
	ABytes[0] = (B4Num >> 24) & 0xFF;
	ABytes[1] = (B4Num >> 16) & 0xFF;
	ABytes[2] = (B4Num >> 8) & 0xFF;
	ABytes[3] = B4Num & 0xFF;
	cout << "Saving int (" << B4Num << ") as: (" << ABytes[0] << ", " << ABytes[1] << ", " << ABytes[2] << ", " << ABytes[3] << ")" << endl;
	OStream.write(ABytes, 4);
}

int fileManager::load4Bytes()
{
	if(OStream.eof())
	{ 
		isOpen = false;
	}

	int Results = 0;
	char b[4];
	
	IStream.read(&b[3], 1);
	IStream.read(&b[2], 1);
	IStream.read(&b[1], 1);
	IStream.read(&b[0], 1);
	int Res = *(int *)b;
	cout << "Reading int (" << Res << ") as: (" << b[3] << ", " << b[2] << ", " << b[1] << ", " << b[0] << ")" << endl;
	return Res;
}

fileManager::~fileManager()
{
	OStream.close();
}
