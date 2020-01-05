/*----------------------------------------------------------------------------*/
/* COPYRIGHT: TriTime (c) 2003/2005 / www.tritime.org                         */
/*----------------------------------------------------------------------------*/
/* PROJEKT  : TTCUT 2005                                                      */
/* FILE     : ttavheader.cpp                                                  */
/*----------------------------------------------------------------------------*/
/* AUTHOR  : b. altendorf (E-Mail: b.altendorf@tritime.de)   DATE: 05/12/2005 */
/* MODIFIED: b. altendorf                                    DATE: 08/13/2005 */
/* MODIFIED:                                                 DATE:            */
/*----------------------------------------------------------------------------*/

// ----------------------------------------------------------------------------
// TTAVHEADER (abstract)
// TTAUDIOHEADER
// TTVIDEOHEADER
// TTSUBTITLEHEADER
// ----------------------------------------------------------------------------

// -----------------------------------------------------------------------------
// Overview
// -----------------------------------------------------------------------------
//
//                               +- TTMpegAudioHeader
//             +- TTAudioHeader -|                  
//             |                 +- TTAC3AudioHeader 
// TTAVHeader -|                 
//             |
//             |                                     +- TTSequenceHeader
//             |                                     |
//             |                                     +- TTSequenceEndHeader
//             +- TTVideoHeader -TTMpeg2VideoHeader -|
//             |                                     +- TTPicturesHeader
//             |                                     |
//             |                                     +- TTGOPHeader
//             |
//             +- TTVideoIndex
//             |
//             +- TTBreakObject
//             |
//             +- TTSubtitleHeader
//
// -----------------------------------------------------------------------------

/*----------------------------------------------------------------------------*/
/* This program is free software; you can redistribute it and/or modify it    */
/* under the terms of the GNU General Public License as published by the Free */
/* Software Foundation;                                                       */
/* either version 2 of the License, or (at your option) any later version.    */
/*                                                                            */
/* This program is distributed in the hope that it will be useful, but WITHOUT*/
/* ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or      */
/* FITNESS FOR A PARTICULAR PURPOSE.                                          */
/* See the GNU General Public License for more details.                       */
/*                                                                            */
/* You should have received a copy of the GNU General Public License along    */
/* with this program; if not, write to the Free Software Foundation,          */
/* Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307, USA.              */
/*----------------------------------------------------------------------------*/

#include "ttavheader.h"

#include <qstring.h>

// /////////////////////////////////////////////////////////////////////////////
// -----------------------------------------------------------------------------
// *** TTAVHeader
// -----------------------------------------------------------------------------
// /////////////////////////////////////////////////////////////////////////////

// default constructor
// -----------------------------------------------------------------------------
TTAVHeader::TTAVHeader()
{
  str_description   = "unknown";
  str_mode          = "unknown";
  str_bit_rate      = "unknown";
  str_sample_rate   = "unknown";
  header_start_code = 0xFF;
  header_offset     = 0;
}

// destructor
// -----------------------------------------------------------------------------
TTAVHeader::~TTAVHeader()
{
  //qDebug("TTAVHeader destructor...");
}

// return header description string
// -----------------------------------------------------------------------------
const QString& TTAVHeader::descString()
{
  return str_description;
}

// return header mode string
// -----------------------------------------------------------------------------
const QString& TTAVHeader::modeString()
{
  return str_mode;
}

// return bit rate string
// -----------------------------------------------------------------------------
const QString& TTAVHeader::bitRateString()
{
  return str_bit_rate;
}

// return sample rate string
// -----------------------------------------------------------------------------
const QString& TTAVHeader::sampleRateString()
{
  return str_sample_rate;
}

// return header type (start code)
// -----------------------------------------------------------------------------
quint8 TTAVHeader::headerType()
{
  return header_start_code;
}

// return header offset in bytes
// -----------------------------------------------------------------------------
quint64 TTAVHeader::headerOffset() const
{
  return header_offset;
}

bool TTAVHeader::operator==(const TTAVHeader& test) const
{
  return (header_offset == test.header_offset);
}


// /////////////////////////////////////////////////////////////////////////////
// -----------------------------------------------------------------------------
// *** TTAudioHeader
// -----------------------------------------------------------------------------
// /////////////////////////////////////////////////////////////////////////////

