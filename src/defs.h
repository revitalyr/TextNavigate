// defs.h

#pragma once

#include <string>
#include <vector>

typedef std::wstring              WideString;
typedef std::string               AnsiString;
typedef std::vector<WideString>   WideStringList;
typedef std::vector<AnsiString>   AnsiStringList;

using string = AnsiString;
