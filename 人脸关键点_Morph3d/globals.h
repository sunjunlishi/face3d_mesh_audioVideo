#ifndef __TTSAPP_GLOBALS_H__
#define __TTSAPP_GLOBALS_H__

// App includes
#include <sapi.h>           // SAPI includes
#pragma warning(push)       // Disable warning C4996: 'GetVersionExA': was declared deprecated (sphelper.h:1319)
#pragma warning(disable: 4996)
#include <sphelper.h>
#pragma warning(pop)
#include <spuihelp.h>

// Constant definitions
#define MAX_SIZE                102400      //100K
#define NORM_SIZE               256
#define NUM_OUTPUTFORMATS       36
#define WM_TTSAPPCUSTOMEVENT       WM_USER+1341          // Window message used for systhesis events
#define CHILD_CLASS             _T("TTSAppChildWin")  // Child window for blitting mouth to
#define WEYESNAR                14              // eye positions
#define WEYESCLO                15
#define NUM_PHONEMES            6
#define CHARACTER_WIDTH         256
#define CHARACTER_HEIGHT        256
#define MAX_FILE_PATH			256
#include <commctrl.h>		// Common controls
// Other global variables
extern int                  g_iBmp;
extern HIMAGELIST           g_hListBmp;
extern const int            g_aMapVisemeToImage[22];

// Output formats
extern const SPSTREAMFORMAT g_aOutputFormat[NUM_OUTPUTFORMATS];
extern TCHAR*               g_aszOutputFormat[NUM_OUTPUTFORMATS];

#endif // __TTSAPP_GLOBALS_H__
