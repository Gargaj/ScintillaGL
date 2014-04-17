// A simple demonstration application using Scintilla
#include <stdio.h>
#include "ShaderEditOverlay.h"
#include <SDL.h>
#include <SDL_syswm.h>
#include <gl/glee.h>
#include "../liveCoding/liveCoding.h"
#include "fwzSetup.h"

#pragma comment(lib,"../bass/bass.lib")

char fragmentSource[65536] = "#version 430 core\n"
  "\n"
  "///////////////////////////////////////////////////////////////////////////////\n"
  "// shader inputs/outputs\n"
  "///////////////////////////////////////////////////////////////////////////////\n"
  "uniform float iGlobalTime; // in seconds\n"
  "uniform vec2 iResolution; // viewport resolution (in pixels) (1080p or 720p)\n"
  "uniform mat4 iMidiPad; // 16 buttons of midi controller\n"
  "uniform float iMidiPadValue; // sum of all elements in iMidiPad/16\n"
  "\n"
  "// all samplers have linear filtering applied, wraping set to repeat\n"
  "//\n"
  "uniform sampler1D iFFTTexture; // 1024\n"
  "// predefined textures\n"
  "//\n"
  "uniform sampler2D iTex1;\n"
  "uniform sampler2D iTex2;\n"
  "uniform sampler2D iTex3;\n"
  "uniform sampler2D iTex4;\n"
  "uniform sampler2D iChecker;\n"
  "\n"
  "// out_color must be written in order to see anything\n"
  "//\n"
  "layout(location = 0) out vec4 out_color;\n"
  "///////////////////////////////////////////////////////////////////////////////\n"
  "///////////////////////////////////////////////////////////////////////////////\n"
  "\n"
  "void main(void)\n"
  "{\n"
  "	vec2 uv = vec2( gl_FragCoord.xy ) / iResolution;\n"
  "	vec2 uv5 = uv - 0.5;\n"
  "	vec2 m;\n"
  "	m.x = atan(uv5.x / uv5.y);\n"
  "	m.y = length(uv5);\n"
  "	float f = texture( iFFTTexture, m.y  ).r * 1000;\n"
  "	m.y -= iGlobalTime;\n"
  "	vec4 t = texture( iChecker, m.xy  );\n"
  "	out_color = f + t;// + uv.xyxy * 0.5 * (sin( iGlobalTime ) + 1.5);\n"
  "}";

static ShaderEditOverlay app;

void Platform_Initialise(HWND hWnd);
void Platform_Finalise();

GLuint program;
char errbuf[65536];

GLuint CompileProgram(GLint srcLen, const char* src, GLint errbufLen, char* errbuf)
{
	GLuint prg = glCreateProgram();
	GLuint shd = glCreateShader(GL_FRAGMENT_SHADER);
	GLint size = 0, result = 0;

	glShaderSource(shd, 1, &src, &srcLen);
	glCompileShader(shd);
	glGetShaderInfoLog(shd, errbufLen, &size, errbuf);
	glGetShaderiv(shd, GL_COMPILE_STATUS, &result);
	if (!result) goto onError;

	glAttachShader(prg, shd);
	glLinkProgram(prg);
	glGetProgramInfoLog(prg, errbufLen-size, &size, errbuf+size);
	glGetProgramiv(prg, GL_LINK_STATUS, &result);

	if (result) goto onSuccess;

onError:
	glDeleteProgram(prg);
	prg = 0;

onSuccess:
	glDeleteShader(shd);

	return prg;
}

liveCoding live;

