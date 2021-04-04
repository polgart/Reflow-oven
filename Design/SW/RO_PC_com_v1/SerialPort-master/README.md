# SerialPort

## C++ Library for Serial Communication with following criteria

*  Plain C++ Compatible
*  Windows Supported
*  Include and Run Type Library
*  supports softwareserial
*  supports serial event
*  fixed minor bugs

## Documentation
[Here is a blog post about the library](https://medium.com/@ManashMandal/serial-communication-with-an-COM_Port-using-c-on-windows-d08710186498#.f94efw74b)

### Initialization
```cpp
#include "SerialPort.hpp"
#include <iostream>

const char *portName = "\\\\.\\COM20";

//Declare a global object
SerialPort *COM_Port;

int main(void)
{
  COM_Port = new SerialPort(portName);
  std::cout << "Is connected: " << COM_Port->isConnected() << std::endl;
}
```

### Sending Data
```cpp
#define DATA_LENGTH 255

#include "SerialPort.hpp"
#include <iostream>

const char *portName = "\\\\.\\COM20";

//Declare a global object
SerialPort *COM_Port;

//Here '\n' is a delimiter 
const char *sendString = "Hello World\n"; 

int main(void)
{
  COM_Port = new SerialPort(portName);
  if (COM_Port->isConnected()){
    bool hasWritten = COM_Port->writeSerialPort(sendString, DATA_LENGTH);
    if (hasWritten) std::cout << "Data Written Successfully" << std::endl;
    else std::cerr << "Data was not written" << std::endl;
  }
}
```

### Receiving Data
```cpp
#define DATA_LENGTH 255

#include "SerialPort.hpp"
#include <iostream>

const char *portName = "\\\\.\\COM20";

//Declare a global object
SerialPort *COM_Port;

char receivedString[DATA_LENGTH];

int main(void)
{
  COM_Port = new SerialPort(portName);
  if (COM_Port->isConnected()){
    int hasRead = COM_Port->readSerialPort(receivedString, DATA_LENGTH);
    if (hasRead) std::cout <<  receivedString << "\n";
    else std::cerr << "Error occured reading data" << "\n";
  }
}
```
