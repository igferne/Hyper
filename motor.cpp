/**************************************************************************************************************
*ORGANISMO: Universidad de La Rioja
*
*AUTOR: Ignacio Fern�ndez Est�banez
*
*RELEASE: 3 de octubre de 2013
*
*DESCRIPCION: archivo de c�digo de la clase motor que permite el control del mirror
**************************************************************************************************************/

#include "StdAfx.h"
#include "motor.h"

//FUNCI�N CONSTRUCTORA
motor::motor(void)
{
	//Conexi�n al puerto serie
	int error = this->connectionHandler();
	//Reset al motor
	if (error == 0)
	{
		error = this->resetHandler();
	}
	//Configuraci�n de la velocidad
	if (error == 0)
	{
		this->setVelocityHandler();
	}

	//DEBUG//
	this->_error = error;
}


//********************************************************
//FUNCIONES INICIALIZACI�N
//********************************************************
/*
DESCRIPCI�N: funci�n que realiza la conexi�n al puerto COM
ENTRADAS:
SALIDAS: el error cometido
�MBITO: public
*/
int motor::connectionHandler()
{
	//Variables auxiliares
	int error = 0;

	//Configuramos la conexi�n serie requerida
	this->_serialPort = gcnew SerialPort("COM1",9600,Parity::None,8,StopBits::One);

	//Apertura del puerto serie
	try
	{	
		this->_serialPort->Open();
	}
	catch(...)
	{
		MessageBox::Show("El puerto est� siendo utilizado por otro dispositivo");
		error = 100;
	}

	return error;
}

/*
DESCRIPCI�N: funci�n de reset del motor
ENTRADAS:
SALIDAS: el error cometido
�MBITO: private
*/
int motor::resetHandler()
{
	//Variables auxiliares
	int error = 0;

	//Enviamos un comando de reset al motor
	this->_serialPort->Write("\x03");
	Sleep(500);

	//Leemos la respuesta del motor
	this->_receivedData  = this->_serialPort->ReadExisting();
	int cnt = 0;
	while((this->_receivedData == "") && (cnt<10))
	{
		Sleep(500);
		this->_receivedData  = this->_serialPort->ReadExisting();
		cnt++;
	}
	if(!this->_receivedData->Contains(">"))
	{
		error = 101;
	}

	return error;
}

/*
DESCRIPCION: funci�n de configuraci�n de la velocidad
ENTRADAS: 
SALIDAS: 
�MBITO: private
*/
void motor::setVelocityHandler()
{
	//Configuraci�n de la velocidad inicial
	this->_serialPort->Write("VI=100\r");
	//Configuraci�n de la velociad final
	this->_serialPort->Write("VM=55888\r");

	//Esperamos la respuesta al movimiento
	String^ _received  = this->_serialPort->ReadExisting();
	int cnt = 0;
	while((_received == "") && (cnt<10))
	{
		Sleep(500);
		_received  = this->_serialPort->ReadExisting();
		cnt++;
	}
}

/*
DESCRIPCI�N: esta funcion elimina los errores que se hayan
podido producir durante la ejecuci�n de una instrucci�n
ENTRADAS: 
SALIDAS: devuelve 0 si el error es eliminado
�MBITO: private
*/
int motor::cleanErrorHandler()
{
	//Variables auxiliares
	int error = 101;
	int cnt_1 = 0;

	//Bucle de eliminaci�n del error
	while ((error != 0) && (cnt_1 <10))
	{
		//Lectura del flag de error
		this->_serialPort->Write("PR ER\r");
	
		//Esperamos la respuesta del motor
		this->_receivedData  = this->_serialPort->ReadExisting();
		int cnt = 0;
		while((this->_receivedData == "") && (cnt<10))
		{
			Sleep(500);
			this->_receivedData  = this->_serialPort->ReadExisting();
			cnt++;
		}
		//Verificamos si la respuesta es correcta
		if(this->_receivedData->Contains(">"))
		{
			error = 102;
		}
		cnt_1++;
	}
	return error;
}


//********************************************************
//FUNCIONES DE MOVIMIENTO
//********************************************************
/*
DESCRIPCI�N: funci�n que realiza un movimiento a una posici�n
absoluta
ENTRADAS: _position: valor de la posici�n a la que se mover� 
SALIDAS: el error cometido
�MBITO:private
*/
int motor::absolutMotionHandler(int _position)
{
	//Variables auxiliares
	int error = 0;
	String^ _motion;

	//Enviamos el movimiento deseado
	_motion = String::Concat("MA ",Convert::ToString(_position),"\r");
	this->_serialPort->Write(_motion);

	//Esperamos la respuesta al movimiento
	this->_receivedData  = this->_serialPort->ReadExisting();
	int cnt = 0;
	while((this->_receivedData == "") && (cnt<10))
	{
		Sleep(500);
		this->_receivedData  = this->_serialPort->ReadExisting();
		cnt++;
	}
	//Verificamos si la respuesta es correcta
	if(!this->_receivedData->Contains(">"))
	{
		//Limpiamos el error en caso de existencia
		if (this->_receivedData->Contains("?"))
		{
			error = this->cleanErrorHandler();
		}
		else
		{
			error = 101;
		}
	}

	//DEBUG//
	this->_command = _motion;
	this->_error = error;
	return error;
}

