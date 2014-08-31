//reg.h
#ifndef __REG_H
#define __REG_H

#define WIN32_LEAN_AND_MEAN
#include <windows.h>

#define DEFAULT_ADDITIONAL_LETTERS L"Р-пр-я№-И_"
#define MAX_CHAR_SET_LENGTH 30
#define KN 0x200

#define HROOT HKEY_CURRENT_USER

//#define REG_KEY_SSS                 "%s%s%s"
#define REG_KEY_ACTIVE              L"Active"
#define REG_KEY_DIGIT_AS_CHAR       L"ConsiderDigitAsChar"
#define REG_KEY_CASE_SENSITIVE      L"CaseSensitive"
#define REG_KEY_CYCLIC_SEARCH       L"CyclicSearch"
#define REG_KEY_SEARCH_SELECTION    L"SearchSelection"
#define REG_KEY_SAVE_BOOKMARKS      L"SaveBookmarks"
#define REG_KEY_ADDITIONAL_LETTERS  L"AdditionalLetters"
#define REG_KEY_BOOKMARKS           L"Bookmarks"

typedef struct
{
  wchar_t s_AdditionalLetters[MAX_CHAR_SET_LENGTH];
  bool b_active;
  bool b_adddigits;
  bool b_casesensitive;
  bool b_cyclicsearch;
  bool b_searchselection;
  bool b_savebookmarks;
} SPluginOptions;

/*******************************************************************************
              class TRegistryStorage
*******************************************************************************/

struct TRegistryStorage
{
    TRegistryStorage();
    void SavePluginOptions();

    int GetRegKey(const wchar_t *Key, const char *ValueName, char *ValueData, char *Default, DWORD DataSize);
    void SetRegKey(const wchar_t *Key, const char *ValueName, const char *ValueData);
    void DeleteRegValue(const wchar_t *Key, const char *Value);
};

extern TRegistryStorage* RegistryStorage;
extern SPluginOptions plugin_options;

#endif /* __REG_H */
