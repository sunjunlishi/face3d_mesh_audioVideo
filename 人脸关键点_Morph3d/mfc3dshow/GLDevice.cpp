
/****************************************************************

  File name   :  GLDevice.cpp
  Author      :  Ò¶·å
  Version     :  1.0a
  Create Date :  2014/03/28  
  Description :  

*****************************************************************/

// INCLUDES -----------------------------------------------------------------------------

#include "GLDevice.h"
#include <gl\glew.h>
#include "gl/gl.h"			// Header File For The OpenGL32 Library
#include "gl/glu.h"			// Header File For The GLu32 Library

// --------------------------------------------------------------------------------------

// --------------------------------------------------------------------------------------

HWND g_hWnd = 0;
HDC g_windowDC = 0;
HGLRC g_glContext = 0;

// --------------------------------------------------------------------------------------

bool SetupGLDevice(HWND hWnd)
{
    if (g_glContext)
    {
        return true;
    }

    g_windowDC = ::GetDC(hWnd);          // Did We Get A Device Context?
    if (!g_windowDC)
    {
        MessageBox(NULL, "[SetupGLDevice]: Can't Create a GL Device Context", "ERROR", MB_OK);
        return false;
    }
	

	  //static	PIXELFORMATDESCRIPTOR pfd =			    // pfd Tells Windows How We Want Things To Be
   // {
   //    sizeof(PIXELFORMATDESCRIPTOR), // size of this pfd
   //   1,                              // version number
   //   PFD_DRAW_TO_WINDOW |            // support window
   //   PFD_SUPPORT_OPENGL |            // support OpenGL
   //   PFD_DOUBLEBUFFER,                // double buffered
   //   PFD_TYPE_RGBA,                  // RGBA type
   //   24,                             // 24-bit color depth
   //   0, 0, 0, 0, 0, 0,               // color bits ignored
   //   0,                              // no alpha buffer
   //   0,                              // shift bit ignored
   //   0,                              // no accumulation buffer
   //   0, 0, 0, 0,                     // accum bits ignored
   //   16,                             // 16-bit z-buffer
   //   0,                              // no stencil buffer
   //   0,                              // no auxiliary buffer
   //   PFD_MAIN_PLANE,                 // main layer
   //   0,                              // reserved
   //   0, 0, 0                         // layer masks ignored
   // };

    static	PIXELFORMATDESCRIPTOR pfd =			    // pfd Tells Windows How We Want Things To Be
    {
        sizeof(PIXELFORMATDESCRIPTOR),				// Size Of This Pixel Format Descriptor
        1,											// Version Number
        PFD_DRAW_TO_WINDOW |						// Format Must Support Window
        PFD_SUPPORT_OPENGL |						// Format Must Support OpenGL
        PFD_DOUBLEBUFFER,							// Must Support Double Buffering
        PFD_TYPE_RGBA,								// Request An RGBA Format
        32,										    // Select Our Color Depth
        0, 0, 0, 0, 0, 0,							// Color Bits Ignored
        0,											// No Alpha Buffer
        0,											// Shift Bit Ignored
        0,											// No Accumulation Buffer
        0, 0, 0, 0,									// Accumulation Bits Ignored
        24,											// 24 Bit Z-Buffer (Depth Buffer)  
        8,											// 8 Bit Stencil Buffer
        0,											// No Auxiliary Buffer
        PFD_MAIN_PLANE,								// Main Drawing Layer
        0,											// Reserved
        0, 0, 0										// Layer Masks Ignored
    };

    int pixelFormat = ::ChoosePixelFormat(g_windowDC, &pfd);
    if (!pixelFormat)                                   // Did Windows Find A Matching Pixel Format?
    {
        MessageBox(NULL, "[SetupGLDevice]: Can't Find A Suitable PixelFormat.", "ERROR", MB_OK);
        ::ReleaseDC(hWnd, g_windowDC);
        g_windowDC = 0;
        return false;
    }

    if (!::SetPixelFormat(g_windowDC, pixelFormat, &pfd))// Are We Able To Set The Pixel Format?
    {
        MessageBox(NULL, "[SetupGLDevice]: Can't Set The PixelFormat.", "ERROR", MB_OK);
        ::ReleaseDC(hWnd, g_windowDC);
        return false;
    }

    g_glContext = ::wglCreateContext(g_windowDC);
    if (!g_glContext)                                   // Are We Able To Get A Rendering Context?
    {
        MessageBox(NULL, "[SetupGLDevice]: Can't Create A GL Rendering Context.", "ERROR", MB_OK);
        ::ReleaseDC(hWnd, g_windowDC);
        g_windowDC = 0;
        return false;
    }

    if(!::wglMakeCurrent(g_windowDC, g_glContext))      // Try To Activate The Rendering Context
    {
        MessageBox(NULL, "[SetupGLDevice]: Can't Activate The GL Rendering Context.", "ERROR", MB_OK);
        ::wglDeleteContext(g_glContext);
        g_glContext = 0;
        ::ReleaseDC(hWnd, g_windowDC);
        g_windowDC = 0;
        return false;
    }

	// ::glClearColor(0.0f,0.0f,0.0f,1.0f);
	 //Specify the back of the buffer as clear depth
	::glClearDepth(1.0f);
	glEnable(GL_LIGHT0);
	glEnable(GL_LIGHTING);
	glEnable(GL_COLOR_MATERIAL);
	glEnable(GL_DEPTH_TEST);

	 glewInit();
     g_hWnd = hWnd;

    return true;
}

bool IsGLDeviceReady()
{
    return (g_glContext != 0);
}

void DestroyGLDevice()
{
    if (!g_glContext)
    {
        return;
    }

    if (!::wglMakeCurrent(NULL, NULL))
    {
        MessageBox(NULL, "Release Of DC And RC Failed.", "ERROR", MB_OK);
    }

    if (!::wglDeleteContext(g_glContext))
    {
        MessageBox(NULL, "Release Rendering Context Failed.", "ERROR", MB_OK);
    }

    if (!::ReleaseDC(g_hWnd, g_windowDC))
    {
        MessageBox(NULL, "Release Window Device Context Failed.", "ERROR", MB_OK);
    }

    g_hWnd = NULL;
    g_windowDC = NULL;
    g_glContext = NULL;
}

void FlushGLDevice()
{
    if (g_windowDC)
    {
        glFlush();
        ::SwapBuffers(g_windowDC);
    }
}
