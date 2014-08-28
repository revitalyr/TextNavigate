//tools.h
#ifndef __TOOLS_H
#define __TOOLS_H

#define WIN32_LEAN_AND_MEAN
#include <windows.h>

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
int get_word(WideString & word, const char* str, int pos, int sln, int& begin_word_pos);

void InitQuickSearch(bool SearchUp, const char* substr, int strlen_word, bool casesensitive);
int QuickSearch_FW(const char* String, const char* substr, int n, int m, int begin_word_pos, bool SearchSelection, bool casesensitive);
int QuickSearch_BW(const char* String, const char* substr, int n, int m, bool SearchSelection, bool casesensitive);

bool GetFile(char const* FileName, char*& buffer, UINT& sz);
int file_exists(WideString const & fname);

void set_cursor_pos(int x, int y, struct EditorInfo *pei);
int get_cursor_pos(int& x, int& String);

int CheckForEsc(void);
PSgmlEl GetChild(PSgmlEl parent, const char* Name);
bool InitParam(char*& field, const char* param);

typedef union SCharData
{
	int  IArr[8];
	char CArr[32];
	void SetBit(unsigned char Bit);
	void ClearBit(unsigned char Bit);
	bool GetBit(unsigned char Bit) const;
} *PCharData;

extern SCharData CharSet;

#ifdef _DEBUG

static wchar_t tmp_str[1500];

#endif //_DEBUG

bool SplitRegExpr(const char* RegExpr, const char* InputStr, SMatches& m);

WideString const	a2w(AnsiString const &s);
AnsiString const	w2a(WideString const &s);
AnsiString const	toLower(AnsiString const &s);
AnsiString const	i2s(long i);


class String;
typedef std::unique_ptr<String>   StringPtr;

StringPtr asStringPtr(AnsiString const &s);
StringPtr asStringPtr(WideString const &s);
#endif /* __TOOLS_H */
