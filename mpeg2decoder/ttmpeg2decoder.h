/*----------------------------------------------------------------------------*/
/* COPYRIGHT: TriTime (c) 2003/2010 / www.tritime.org                         */
/*----------------------------------------------------------------------------*/
/* PROJEKT  : TTCUT 2008                                                     */
/* FILE     : ttmpeg2decoder.h                                                */
/*----------------------------------------------------------------------------*/
/* AUTHOR  : b. altendorf (E-Mail: b.altendorf@tritime.de)   DATE: 02/23/2005 */
/* MODIFIED: b. altendorf                                    DATE: 03/01/2005 */
/* MODIFIED: b. altendorf                                    DATE: 03/31/2005 */
/* MODIFIED: b. altendorf                                    DATE: 04/30/2005 */
/*----------------------------------------------------------------------------*/

// ----------------------------------------------------------------------------
// TTMPEG2DECODER
// ----------------------------------------------------------------------------

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

#ifndef TTMPEG2DECODER_H
#define TTMPEG2DECODER_H

#include "../common/ttcut.h"

// standard C header files
#include <math.h>
#include <stdio.h>
#include <stdlib.h>

// libmpeg2-dec header files
extern "C"
{
#include <mpeg2dec/mpeg2.h>
#include <mpeg2dec/mpeg2convert.h>
}

// Qt header files
#include <qstring.h>

#include "../avstream/ttavheader.h"
#include "../avstream/ttvideoheaderlist.h"
#include "../avstream/ttvideoindexlist.h"
#include "../avstream/ttmpeg2videoheader.h"

/* /////////////////////////////////////////////////////////////////////////////
 * constants for mpeg2 pixel format
 */
enum TPixelFormat
{
  formatRGB24 = 1,  // RGB interleaved; default
  formatRGB32 = 2,
  formatRGB8  = 3,
  formatYV12  = 4,
  formatYUV24 = 5   // YUV planes
};

/* /////////////////////////////////////////////////////////////////////////////
 * Frame info struct
 */
typedef struct
{
  quint8* Y;
  quint8* U;
  quint8* V;
  int      width;
  int      height;
  int      size;
  int      type;
  int      chroma_width;
  int      chroma_height;
  int      chroma_size;
} TFrameInfo;

/* /////////////////////////////////////////////////////////////////////////////
 * Sequence properties struct
 */
typedef struct
{
  int          width;
  int          height;
  unsigned int byte_rate;
  unsigned int vbv_buffer_size;
  unsigned int frame_period;
} TSequenceInfo;

/* /////////////////////////////////////////////////////////////////////////////
 * Stream properties struct
 */
typedef struct
{
  quint64 size;
  quint64 position;
  int     frame;
  int     videoPTS;
} TStreamInfo;

/* /////////////////////////////////////////////////////////////////////////////
 * Initial buffer size
 */
const int initialStreamBufferSize  = 65536;
const int initialDecoderBufferSize = 5129;


/* /////////////////////////////////////////////////////////////////////////////
 * TTMpeg2Decoder class declaration
 */
class TTMpeg2Decoder
{
 public:
  TTMpeg2Decoder(QString cFName,
                 TTVideoIndexList* viIndex, TTVideoHeaderList* viHeader,
                 TPixelFormat pixelFormat=formatRGB32);
  ~TTMpeg2Decoder();

  void        openMPEG2File(QString cFName);
  quint64     fileSize();
  QString     fileName();
  int         moveToFrameIndex(int iFramePos);
  TFrameInfo* decodeFirstMPEG2Frame(TPixelFormat pixelFormat=formatRGB32);
  TFrameInfo* decodeMPEG2Frame(TPixelFormat pixelFormat=formatRGB32);
  int         gotoNextFrame();
  void        getCurrentFrameData(quint8* data);
  TFrameInfo* getFrameInfo();

  int desiredFrameType;
  int desiredFramePos;

 protected:
  void initDecoder(TPixelFormat pixelFormat=formatRGB32);
  int  decodeNextFrame();
  int  skipFrames(int count);
  int  seek(quint64 seqHeaderOffset);

private:
  QFile*              mpeg2Stream;
  mpeg2dec_t*         mpeg2Decoder;
  mpeg2_state_t       state;
  quint8*             streamBuffer;
  quint8*             decoderBuffer;
  int                 streamBufferSize;
  int                 decoderBufferSize;
  const mpeg2_info_t* mpeg2Info;
  quint8*             sliceData;
  TTVideoHeaderList*  videoHeaderList;
  TTVideoIndexList*   videoIndexList;
  TPixelFormat        convType;
  TFrameInfo*         t_frame_info;
};

/* /////////////////////////////////////////////////////////////////////////////
 * Decoder exception class
 */
class TTMpeg2DecoderException
{
  public:
    enum ExceptionType
    {
      ArgumentNull,
      DecoderInit,
      StreamOpen
    };

    TTMpeg2DecoderException(ExceptionType type);

    QString message();

  protected:
    ExceptionType ex_type;
};
#endif //TTMPEG2DECODER_H