// default constructor
// -----------------------------------------------------------------------------
TTAudioHeader::TTAudioHeader()
{
  position       = 0;
  frame_time     = 0.0;
  abs_frame_time = 0.0;
  frame_length   = 0;
}

// -----------------------------------------------------------------------------
// methods common for all audio header objects
// -----------------------------------------------------------------------------

// return bit rate
// -----------------------------------------------------------------------------
int TTAudioHeader::bitRate()
{
  return 0;
}

// return sample rate
// -----------------------------------------------------------------------------
int TTAudioHeader::sampleRate()
{
  return 0;
}


double TTAudioHeader::absFrameEndTime()
{
  return 0.0;
}


int TTAudioHeader::compareTo()
{
  return 0;
}

int TTAudioHeader::frameLength()
{
  return frame_length;
}


// /////////////////////////////////////////////////////////////////////////////
// -----------------------------------------------------------------------------
// *** TTVideoHeader
// -----------------------------------------------------------------------------
// /////////////////////////////////////////////////////////////////////////////

// default constructor
// -----------------------------------------------------------------------------
TTVideoHeader::TTVideoHeader()
{
}

// /////////////////////////////////////////////////////////////////////////////
// -----------------------------------------------------------------------------
// *** TTVideoIndex
// -----------------------------------------------------------------------------
// /////////////////////////////////////////////////////////////////////////////
void TTVideoIndex::setDisplayOrder(int value)
{
  display_order = value;
}

int TTVideoIndex::getDisplayOrder()
{
  return display_order;
}

void TTVideoIndex::setHeaderListIndex(int value)
{
  header_list_index = value;
}

int TTVideoIndex::getHeaderListIndex()
{
  return header_list_index;
}

void TTVideoIndex::setPictureCodingType(int value)
{
  picture_coding_type = value;
}

int TTVideoIndex::getPictureCodingType()
{
  return picture_coding_type;
}

// /////////////////////////////////////////////////////////////////////////////
// -----------------------------------------------------------------------------
// *** TTBreakObject
// -----------------------------------------------------------------------------
// /////////////////////////////////////////////////////////////////////////////
TTBreakObject::TTBreakObject()
{
  stop_object          = (TTVideoHeader*)NULL;
  restart_object       = (TTVideoHeader*)NULL;
  stop_object_index    = -1;
  restart_object_index = -1;
}

TTBreakObject::~TTBreakObject()
{
}

void TTBreakObject::setStopObject( TTVideoHeader* stop, int index )
{
  stop_object = stop;
  stop_object_index = index;
}


void TTBreakObject::setRestartObject( TTVideoHeader* restart, int index )
{
  restart_object = restart;
  restart_object_index = index;
}


TTVideoHeader* TTBreakObject::stopObject()
{
  return stop_object;
}


TTVideoHeader* TTBreakObject::restartObject()
{
  return restart_object;
}


int TTBreakObject::stopObjectIndex()
{
  return stop_object_index;
}


int TTBreakObject::restartObjectIndex()
{
  return restart_object_index;
}

// /////////////////////////////////////////////////////////////////////////////
// -----------------------------------------------------------------------------
// *** TTSubtitleHeader
// -----------------------------------------------------------------------------
// /////////////////////////////////////////////////////////////////////////////
TTSubtitleHeader::TTSubtitleHeader()
{
  mText = "";
  mStartMSec = 0;
  mEndMSec = 0;
}

QString TTSubtitleHeader::text()
{
  return mText;
}

void TTSubtitleHeader::setText(QString text)
{
  mText = text;
}

QTime TTSubtitleHeader::startTime()
{
  return QTime::fromMSecsSinceStartOfDay(mStartMSec);
}

int TTSubtitleHeader::startMSec()
{
  return mStartMSec;
}

void TTSubtitleHeader::setStartTime(QTime start)
{
  mStartMSec = start.msecsSinceStartOfDay();
}

void TTSubtitleHeader::setStartTime(int mSec)
{
  mStartMSec = mSec;
}

QTime TTSubtitleHeader::endTime()
{
  return QTime::fromMSecsSinceStartOfDay(mEndMSec);
}

int TTSubtitleHeader::endMSec()
{
  return mEndMSec;
}

void TTSubtitleHeader::setEndTime(QTime end)
{
  mEndMSec = end.msecsSinceStartOfDay();
}

void TTSubtitleHeader::setEndTime(int mSec)
{
  mEndMSec = mSec;
}

