/*******************************************************************************
*   2005-2014, plastic demoscene group
*	authors: misz, bonzaj
*******************************************************************************/

#pragma once

#include "util.h"
#include "midi.h"

#define FFT_SIZE 1024
#define FFT_BINS 8
#define FFT_HISTORY_LENGTH 1024

struct liveCoding
{
	liveCoding()
		: startTimeMS_( 0 )
		, resolutionX_( 1 )
		, resolutionY_( 1 )
		, windowPosX_( 0 )
		, windowPosY_( 0 )
		, fullscreen_( 0 )
		, reloadShaderRequested_( true )
		, freshSourceCode_( NULL )
		, freshSourceCodeSize_( 0 )

		, freshBinary_( NULL )
		, freshBinarySize_( 0 )
		, freshBinaryFormat_( 0 )

		, iFFTTexture_( 0 )
		, iFFTTextureUnit_( -1 )
    , iFFTsHistoryTexture_( 0 )
    , iFFTsHistoryTextureUnit_( -1 )
    , iFFTsHistoryData_( NULL )

/*
		, hPipe_( NULL )
		, threadHandle_( NULL )
		, threadId_( 0 )
		, shutDownThread_( false )
*/

		// bass
		//
		, hRecord_( NULL )

		// midi
		//
		, midiController_( NULL )
  {	
    memset( fftBassBuf_, 0, sizeof(fftBassBuf_) );
    memset( fft_, 0, sizeof(fft_) );
    memset( ffts_, 0, sizeof(ffts_) );
    memset( ffti_, 0, sizeof(ffti_) );
    memset( fftsi_, 0, sizeof(fftsi_) );
  }

	int startUp( int argc, char* argv[] );
	void shutDown();
/*
	void readConfig();
	void writeConfig();
	void setupDefaultConfig();
*/

	void draw();

  void setUniforms();

  void setupShaderInputs();
	void setShaderInputs();

 	void reloadShader();
// 	int reloadShaderSource( const char* buf, size_t bufSize );
// 	int reloadShaderBinary( const u8* buf, size_t bufSize, GLenum format );
	GLuint createGLSLShaderFromBuf( const char* buf, size_t bufSize, GLenum profile );

	// pipes
	//
/*
	int startUpPipe();
	void shutDownPipe();

	BOOL tryConnect();
	void handlePipe();

	static unsigned int __stdcall thread_funcStatic( void *arg );
	unsigned int thread_func( void *arg );
*/

	// bass
	//
	int bass_startUp();
	void bass_shutDown();
	void bass_startCapture();
	void bass_stopCapture();

	struct Tex
	{
		Tex()
			: glTexID_( 0 )
		{	}

		Tex( const char* samplerName, const char* filename )
			: samplerName_( samplerName )
			, filename_( filename )
		{	}

		std::string samplerName_;
		std::string filename_;
		GLuint glTexID_;
	};

	//config_t config_;

	std::string liveCodingDir_;
	int resolutionX_;
	int resolutionY_;
	int windowPosX_;
	int windowPosY_;
	int fullscreen_;
	u32 startTimeMS_;

	GLuint vertexShader_;
	GLuint fragmentShader_;
	GLuint program_;

	bool reloadShaderRequested_;
	char* freshSourceCode_;
	size_t freshSourceCodeSize_;

	u8* freshBinary_;
	size_t freshBinarySize_;
	GLenum freshBinaryFormat_;

	std::vector<Tex> textures_;
	GLuint iFFTTexture_;
	int iFFTTextureUnit_;
  GLuint iFFTsHistoryTexture_;
  int iFFTsHistoryTextureUnit_;
  float* iFFTsHistoryData_;

	// pipes
	//
/*
	enum { e_PipeBUFSIZE = 256 * 1024 };

	HANDLE hPipe_;
	uintptr_t threadHandle_;
	unsigned int threadId_;

	CRITICAL_SECTION pipeMutex_;
	bool shutDownThread_;
*/

	// bass
	//
	HRECORD hRecord_;
  float fftBassBuf_[FFT_SIZE];
  float fft_[FFT_BINS];
  float ffts_[FFT_BINS];
  float ffti_[FFT_BINS];
  float fftsi_[FFT_BINS];

	// midi
	//
	midiController* midiController_;
};