int main(int /*argc*/, char** /*argv*/)
{
  fwzSettings setup;
  setup.hInstance = GetModuleHandle(NULL);
#ifdef _DEBUG
  setup.scrWidth  = 1280;
  setup.scrHeight = 720;
  setup.nWindowed = true;
#else
  setup.scrWidth  = GetSystemMetrics( SM_CXSCREEN );
  setup.scrHeight = GetSystemMetrics( SM_CYSCREEN );
  setup.nWindowed = false;
  if (!OpenSetupDialog( & setup ))
    return 0;
#endif // _DEBUG

  SDL_Surface* mScreen;

	if(SDL_Init(SDL_INIT_VIDEO|SDL_INIT_TIMER)<0)											// Init The SDL Library, The VIDEO Subsystem
	{
		return 0;															// Get Out Of Here. Sorry.
	}

  FILE * f = fopen("shader.fs","rt");
  if (f)
  {
    ZeroMemory(fragmentSource,65536);
    int t = fread( fragmentSource, 1, 65535, f );
    fragmentSource[t] = 0;
    fclose(f);
  }

  int w = setup.scrWidth;
  int h = setup.scrHeight;
  uint32_t flags = SDL_HWSURFACE|SDL_OPENGLBLIT;									// We Want A Hardware Surface And Special OpenGLBlit Mode
  if (!setup.nWindowed)
    flags |= SDL_FULLSCREEN;

	SDL_GL_SetAttribute( SDL_GL_RED_SIZE, 8 );								// In order to use SDL_OPENGLBLIT we have to
	SDL_GL_SetAttribute( SDL_GL_GREEN_SIZE, 8 );							// set GL attributes first
	SDL_GL_SetAttribute( SDL_GL_BLUE_SIZE, 8 );
	SDL_GL_SetAttribute( SDL_GL_ALPHA_SIZE, 8 );
	SDL_GL_SetAttribute( SDL_GL_DEPTH_SIZE, 24 );
	SDL_GL_SetAttribute( SDL_GL_STENCIL_SIZE, 8 );
	SDL_GL_SetAttribute( SDL_GL_DOUBLEBUFFER, TRUE );							// colors and doublebuffering

	mScreen = SDL_SetVideoMode(w, h, 32, flags);
	if (!mScreen)
	{
		SDL_Quit();
		return 0;															// And Exit
	}
	
	SDL_EnableUNICODE(TRUE);
	SDL_EnableKeyRepeat(250, 20);

	SDL_SysWMinfo info = {{0, 0}, 0, 0};
	SDL_GetWMInfo(&info);

	Platform_Initialise(info.window);

	app.initialise(w, h);

  live.resolutionX_ = w;
  live.resolutionY_ = h;

  live.startUp( 0, NULL );

	Scintilla_LinkLexers();

	bool run = true;
	bool visible = true;

  program = CompileProgram(strlen(fragmentSource), fragmentSource, sizeof(errbuf), errbuf);
  app.addPrograms(1, &program);
  app.reset();

	while (run)
	{
		SDL_Event	E;
		while (SDL_PollEvent(&E))
		{
			if (E.type==SDL_QUIT) run=false;
			else if (E.type == SDL_KEYDOWN)
			{
				if (E.key.keysym.sym==SDLK_F4 && (E.key.keysym.mod == KMOD_LALT || E.key.keysym.mod == KMOD_RALT)) 
          run=false;

				if (E.key.keysym.sym==SDLK_F11)
				{
					visible = !visible;
// 					if (visible)
// 					{
// 						
// 					}
				}

				if (!visible) continue;
				app.handleKeyDown(E.key);
			}
      else if (E.type == SDL_MOUSEBUTTONDOWN)
      {
        app.handleMouseDown(E.button);
      }
		}

		glClearColor(0.08f, 0.18f, 0.18f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT|GL_STENCIL_BUFFER_BIT);
		
		glMatrixMode(GL_MODELVIEW);
		glLoadIdentity();
		glMatrixMode(GL_PROJECTION);
		glLoadIdentity();

    glEnable(GL_TEXTURE_2D);

    live.program_ = program;
    live.reloadShader();
    live.setUniforms();

		glUseProgram(program);
		glColor4f(0.0f, 1.0f, 0.0f, 1.0f);
		glBegin(GL_QUADS);
		glTexCoord2f(0.0f, 0.0f); glVertex2f(-1.00f, -1.00f);
		glTexCoord2f(1.0f, 0.0f); glVertex2f( 1.00f, -1.00f);
		glTexCoord2f(1.0f, 1.0f); glVertex2f( 1.00f,  1.00f);
		glTexCoord2f(0.0f, 1.0f); glVertex2f(-1.00f,  1.00f);
		glEnd();
		glUseProgram(0);

		if (visible)
		{
			app.renderFullscreen();
		}

		SDL_GL_SwapBuffers();

	}

	if (program) glDeleteProgram(program);

	Platform_Finalise();

	SDL_Quit();

	return 0;
}
