//tools.h
#ifndef __TOOLS_H
#define __TOOLS_H

#define WIN32_LEAN_AND_MEAN
#include <windows.h>

//#define UCHAR do not use

#include <memory>

#include "defs.h"
#include "TextNavigate.h"
#include "sgml.h"
#include "cregexp/CRegExp.h"

void InitUnion();
void AddEngCharsToUnion(void);
void AddDigitsToUnion(void);
void AddSubSetToUnion(UCHAR *set);
int is_char(UCHAR c);
int get_word(AnsiString & word, AnsiString const & str, int pos, int sln, int& begin_word_pos);
int get_word(WideString & word, WideString const & str, int pos, int sln, int& begin_word_pos);

void InitQuickSearch(bool SearchUp, const char* substr, int strlen_word, bool casesensitive);
int QuickSearch_FW(const char* String, const char* substr, int n, int m, int begin_word_pos, bool SearchSelection, bool casesensitive);
int QuickSearch_BW(const char* String, const char* substr, int n, int m, bool SearchSelection, bool casesensitive);

bool GetFile(WideString const & FileName, char*& buffer, UINT& sz);
int file_exists(WideString const & fname);

void set_cursor_pos(int x, int y, struct EditorInfo *pei);
int get_cursor_pos(int& x, int& String);

int CheckForEsc(void);
PSgmlEl GetChild(PSgmlEl parent, const wchar_t* Name);
bool InitParam(char*& field, const char* param);
bool InitParam(WideString & field, const char* param);
bool InitParam(WideString & field, const wchar_t* param);
bool InitParam(WideString & field, WideString const & param);

#ifdef _DEBUG

static wchar_t tmp_str[1500];

#endif //_DEBUG

bool SplitRegExpr(const char* RegExpr, const char* InputStr, SMatches& m);

typedef union SCharData
{
  int  IArr[8];
  char CArr[32];
  void SetBit (unsigned char Bit) {
    int p = Bit / 8;
    CArr [p] |= (char) (1 << Bit % 8);
  }
  void ClearBit (unsigned char Bit) {
    int p = Bit / 8;
    CArr [p] &= (char) (~(1 << Bit % 8));
  }
  bool GetBit (unsigned char Bit) const {
    int p = (unsigned char) Bit / 8;
    return (CArr [p] & (1 << Bit % 8)) != 0;
  }
} *PCharData;


void ReplaceSpecRegSymbols(WideString &str);

struct PluginStartupInfo;
WideString const	getEditorFilename (PluginStartupInfo const & info);

WideString const	a2w(AnsiString const      & s);
WideString const	a2w(char const            * s);
AnsiString const	w2a(WideString const      & s);
AnsiString const	w2a(wchar_t const         * s);

WideString const	toLower(WideString const  & s);
AnsiString const	toLower(AnsiString const  & s);
WideString const	toUpper(WideString const  & s);
AnsiString const	toUpper(AnsiString const  & s);
void              toUpper(char              * s);

WideString const	i2s(long i);

int               strcmp(char const         * sl,
                         wchar_t const      * sr);    
class String;
typedef std::unique_ptr<String>   StringPtr;

StringPtr asStringPtr(AnsiString const &s);
StringPtr asStringPtr(WideString const &s);
#endif /* __TOOLS_H */
