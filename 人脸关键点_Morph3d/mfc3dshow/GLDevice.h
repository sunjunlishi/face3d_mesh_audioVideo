
/****************************************************************

  File name   :  GLDevice.h
  Author      :  Ò¶·å
  Version     :  1.0a
  Create Date :  2014/03/28  
  Description :  

*****************************************************************/

#ifndef _GLDevice_H_
#define _GLDevice_H_

// INCLUDES -----------------------------------------------------------------------------

#include <Windows.h>
extern  HWND g_hWnd ;
extern  HDC g_windowDC ;
extern  HGLRC g_glContext ;
// --------------------------------------------------------------------------------------

bool SetupGLDevice(HWND hWnd);

bool IsGLDeviceReady();

void DestroyGLDevice();

void FlushGLDevice();

// --------------------------------------------------------------------------------------

#endif