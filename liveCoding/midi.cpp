/*******************************************************************************
*   2005-2014, plastic demoscene group
*	authors: misz, bonzaj
*******************************************************************************/

#include "stdafx.h"
#include "midi.h"

#ifdef _DEBUG
#define new _DEBUG_NEW
#endif

void CALLBACK MidiInProcWin( HMIDIIN hMidiIn, UINT wMsg, DWORD_PTR dwInstance, DWORD_PTR dwParam1, DWORD_PTR dwParam2 )
{
	(void)dwParam2;
	(void)hMidiIn;
	if( wMsg == MIM_DATA )
	{
		midiDevice* dev = (midiDevice*)dwInstance;
		dev->addData( (u32)dwParam1 );
	}
}


int midiDevice::startUp( const char* deviceName )
{
	unsigned deviceId = ~0u;

	const unsigned numDevices = midiInGetNumDevs();
	for( unsigned i = 0; i < numDevices; ++i )
	{
		MIDIINCAPS capabilities;
		MMRESULT res = midiInGetDevCaps(i, &capabilities, sizeof(MIDIINCAPS));
		if ( res != MMSYSERR_NOERROR ) 
		{
			fprintf( stderr, "midiInGetDevCaps failed! Err=%d", res );
			return -1;
		}

		if( !strcmp( capabilities.szPname, deviceName ) )
		{
			deviceId = i;
			break;
		}
	}

	if( deviceId == ~0u )
	{
		fprintf( stderr, "Can not open MIDI device '%s': device not found\n", deviceName );
		printf( "Available '%d' devices:n", numDevices );
		for( unsigned i = 0; i < numDevices; ++i )
		{
			MIDIINCAPS capabilities;
			MMRESULT res = midiInGetDevCaps(deviceId, &capabilities, sizeof(MIDIINCAPS));
			if ( res != MMSYSERR_NOERROR ) 
			{
				printf( "%s\n", capabilities.szPname );
			}
		}

		return -1;
	}

	csValid_ = true;
	InitializeCriticalSection( &cs_ );

	MMRESULT result = midiInOpen( &device_, deviceId, (DWORD_PTR)(MidiInProcWin), (DWORD)(this), CALLBACK_FUNCTION );
	if (result != MMSYSERR_NOERROR) 
	{
		fprintf( stderr, "midiInOpen failed! Err=%d\n", result );
		return -1;
	}

	result = midiInStart(device_);
	if (result != MMSYSERR_NOERROR) 
	{
		fprintf( stderr, "midiInOpen failed! Err=%d\n", result );
		return -1;
	}

	deviceId_ = deviceId;

	return 0;
}

void midiDevice::shutDown()
{
	if ( device_ )
	{
		midiInStop( device_ );
		midiInClose( device_ );
		device_ = NULL;
	}

	midiData_.clear();

	if ( csValid_ )
	{
		csValid_ = false;
		DeleteCriticalSection( &cs_ );
	}
}

void midiDevice::addData( u32 data )
{
	EnterCriticalSection( &cs_ );
	midiData_.push_back( data );
	LeaveCriticalSection( &cs_ );
}

int midiDevice::poll( midiEvent* events, int capacity )
{
	int size = 0;

	union Helper
	{
		u32 dword;
		struct Bytes
		{
			u8 _0;
			u8 _1;
			u8 _2;
			u8 _3;
		} bytes;
	} helper;

	EnterCriticalSection( &cs_ );

	const size_t nData = midiData_.size();
	for ( size_t i = 0; i < nData; ++i )
	{
		if( size >= capacity )
			break;

		u32 data = midiData_.front();
		midiData_.pop_front();

		helper.dword = data;
		midiEvent& e = events[size];
		e.type = helper.bytes._0;
		e.channel = helper.bytes._1;
		e.value = helper.bytes._2;
		++ size;
	}

	LeaveCriticalSection( &cs_ );

	return size;

}




midiController::midiController( const char* name )
	: deviceName_( name )
{
	memset( padData_, 0, sizeof(padData_) );
}

