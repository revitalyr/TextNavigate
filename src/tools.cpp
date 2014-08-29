//tools.cpp
#include <sstream>
#include <locale>         // std::locale, std::ctype, std::use_facet

#include "Headers.c/plugin.hpp"

#include "tools.h"
#include "commons.h"

#include<unicode/DString.h>

SCharData CharSet;

void SCharData::SetBit(unsigned char Bit)
{
  int p = Bit/8;
  CArr[p] |= (char)(1 << Bit%8);
}

void SCharData::ClearBit(unsigned char Bit)
{
  int p = Bit/8;
  CArr[p] &= (char)(~(1 << Bit%8));
}

bool SCharData::GetBit(unsigned char Bit) const
{
  int p = (unsigned char)Bit/8;
  return (CArr[p] & (1 << Bit%8))!=0;
}


void AddEngCharsToUnion(void)
{
  //��������� ���������� �����
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

int get_word(WideString &word, WideString const &str, int pos, int sln, int &begin_word_pos)
{
  int i, j;
  for (i = pos; i > 0 && is_char(str[i]); i--);
  if (is_char((UCHAR)str[i])) i--;
  begin_word_pos = i+1;

  for (j = pos; j < sln && is_char((UCHAR)str[j]); j++);
  if (is_char((UCHAR)str[j])) j++;
  j -= i + 1;

  if (j <= 0)
    return false;
  //if (j > MAX_WORD_LENGTH - 1)
  if (j > MAX_PATH - 1)
    return false;
  word = AnsiString(str+i+1, str+j + 1);
  return (j != 0);
} //get_word

//static const int ASIZE = 256;
#define ASIZE 256
static UCHAR qs_bc[ASIZE];

void InitQuickSearch(bool SearchUp, WideString const &substr, int strlen_word, bool casesensitive)
{
  if (SearchUp)
  {
    FillMemory(qs_bc, strlen_word, ASIZE);
    for (int i = 0; i < strlen_word; i++)
      if (qs_bc[(UCHAR)substr[i]] == strlen_word)
        qs_bc[(UCHAR)substr[i]] = i ? (UCHAR)i : (UCHAR)strlen_word;

    if (!casesensitive)
    {
      char buffer[MAX_WORD_LENGTH];
	  strncpy(buffer, substr, MAX_WORD_LENGTH);
      FSF.LStrupr(buffer);
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
	  strncpy(buffer, substr, MAX_WORD_LENGTH);
      FSF.LStrupr(buffer);
      for (int i = 0; i < strlen_word; i++)
        qs_bc[(UCHAR)buffer[i]] = (UCHAR)(strlen_word - i);
    }
  }
} //InitQuickSearch

int QuickSearch_FW(WideString const & string, const char* substr, int n, int m, int begin_word_pos, bool SearchSelection, bool casesensitive)
{
  int i;

  i = begin_word_pos; bool res;
  while (i <= n - m)
  {
    if (casesensitive)
      res = memcmp(&String[i], substr, m) != 0;
    else
      res = FSF.LStrnicmp(&String[i], substr, m) != 0;

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

int QuickSearch_BW(WideString const & string, WideString const & substr, int n, int m, bool SearchSelection, bool casesensitive)
{
  int i;

  i = n - m; bool res;
  while (i >= 0)
  {
    if (casesensitive)
      res = memcmp(string.data() + i, substr.data(), m) != 0;
    else
      res = FSF.LStrnicmp(string.data() + i, substr, m) != 0;

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

bool GetFile(char const * FileName, char*& buffer, UINT& sz)
{
  buffer = NULL;
  HANDLE s = CreateFile(a2w(FileName).c_str(), GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
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
  if (!Info.EditorControl(ECTL_GETINFO, &EInfo))
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
  Info.EditorControl(ECTL_SETPOSITION, &esp);

  if (y != -1)
  {
    esp.CurLine = y;
    esp.CurPos = x;

    esp.TopScreenLine = -1;
    esp.LeftPos = -1;
    esp.CurTabPos = -1;

    Info.EditorControl(ECTL_SETPOSITION, &esp);
  }
  Info.EditorControl(ECTL_REDRAW, NULL);
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

PSgmlEl GetChild(PSgmlEl parent, const char* Name)
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

WideString const	getEditorFilename (PluginStartupInfo const & info) {
  size_t      fileNameSize = info.EditorControl(-1,ECTL_GETFILENAME,0,0);
  WideString  fileName(fileNameSize + 1, '\0');
        
  Info.EditorControl(-1, ECTL_GETFILENAME, fileNameSize, const_cast<LPWSTR> (fileName.c_str()));

  return fileName;
}

WideString const	a2w(AnsiString const &s) {
	return WideString(s.begin(), s.end());
}

AnsiString const	w2a(WideString const &s) {
	return AnsiString(s.begin(), s.end());
}

StringPtr asStringPtr(AnsiString const &s) {
  String    * result = new DString(s.c_str());
  return StringPtr(result);
}

StringPtr asStringPtr(WideString const &s) {
  String    * result = new DString(w2a(s).c_str());
  return StringPtr(result);
}

WideString const	toLower(WideString const &s) {
  std::locale               loc;
  AnsiString                result (s);
  std::ctype<char> const  & facet = std::use_facet <std::ctype<char>> (loc);
  char                    * beg = const_cast<char*>(result.c_str());
  char const              * end = beg + result.length();

  facet.tolower (beg, end);

  return result;
}

AnsiString const	i2s(long i) {
  std::ostringstream  buf;
  buf << i;
  return buf.str();
}