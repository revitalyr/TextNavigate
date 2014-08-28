#ifndef __STRING_HPP
#define __STRING_HPP

#include <windows.h>
#include <stddef.h>

#include "defs.h"

class string
{
  private:
    size_t current_size;
    size_t actual_size;
    size_t hash_start;
    unsigned char *data;
    unsigned char default_data[1];
    HANDLE heap;
    void init(void);
    void copy(const string& Value);
    void copy(const unsigned char *Value);
    void copy(const unsigned char *Value,size_t size);
    bool enlarge(size_t size);
  public:
    string();
    string(const unsigned char *Value);
    string(const unsigned char *Value,size_t size);
    string(const string& Value);
    ~string();
    string &operator=(const string& Value);
    string &operator=(const unsigned char *Value);
    string &operator()(const unsigned char *Value, size_t size);
    operator const unsigned char *() const;
    unsigned char &operator[](size_t index);
    size_t length(void) const;
    //void reverse(void);
    unsigned long hash(void);
    void hash(size_t pos);
    unsigned char *get(void);
    void clear(void);
    string& operator+=(unsigned char Value);
    string& operator+=(const char* Value);
};

int operator==(const string& x,const string& y);

#endif /* __STRING_HPP */
