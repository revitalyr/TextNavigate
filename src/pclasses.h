//pclasses.h
#ifndef __PCLASSES_H
#define __PCLASSES_H

#define WIN32_LEAN_AND_MEAN
#include <windows.h>

#include <list>

//#include "crtdll.h"
#include "commons.h"
#include "string.hpp"
#include "Dialog.h"
#include "sgml.h"
#include "cregexp/CRegExp.h"
#include "Dialog.h"
#include "tools.h"
#include "TextNavigate.h"
#include "reg.h"

typedef struct _FOUND
{
  unsigned int  path_index;
  WideString    file_name;
  WideString    file_path;
} FOUND, *LPFND;

//#define MAX_ENV_VARS 16
struct ENV_VAR
{
  WideString    EnvVar;
  WideString    EnvVarValue;
};

using ENV_VARS = std::list<ENV_VAR>;

#define PWL 0x400
#define MAX_PATHWAYS_COUNT 0x100   //кол-во pathways
#define MAX_FOUND_COUNT 50         //кол-во wasfound

#define MAX_WORD_LENGTH 100
#define MAX_INCREMENTAL_SEARCH_LENGTH 20

//forward declarations
struct SLanguage;
class CXMLFile;

/*******************************************************************************
              class CFoundDataArray
*******************************************************************************/

class CFoundDataArray : public CRefArray<_FOUND>
{
  private:
    int m_count;
  public:
    CFoundDataArray(int Capacity) : CRefArray<_FOUND>(Capacity), m_count(0)
    { }

    ~CFoundDataArray()
    {
      cleanup();
    }

    void insert(WideString const &FileName, int index)
    {
      _FOUND* item = Item(m_count++);
      item->file_name = FileName;
      item = Item(index);
      item->path_index = index;
      item->file_path.clear();
      item->path_index = -1;
    }

    void cleanup()
    {
      for (int i = 0; i < m_count; i++)
      {
        Item(i)->file_name.clear();
        Item(i)->file_path.clear();
      }
      m_count = 0;
    }

    int count()
    {
      return m_count;
    }
}; //CFoundDataArray

/*******************************************************************************
              class CPathwaysArray
*******************************************************************************/

class CPathwaysArray
{
  private:
    WideStringList  str_array;
  public:
    WideString const & operator[](int index) const {
      return str_array [index];
    }

    void insert (int index, WideString const & str) {
      str_array [index] = str;
    }

    void add (WideString const & str) {
      insert (str_array.size(), str);
    }

    WideString const & get (int index) const {
      return operator[](index);
    }

    void _delete (int index) {
      str_array [index] = L"";
    }

    int count () {
      return str_array.size();
    }

    void cleanup () {
      str_array.clear();
    }
}; //CPathwaysArray

/*******************************************************************************
              class CSearchPaths
*******************************************************************************/

class CSearchPaths
{
  private:
    WideString        s_PathWays;
    WideString        file_name;
    WideString        last_path;
    CPathwaysArray    pathways;
    CFoundDataArray   was_found;
    WideString        find_file_name; //указатель на имя
    WideString        find_file_path; //указатель на путь
    ENV_VARS          env_vars;
    int               EnvVarsCount;

    WideString const get_def_path();
    void ResolveEnvVars();
    int AlreadyFound(WideString const &path, WideString const &file_name);
    int get_full_file_name(WideString & dest, LPFND fnd); //!!!

    void free_find_data();
    void PrepareForSearch();
    void MakePathWays();
    void FindAllSubDirs(WideString const &RootDir);
 public:
   CSearchPaths(PSgmlEl elem);
   ~CSearchPaths();

   bool ProcessCtrlEnter(SLanguage* Language);

   int get_filename();
   int find_file(wchar_t const *ExcludedFileExts);
   int get_full_file_name(WideString & dest, int pos);
   void cleanup();
   void ClearAfterSearch();

   void AddSearchPaths(PSgmlEl elem);
   int ShowSelectFileMenu();
}; //CSearchPaths

/*******************************************************************************
              struct SMethod
*******************************************************************************/

struct SMethod
{
  SMethod(PSgmlEl elem, WideString const & Tag);
  ~SMethod();

  WideString    Name;
  WideString    Type;
  WideString    Definition;
  WideString    Implementation;
}; //SMethod

/*******************************************************************************
              struct SClass
*******************************************************************************/

struct SClass
{
  SClass(PSgmlEl elem);
  ~SClass();

