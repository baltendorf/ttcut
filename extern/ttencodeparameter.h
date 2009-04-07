/*----------------------------------------------------------------------------*/
/* COPYRIGHT: TriTime (c) 2003/2010 / www.tritime.org                         */
/*----------------------------------------------------------------------------*/
/* PROJEKT  : TTCUT 2008                                                      */
/* FILE     : ttencodeparameter.h                                             */
/*----------------------------------------------------------------------------*/
/* AUTHOR  : b. altendorf (E-Mail: b.altendorf@tritime.de)   DATE: 11/07/2008 */
/*----------------------------------------------------------------------------*/

// ----------------------------------------------------------------------------
// TTENCODEPARAMETER
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

#ifndef TTENCODEPARAMETER_H
#define TTENCODEPARAMETER_H

/* /////////////////////////////////////////////////////////////////////////////
 * Class for parameter common for all encoder
 */
class TTEncodeParameter
{
  public:
    TTEncodeParameter(){};
    ~TTEncodeParameter(){};

    void      setAVIFileInfo(const QFileInfo& value)   { aviFInfo = value; }
    QFileInfo aviFileInfo()                            { return aviFInfo; }
    void      setMpeg2FileInfo(const QFileInfo& value) { mpeg2FInfo = value; }
    QFileInfo mpeg2FileInfo()                          { return mpeg2FInfo; }
    void      setVideoWidth(int value)                 { vWidth = value; }
    int       videoWidth()                             { return vWidth; }
    void      setVideoHeight(int value)                { vHeight = value; }
    int       videoHeight()                            { return vHeight; }
    void      setVideoFPS(float value)                 { vFPS = value; }
    float     videoFPS()                               { return vFPS; }
    void      setVideoAspectCode(int value)            { vAspectCode = value; }
    int       videoAspectCode()                        { return vAspectCode; }
    void      setVideoBitrate(float value)             { vBitrate = value; }
    float     videoBitrate()                           { return vBitrate; }
    void      setVideoMaxBitrate(float value)          { vMaxBitrate = value; }
    float     videoMaxBitrate()                        { return vMaxBitrate; }
    int       start() { return mStartIndex; }
    void       setStart(int value) { mStartIndex = value; }
    int       end() { return mEndIndex; }
    void      setEnd(int value) { mEndIndex = value; }

    void      print(char* prefix);

  private:
    QFileInfo aviFInfo;
    QFileInfo mpeg2FInfo;
    int       mStartIndex;
    int       mEndIndex;
    int       vWidth;
    int       vHeight;
    float     vFPS;
    int       vAspectCode;
    float     vBitrate;
    float     vMaxBitrate;
};

#endif //TTENCODEPARAMETER_H
