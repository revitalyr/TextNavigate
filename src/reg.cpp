//reg.cpp
#define WIN32_LEAN_AND_MEAN
#include <windows.h>

#include "Headers.c/plugin.hpp"
#include "Headers.c/PluginSettings.hpp"

#include "reg.h"
#include "TextNavigate.h"
#include "guids.h"

SPluginOptions    plugin_options;
TRegistryStorage* RegistryStorage;

/*******************************************************************************
  TRegistryStorage
*******************************************************************************/

TRegistryStorage::TRegistryStorage()
{
  PluginSettings settings(MainGuid, Info.SettingsControl);

  plugin_options.b_active = settings.Get(0, REG_KEY_ACTIVE, true);
  plugin_options.b_adddigits = settings.Get(0, REG_KEY_DIGIT_AS_CHAR, true);
  plugin_options.b_casesensitive = settings.Get(0, REG_KEY_CASE_SENSITIVE, true);
  plugin_options.b_cyclicsearch = settings.Get(0, REG_KEY_CYCLIC_SEARCH, true);
  plugin_options.b_searchselection = settings.Get(0, REG_KEY_SEARCH_SELECTION, false);
  plugin_options.b_savebookmarks = settings.Get(0, REG_KEY_SAVE_BOOKMARKS, true);
  settings.Get(0, REG_KEY_ADDITIONAL_LETTERS, plugin_options.s_AdditionalLetters, MAX_CHAR_SET_LENGTH, DEFAULT_ADDITIONAL_LETTERS);
}

void TRegistryStorage::SavePluginOptions()
{
  PluginSettings settings(MainGuid, Info.SettingsControl);

  settings.Set(0, REG_KEY_ACTIVE, plugin_options.b_active);
  settings.Set(0, REG_KEY_DIGIT_AS_CHAR, plugin_options.b_adddigits);
  settings.Set(0, REG_KEY_CASE_SENSITIVE, plugin_options.b_casesensitive);
  settings.Set(0, REG_KEY_CYCLIC_SEARCH, plugin_options.b_cyclicsearch);
  settings.Set(0, REG_KEY_SEARCH_SELECTION, plugin_options.b_searchselection);
  settings.Set(0, REG_KEY_SAVE_BOOKMARKS, plugin_options.b_savebookmarks);
  settings.Set(0, REG_KEY_ADDITIONAL_LETTERS, plugin_options.s_AdditionalLetters);
}

void TRegistryStorage::DeleteRegValue(const wchar_t *Key, const char *Value)
{
  PluginSettings settings(MainGuid, Info.SettingsControl);

  settings.OpenSubKey(0, Key);
  settings.DeleteValue(0, a2w(Value).c_str());
}

