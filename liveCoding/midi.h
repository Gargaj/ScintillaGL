/*******************************************************************************
*   2005-2014, plastic demoscene group
*	authors: misz, bonzaj
*******************************************************************************/

#pragma once
#include "stdafx.h"
#include <mmsystem.h>

struct midiEvent
{
	u8 type;
	u8 channel;
	u8 value;
	u8 padd0__[1];
};

class midiDevice
{
public:
	midiDevice()
		: deviceId_( ~0U )
		, device_( NULL )
		, csValid_( false )
	{
		memset( &cs_, 0, sizeof(cs_) );
	}
	~midiDevice() {}

	int startUp( const char* deviceName );
	void shutDown();

	int poll( midiEvent* events, int capacity );
	//bool opened() const;
	//bool connected() const;

	void addData( u32 data );

protected:
	unsigned _GetDeviceCount();
	void _GetDeviceName( char* dstName, int dstNameCapacity, unsigned deviceId );

	//int _OpenImpl( unsigned deviceId );
	//void _CloseImpl();
	//int _PollImpl( midiEvent* events, int capacity );

	u32 deviceId_;
	HMIDIIN device_;

	std::list<u32> midiData_;
	CRITICAL_SECTION cs_;
	bool csValid_;
};

class midiController
{
public:
	enum 
	{
		ePad0,
		ePad1,
		ePad2,
		ePad3,
		ePad4,
		ePad5,
		ePad6,
		ePad7,
		ePad8,
		ePad9,
		ePad10,
		ePad11,
		ePad12,
		ePad13,
		ePad14,
		ePad15,
		ePadCount,
	};

	midiController( const char* name );
	virtual ~midiController() {}

	int startUp();
	void shutDown();

	void update();

	void getData( float dst[ePadCount] ) const
	{
		memcpy( dst, padData_, sizeof(padData_) );
	}

protected:
	virtual void _Map( const midiEvent& e ) = 0;

protected:
	std::string deviceName_;
	midiDevice device_;
	float padData_[ePadCount];
};

class midiController_CME_U2MIDI : public midiController
{
public:
	midiController_CME_U2MIDI();

protected:
	void _Map( const midiEvent& e );
	void _SetValue( int channel, float value );

	u8 mapping_[128];
};

//class midiController_Akai_MPD26 : public midiController
//{
//public:
//	midiController_Akai_MPD26();
//
//protected:
//	void _Map( const midiEvent& e );
//	void _SetValue( int channel, float value );
//
//	u8 mapping_[128];
//};
