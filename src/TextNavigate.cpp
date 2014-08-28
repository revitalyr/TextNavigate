//TextNavigate.cpp

#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include "Headers.c/plugin.hpp"
#include "TextNavigate.h"

#include "crtdll.h"
#include "commons.h"
#include "reg.h"
#include "cregexp/cregexp.h"
#include "Dialog.h"
#include "sgml.h"
#include "pclasses.h"
#include "TextNavigate.h"
#include "guids.h"

struct PluginStartupInfo Info;
struct FarStandardFunctions FSF;

CTextNavigate* TextNavigate = NULL;

const wchar_t *get_msg(int id)
{
  return Info.GetMsg(&MainGuid, id);
}

#ifdef __cplusplus
extern "C"
#endif

DllExport void WINAPI SetStartupInfo(const struct PluginStartupInfo *PSInfo)
{
  ::Info = *PSInfo;
  FSF = *PSInfo->FSF;
  Info.FSF = &FSF;
  DebugString("SetStartupInfo:");

  RegistryStorage = new TRegistryStorage();
  TextNavigate = new CTextNavigate();
} //SetStartupInfo

DllExport void GetPluginInfo(struct PluginInfo *PInfo)
{
  DebugString("GetPluginInfo");
  PInfo->StructSize = sizeof(struct PluginInfo);
  PInfo->Flags = PF_DISABLEPANELS|PF_EDITOR;

  static const char *PMStrings[] = { get_msg(STitle) };

  PInfo->PluginMenuStrings = PMStrings;
  PInfo->PluginMenuStringsNumber = 1;
  PInfo->PluginConfigStrings = PMStrings;
  PInfo->PluginConfigStringsNumber = 1;
} //GetPluginInfo

DllExport int GetMinFarVersion(void) { return (MAKEFARVERSION(1, 70, 0)); };

DllExport HANDLE OpenPlugin(int, int)
{
  if (TextNavigate)
    TextNavigate->ShowPluginMenu();

  return INVALID_HANDLE_VALUE;
} //OpenPlugin

DllExport void  ExitFAR()
{
  delete RegistryStorage;
  delete TextNavigate;
  TextNavigate = NULL;
} //ExitFAR

int Configure(int)
{
  if (TextNavigate)
    TextNavigate->config_plugin();
  return false;
} //Configure

DllExport int ProcessEditorInput(const INPUT_RECORD *Rec)
{
  if (!plugin_options.b_active || !TextNavigate) return 0;
  return TextNavigate->ProcessEditorInput(Rec);
} //ProcessEditorInput

DllExport int ProcessEditorEvent(int Event, void *Param)
{
  //if (!b_active) return 0;
  if (!TextNavigate) return 0;
  return TextNavigate->ProcessEditorEvent(Event, Param);
} //ProcessEditorEvent

#ifdef __cplusplus
extern "C"{
#endif
  DllExport bool DllMainCRTStartup(HANDLE hDll, DWORD dwReason, LPVOID lpReserved);
#ifdef __cplusplus
};
#endif

DllExport bool DllMainCRTStartup(HANDLE hDll, DWORD dwReason, LPVOID lpReserved)
{
  return true;
}
