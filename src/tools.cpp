//tools.cpp
#include <sstream>
#include <locale>         // std::locale, std::ctype, std::use_facet
#include <regex>
#include "Headers.c/plugin.hpp"

#include "tools.h"
#include "commons.h"

#include<unicode/DString.h>

SCharData CharSet;

void AddEngCharsToUnion(void)
{
  //добавляем английские буквы
  UCHAR ch = 'A';
  while (ch <= 'Z')
    CharSet.SetBit(ch++);
  ch = 'a';
  while (ch <= 'z')
    CharSet.SetBit(ch++);
} //AddEngCharsToUnion

void AddDigitsToUnion(void)
{
  UCHAR ch = '0';
  while (ch <= '9')
    CharSet.SetBit(ch++);
} //AddDigitsToUnion

void AddSubSetToUnion(UCHAR *set)
{
  UCHAR *ch = set;

  while (*ch)
  {
    UCHAR BeginChar = *ch++;
    if (*ch == '-' && ch[1])
    {
      UCHAR EndChar = *++ch;
      if (BeginChar <= EndChar)
        while (BeginChar <= EndChar && BeginChar)
         CharSet.SetBit(BeginChar++);
      ch++;
    }
    else
     CharSet.SetBit(BeginChar);
  }
} //AddSubSetToUnion

void InitUnion()
{
  ZeroMemory(&CharSet, 32);
  AddEngCharsToUnion();
  if (plugin_options.b_adddigits)
    AddDigitsToUnion();
  AddSubSetToUnion((UCHAR *)plugin_options.s_AdditionalLetters);
} //InitUnion

int is_char(UCHAR c)
{
  return CharSet.GetBit(c);
} //is_char

int get_word(AnsiString &word, AnsiString const &str, int pos, int sln, int &begin_word_pos)
{
  int i, j;
  for (i = pos; i > 0 && is_char(str[i]); i--);
  if (is_char(str[i])) i--;
  begin_word_pos = i+1;

  for (j = pos; j < sln && is_char(str[j]); j++);
  if (is_char(str[j])) j++;
  j -= i + 1;

  if (j <= 0)
    return false;
  //if (j > MAX_WORD_LENGTH - 1)
  if (j > MAX_PATH - 1)
    return false;
  word = AnsiString(str.c_str()+i+1, str.c_str()+j + 1);
  return (j != 0);
} //get_word

int get_word(WideString & word, WideString const & str, int pos, int sln, int& begin_word_pos) {
  AnsiString    tmp_word = w2a(word); 
  int           res = get_word(tmp_word, w2a(str), pos, sln, begin_word_pos);
  word = a2w(tmp_word);

  return res;
}

//static const int ASIZE = 256;
#define ASIZE 256
static UCHAR qs_bc[ASIZE];

void InitQuickSearch(bool searchUp, AnsiString const & substr, bool casesensitive)
{
  int strlen_word = substr.length();
  if (searchUp)
  {
    FillMemory(qs_bc, strlen_word, ASIZE);
    for (int i = 0; i < strlen_word; i++)
      if (qs_bc[(UCHAR)substr[i]] == strlen_word)
        qs_bc[(UCHAR)substr[i]] = i ? (UCHAR)i : (UCHAR)strlen_word;

    if (!casesensitive)
    {
      char buffer[MAX_WORD_LENGTH];
	    strncpy(buffer, substr.c_str(), MAX_WORD_LENGTH);
      toUpper(buffer);  //FSF.LStrupr(buffer);
      for (int i = 0; i < strlen_word; i++)
        if (qs_bc[(UCHAR)buffer[i]] == strlen_word)
          qs_bc[(UCHAR)buffer[i]] = i ? (UCHAR)i : (UCHAR)strlen_word;
    }
  }
  else //search down
  {
    FillMemory(qs_bc, (UCHAR)(strlen_word + 1), ASIZE);
    for (int i = 0; i < strlen_word; i++)
      qs_bc[(UCHAR)substr[i]] = (UCHAR)(strlen_word - i);

    if (!casesensitive)
    {
      char buffer[MAX_WORD_LENGTH];
	    strncpy(buffer, substr.c_str(), MAX_WORD_LENGTH);
      toUpper(buffer);  //FSF.LStrupr(buffer);
      for (int i = 0; i < strlen_word; i++)
        qs_bc[(UCHAR)buffer[i]] = (UCHAR)(strlen_word - i);
    }
  }
} //InitQuickSearch

