/***************************************************************************
                          settings.h  -  description
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
#ifndef SETTINGS_H
#define SETTINGS_H

#include <map>
#include <qstring.h>

class Settings
{

public:
  Settings();
  Settings(QString domain_, QString product_);
  virtual ~Settings() {};

  void init(QString domain_, QString product_);
	
  QString getString(QString group, QString key);
	int getInt(QString group, QString key);
	double getDouble(QString group, QString key);
  bool getBool(QString group, QString key);
  
	void setString(QString group, QString key, QString value);
  void setInt(QString group, QString key, int value);
  void setDouble(QString group, QString key, double value);
  void setBool(QString group, QString key, bool value);

	//void loadDefaults();

	void load(); //loads settings in from disk
  void save(); //saves settings out to disk

  void print();
private:
	std::map<QString, std::map<QString, QString> > settings; /*< A memory version of the settings on disk. Only the Settings form can change these values. */
	//std::map<QString, std::map<QString, QString> > defaults; /*< Default settings */
  
  QString domain;
  QString product;
};

//extern Settings *settings;

#endif 
