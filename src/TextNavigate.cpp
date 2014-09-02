//TextNavigate.cpp

#define WIN32_LEAN_AND_MEAN
#include <windows.h>

#include "Headers.c/plugin.hpp"
#include "Headers.c/farversion.hpp"

#include "TextNavigate.h"

//#include "crtdll.h"
#include "commons.h"
#include "reg.h"
#include "cregexp/cregexp.h"
#include "Dialog.h"
#include "sgml.h"
#include "pclasses.h"
#include "TextNavigate.h"
#include "guids.h"

#define PLUGIN_BUILD 29
#define PLUGIN_DESC L"Navigator plugin for FAR"
#define PLUGIN_NAME L"TextNavigate"
#define PLUGIN_FILENAME L"TextNavigate.dll"
#define PLUGIN_AUTHOR FARCOMPANYNAME
#define PLUGIN_VERSION MAKEFARVERSION(FARMANAGERVERSION_MAJOR,FARMANAGERVERSION_MINOR,FARMANAGERVERSION_REVISION,PLUGIN_BUILD,VS_RELEASE)

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

void WINAPI SetStartupInfoW(const struct PluginStartupInfo *PSInfo)
{
  DebugString(L"SetStartupInfoW:");
  if (PSInfo->StructSize >= sizeof(PluginStartupInfo)) {
    ::Info = *PSInfo;
    FSF = *PSInfo->FSF;
    Info.FSF = &FSF;

    RegistryStorage = new TRegistryStorage();
    TextNavigate = new CTextNavigate();
  }
} //SetStartupInfo

void WINAPI GetPluginInfoW(struct PluginInfo *PInfo)
{
  DebugString(L"GetPluginInfoW");
  PInfo->StructSize = sizeof(struct PluginInfo);
  PInfo->Flags = PF_DISABLEPANELS|PF_EDITOR;

  static const wchar_t *PMStrings[] = { get_msg(STitle) };

  PInfo->PluginMenu.Guids=&MenuGuid;
  PInfo->PluginMenu.Strings = PMStrings;
  PInfo->PluginMenu.Count = 1;
  PInfo->PluginConfig.Guids=&MenuGuid;
  PInfo->PluginConfig.Strings = PMStrings;
  PInfo->PluginConfig.Count = 1;
} //GetPluginInfo

void WINAPI GetGlobalInfoW(struct GlobalInfo *Info)
{
  DebugString(L"GetGlobalInfoW");
  Info->StructSize=sizeof(GlobalInfo);
  Info->MinFarVersion=FARMANAGERVERSION;
  Info->Version=PLUGIN_VERSION;
  Info->Guid=MainGuid;
  Info->Title=PLUGIN_NAME;
  Info->Description=PLUGIN_DESC;
  Info->Author=PLUGIN_AUTHOR;
}

HANDLE WINAPI OpenW(const struct OpenInfo *Info)
{
  if (TextNavigate)
    TextNavigate->ShowPluginMenu();

  return INVALID_HANDLE_VALUE;
} //OpenPlugin

void WINAPI ExitFARW(const struct ExitInfo *Info) {
  delete RegistryStorage;
  delete TextNavigate;
  TextNavigate = NULL;
} //ExitFAR

intptr_t WINAPI ConfigureW(const struct ConfigureInfo *Info) {
  if (TextNavigate)
    TextNavigate->config_plugin();
  return false;
} //Configure

intptr_t WINAPI ProcessEditorInputW(const struct ProcessEditorInputInfo *Info) {
  if ((Info->StructSize < sizeof(ProcessEditorInputInfo)) || !plugin_options.b_active || !TextNavigate) return 0;
  return TextNavigate->ProcessEditorInput(&Info->Rec);
} //ProcessEditorInput

intptr_t WINAPI ProcessEditorEventW(const struct ProcessEditorEventInfo *Info) {
  //if (!b_active) return 0;
  if ((Info->StructSize < sizeof(ProcessEditorEventInfo)) || !TextNavigate) return 0;
  return TextNavigate->ProcessEditorEvent(Info->Event, Info->Param);
} //ProcessEditorEvent

//#ifdef __cplusplus
//extern "C"{
//#endif
//  bool DllMainCRTStartup(HANDLE hDll, DWORD dwReason, LPVOID lpReserved);
//#ifdef __cplusplus
//};
//#endif
//
//bool DllMainCRTStartup(HANDLE hDll, DWORD dwReason, LPVOID lpReserved)
//{
//  return true;
//}
