/*----------------------------------------------------------------------------*/
/* COPYRIGHT: TriTime (c) 2003/2010 / www.tritime.org                         */
/*----------------------------------------------------------------------------*/
/* PROJEKT  : TTCUT 2008                                                      */
/* FILE     : ttavcutparameter.h                                              */
/*----------------------------------------------------------------------------*/
/* AUTHOR  : b. altendorf (E-Mail: b.altendorf@tritime.de)   DATE: 02/23/2005 */
/* MODIFIED: b. altendorf                                    DATE: 06/18/2008 */
/*----------------------------------------------------------------------------*/

// ----------------------------------------------------------------------------
// TTCUTPARAMETER
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
#ifndef TTCUTPARAMETER_H
#define TTCUTPARAMETER_H

#include <qdatetime.h>

#include <QVector>

#include "avstream/ttcommon.h"
#include "avstream/ttfilebuffer.h"
#include "avstream/ttvideoheaderlist.h"

/*//////////////////////////////////////////////////////////////////////////////
 * TTCutParameter
 */
class TTCutParameter
{
  public:
    TTCutParameter(TTFileBuffer* fBuffer);
    ~TTCutParameter();

    TTFileBuffer* getTargetStreamBuffer();
    bool getIsWriteSequenceEnd();
    void setIsWriteSequenceEnd(bool value);
    bool getIsWriteMaxBitrate();
    void setIsWriteMaxBitrate(bool value);
    bool getIsDVDCompliantStream();
    void setIsDVDCompliantStream(bool value);
    int  getNumPicturesWritten();
    void setNumPicturesWritten(int value);
    int  getMaxBitrate();
    void setMaxBitrate(int value);
    int  getCutInIndex();
    void setCutInIndex(int value);
    int  getCutOutIndex();
    void setCutOutIndex(int value);
    void firstCall();
    void lastCall();

  private:
    void writeSequenceEndHeader();

    TTFileBuffer*        targetStreamBuffer;
    bool                 isWriteSequenceEnd;
    bool                 isDVDCompliantStream;
    bool                 isWriteMaxBitrate;
    int                  cutInIndex;
    int                  cutOutIndex;
    int                  numPicturesWritten;
    int                  maxBitrateValue;
    float                dvdCompliantMaxBitrate;
    int                  dvdCompliantMaxMrames;
};
#endif //TTCUTPARAMETER_H