  WideString    Name;
  WideString    Definition;
  WideString    End;
  SMethod     * Method;
}; //SClass

/*******************************************************************************
              struct SLanguage
*******************************************************************************/

struct SLanguage
{
  SLanguage(CXMLFile* owner, PSgmlEl elem = NULL);
  ~SLanguage();

  WideString    name;
  WideString    FileExts;
  WideString    ExcludedFileExts;
  WideString    SourceFiles;
  WideString    Headers;
  SClass      * Class;
  SMethod     * Method;
  CXMLFile    * Owner;
}; //SLanguage

/*******************************************************************************
              class CXMLFile
*******************************************************************************/

class CXMLFile
{
  private:
    WideString    XMLFileName;
    WideString    filepath;
  public:
    SLanguage*    Language;
    CSearchPaths  SearchPaths;

  private:
    PSgmlEl searchbasetag(PSgmlEl base);
    bool loaddata(char *data, int len);
    bool includefile(char const *param);
    void readcdata(PSgmlEl basetag);
  public:
    CXMLFile();
    CXMLFile(WideString const &xml_file_name);
    ~CXMLFile();
    void init(WideString const &file_name);
    void AddSearchPaths(PSgmlEl elem);
}; //CXMLFile

/*******************************************************************************
              class CSortedRefColl
  collection sorted by id
*******************************************************************************/

template<class T, bool DeleteEqual = false>
class CSortedRefColl : public CRefColl<T>
{
  typedef CRefColl<T> ancestor;

  protected:
    virtual int compare_refs(const T* ref1, const T* ref2)
    { return 0; }
    virtual void replaceby(T* ref_to_replace, const T* ref)
    { }

  public:
    explicit CSortedRefColl() : ancestor(true)
    { }

    T *Add(int fid = -1, T *&ref = (T *)NULL)
    {
      ancestor *pos = NULL;
      ancestor *fl = next;
      while (fl != this)
      {
        if (fl->id() <= fid && (fl->next->id() > fid || fl->next->main()))
        {
          pos = fl;
          break;
        }
        fl = fl->next;
      }
      fl = pos ? pos : this;
      if (fl->id() == fid && (0 == compare_refs(fl->Ref, ref)))
      {
        if (!DeleteEqual)
        {
          replaceby(fl->Ref, ref);
          delete ref;
          ref = NULL;
          return fl->Ref;
        }
        else
        {
          delete fl;
          return ref;
        }
      }
      else
        return _insert_after(fl, fid, ref);
    }
}; //CSortedRefColl

/*******************************************************************************
              class CPositionsRefColl
  sorted collection of cursor positions
*******************************************************************************/

class CPositionsRefColl : public CSortedRefColl<EditorInfo, true>
{
  typedef CSortedRefColl<EditorInfo, true> ancestor;

  private:
    char const * get_str_repr(const EditorInfo* pEditorInfo);

  protected:
    int compare_refs(const EditorInfo* ref1, const EditorInfo* ref2);
    void replaceby(EditorInfo* ref_to_replace, const EditorInfo* ref);

  public:
    CPositionsRefColl() : ancestor()
    { }

    CRefColl<EditorInfo>* GetNextTo(int row, int pos);
    CRefColl<EditorInfo>* GetPrevTo(int row, int pos);

    char* GetStringRepresentation();
}; //CPositionsRefColl

/*******************************************************************************
              class CXMLFilesColl
  collection of XML files
*******************************************************************************/

//class CXMLFilesColl : public CRefColl<CXMLFile>
//{
//  public:
//    CXMLFilesColl(CXMLFile* XMLFile = NULL) : CRefColl<CXMLFile>(true, XMLFile)
//    { }
//}; //CXMLFilesColl

struct CXMLFilesColl : public CCollection<CXMLFile> {
};
/*******************************************************************************
              class CBookmarksStack
  stacked bookmarks
*******************************************************************************/
class CBookmarksStack : public CStack<EditorInfo>
{
  public:
    CBookmarksStack() : CStack<EditorInfo>()
    { }
   
}; //CBookmarksStack

/*******************************************************************************
              class TWindowData
  holds editor window info
*******************************************************************************/
class TWindowData
{
private:
  int eid; //editor id
  string FullFileName;
  AutoPtr<CPositionsRefColl> FPositionsColl; //standard bookmarks
  AutoPtr<CBookmarksStack> FBookmarksStack; //stacked bookmarks
public:
  TWindowData(int id, char* FileName);
  ~TWindowData();
  void clear(bool RegistryAlso);
  
