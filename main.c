// Terminal.cpp : Tento soubor obsahuje funkci main. Provádění programu se tam zahajuje a ukončuje.
//

#include<windows.h>
#include <stdio.h>
#include <conio.h>

int state = 0;
int LED1, LED2;
int TL;

boolean myputchar(HANDLE hComm, char c);

int mygetchar(HANDLE hComm);

void setState(int newState) {
    printf("new state: %d\n", newState);
    state = newState;
}

void setLED1(int newState, HANDLE handle) {
    printf("LED1: %d\n", newState);
    LED1 = newState;
    myputchar(handle, 's');
    myputchar(handle, 'e');
    myputchar(handle, 't');
    myputchar(handle, 'T');
    myputchar(handle, 'o');
    myputchar(handle, ' ');
    myputchar(handle, LED1 + '0');
    myputchar(handle, '\n');
}

void setLED2(int newState, HANDLE handle) {
    printf("LED2: %d\n", newState);
    LED2 = newState;
    myputchar(handle, 's');
    myputchar(handle, 'e');
    myputchar(handle, 't');
    myputchar(handle, 'T');
    myputchar(handle, 'o');
    myputchar(handle, ' ');
    myputchar(handle, LED2 + '0');
    myputchar(handle, '\n');
}

void processInput(char input, HANDLE handle) {
    // CR LF = 13 10 workaround
    if (input == 13) return;
    switch (state) {
        case 0:
            LED1 = LED2 = 0;
            TL = 0;
            setState(1);
            break;
        case 1:
            if (input == -1) return;
            if (input == 'L') setState(2);
            else if (input == 'T') setState(14);
            else setState(13);
            break;
        case 2:
            if (input == -1) return;
            if (input == 'E') setState(3);
            else setState(13);
            break;
        case 3:
            if (input == -1) return;
            if (input == 'D') setState(4);
            else setState(13);
            break;
        case 4:
            if (input == -1) return;
            if (input == '1') setState(6);
            else if (input == '2') setState(5);
            else setState(13);
            break;
        case 5:
            if (input == -1) return;
            if (input == '\n') {
                myputchar(handle, 'L');
                myputchar(handle, 'E');
                myputchar(handle, 'D');
                myputchar(handle, '2');
                myputchar(handle, '=');
                myputchar(handle, LED2 + '0');
                myputchar(handle, '\n');
                setState(1);
            } else if (input == '=') setState(10);
            else setState(13);
            break;
        case 6:
            if (input == -1) return;
            if (input == '\n') {
                myputchar(handle, 'L');
                myputchar(handle, 'E');
                myputchar(handle, 'D');
                myputchar(handle, '1');
                myputchar(handle, '=');
                myputchar(handle, LED1 + '0');
                myputchar(handle, '\n');
                setState(1);
            } else if (input == '=') setState(7);
            else setState(13);
            break;
        case 7:
            if (input == -1) return;
            if (input == '0') setState(8);
            else if (input == '1')setState(9);
            else setState(13);
            break;
        case 8:
            if (input == -1) return;
            if (input == '\n') {
                setLED1(0, handle);
                setState(1);
            } else setState(13);
            break;
        case 9:
            if (input == -1) return;
            if (input == '\n') {
                setLED1(1, handle);
                setState(1);
            } else setState(13);
            break;
        case 10:
            if (input == -1) return;
            if (input == '0') setState(12);
            else if (input == '1')setState(11);
            else setState(13);
            break;
        case 11:
            if (input == -1) return;
            if (input == '\n') {
                setLED2(1, handle);
                setState(1);
            } else setState(13);
            break;
        case 12:
            if (input == -1) return;
            if (input == '\n') {
                setLED2(0, handle);
                setState(1);
            } else setState(13);
            break;
        case 13:
            // ERROR CASE
            myputchar(handle, 'E');
            myputchar(handle, 'R');
            myputchar(handle, 'R');
            myputchar(handle, 'O');
            myputchar(handle, 'R');
            myputchar(handle, '\n');
            setState(1);
            break;
        case 14:
            if (input == -1) return;
            if (input == 'L') setState(15);
            else setState(13);
            break;
        case 15:
            if (input == -1) return;
            if (input == '\n') {
                myputchar(handle, 'T');
                myputchar(handle, 'L');
                myputchar(handle, '=');
                myputchar(handle, TL + '0');
                myputchar(handle, '\n');
                setState(1);
            } else setState(13);
            break;
    }
}

