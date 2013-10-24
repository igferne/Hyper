/**************************************************************************************************************
*ORGANISMO: Universidad de La Rioja
*
*AUTOR: Ignacio Fernández Estébanez
*
*RELEASE: 3 de octubre de 2013
*
*DESCRIPCION: archivo de encabezado de la clase motor que permite el control del mirror
**************************************************************************************************************/

#pragma once
#include <windows.h>

using namespace System::IO::Ports;
using namespace System::Threading;
using namespace System::Windows::Forms;
using namespace System::Text;
using namespace System;

ref class motor
{
public:
	//Public methods
	motor(void);
	
	int resetHandler();
	int absolutMotionHandler(int);
	int relativeMotionHandler(int);
	int goHomeHandler();
	bool movingFlagHandler();
	void readPositionHandler();

	//Public properties
	static int _error;
	static String^ _command;
	static String^ _receivedData;
	static String^ _position;

private:
	//Private properies
	static SerialPort^ _serialPort;

	//Private methods
	int connectionHandler();
	void setVelocityHandler();
	int cleanErrorHandler();
};


