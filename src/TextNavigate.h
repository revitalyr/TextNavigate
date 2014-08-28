//TextNavigate.h
#ifndef __TEXTNAVIGATE_H
#define __TEXTNAVIGATE_H

//#include "plugin.h"
#include "defs.h"
#include "pclasses.h"
#include "string.hpp"
#include "TextNavigate-lng.h"

#define FarSprintf FSF.sprintf
#define MAX_BOOKMARKS_STR_LENGTH 1000

extern struct PluginStartupInfo Info;
extern struct FarStandardFunctions FSF;

#if defined _DEBUG || defined DEBUGSTRING

#include <sstream>

static void _DebugString(const WideString& str, const char *filename, int line)
{
  size_t	n = strlen(filename),
			i = n - 1;
  while (filename[i] != '\\' && i > 0)
    i--;

  std::wostringstream	buf;
  buf << &filename[i == 0 ? 0 : i + 1] << ", " << line << ": " << str << "\n";
  OutputDebugString(buf.str().c_str());

/*
  char short_filename[MAX_PATH];
  lstrcpyA(short_filename, &filename[i == 0 ? 0 : i + 1]);

  OutputDebugString(short_filename);
  OutputDebugString(", ");
  char _str[20];
  FSF.itoa(line, _str, 10);
  OutputDebugString(_str);
  OutputDebugString(": ");
  OutputDebugString(str);
  OutputDebugString("\n");
*/
}

#define DebugString(x) _DebugString((x), __FILE__, __LINE__)
#define assert(x) if (!(x)) DebugString("Assertion failed");

#else

#define DebugString(x)
#define assert(x)

#endif //_DEBUG

typedef struct FarMenuItem FMI, *LPFMI;

const wchar_t *get_msg(int id);

extern "C" {

//int WINAPI _export GetMinFarVersion(void);
//HANDLE WINAPI _export OpenPlugin(int OpenFrom, int Item);
//void WINAPI _export ExitFAR();
//void WINAPI _export SetStartupInfo(const struct PluginStartupInfo *Info);
//void WINAPI _export GetPluginInfo(struct PluginInfo *Info);
//int WINAPI _export Configure(int ItemNumber);
//int WINAPI _export ProcessEditorEvent(int Event, void* Param);
//int WINAPI _export ProcessEditorInput(const INPUT_RECORD* Rec);

}; //extern "C"

#endif /* __TEXTNAVIGATE_H */