int QuickSearch_FW(const char* String, const char* substr, int n, int m, int begin_word_pos, bool SearchSelection, bool casesensitive)
{
  int i;

  i = begin_word_pos; bool res;
  while (i <= n - m)
  {
    if (casesensitive)
      res = memcmp(&String[i], substr, m) != 0;
    else
      res = strnicmp /*FSF.LStrnicmp*/(&String[i], substr, m) != 0;

    if (!res && (SearchSelection ||
                 (!is_char(String[i + m]) &&
                  !((i > 0) && is_char(String[i-1])))
                )
       )
      return (i);

    i += qs_bc[(UCHAR)String[i + m]];
  }
  return -1;
} //QuickSearch_FW

int QuickSearch_BW(const char* String, const char* substr, int n, int m, bool SearchSelection, bool casesensitive)
{
  int i;

  i = n - m; bool res;
  while (i >= 0)
  {
    if (casesensitive)
      res = memcmp(&String[i], substr, m) != 0;
    else
      res = res = strnicmp /*FSF.LStrnicmp*/(&String[i], substr, m) != 0;

    if (!res && (SearchSelection ||
                 (!is_char(String[i + m]) &&
                  !((i > 0) && is_char(String[i-1]))
                 )
                )
       )
      return (i);
    i -= qs_bc[(UCHAR)String[i]];
  }
  return -1;
} //QuickSearch_BW

