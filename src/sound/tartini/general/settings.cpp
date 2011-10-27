/***************************************************************************
                          settings.cpp  -  description
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
#include "settings.h"

#include "myassert.h"

#include <qsettings.h>
#include <qdir.h>

// Load the settings from disk into the map.
Settings::Settings()
{
}

Settings::Settings(QString domain_, QString product_)
{
  init(domain_, product_);
}

void Settings::init(QString domain_, QString product_)
{
  domain = domain_;
  product = product_;
}

// Return the value indexed in the map, or load it in from the defaults if need be
QString Settings::getString(QString group, QString key)
{
  // Preconditions
  myassert( !group.isNull() && !key.isNull() );

  // First try to load it from the map
  std::map<QString, std::map<QString, QString> >::const_iterator iter = settings.find(group);
  if (iter != settings.end()) {
    std::map<QString, QString>::const_iterator iterValue = (iter->second).find(key);
    if (iterValue != (iter->second).end()) {
      return iterValue->second;
    } else {
      fprintf(stderr, "No defined key[%s] in group[%s]. (%s, %s)\n", key.ascii(), group.ascii(), domain.ascii(), product.ascii());
      myassert(false); //The defaults haven't been defined for this key!
      return QString("");
    }
  } else {
    fprintf(stderr, "No defined group[%s], can't get key[%s]. (%s, %s)\n", group.ascii(), key.ascii(), domain.ascii(), product.ascii());
    myassert(false); //The defaults haven't been defined for this key!
    return QString("");
  }

/*
  // If we're here, then the value was not found. Load it in from the defaults.
  std::map<QString, std::map<QString, QString> >::const_iterator defIter = defaults.find(group);
  if (defIter != settings.end()) {
    std::map<QString, QString>::const_iterator defIterValue = (defIter->second).find(key);
    if (defIterValue != (defIter->second).end()) {
      QString value = defIterValue->second;
      (settings[group])[key] = defIterValue->second;
    } else {
      fprintf(stderr, "Default setting for group[%s], key[%s] does not exist!\n", group.ascii(), key.ascii());
      myassert(false); //The defaults haven't been defined for this key!
      return QString("");
    }
  }
  return settings[group][key];
*/
	
}

// Use getString, convert to an int
int Settings::getInt(QString group, QString key)
{
  // Preconditions
  myassert( !group.isNull() && !key.isNull() );

  bool ok = false;
  int toReturn = getString(group, key).toInt(&ok);

  myassert(ok);
  return toReturn;
}

// Use getString, convert to a double
double Settings::getDouble(QString group, QString key)
{
  // Preconditions
  myassert( !group.isNull() && !key.isNull() );

  bool ok = false;
  double toReturn = getString(group, key).toDouble(&ok);

  myassert(ok);
  return toReturn;
}

// Use getString, convert to a bool
bool Settings::getBool(QString group, QString key)
{
  // Preconditions
  myassert( !group.isNull() && !key.isNull() );

  if(getString(group, key).at(0).lower() == QChar('t')) return true;
  else return false;
}

void Settings::setString(QString group, QString key, QString value)
{
  // Preconditions
  myassert( !group.isNull() && !key.isNull() && !value.isNull());

  settings[group][key] = value;

  // Postconditions
  myassert((settings[group])[key] == value);
}

void Settings::setInt(QString group, QString key, int value)
{
  QString s;
  setString(group, key, s.setNum(value));
}

void Settings::setDouble(QString group, QString key, double value)
{
  QString s;
  setString(group, key, s.setNum(value));
}

void Settings::setBool(QString group, QString key, bool value)
{
  setString(group, key, (value) ? "true" : "false");
}

void Settings::load()
{
  myassert(domain != "" && product != "");
  
//#ifdef MACX
//  QSettings onDiskSettings(QSettings::Ini);
//#else
//  QSettings onDiskSettings(QSettings::Native);
//#endif
  QSettings onDiskSettings(domain, product);

  //onDiskSettings.setPath(domain, product, QSettings::UserScope);
  onDiskSettings.beginGroup(QString("/") + product);
  //onDiskSettings.beginGroup(QString("/General"));
  
  /* QT is the stupidest thing in the world. Instead of being able to use the 
   * subkeylist function, which is meant to do _exactly_ what I want, we can't 
   * use it in anything but Windows. So, we have to iterate through our 
   * defaults map and use the first key as a category.
   */
  QStringList subkeys;
  std::map<QString, std::map<QString, QString> >::const_iterator iter = settings.begin();
  while (!(iter == settings.end())) {
    subkeys += iter->first;
    iter++;
  }
  
  // Get all the entries in the folders, and stick them in the map
  QStringList entries;
  QString key;
  for (int i = 0; i < subkeys.size(); i++) {
    entries = onDiskSettings.entryList("/" + *(subkeys.at(i)));
    //printf("There are %d entries in category %s.\n", entries.size(), (*subkeys.at(i)).data());
    for (int n = 0; n < entries.size(); n++) {
      //key = QString("/") + *(subkeys.at(i)) + "/" + *(entries.at(n));
      key = "/" + subkeys.at(i) + "/" + entries.at(n);
      //(settings[*(subkeys.at(i))])[*(entries.at(n))] = onDiskSettings.readEntry(key, "");
      (settings[subkeys.at(i)])[entries.at(n)] = onDiskSettings.readEntry(key, "");
    }
  }

  onDiskSettings.endGroup();
}

void Settings::save()
{
  myassert(domain != "" && product != "");
  // Save settings to www.cs.otago.ac.nz/Pitch/Pitch/_GroupName_/_Key_/
  // Needs to be like this so it works properly in Unix.

//#ifdef MACX
//  QSettings onDiskSettings(QSettings::Ini);
//#else
//  QSettings onDiskSettings(QSettings::Native);
//#endif

  QSettings onDiskSettings(domain, product);

  //onDiskSettings.setPath(domain, product, QSettings::UserScope);
  onDiskSettings.beginGroup(QString("/") + product);

  std::map<QString, std::map<QString, QString> >::const_iterator iter = settings.begin();
  while (iter != settings.end()) {
    onDiskSettings.beginGroup(iter->first);
    //qDebug(onDiskSettings.group());
    std::map<QString, QString>::const_iterator iterValue = iter->second.begin();
    while (iterValue != iter->second.end()) {
      onDiskSettings.writeEntry("/" + iterValue->first, iterValue->second);
		  //qDebug("Wrote /" + iterValue->first + "= " + iterValue->second);
      iterValue++;
    }
    onDiskSettings.endGroup();
    iter++;
  }
  onDiskSettings.endGroup();
}

void Settings::print()
{
  std::map<QString, std::map<QString, QString> >::const_iterator iter = settings.begin();
  for(; iter != settings.end(); iter++) {
    std::map<QString, QString>::const_iterator iterValue = (iter->second).begin();
    for(; iterValue != (iter->second).end(); iterValue++) {
      printf("%s/%s=%s\n", iter->first.latin1(), iterValue->first.latin1(), iterValue->second.latin1());
    }
  }
  fflush(stdout);
}
