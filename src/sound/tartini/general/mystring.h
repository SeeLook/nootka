/***************************************************************************
                          mystring.h  -  description
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
#ifndef MYSTRING_H
#define MYSTRING_H

//return the index of the first occurance of char c in string s.
//or -1 if not found
int find_pos(const char *s, const char c);

//returns a pointer to a copy of the string s
//allocating the memory required
char *copy_string(const char *s);

//works exactly like strcasecmp but also works on windows
int str_case_cmp(const char *s1, const char *s2);

//converts string to lower case
//returns the string
char *to_lower_case(char *s);

//modifies the string, removing any leading or trailing spaces
//returns a pointer to the string
char *strip_space(char *s);

//returns a pointer to the file extention part or NULL
//if no file extention found
const char *getFileExtension(const char *filename);

//returns a pointer to the filename part of a full path name
//or NULL if no filename is found
const char *getFilenamePart(const char *filename);

//retuns a string which contains only the folder name part of the string.
//if no folder name is found it return "./"
//NOTE: The return value is static so the next call to getFolderPart or withTrailingSlash will overwrite it
const char *getFolderPart(const char *filename);

//If filename ends in a slash it is returned
//otherwise a pointer to static string is returned which contains a trailing slash
//NOTE: The return value is static so the next call to getFolderPart or withTrailingSlash will overwrite it
const char *withTrailingSlash(const char *filename);

#endif