boolean myputchar(HANDLE hComm, char c) {
    char lp[] = {c};        // lpBuffer should be  char or byte array, otherwise write wil fail	DWORD  NumWritten;
    DWORD dNoOFBytestoWrite;              // No of bytes to write into the port
    DWORD dNoOfBytesWritten = 0;          // No of bytes written to the port

    dNoOFBytestoWrite = sizeof(lp); // Calculating the no of bytes to write into the port

    if (!WriteFile(hComm, lp, dNoOFBytestoWrite, &dNoOfBytesWritten, NULL)) {
        printf("Error writing text to COM\n");
        printf("\nError %d in Writing to Serial Port\n", GetLastError());
    } else {
        // printf("%d bytes written to COM\n", dNoOfBytesWritten);
    }

    return 0;
}

int mygetchar(HANDLE hComm) {
    char SerialBuffer[2];
    DWORD dNoBytesRead;

    if (ReadFile(hComm, SerialBuffer, 1, &dNoBytesRead, NULL)) {
        //printf("%d bytes read from COM\n", dNoBytesRead);
    }
    if (dNoBytesRead) return SerialBuffer[0]; else return -1;
}

int main() {
    HANDLE hComm;
    DCB dcbSerialParams;                    // Initializing DCB structure
    BOOL Write_Status;
    int input;
    char buf[1];
    COMMTIMEOUTS timeouts;

    hComm = CreateFileA("\\\\.\\COM4",                //port name
                        GENERIC_READ | GENERIC_WRITE, //Read/Write
                        0,                            // No Sharing
                        NULL,                         // No Security
                        OPEN_EXISTING,// Open existing port only
                        0,            // Non Overlapped I/O
                        NULL);        // Null for Comm Devices

    if (hComm == INVALID_HANDLE_VALUE) {

        if (GetLastError() == ERROR_FILE_NOT_FOUND) {
            printf("cannot open port!\n");
            return EXIT_FAILURE;
        }

        printf("invalid handle value!\n");
        return EXIT_FAILURE;
    } else
        printf("opening serial port successful\n");

    dcbSerialParams.DCBlength = sizeof(dcbSerialParams);

    Write_Status = GetCommState(hComm, &dcbSerialParams);     //retreives  the current settings

    if (Write_Status == FALSE) {
        printf("Error! in GetCommState()\n");
        CloseHandle(hComm);
        return 1;
    }


    dcbSerialParams.BaudRate = CBR_57600;      // Setting BaudRate = 9600
    dcbSerialParams.ByteSize = 8;             // Setting ByteSize = 8
    //dcbSerialParams.
    dcbSerialParams.StopBits = ONESTOPBIT;    // Setting StopBits = 1
    dcbSerialParams.Parity = ODDPARITY;      // Setting Parity = None

    Write_Status = SetCommState(hComm, &dcbSerialParams);  //Configuring the port according to settings in DCB

    if (Write_Status == FALSE) {
        printf("Error! in Setting DCB Structure\n");
        CloseHandle(hComm);
        return 1;
    } else {
        printf("\n   Setting DCB Structure Successful\n");
        printf("\n       Baudrate = %d", dcbSerialParams.BaudRate);
        printf("\n       ByteSize = %d", dcbSerialParams.ByteSize);
        printf("\n       StopBits = %d", dcbSerialParams.StopBits);
        printf("\n       Parity   = %d\n\n", dcbSerialParams.Parity);
    }

    // set short timeouts on the comm port.
    timeouts.ReadIntervalTimeout = 1;
    timeouts.ReadTotalTimeoutMultiplier = 100;
    timeouts.ReadTotalTimeoutConstant = 1;
    timeouts.WriteTotalTimeoutMultiplier = 1;
    timeouts.WriteTotalTimeoutConstant = 1;
    Write_Status = SetCommTimeouts(hComm, &timeouts);

    if (Write_Status == FALSE) {
        printf("Error! Timeout Settings Failed\n");
        CloseHandle(hComm);
        return 1;
    }

    while (1) {
        input = mygetchar(hComm);
        if (input > 0) {
            printf("Serial input char = %c\n", input);
            myputchar(hComm, input);
        }
        processInput(input, hComm);
        if (input != -1)
            if (kbhit()) {
                input = getch();
                if (input > 0) {
                    printf("Keyboard input char = %c\n", input);
                    myputchar(hComm, input);
                }
            }
    }

    CloseHandle(hComm);//Closing the Serial Port

    return EXIT_SUCCESS;
}