bool GetFile(WideString const & FileName, char*& buffer, UINT& sz)
{
  buffer = NULL;
  HANDLE s = CreateFile(FileName.c_str(), GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
  if (s != INVALID_HANDLE_VALUE)
  {
    sz = GetFileSize(s, NULL);
    AutoPtr<char> _buffer(new char[sz+1]);
    if (sz && _buffer != NULL)
    {
      DWORD rb = 0;
      if (ReadFile(s, _buffer, sz, &rb, NULL) && rb)
      {
        buffer = _buffer.Detach();
        buffer[rb] = 0;
      }
      else
        return false;
    }
  }
  CloseHandle(s);
  return true;
} //GetFile

int file_exists(WideString const & fname)
{
  return (GetFileAttributes(fname.c_str()) != (DWORD)(-1));
} //file_exists

int get_cursor_pos(int &x, int &String)
{
  struct EditorInfo EInfo;
  if (!Info.EditorControl(-1, ECTL_GETINFO, 0, &EInfo))
    return false;
  x = EInfo.CurPos;
  String = EInfo.CurLine;
  return true;
} //get_cursor_pos

void set_cursor_pos(int x, int y, struct EditorInfo* pei)
{
  struct EditorSetPosition esp;

  esp.CurLine = pei->CurLine;
  esp.CurPos = pei->CurPos;
  esp.TopScreenLine = pei->TopScreenLine;
  esp.LeftPos = pei->LeftPos;
  esp.CurTabPos = -1;
  esp.Overtype = -1;
  Info.EditorControl(-1, ECTL_SETPOSITION, 0, &esp);

  if (y != -1)
  {
    esp.CurLine = y;
    esp.CurPos = x;

    esp.TopScreenLine = -1;
    esp.LeftPos = -1;
    esp.CurTabPos = -1;

    Info.EditorControl(-1, ECTL_SETPOSITION, 0, &esp);
  }
  Info.EditorControl(-1, ECTL_REDRAW, 0, NULL);
} //set_cursor_pos

int CheckForEsc(void)
{
  INPUT_RECORD rec;
  static HANDLE hConInp = GetStdHandle(STD_INPUT_HANDLE);
  DWORD ReadCount;

  PeekConsoleInput(hConInp, &rec, 1, &ReadCount);
  if (0 == ReadCount) return false;
  ReadConsoleInput(hConInp, &rec, 1, &ReadCount);
  if (rec.EventType == KEY_EVENT)
   if (rec.Event.KeyEvent.wVirtualKeyCode == VK_ESCAPE &&
     rec.Event.KeyEvent.bKeyDown) return true;
  return false;
} //CheckForEsc

PSgmlEl GetChild(PSgmlEl parent, const wchar_t* Name)
{
  if (!parent || !Name)
   return NULL;
  PSgmlEl child = parent->child();

  if (!child) return NULL;

  if (child->getname() && !strcmp(child->getname(), Name))
    return (child);
  else
    return (child->search(Name));
} //GetChild

bool InitParam(char*& field, const char* param)
{
  if (param)
  {
    size_t len = strlen(param) + 1;
    field = new char[len];
    lstrcpyA(field, param);
    return true;
  }
  return false;
} //InitParam

bool InitParam(WideString & field, const char* param){
  if (param) {
    field = a2w(param);
    return true;
  }
  return false;
}

bool InitParam(WideString & field, const wchar_t* param){
  if (param) {
    field = param;
    return true;
  }
  return false;
}

bool InitParam(WideString & field, WideString const & param) {
  if (!param.empty()) {
    field = param;
    return true;
  }
  return false;
}

bool SplitRegExpr(const char* RegExpr, const char* InputStr, SMatches& m)
{
  CRegExp reg;
  if (!reg.setRE(asStringPtr(RegExpr).get()))
    return false;

  int cm(0);
  bool exec;
  SMatches lm;
  char *s = (char*)InputStr; const char *e = InputStr + strlen(InputStr);
  do
  {
    exec = s < e;
    if (reg.parse(asStringPtr(s).get(), &lm))
    {
      m.s[cm] = (short)(s - InputStr);
      m.e[cm] = m.s[cm] + lm.s[0];
      cm++;
      s += lm.e[0];
    }
    else if (exec)
    {
      m.s[cm] = (short)(s - InputStr);
      m.e[cm] = m.s[cm] + (short)(e - s);
      cm++;
      exec = false;
    }
  } while (exec);
  m.cMatch = cm;
  return true;
}

void ReplaceSpecRegSymbols(WideString &str)
{
  using std::wregex;
  using std::regex_replace;

  const static wregex   specRE (LR"(\(|\)|\[|\]|\{|\}|\|\$)");
  str = regex_replace(str, specRE, LR"(\$&)");
} //ReplaceSpecRegSymbols


WideString const	getEditorFilename (PluginStartupInfo const & info) {
  size_t      fileNameSize = info.EditorControl(-1,ECTL_GETFILENAME,0,0);
  WideString  fileName(fileNameSize + 1, '\0');
        
  Info.EditorControl(-1, ECTL_GETFILENAME, fileNameSize, const_cast<LPWSTR> (fileName.c_str()));

  return fileName;
}

WideString const	a2w(AnsiString const &s) {
	return WideString(s.begin(), s.end());
}

WideString const	a2w(char const *s) {
	return WideString(s, s + strlen(s));
}

AnsiString const	w2a(WideString const &s) {
	return AnsiString(s.begin(), s.end());
}

AnsiString const	w2a(wchar_t const     * s) {
	return AnsiString(s, s + wcslen(s));
}

StringPtr asStringPtr(AnsiString const &s) {
  String    * result = new DString(s.c_str());
  return StringPtr(result);
}

StringPtr asStringPtr(WideString const &s) {
  String    * result = new DString(w2a(s).c_str());
  return StringPtr(result);
}

template <typename charT>
std::basic_string<charT> const transformCase(std::basic_string<charT> & s, const charT* (std::ctype<charT>::*functor) (charT* low, const charT* high) const) {
  std::locale               loc;
  std::basic_string<charT>  result (s);
  std::ctype<charT> const & facet = std::use_facet <std::ctype<charT>> (loc);
  charT                   * beg = const_cast<charT*>(result.c_str());
  charT const             * end = beg + result.length();

  (facet.*functor) (beg, end);

  return result;
}

WideString const	toLower(WideString const &s) {
  WideString                result (s);
  return transformCase<wchar_t> (result, (const wchar_t* (std::ctype<wchar_t>::*) (wchar_t* low, const wchar_t* high) const) &std::ctype<wchar_t>::tolower);
}

AnsiString const	toLower(AnsiString const &s) {
  AnsiString                result (s);
  return transformCase<char> (result, (const char* (std::ctype<char>::*) (char* low, const char* high) const) &std::ctype<char>::tolower);
}

WideString const	toUpper(WideString const &s) {
  WideString                result (s);
  return transformCase<wchar_t> (result, (const wchar_t* (std::ctype<wchar_t>::*) (wchar_t* low, const wchar_t* high) const) &std::ctype<wchar_t>::toupper);
}

AnsiString const	toUpper(AnsiString const &s) {
  AnsiString                result (s);
  return transformCase<char> (result, (const char* (std::ctype<char>::*) (char* low, const char* high) const) &std::ctype<char>::toupper);
}

void toUpper(char *s) {
  std::locale               loc;
  std::ctype<char> const  & facet = std::use_facet <std::ctype<char>> (loc);
  facet.toupper (s, s + strlen(s));
}

WideString const	i2s(long i) {
  std::wostringstream  buf;
  buf << i;
  return buf.str();
}

int strcmp(char const * src, wchar_t const * dst) {
  //Program Files (x86)\Microsoft Visual Studio 12.0\VC\crt\src\strcmp.c
  int ret = 0 ;

  while( ! (ret = *(unsigned char *)src - *dst) && *dst)
          ++src, ++dst;

  if ( ret < 0 )
          ret = -1 ;
  else if ( ret > 0 )
          ret = 1 ;

  return( ret );
}    
