/***************************************************************************
                          mystring.cpp  -  description
                             -------------------
    begin                : 2004
    copyright            : (C) 2004-2005 by Philip McLeod
    email                : pmcleod@cs.otago.ac.nz
 
   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2 of the License, or
   (at your option) any later version.
   
   Please read LICENSE.txt for details.
 ***************************************************************************/
#include <qglobal.h>
#include "mystring.h"
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#ifdef WINDOWS
#include <string.h>
#else
#include <strings.h>
#endif

//return the index of the first occurance of char c in string s.
//or -1 if not found
int find_pos(const char *s, const char c)
{
    int len = strlen(s);
    for(int j=0; j<len; j++)
	if(s[j] == c) return j;
    return -1;
}

//returns a pointer to a copy of the string s
//allocating the memory required
char *copy_string(const char *s)
{
    if(!s) return NULL;
    char *t = (char *)malloc(strlen(s) + 1);
    strcpy(t, s);
    return t;
}

int str_case_cmp(const char *s1, const char *s2)
{
#ifdef WINDOWS
	return _stricmp(s1, s2);
#else
	return strcasecmp(s1, s2);
#endif
}

//converts string to lower case
//returns the string
char *to_lower_case(char *s)
{
    for(char *a=s; *a!='\0'; a++) *a = tolower(*a);
    return s;
}

//modifies the string, removing any leading or trailing spaces
//returns a pointer to the string
char *strip_space(char *s)
{
    if(s[0] == ' ') {
	int pos = 0;
	while(s[pos] == ' ') pos++;
	memmove(s, s+pos, strlen(s)+1-pos);
    }
    int len = strlen(s);
    for(int j=len-1; j>=0; j--)
	if(s[j] == ' ') s[j] = '\0';
	else break;
    return s;
}

//returns a pointer to the file extention part or NULL
//if no file extention found
const char *getFileExtension(const char *filename)
{
  const char *ext;
  if(filename && (ext = strrchr(filename, '.')) != NULL) return ext+1;
  else return NULL;
}

//returns a pointer to the filename part of a full path name
//or NULL if no filename is found
const char *getFilenamePart(const char *filename)
{
  const char *ext;
  if(!filename) return NULL;
  if((ext = strrchr(filename, '/')) != NULL) return ext+1;
  if((ext = strrchr(filename, '\\')) != NULL) return ext+1;
  return filename;
}

//retuns a malloced string which contains only the folder name part of the string.
//if no folder name is found it return "."
//NOTE: The value return needs to be freed with free when done with
/*
char *getFolderPart(char *filename)
{
    char *folder;
    if(!filename) {
	folder = (char *)malloc(sizeof(char) * 2);
	folder[0] = '.';
	folder[1] = '\0';
	return folder;
    }
    int len = strlen(filename);
    for(; len > 0; len--) {
	if(filename[len-1] == '/' || filename[len-1] == '\\') break;
    }
    if(len == 0) {
	folder = (char *)malloc(sizeof(char) * 2);
	folder[0] = '.';
	folder[1] = '\0';
    } else {
	folder = (char *)malloc(sizeof(char) * (len + 1));
	memcpy(folder, filename, sizeof(char) * len);
	folder[len] = '\0';
    }
    return folder;
}
*/

//a static string used by getFolderPart and withTrailingSlash
char mystring_folder[2048];

//retuns a string which contains only the folder name part of the string.
//if no folder name is found it return "./"
//NOTE: The return value is static so the next call to getFolderPart or withTrailingSlash will overwrite it
const char *getFolderPart(const char *filename)
{
  //static char mystring_folder[2048];
  if(!filename) {
    strcpy(mystring_folder, "./");
    return mystring_folder;
  }
  int len = strlen(filename);
  for(; len > 0; len--) {
	  if(filename[len-1] == '/' || filename[len-1] == '\\') break;
  }
  if(len == 0) {
	  strcpy(mystring_folder, "./");
  } else {
	  if(len > 2047) len = 2047;
	  memcpy(mystring_folder, filename, sizeof(char) * len);
	  mystring_folder[len] = '\0';
  }
  return mystring_folder;
}

//If filename ends in a slash it is returned
//otherwise a pointer to static string is returned which contains a trailing slash
//NOTE: The return value is static so the next call to getFolderPart or withTrailingSlash will overwrite it
const char *withTrailingSlash(const char *filename)
{
  if(!filename) {
    strcpy(mystring_folder, "/");
    return mystring_folder;
  }
  int len = strlen(filename);
  if(filename[len-1] == '/') return filename;
  else {
    if(len > 2046) len = 2046;
	  memcpy(mystring_folder, filename, sizeof(char) * len);
    mystring_folder[len] = '/';
    mystring_folder[len+1] = '\0';
    return mystring_folder;
  }
}
