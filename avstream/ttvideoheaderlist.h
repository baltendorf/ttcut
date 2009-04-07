/*----------------------------------------------------------------------------*/
/* COPYRIGHT: TriTime (c) 2003/2005 / www.tritime.org                         */
/*----------------------------------------------------------------------------*/
/* PROJEKT  : TTCUT 2005                                                      */
/* FILE     : ttvideoheaderlist.h                                             */
/*----------------------------------------------------------------------------*/
/* AUTHOR  : b. altendorf (E-Mail: b.altendorf@tritime.de)   DATE: 05/12/2005 */
/* MODIFIED: b. altendorf                                    DATE: 08/29/2005 */
/* MODIFIED:                                                 DATE:            */
/*----------------------------------------------------------------------------*/

// ----------------------------------------------------------------------------
// TTVIDEOHEADERLIST
// ----------------------------------------------------------------------------

// -----------------------------------------------------------------------------
// Overview
// -----------------------------------------------------------------------------
//
//               +- TTAudioHeaderList 
//               | 
//               +- TTAudioIndexList
// TTHeaderList -|
//               +- TTVideoHeaderList
//               |
//               +- TTVideoIndexList
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

#ifndef TTVIDEOHEADERLIST_H
#define TTVIDEOHEADERLIST_H

#include "ttheaderlist.h"
#include "ttmpeg2videoheader.h"

class TTSequenceHeader;
class TTPicturesHeader;
class TTGOPHeader;

/* /////////////////////////////////////////////////////////////////////////////   
 * TTVideoHeaderList: Pointer list MPEG2 header objects
 */
class TTVideoHeaderList : public TTHeaderList
{
  public:
    TTVideoHeaderList(int size);
    virtual ~TTVideoHeaderList();

    quint8            headerTypeAt(int index);
    TTVideoHeader*    headerAt(int index);
    TTVideoHeader*    getPrevHeader(int startPos, TTMpeg2VideoHeader::mpeg2StartCodes type = TTMpeg2VideoHeader::ndef);
    TTVideoHeader*    getNextHeader(int startPos, TTMpeg2VideoHeader::mpeg2StartCodes type = TTMpeg2VideoHeader::ndef);
    TTVideoHeader*    getPrevHeader(TTVideoHeader* current, TTMpeg2VideoHeader::mpeg2StartCodes type = TTMpeg2VideoHeader::ndef);
    TTVideoHeader*    getNextHeader(TTVideoHeader* current, TTMpeg2VideoHeader::mpeg2StartCodes type = TTMpeg2VideoHeader::ndef);
    TTSequenceHeader* sequenceHeaderAt(int index);
    TTSequenceHeader* firstSequenceHeader();
    TTPicturesHeader* pictureHeaderAt(int index);
    TTGOPHeader*      gopHeaderAt(int index);

    int headerIndex(TTVideoHeader* current);

  protected:
     virtual int findIndexOf(TTVideoHeader* current);
    virtual void sort();
};
#endif //TTVIDEOHEADERLIST_H