/*
DESCRIPCI�N: funci�n que realiza un movimiento a una posici�n
relativa
ENTRADAS: _position: valor del incremento de posici�n 
SALIDAS: el error cometido
�MBITO: private
*/
int motor::relativeMotionHandler(int _position)
{
	//Variables auxiliares
	int error = 0;
	String^ _motion;

	//Enviamos el movimiento deseado
	_motion = String::Concat("MR ",Convert::ToString(_position),"\r");
	this->_serialPort->Write(_motion);

	//Esperamos la respuesta al movimiento
	this->_receivedData  = this->_serialPort->ReadExisting();
	int cnt = 0;
	while((this->_receivedData == "") && (cnt<10))
	{
		Sleep(500);
		this->_receivedData  = this->_serialPort->ReadExisting();
		cnt++;
	}
	//Verificamos si la respuesta es correcta
	if(!this->_receivedData->Contains(">"))
	{
		//Limpiamos el error en caso de existencia
		if (this->_receivedData->Contains("?"))
		{
			error = this->cleanErrorHandler();
		}
		else
		{
			error = 101;
		}
	}

	//DEBUG//
	this->_command = _motion;
	this->_error = error;
	return error;
}

/*
DESCRIPCI�N: funci�n que devuelve el motor a la posici�n 
incial
ENTRADAS: 
SALIDAS: el error cometido
�MBITO: public
*/
int motor::goHomeHandler()
{
	//Variables auxiliares
	int error = 0;
	String^ _motion = "MA 0\r";

	//Enviamos el movimiento deseado
	this->_serialPort->Write(_motion);

	//Esperamos la respuesta al movimiento
	this->_receivedData  = this->_serialPort->ReadExisting();
	int cnt = 0;
	while((this->_receivedData == "") && (cnt<10))
	{
		Sleep(500);
		this->_receivedData  = this->_serialPort->ReadExisting();
		cnt++;
	}
	//Verificamos si la respuesta es correcta
	if(!this->_receivedData->Contains(">"))
	{
		//Limpiamos el error en caso de existencia
		if (this->_receivedData->Contains("?"))
		{
			error = this->cleanErrorHandler();
		}
		else
		{
			error = 101;
		}
	}

	//DEBUG//
	this->_command = _motion;
	this->_error = error;
	return error;
}

/*
DESCRIPCI�N: esta funci�n verifica si el motor se encuentra
realizando un movimiento
ENTRADAS:
SALIDAS: devuelve 'true' cuando el motor est� en movimiento
�MBITO: private
*/
bool motor::movingFlagHandler()
{
	//Variables auxiliares
	bool _motionFlag;
	int error = 0;

	//Env�o comando de lectura del flag
	this->_serialPort->Write("PR MV\r");

	//Esperamos la respuesta al movimiento
	this->_receivedData  = this->_serialPort->ReadExisting();
	int cnt = 0;
	while((this->_receivedData == "") && (cnt<10))
	{
		Sleep(500);
		this->_receivedData  = this->_serialPort->ReadExisting();
		cnt++;
	}
	//Verificamos si la respuesta es correcta
	if(!this->_receivedData->Contains(">"))
	{
		//Limpiamos el error en caso de existencia
		if (this->_receivedData->Contains("?"))
		{
			error = this->cleanErrorHandler();
		}
		else
		{
			error = 101;
		}
	}
	//Lectura del flag
	if (error == 0)
	{
		if(this->_receivedData->Contains("1"))
		{
			_motionFlag = true;
		}
		else if(this->_receivedData->Contains("0"))
		{
			_motionFlag = false;
		}
	}
	//DEBUG//
	MessageBox::Show(Convert::ToString(_motionFlag));
	return _motionFlag;
}

/*
DESCRIPCI�N: esta funci�n lee la posici�n en la que se encuentra
el motor y actualiza la propiedad '_position'
ENTRADAS: 
SALIDAS: actualiza la posici�n del motor
�MBITO: public
*/
void motor::readPositionHandler()
{
	//Variables auxiliares 
	int error = 0;

	//Env�o comando de lectura del flag
	this->_serialPort->Write("PR P\r");

	//Esperamos la respuesta al movimiento
	this->_receivedData  = this->_serialPort->ReadExisting();
	int cnt = 0;
	while((this->_receivedData == "") && (cnt<10))
	{
		Sleep(500);
		this->_receivedData  = this->_serialPort->ReadExisting();
		cnt++;
	}
	//Verificamos si la respuesta es correcta
	if(!this->_receivedData->Contains(">"))
	{
		//Limpiamos el error en caso de existencia
		if (this->_receivedData->Contains("?"))
		{
			error = this->cleanErrorHandler();
		}
		else
		{
			error = 101;
		}
	}
	//Lectura del flag
	if (error == 0)
	{
		this->_receivedData = this->_receivedData->Remove(0,4);
		this->_receivedData = this->_receivedData->Replace(">","");
		this->_receivedData = this->_receivedData->Replace("\r","");
		this->_receivedData = this->_receivedData->Replace("\n","");
		this->_position = this->_receivedData;
	}
}