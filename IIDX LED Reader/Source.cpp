#include <Windows.h>
#include <stdio.h>
#include <iostream>
#include <tlhelp32.h>
#include <fstream>
#include <vector>
#include <thread>
BOOL FindProcess(LPWSTR pProcess)
{
	PROCESSENTRY32 pe32;
	pe32.dwSize = sizeof(PROCESSENTRY32);
	HANDLE hProcessSnap = ::CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
	if (hProcessSnap == INVALID_HANDLE_VALUE)
	{
		return false;
	}
	BOOL bMore = ::Process32First(hProcessSnap, &pe32);
	while (bMore)
	{
		if (wcscmp(pProcess, pe32.szExeFile) == 0){return true;}
		bMore = ::Process32Next(hProcessSnap, &pe32);
	}
	return false;
}

size_t HEXtoUINT(LPSTR str)
{
	UINT strLen = strlen(str);
	if (strLen != 8)return 0;
	size_t res = 0;
	for (UINT i = 0; i < strLen; i += 2)
	{
		res = res << 8;
		if ((str[i] < '0') || (str[i] > 'F' || ((str[i] > '9')) && (str[i] < 'A')))return E_INVALIDARG;
		if ('0' <= str[i] && str[i] <= '9') 
			res |= ((UINT)(str[i] - '0')) << 4;
		else
			res |= ((UINT)(str[i] - 'A' + 10)) << 4;
		if ('0' <= str[i + 1] && str[i + 1] <= '9')
			res |= ((UINT)(str[i + 1] - '0')) << 0;
		else
			res |= ((UINT)(str[i + 1] - 'A' + 10)) << 0;
	}
	return res;
}


