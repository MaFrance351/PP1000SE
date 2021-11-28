#include <windows.h>
#include <iostream>
#include <string>
using namespace std;
HANDLE hSerial;
#define STX 0x02
#define ETX 0x03
#define ACK 0x06
#define EOT 0x04
#define NAK 0x15
string input;
string text = "";
uint8_t ReadCOM() {
    DWORD iSize;
    uint8_t sReceivedByte = 0;
        ReadFile(hSerial, &sReceivedByte, 1, &iSize, 0);
        if(sReceivedByte == 0x06) cout << "ACK received";
        else if(sReceivedByte == 0x15) cout << "NAK received";
        if(iSize > 0) return sReceivedByte;
        else return 0;
}
uint8_t BCC(string data) {
char temp = data[1];
for(int i = 2; i < data.length(); i++) temp ^= data[i];
return temp;
}
int main() {
    cout << "Vvedite vash text: ";
    cin >> text;
    LPCTSTR sPortName = "COM3";
    hSerial = ::CreateFile(sPortName, GENERIC_READ | GENERIC_WRITE, 0, 0, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);
    DCB dcbSerialParams = { 0 };
    dcbSerialParams.DCBlength = sizeof(dcbSerialParams);
    if(!GetCommState(hSerial, &dcbSerialParams)){
        cout << "getting state error\n";
        exit(0);
    }
    dcbSerialParams.BaudRate = CBR_9600;
    dcbSerialParams.ByteSize = 7;
    dcbSerialParams.StopBits = ONESTOPBIT;
    dcbSerialParams.Parity = EVENPARITY;
    if(!SetCommState(hSerial, &dcbSerialParams)){
        cout << "error setting serial port state\n";
        exit(0);
    }
    input += STX;
    input += 0x5A;
    input += 0x32;
    input += 0x1A;
    input += text;
    input += ETX;
    input += BCC(input);
    int len = input.length();
    DWORD dwBytesWritten;
const char *toSerial = input.c_str();
    DWORD dwSize = len;
   cout << "Send data: " << toSerial << endl;
    WriteFile(hSerial, toSerial, dwSize, &dwBytesWritten, NULL);
    cout << "Send bytes: " << dwBytesWritten << endl;
    if(ReadCOM() != 0x06) cout << "error" << endl;
    else cout << "success";
}
