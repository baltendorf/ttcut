/*----------------------------------------------------------------------------*/
/* COPYRIGHT: TriTime (c) 2003/2005/2010 / www.tritime.org                    */
/*----------------------------------------------------------------------------*/
/* PROJEKT  : TTCUT 2005                                                      */
/* FILE     : ttavheader.h                                                    */
/*----------------------------------------------------------------------------*/
/* AUTHOR  : b. altendorf (E-Mail: b.altendorf@tritime.de)   DATE: 05/12/2005 */
/* MODIFIED: b. altendorf                                    DATE: 05/08/2008 */
/* MODIFIED:                                                 DATE:            */
/*----------------------------------------------------------------------------*/

// ----------------------------------------------------------------------------
// TTAVHEADER (abstract)
// TTAUDIOHEADER
// TTVIDEOHEADER
// TTVIDEOINDEX
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

#ifndef TTAVHEADER_H
#define TTAVHEADER_H

#include "ttcommon.h"
#include "common/ttmessagelogger.h"
#include "ttfilebuffer.h"

class QString;

// -----------------------------------------------------------------------------
// *** TTAVHeader: Base class for all header objects
// -----------------------------------------------------------------------------
class TTAVHeader
{
public:
  TTAVHeader();
  virtual ~TTAVHeader();

  virtual const QString& descString();
  virtual const QString& modeString();
  virtual const QString& bitRateString();
  virtual const QString& sampleRateString();

  virtual quint8   headerType();
  virtual void     setHeaderType( quint8 h_type ) {header_start_code = h_type;}
  virtual quint64  headerOffset() const;
  virtual void     setHeaderOffset( quint64 h_offset ){header_offset = h_offset;}

  virtual bool operator==(const TTAVHeader& test) const;

protected:
  quint64          header_offset;
  quint8           header_start_code;
  QString          str_description;
  QString          str_mode;
  QString          str_bit_rate;
  QString          str_sample_rate;
  TTMessageLogger* log;
};


// -----------------------------------------------------------------------------
// *** TTAudioHeader: Base class for all audio header objects
// -----------------------------------------------------------------------------
class TTAudioHeader : public TTAVHeader
{
public:
  TTAudioHeader();

  virtual int     bitRate();
  virtual int     sampleRate();
  virtual double  absFrameEndTime();
  virtual int     compareTo();
  virtual int     frameLength();

  //protected:
  long   position;  // header offset ???
  float  frame_time;
  float  abs_frame_time;
  int    frame_length;
  int    bit_rate;
  int    sample_rate;
};

// -----------------------------------------------------------------------------
// *** TTVideoHeader: Base class for all video header objects
// -----------------------------------------------------------------------------
class TTVideoHeader : public TTAVHeader
{
public:
  TTVideoHeader();

  virtual bool readHeader(TTFileBuffer* mpeg2_stream) = 0;
  virtual bool readHeader(TTFileBuffer* mpeg2_stream, quint64 offset) = 0;
  virtual void parseBasicData(quint8* data, int offset=0) = 0;


 protected:
  typedef struct
  {
    bool    drop_frame_flag;
    int     hours;
    int     minutes;
    bool    marker_bit;
    int     seconds;
    int     pictures;
  } TTimeCode;
};

// -----------------------------------------------------------------------------
// TTVideoIndex: Object (data) for the video index list
// -----------------------------------------------------------------------------
class TTVideoIndex
{
 public:
   TTVideoIndex(){};

   void setDisplayOrder(int value);
   int  getDisplayOrder();
   void setHeaderListIndex(int value);
   int  getHeaderListIndex();
   void setPictureCodingType(int value);
   int  getPictureCodingType();

 protected:
   int display_order;
   int header_list_index;
   int picture_coding_type;
};


// -----------------------------------------------------------------------------
// TTBreakObject
// -----------------------------------------------------------------------------
class TTBreakObject
{
 public:
  TTBreakObject();
  ~TTBreakObject();

  void setStopObject( TTVideoHeader* stop, int index=-1 );
  void setRestartObject( TTVideoHeader* restart, int index=-1 );
  TTVideoHeader* stopObject();
  TTVideoHeader* restartObject();
  int stopObjectIndex();
  int restartObjectIndex();

 private:
  TTVideoHeader* stop_object;
  TTVideoHeader* restart_object;
  int stop_object_index;
  int restart_object_index;
};
#endif //TTAVHEADER_H


