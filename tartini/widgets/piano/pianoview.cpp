/***************************************************************************
                          pianoview.cpp  -  description
                             -------------------
    begin                : 24 Mar 2005
    copyright            : (C) 2005 by Philip McLeod
    email                : pmcleod@cs.otago.ac.nz
 
   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2 of the License, or
   (at your option) any later version.
   
   Please read LICENSE.txt for details.
 ***************************************************************************/
#include <QResizeEvent>

#include "pianoview.h"
#include "pianowidget.h"
#include "gdata.h"
#include "channel.h"
#include "musicnotes.h"

PianoView::PianoView( int viewID_, QWidget *parent )
 : ViewWidget( viewID_, parent)
{
  //setCaption("Piano view");

  pianoWidget = new PianoWidget(this);
  pianoWidget->show();

  //make the widget get updated when the view changes
  connect(gdata->view, SIGNAL(onFastUpdate(double)), this, SLOT(changeKey()));
}

PianoView::~PianoView()
{
  delete pianoWidget;
}

void PianoView::resizeEvent(QResizeEvent *)
{
  pianoWidget->resize(size());
}

void PianoView::changeKey()
{
  Channel *active = gdata->getActiveChannel();
  //AnalysisData *data = Channel::getActiveChannelCurrentChunkData();
  if(active) {
    AnalysisData *data = active->dataAtCurrentChunk();
    //if(data && active->isVisibleNote(data->noteIndex)) {
    if(data && active->isVisibleChunk(data)) {
      float pitch = data->pitch;
      //if (note > 0) {
        //pianoWidget->setCurrentNote(noteValue(note), data->volumeValue);
        pianoWidget->setCurrentNote(noteValue(pitch), data->correlation());
      //} else {
      //  pianoWidget->setNoNote();
      //}
    } else {
      pianoWidget->setNoNote();
    }
  } else {
    pianoWidget->setNoNote();
  }
  pianoWidget->update();
}
