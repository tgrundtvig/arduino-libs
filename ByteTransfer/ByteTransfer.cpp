#include "Arduino.h"
#include "ByteTransfer.h"

ByteTransfer::ByteTransfer(int inPin, int dataPin, int outPin, void (*OnDataIn) (uint8_t data), bool (*OnDataOut) (uint8_t &data))
{
	_inPin = inPin;
	_dataPin = dataPin;
	_outPin = outPin;
	_OnDataIn = OnDataIn;
	_OnDataOut = OnDataOut;
	_bIn = false;
	_bOut = false;
	_bDataOut = false;
	_data = 0;
	_mask = 0;
	_state = 0;
};

void ByteTransfer::Initialize()
{
	pinMode(_inPin, INPUT);
	pinMode(_outPin, OUTPUT);
	if(digitalRead(_inPin))
	{
		//We are second
		pinMode(_dataPin, INPUT);
		_bDataOut = false;
		_state = 2;
	}
	else
	{
		//We are first  
		pinMode(_dataPin, OUTPUT);
		digitalWrite(_dataPin, LOW);
		digitalWrite(_outPin, HIGH);
		_bDataOut = true;
		_bOut = true;
		_state = 1;
  }
};

void ByteTransfer::Update()
{
	if(!Notified()) return;
	switch(_state)
	{
		case 0:
			Initialize();
			break;
		case 1: //Finished writing
			HandleState_1();
			break;
		case 2: //Ready to read
			HandleState_2();
			break;
		case 3: //Ready to write
			HandleState_3();
			break;
		case 4: //Reading data
			HandleState_4();
			break;
		case 5: //Writing data
			HandleState_5();
			break;
	}
	Notify();
};

void ByteTransfer::Notify()
{
	_bOut = !_bOut;
	digitalWrite(_outPin, _bOut);
};

bool ByteTransfer::Notified()
{
	if(digitalRead(_inPin) != _bIn)
	{
		_bIn = !_bIn;
		return true;
	}
	return false;
};

void ByteTransfer::HandleState_1() //Finished writing
{
	pinMode(_dataPin, INPUT);
	_bDataOut = false;
	_state = 2;
};

void ByteTransfer::HandleState_2() //Ready to read
{
  bool dataWaiting = digitalRead(_dataPin);
  if(dataWaiting)
  {
	_data = 0;
	_mask = 128;
	_state = 4;
  }
  else
  {
    _state = 3;
  }
};

void ByteTransfer::HandleState_3() //Ready to write
{
	if(!_bDataOut)
	{
		pinMode(_dataPin, OUTPUT);
		_bDataOut = true;
	}
	if(!_OnDataOut(_data))
	{
		digitalWrite(_dataPin, LOW);
		_state = 1;
	}
	else
	{
		digitalWrite(_dataPin, HIGH);
		_mask = 128;
		_state = 5;
	}
};

void ByteTransfer::HandleState_4() //Reading data
{
	if(digitalRead(_dataPin))
	{
		_data += _mask;
	}
	_mask >>= 1;
	if(_mask == 0)
	{
		_state = 2;
		_OnDataIn(_data);
	}
};

void ByteTransfer::HandleState_5() //Writing data
{
	digitalWrite(_dataPin, _mask & _data);
	_mask >>= 1;
	if(_mask == 0)
	{
		_state = 3;
	}
};