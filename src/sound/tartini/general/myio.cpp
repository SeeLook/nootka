/***************************************************************************
                          myio.cpp  -  low level io stuff
                             -------------------
    begin                : Unknown
    copyright            : Unknown
    email                : Unknown
 
   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2 of the License, or
   (at your option) any later version.
   
   Please read LICENSE.txt for details.
 ***************************************************************************/
#include <stdio.h>
#include "myio.h"

/* igetw:
 *  Reads a 16 bit word from a file, using intel byte ordering.
 */
int igetw(FILE *f)
{
   int b1, b2;

   if((b1 = fgetc(f)) != EOF)
    if((b2 = fgetc(f)) != EOF)
	  return ((b2 << 8) | b1);

   return EOF;
}



/* igetl:
 *  Reads a 32 bit long from a file, using intel byte ordering.
 */
long igetl(FILE *f)
{
   int b1, b2, b3, b4;

   if((b1 = fgetc(f)) != EOF)
    if((b2 = fgetc(f)) != EOF)
	  if((b3 = fgetc(f)) != EOF)
	   if((b4 = fgetc(f)) != EOF)
	    return (((long)b4 << 24) | ((long)b3 << 16) |
		        ((long)b2 << 8) | (long)b1);

   return EOF;
}



/* iputw:
 *  Writes a 16 bit int to a file, using intel byte ordering.
 */
int iputw(int w, FILE *f)
{
   int b1, b2;

   b1 = (w & 0xFF00) >> 8;
   b2 = w & 0x00FF;

   if (fputc(b2,f)==b2)
      if (fputc(b1,f)==b1)
	 return w;

   return EOF;
}



/* iputw:
 *  Writes a 32 bit long to a file, using intel byte ordering.
 */
long iputl(long l, FILE *f)
{
   int b1, b2, b3, b4;

   b1 = (int)((l & 0xFF000000L) >> 24);
   b2 = (int)((l & 0x00FF0000L) >> 16);
   b3 = (int)((l & 0x0000FF00L) >> 8);
   b4 = (int)l & 0x00FF;

   if(fputc(b4,f)==b4)
    if(fputc(b3,f)==b3)
	  if(fputc(b2,f)==b2)
	   if(fputc(b1,f)==b1)
	    return l;

   return EOF;
}



/* mgetw:
 *  Reads a 16 bit int from a file, using motorola byte-ordering.
 */
int mgetw(FILE *f)
{
   int b1, b2;

   if((b1 = fgetc(f)) != EOF)
    if((b2 = fgetc(f)) != EOF)
	  return ((b1 << 8) | b2);

   return EOF;
}



/* mgetl:
 *  Reads a 32 bit long from a file, using motorola byte-ordering.
 */
long mgetl(FILE *f)
{
   int b1, b2, b3, b4;

   if((b1 = fgetc(f)) != EOF)
    if((b2 = fgetc(f)) != EOF)
	  if((b3 = fgetc(f)) != EOF)
	   if((b4 = fgetc(f)) != EOF)
	    return (((long)b1 << 24) | ((long)b2 << 16) |
		        ((long)b3 << 8) | (long)b4);

   return EOF;
}



/* mputw:
 *  Writes a 16 bit int to a file, using motorola byte-ordering.
 */
int mputw(int w, FILE *f)
{
   int b1, b2;

   b1 = (w & 0xFF00) >> 8;
   b2 = w & 0x00FF;

   if(fputc(b1,f)==b1)
    if(fputc(b2,f)==b2)
	  return w;

   return EOF;
}



/* mputl:
 *  Writes a 32 bit long to a file, using motorola byte-ordering.
 */
long mputl(long l, FILE *f)
{
   int b1, b2, b3, b4;

   b1 = (int)((l & 0xFF000000L) >> 24);
   b2 = (int)((l & 0x00FF0000L) >> 16);
   b3 = (int)((l & 0x0000FF00L) >> 8);
   b4 = (int)l & 0x00FF;

   if(fputc(b1,f)==b1)
    if(fputc(b2,f)==b2)
	  if(fputc(b3,f)==b3)
	   if(fputc(b4,f)==b4)
	    return l;

   return EOF;
}


