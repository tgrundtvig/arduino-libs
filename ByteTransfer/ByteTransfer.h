/*
  ByteTransfer.h - TODO: Document...
*/
#ifndef ByteTransfer_h
#define ByteTransfer_h

#include "Arduino.h"

class ByteTransfer
{
	public:
		ByteTransfer(int inPin, int dataPin, int outPin, void (*OnDataIn) (uint8_t data), bool (*OnDataOut) (uint8_t &data));
		void Initialize();
		void Update();
	private:
		void Notify();
		bool Notified();
		void HandleState_1();
		void HandleState_2();
		void HandleState_3();
		void HandleState_4();
		void HandleState_5();
		void (*_OnDataIn) (uint8_t data);
		bool (*_OnDataOut) (uint8_t &data);
		int _inPin;
		int _dataPin;
		int _outPin;
		bool _bIn;
		bool _bOut;
		bool _bDataOut;
		uint8_t _data;
		uint8_t _mask;
		uint8_t _state;
};

#endif