  void AddBookmark(int CurLine, int CurPos, int TopScreenLine, int LeftPos);
  void MoveToNextBookmark(bool Next);
  void LoadBookmarks();
  void SaveBookmarks();

  void PushBookmark(int CurLine, int CurPos, int TopScreenLine, int LeftPos);
  void PopBookmark();
};

//class CWindowsColl : public CRefColl<TWindowData>
//{
//  public:
//    CWindowsColl(TWindowData* window_data = NULL)
//    : CRefColl<TWindowData>(true, window_data)
//    { }
//};

struct CWindowsColl : public CCollection<TWindowData*> {
};

/*******************************************************************************
              class CTextNavigate
*******************************************************************************/

enum TEditorState {
  esStateNormal,
  esCtrlKPressed,
  esCtrlIPressed
};

class CTextNavigate
{
  private:
    CRegExp         reg;
    int             FKeyCode;
    int             FPrevKeyCode;
    TEditorState    FEditorState;
    wchar_t         FIncrementalSearchBuffer[MAX_INCREMENTAL_SEARCH_LENGTH];
    wchar_t       * FIncrementalSearchBufferEnd;

    CXMLFile        XMLFile;
    CXMLFilesColl   XMLFilesColl;
    CWindowsColl    windows;

    WideString      word;

    WideString      MethodType;
    WideString      MethodName;
    WideString      ClassName;

    bool            IsHeader;
    bool            GlobalProc;
    WideString      MethodDefinition;
    WideString      ClassDefinition;
    WideString      MethodImplementation;
    int             MethodNamePos;

    SClass        * Class;
    SMethod       * Method;

    int strreplace(WideString & str, WideString const  &pattern, WideString const & value);
    int GetMatch(WideString & Match, const SMatches &m, WideString const & str, int n);
    void DrawTitle();

    int do_search(WideString const & string, WideString const & substr, bool SearchUp, int begin_word_pos, int &CurLine, bool SearchSelection, bool casesensitive);

    bool SearchBackward(WideString const & RegExpr, int &CurLine, WideString & StringText, SMatches &m);
    bool SearchForward(WideString const & RegExpr, int &CurLine, WideString & StringText, SMatches &m);
    bool IsInClassDefinition(int CurLine);
    bool IsGlobalMethodImplementation(int CurLine);
    int SearchForMethodImplementation(int &CurLine, int GlobalProc);
    bool SearchForMethodImplementation2(int &CurLine, int &x_pos);
    bool SearchForMethodDefinition2(int &CurLine, bool GlobalProc);
    bool SearchForMethodDefinition(int &CurLine, int &x_pos, bool GlobalProc);
    bool FindMethodDefinition(int &CurLine, int &x_pos);
    bool FindMethodImplementation(int &CurLine, int &x_pos);

    void MoveToNextBookmark(bool Next);
    void SaveBookmarks(int id);
    void LoadBookmarks(int id);
    
    void SelectFound(int StringNumber, int StartPos, int Len);

  public:
    CTextNavigate();
    ~CTextNavigate();

    int ProcessEditorInput(const INPUT_RECORD *Rec);
    int ProcessEditorEvent(int Event, void *Param);

    int processCtrlAltUpDown(int FKeyCode, int FPrevKeyCode);
    int processCtrlEnter(void);
    int processCtrlShiftUpDown(int FKeyCode);

    void AddBookmark();
    void ClearBookmarks(int id, bool RegistryAlso);

    void StartIncrementalSearch();

    void Push();
    void Pop();
    
    int ShowPluginMenu();
    void config_plugin();
};

struct FFarMenuItem
{
  WideString  Text;
  int         Selected;
  int         Checked;
  int         Separator;
};

typedef struct FFarMenuItem* PFFarMenuItem;
typedef struct FarMenuItem* PFarMenuItem;
typedef class CPluginMenu* PPluginMenu;

/*******************************************************************************
              class CPluginMenu
*******************************************************************************/

class CPluginMenu : public CRefArray<FarMenuItem>
{
 public:
  CPluginMenu(int NumElements, PFFarMenuItem Elements, int FNumElements = -1);
  FarMenuItem *Setup(int num, const FFarMenuItem &mi);
  void Setup(int From, PFFarMenuItem p, int count);
  int Execute(WideString const &Title, WideString const &Bottom);
};

extern CTextNavigate* TextNavigate;

#endif /* __PCLASSES_H */
