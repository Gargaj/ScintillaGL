/*******************************************************************************
*   2005-2014, plastic demoscene group
*	authors: misz, bonzaj
*******************************************************************************/

#include <GLee.h>
#include "util.h"
#include <stdio.h>
#include <assert.h>

#define STBI_HEADER_FILE_ONLY
#include "stb_image.c"
#include <gl\glu.h>

static unsigned __int64 getHz()
{
	LARGE_INTEGER tf;
	QueryPerformanceFrequency(&tf);
	return tf.QuadPart;
}

static unsigned __int64 _gQPF = getHz();

unsigned int getTimeMS()
{
	LARGE_INTEGER ct;
	QueryPerformanceCounter( &ct );

	unsigned int val = (unsigned int)( (ct.QuadPart * 1000) / _gQPF );
	return val;
}

int loadFile( const char* filename, char** dstBuf, size_t* dstBufSize )
{
	FILE* f = fopen( filename, "rb" );
	if ( !f )
	{
		fprintf( stderr, "fopen failed! (%s)", filename );
		return -1;
	}

	fseek( f, 0, SEEK_END );
	size_t sizeInBytes = ftell( f );
	fseek( f, 0, SEEK_SET );

	char* buf = new char[sizeInBytes+1];
	size_t readBytes = fread( buf, 1, sizeInBytes, f );
	(void)readBytes;
	assert( readBytes == sizeInBytes );
	int ret = fclose( f );
	(void)ret;
	assert( ret != EOF );

	buf[sizeInBytes] = NULL;

	*dstBuf = buf;
	*dstBufSize = sizeInBytes;

	return 0;
}

GLuint readTexture( const char* filename )
{
/*
	png_structp png_ptr;
	png_infop info_ptr;
	unsigned int sig_read = 0;
	FILE *fp;

	if ((fp = fopen(filename, "rb")) == NULL)
		return 0;
	
	/ * Create and initialize the png_struct with the desired error handler
	* functions.  If you want to use the default stderr and longjump method,
	* you can supply NULL for the last three parameters.  We also supply the
	* the compiler header file version, so that we know if the application
	* was compiled with a compatible version of the library.  REQUIRED
	* /
	png_ptr = png_create_read_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL );
		//png_voidp user_error_ptr, user_error_fn, user_warning_fn);

	if (png_ptr == NULL)
	{
		fclose(fp);
		return 0;
	}

	/ * Allocate/initialize the memory for image information.  REQUIRED. * /
	info_ptr = png_create_info_struct(png_ptr);
	if (info_ptr == NULL)
	{
		fclose(fp);
		png_destroy_read_struct(&png_ptr, NULL, NULL);
		return 0;
	}

	/ * Set error handling if you are using the setjmp/longjmp method (this is
	* the normal method of doing things with libpng).  REQUIRED unless you
	* set up your own error handlers in the png_create_read_struct() earlier.
	* /

	if (setjmp(png_jmpbuf(png_ptr)))
	{
		/ * Free all of the memory associated with the png_ptr and info_ptr * /
		png_destroy_read_struct(&png_ptr, &info_ptr, NULL);
		fclose(fp);
		/ * If we get here, we had a problem reading the file * /
		return 0;
	}

	/ * Set up the input control if you are using standard C streams * /
	png_init_io(png_ptr, fp);

	
	/ * If we have already read some of the signature * /
	png_set_sig_bytes(png_ptr, sig_read);

	/ *
	* If you have enough memory to read in the entire image at once,
	* and you need to specify only transforms that can be controlled
	* with one of the PNG_TRANSFORM_* bits (this presently excludes
	* dithering, filling, setting background, and doing gamma
	* adjustment), then you can read the entire image (including
	* pixels) into the info structure with this call:
	* /
	png_read_png(png_ptr, info_ptr, NULL, NULL);

	//png_read_image(png_ptr, row_pointers);

	GLenum internalFormat = GL_SRGB8_ALPHA8;
	GLenum srcFormat = 0;

	if( info_ptr->bit_depth == 8 )
	{

	}		
	else
	{
		/ * Free all of the memory associated with the png_ptr and info_ptr * /
		png_destroy_read_struct(&png_ptr, &info_ptr, NULL);
		fclose(fp);
		/ * If we get here, we had a problem reading the file * /
		return 0;
	}

	if ( info_ptr->channels == 3 )
	{
		srcFormat = GL_RGB;
	}
	else if ( info_ptr->channels == 4 )
	{
		srcFormat = GL_RGBA;
	}
	else
	{
		/ * Free all of the memory associated with the png_ptr and info_ptr * /
		png_destroy_read_struct(&png_ptr, &info_ptr, NULL);
		fclose(fp);
		/ * If we get here, we had a problem reading the file * /
		return 0;
	}

	const unsigned pixelSize = info_ptr->channels;
	const unsigned rowSize = info_ptr->width * pixelSize;
	const unsigned size = rowSize * info_ptr->height;

	unsigned char* pixels = new unsigned char[size];
	unsigned char* tmpPixels = pixels;

	for (int y = info_ptr->height-1; y >= 0; --y)
	{
		memcpy( tmpPixels, info_ptr->row_pointers[y], rowSize );
		tmpPixels += rowSize;
	}
*/

  int comp = 0;
  int width = 0;
  int height = 0;
  unsigned char * c = stbi_load( filename, (int*)&width, (int*)&height, &comp, STBI_rgb_alpha );

	GLuint glTexId = 0;
	glGenTextures( 1, &glTexId );
	glBindTexture( GL_TEXTURE_2D, glTexId );

	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT );
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT );
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );

  GLenum internalFormat = GL_SRGB8_ALPHA8;
  GLenum srcFormat = GL_RGBA;

	glTexImage2D( GL_TEXTURE_2D, 0, internalFormat, width, height, 0, srcFormat, GL_UNSIGNED_BYTE, c );

  stbi_image_free(c);

	glBindTexture( GL_TEXTURE_2D, 0 );

	//delete[] pixels;

	//png_destroy_read_struct(&png_ptr, &info_ptr, NULL);
	//fclose( fp );

	return glTexId;
}