void midiController::update()
{
	const float normalizationFactor127 = 1.f / 127;
	const int MAX_EVENTS = 128;
	midiEvent events[MAX_EVENTS];
	const int numEvents = device_.poll( events, MAX_EVENTS );

	for( int i = 0; i < numEvents; ++i )
	{
		const midiEvent& e = events[i];
		_Map( e );
	}
}

int midiController::startUp()
{
	int ires = device_.startUp( deviceName_.c_str() );
	if ( ires )
	{
		device_.shutDown();
		return ires;
	}

	return 0;
}

void midiController::shutDown()
{
	device_.shutDown();
}

midiController_CME_U2MIDI::midiController_CME_U2MIDI()
	: midiController( "CME U2MIDI" )
{
	memset( mapping_, ePadCount, sizeof(mapping_) );

	mapping_[36] = ePad0;
	mapping_[37] = ePad1;
	mapping_[38] = ePad2;
	mapping_[39] = ePad3;
	mapping_[40] = ePad4;
	mapping_[41] = ePad5;
	mapping_[42] = ePad6;
	mapping_[43] = ePad7;
	mapping_[44] = ePad8;
	mapping_[45] = ePad9;

	mapping_[46] = ePad10;
	mapping_[47] = ePad11;
	mapping_[48] = ePad12;
	mapping_[49] = ePad13;
	mapping_[50] = ePad14;
	mapping_[51] = ePad15;
}

void midiController_CME_U2MIDI::_Map( const midiEvent& e )
{
	const float normalizationFactor127 = 1.f / 127;

	u8 channelModified = 0xFF;
	if( (e.type & 0xF0) == 0x90 ) // note on
	{
		_SetValue( e.channel, e.value * normalizationFactor127 );
		channelModified = e.channel;
	}
	else if( (e.type & 0xF0) == 0x80 )
	{
		_SetValue( e.channel, 0.f );
		channelModified = e.channel;
	}
}

void midiController_CME_U2MIDI::_SetValue( int channel, float value )
{
	//printf( "channel: %d\n", channel );

	if ( channel >= 0 && channel < 128 )
	{
		int index = mapping_[channel];
		if( index >= 0  && index < ePadCount )
		{
			if ( value > 0 )
				padData_[index] = 1;
			else
				padData_[index] = 0;
		}
	}
}

//midiController_Akai_MPD26::midiController_Akai_MPD26()
//	: midiController( "Akai MPD26" )
//{
//	memset( mapping_, ePadCount, sizeof(mapping_) );
//
//	mapping_[36] = ePad0;
//	mapping_[37] = ePad1;
//	mapping_[38] = ePad2;
//	mapping_[39] = ePad3;
//	mapping_[40] = ePad4;
//	mapping_[41] = ePad5;
//	mapping_[42] = ePad6;
//	mapping_[43] = ePad7;
//	mapping_[44] = ePad8;
//	mapping_[45] = ePad9;
//
//	mapping_[46] = ePad10;
//	mapping_[47] = ePad11;
//	mapping_[48] = ePad12;
//	mapping_[49] = ePad13;
//	mapping_[50] = ePad14;
//	mapping_[51] = ePad15;
//}
//
//void midiController_Akai_MPD26::_Map( const midiEvent& e )
//{
//	const float normalizationFactor127 = 1.f / 127;
//
//	u8 channelModified = 0xFF;
//	if( (e.type & 0xF0) == 0x90 ) // note on
//	{
//		_SetValue( e.channel, e.value * normalizationFactor127 );
//		channelModified = e.channel;
//	}
//	else if( (e.type & 0xF0) == 0x80 )
//	{
//		_SetValue( e.channel, 0.f );
//		channelModified = e.channel;
//	}
//}
//
//void midiController_Akai_MPD26::_SetValue( int channel, float value )
//{
//	printf( "channel: %d\n", channel );
//
//	if ( channel >= 0 && channel < 128 )
//	{
//		int index = mapping_[channel];
//		if( index >= 0  && index < ePadCount )
//		{
//			if ( value > 0 )
//				padData_[index] = 1;
//			else
//				padData_[index] = 0;
//		}
//	}
//}
