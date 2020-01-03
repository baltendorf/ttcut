/*----------------------------------------------------------------------------*/
/* COPYRIGHT: TriTime (c) 2003/2010 / www.tritime.org                         */
/*----------------------------------------------------------------------------*/
/* PROJEKT  : TTCUT 2008                                                      */
/* FILE     : ttmpeg2videostream.h                                            */
/*----------------------------------------------------------------------------*/
/* AUTHOR  : b. altendorf (E-Mail: b.altendorf@tritime.de)   DATE: 05/12/2005 */
/* MODIFIED: b. altendorf                                    DATE: 06/18/2005 */
/* MODIFIED:                                                 DATE:            */
/*----------------------------------------------------------------------------*/

// ----------------------------------------------------------------------------
// TTMPEG2VIDEOSTREAM
// ----------------------------------------------------------------------------

// -----------------------------------------------------------------------------
// Overview
// -----------------------------------------------------------------------------
//
//                               +- TTMpegAudioStream
//             +- TTAudioStream -|
//             |                 +- TTAC3AudioStream
// TTAVStream -|
//             |
//             +- TTVideoStream - TTMpeg2VideoStream
//             |
//             +- TTSubtitleStream - TTSrtSubtitleStream
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

#ifndef TTMPEG2VIDEOSTREAM_H
#define TTMPEG2VIDEOSTREAM_H

#include "ttavstream.h"
#include "ttmpeg2videoheader.h"
#include "ttaviwriter.h"

#include "common/ttmessagelogger.h"
#include "extern/tttranscode.h"

#include <QString>
#include <QFileInfo>
#include <QStack>

class TTAudioList;
class TTCutList;

// -----------------------------------------------------------------------------
// TTMpeg2VideoStream
// -----------------------------------------------------------------------------
class TTMpeg2VideoStream : public TTVideoStream
{
  public:
    TTMpeg2VideoStream( const QFileInfo &f_info );
    virtual ~TTMpeg2VideoStream();

    void makeSharedCopy( TTMpeg2VideoStream* v_stream );

    virtual int createHeaderList();
    virtual int createIndexList();

    virtual TTAVTypes::AVStreamType streamType() const;

    virtual bool isCutInPoint( int pos );
    virtual bool isCutOutPoint( int pos );

    virtual void cut(int start, int end, TTCutParameter* cp);

    TTVideoHeader* getCutStartObject(int cutInPos, TTCutParameter* cutParams);
    TTVideoHeader* getCutEndObject(int cutOutPos, TTCutParameter* cutParams);
    TTVideoHeader* checkIFrameSequence(int iFramePos, TTCutParameter* cutParams);
    void transferCutObjects(TTVideoHeader* startObject, TTVideoHeader* endObject, TTCutParameter* cutParams);
    void writeSequenceEndHeader();

    void rewriteGOP(quint8* buffer, quint64 absPos, TTGOPHeader* gop, bool closeGOP, TTCutParameter* cr);
    void removeOrphanedBFrames(QStack<TTBreakObject*>* breakObjects, TTVideoHeader* currentObject);
    void rewriteTempRefData(quint8* buffer, TTPicturesHeader* currentPicture, quint64 bufferStartOffset, int tempRefDelta);
    void encodePart( int start, int end, TTCutParameter* cr);

    //Test!
    bool    createHeaderListFromMpeg2();

  protected:
    bool    openStream();
    bool    closeStream();
    bool    createHeaderListFromIdd(TTFileBuffer* iddStream);
    void    writeIDDFile( );
    void    readIDDHeader(TTFileBuffer* iddStream, quint8 iddFileVersion);
    quint64 getByteCount(TTVideoHeader* startObject, TTVideoHeader* endObject);

  protected:
    TTMessageLogger* log;
};

#endif //TTMPEG2VIDEOSTREAM_H
