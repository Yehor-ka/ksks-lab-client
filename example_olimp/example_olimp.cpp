#pragma comment(lib, "Ws2_32.lib")
#pragma warning( disable : 4996)
#include <stdio.h>
#include <string>
#include <winsock2.h>
#include <windows.h>
#include <ctime>
#include <iostream>
using namespace std;

#define PORT 54000
#define SERVERADDR "127.0.0.1"

HANDLE hSerial;
LPCTSTR sPortName = L"COM3";



int main(int argc, char* argv[])
{
	char buff[10 * 1014];
	char buff1[10 * 1014];


	printf("UDP Client \nType quit to quit\n");
	// Крок 1 - ініціалізація бібліотеки Winsock

	if (WSAStartup(0x202, (WSADATA*)&buff[0]))
	{
		printf("WSAStartup error : %d \n", WSAGetLastError());
		return -1;
	}
	// Крок 2 - відкриття сокета

	SOCKET my_sock = socket(AF_INET, SOCK_DGRAM, 0);
	if (my_sock == INVALID_SOCKET)
	{
		printf("socket() error: %d \n", WSAGetLastError());
		WSACleanup();
		return -1;
	}
	// Крок 3 - обмін повідомлень з сервером

	HOSTENT* hst;
	sockaddr_in dest_addr;
	dest_addr.sin_family = AF_INET;
	dest_addr.sin_port = htons(PORT);
	// Визначення IP_адреса вузла
	if (inet_addr(SERVERADDR))
		dest_addr.sin_addr.s_addr = inet_addr(SERVERADDR);
	else
		if (hst = gethostbyname(SERVERADDR))
			dest_addr.sin_addr.s_addr = ((unsigned long**)hst->h_addr_list)[0][0];
		else
		{
			printf("Unknown host : %d \n", WSAGetLastError());
			closesocket(my_sock);
			WSACleanup();
			return -1;
		}
	int commandC = 0;
	int old_sec = 0;
	//fgets(&buff[0], sizeof(buff) - 1, stdin);


	hSerial = ::CreateFile(sPortName, GENERIC_READ | GENERIC_WRITE, 0, 0, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);
	if (hSerial == INVALID_HANDLE_VALUE)
	{
		if (GetLastError() == ERROR_FILE_NOT_FOUND)
		{
			cout << "serial port does not exist.\n";
		}
		cout << "some other error occurred.\n";
	}
	DCB dcbSerialParams = { 0 };
	dcbSerialParams.DCBlength = sizeof(dcbSerialParams);
	if (!GetCommState(hSerial, &dcbSerialParams))
	{
		cout << "getting state error\n";
	}
	dcbSerialParams.BaudRate = CBR_9600;
	dcbSerialParams.ByteSize = 8;
	dcbSerialParams.StopBits = ONESTOPBIT;
	dcbSerialParams.Parity = NOPARITY;
	if (!SetCommState(hSerial, &dcbSerialParams))
	{
		cout << "error setting serial port state\n";
	}

	boolean clockMode = false;
	DWORD iSize;
	char sReceivedChar;
	while (1)
	{

		Sleep(2000);
		ReadFile(hSerial, &sReceivedChar, 1, &iSize, 0);
		string s1 = "";
		// если что-то принято, выводим
		if (iSize > 0) {

			while (sReceivedChar != '\n') {
				s1 += sReceivedChar;
				ReadFile(hSerial, &sReceivedChar, 1, &iSize, 0);
			}
			cout << "C=>S: " << &s1[0] << endl;

			sendto(my_sock, &s1[0], strlen(&s1[0]), 0, (sockaddr*)&dest_addr, sizeof(dest_addr));
			sockaddr_in server_addr;
			int server_addr_size = sizeof(server_addr);

		}


	}


	closesocket(my_sock);
	WSACleanup();
	return 0;
}
