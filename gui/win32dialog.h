/////////////////////////////////////////////////////////////////////////
// $Id$
/////////////////////////////////////////////////////////////////////////

#include "config.h"

#if BX_USE_TEXTCONFIG && defined(WIN32)

extern "C" {
#include <windows.h>
}

HWND GetBochsWindow();
void LogAskDialog(BxEvent *event);
int AskFilename(HWND hwnd, bx_param_filename_c *param, const char *ext);
int AskString(bx_param_string_c *param);
int FloppyDialog(bx_param_filename_c *param);
int Cdrom1Dialog();
int RuntimeOptionsDialog();

#endif