int CALLBACK WinMain(
	HINSTANCE hInstance,
	HINSTANCE hPrevInstance,
	LPSTR     lpCmdLine,
	int       nCmdShow
) 
{
	AllocConsole();
	freopen("CONOUT$", "w", stdout);

	std::cout << "IIDX Memory Reader ver 1.0 Made by simakeng@http://github.com/simakeng" << std::endl;
	Sleep(2000);
	std::cout << "Memory Reader Starter......" << std::endl;
	Sleep(1000);
	std::cout << "Loading iidxmrconfig.ini" << std::endl;
	size_t addr = 0x116A9D70;
	size_t readbyte;
	int bitrate;
	char port[16] = { 0 };
	char GameStart[256] = { 0 };
	{
		std::ifstream ReadStream;
		ReadStream.open("iidxmrconfig.ini", std::ifstream::in);
		if (!ReadStream.is_open())
		{
			std::cout << "File not Found......" << std::endl;
			std::cout << "exiting......" << std::endl;
			Sleep(2000);
			FreeConsole();
			system("pause");
			return -1;
		}
		size_t CharCout = 0;
		while (ReadStream.get() != EOF)CharCout++;
		ReadStream.close();
	    ReadStream.open("iidxmrconfig.ini", std::ifstream::in);
		char* readbuffer = new char[CharCout + 1];
		ZeroMemory(readbuffer, sizeof(char)*(CharCout + 1));
		for (size_t i = 0; i < CharCout; i++)
		{
			readbuffer[i] = ReadStream.get();
		}
		ReadStream.close();
		size_t linecount = 0;
		std::vector<size_t> LineStart;
		LineStart.push_back(0);
		for (size_t i = 0; i < CharCout; i++) 
		{
			if (readbuffer[i] == '\n') 
			{ 
				readbuffer[i] = '\0'; 
				LineStart.push_back(i + 1);
			}
		}
		for (int i : LineStart) 
		{
			if (strstr(&(readbuffer[i]), "MemoryAddress") != 0)
			{
				char* str = &readbuffer[i] + strlen("MemoryAddress");
				for (size_t j = 0; j < strlen(str); j++)
				{
					if (str[j] == ' ' || str[j] == '=') 
					{
						str++;
						j--;
						continue;
					}
				}
				addr = HEXtoUINT(str);
			}
			if (strstr(&(readbuffer[i]), "ReadBytes") != 0)
			{
				char* str = &readbuffer[i] + strlen("ReadBytes");
				for (size_t j = 0; j < strlen(str); j++)
				{
					if (str[j] == ' ' || str[j] == '=')
					{
						str++;
						j--;
						continue;
					}
				}
				readbyte = atoi(str);
			}
			if (strstr(&(readbuffer[i]), "bitrate") != 0)
			{
				char* str = &readbuffer[i] + strlen("bitrate");
				for (size_t j = 0; j < strlen(str); j++)
				{
					if (str[j] == ' ' || str[j] == '=')
					{
						str++;
						j--;
						continue;
					}
				}
				bitrate = atoi(str);
			}
			if (strstr(&(readbuffer[i]), "Port") != 0)
			{
				char* str = &readbuffer[i] + strlen("Port");
				for (size_t j = 0; j < strlen(str); j++)
				{
					if (str[j] == ' ' || str[j] == '=')
					{
						str++;
						j--;
						continue;
					}
				}
				strcpy(port, str);
			}
			if (strstr(&(readbuffer[i]), "GameStart") != 0)
			{
				char* str = &readbuffer[i] + strlen("GameStart");
				for (size_t j = 0; j < strlen(str); j++)
				{
					if (str[j] == ' ' || str[j] == '=')
					{
						str++;
						j--;
						continue;
					}
				}
				strcpy(GameStart, str);
			}
		}
	}
	std::cout<<"Launch game......" << std::endl;
	if (FindProcess(TEXT("launcher.exe"))) 
	{
		std::cout << "launcher.exe is already existed." << std::endl;
	}
	else
	{
		std::cout << "launcher.exe ......" << std::endl;
		ShellExecuteA(NULL, "open", GameStart, NULL, NULL, NULL);

	}
	//std::thread th(system, GameStart);
	//Sleep(20000);
	size_t id = 0;
	std::cout << "Waiting game start......" << std::endl;
	while (id == 0) 
	{
		Sleep(10);
		PROCESSENTRY32 pe32;
		pe32.dwSize = sizeof(PROCESSENTRY32);
		HANDLE hProcessSnap = ::CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
		if (hProcessSnap == INVALID_HANDLE_VALUE)
		{
			return false;
		}
		BOOL bMore = ::Process32First(hProcessSnap, &pe32);

		while (bMore)
		{
			if (wcscmp(TEXT("launcher.exe"), pe32.szExeFile) == 0)
			{
				id = pe32.th32ProcessID;
				break;
			}
			bMore = ::Process32Next(hProcessSnap, &pe32);
		}
	}
	std::cout << "Game Started!" << std::endl;
	HANDLE hProc = OpenProcess(PROCESS_ALL_ACCESS, FALSE, id);
	if (hProc == 0)
	{
		std::cout << "Error occured when OpenProcess" << std::endl;
		system("pause");
		return -1;
	}


	HANDLE hCom = CreateFileA(
		port, //COM1口
		GENERIC_READ | GENERIC_WRITE, //允许读和写
		0, //独占方式
		NULL,
		OPEN_EXISTING, //打开而不是创建
		FILE_ATTRIBUTE_NORMAL | FILE_FLAG_OVERLAPPED, //重叠方式
		NULL);
	if (hCom == INVALID_HANDLE_VALUE)
	{
		std::cout << port << " Open Fail......" << std::endl;
	}
	DCB portDCB;
	GetCommState(
		hCom,//标识通讯端口的句柄
		&portDCB);//指向一个设备控制块（DCB结构）的指针
	portDCB.BaudRate = bitrate;
		SetCommState(hCom,//标识通讯端口的句柄
			&portDCB);//指向一个设备控制块（DCB结构）的指针
		bool init = false;
	while (true)
	{
		char *pBuf = new char[readbyte + 1];
		ZeroMemory(pBuf, readbyte);
		LPCVOID address = (BYTE*)0+ addr;
		bool fail = ReadProcessMemory(hProc, address, pBuf, readbyte, NULL);
		if (!fail)
		{
			if(init)
			{
				std::cout << "Error occured when ReadProcessMemory" << std::endl;
				system("pause");
				CloseHandle(hCom);
				CloseHandle(hProc);
				return -1;
			}
			std::cout << "Waiting For Game init......" << std::endl;
			while (!ReadProcessMemory(hProc, address, pBuf, readbyte, NULL))Sleep(1000);
			std::cout << "Sync Started!" << std::endl;
			Sleep(2000);
			std::cout << "Have Fun." << std::endl;
			Sleep(2000);
		}
		pBuf[readbyte] = 0;
		for (size_t i = 0; i < readbyte; i++) 
		{
			if (pBuf[i] == 'm')pBuf[i] == '.';
		}
		WriteFile(hCom, pBuf, readbyte, nullptr, NULL);
		
		std::cout << pBuf << std::endl;
		
		FILE* fp = fopen("debug.txt","w");
		fwrite(pBuf, 1, readbyte, fp);
		fclose(fp);
		
		Sleep(240);
		if (!FindProcess(TEXT("launcher.exe"))) 
		{
			std::cout << "Exiting.............." << std::endl;
			CloseHandle(hProc);
			CloseHandle(hCom);
			FreeConsole();
			return 0;
		}
	}
}
