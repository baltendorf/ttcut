/*----------------------------------------------------------------------------*/
/* COPYRIGHT: TriTime (c) 2003/2010 / www.tritime.org                         */
/*----------------------------------------------------------------------------*/
/* PROJEKT  : TTCUT 2008                                                      */
/* FILE     : ttmpeg2window.h                                                 */
/*----------------------------------------------------------------------------*/
/* AUTHOR  : b. altendorf (E-Mail: b.altendorf@tritime.de)   DATE: 12/16/2008 */
/*----------------------------------------------------------------------------*/

// ----------------------------------------------------------------------------
// TTMPEG2WINDOW
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


#ifndef TTMPEG2WINDOW2_H
#define TTMPEG2WINDOW2_H

//#include <QDateTime>
#include <QImage>
#include <QFileDialog>
#include <QLabel>

#include "common/ttcut.h"
#include "mpeg2decoder/ttmpeg2decoder.h"
#include "common/ttmessagelogger.h"
#include "avstream/ttvideoheaderlist.h"
#include "avstream/ttvideoindexlist.h"
#include "avstream/ttmpeg2videostream.h"

class TTMPEG2Window2 : public QLabel
{
  Q_OBJECT

  public:
    TTMPEG2Window2( QWidget* parent=0 );

    void resizeEvent(QResizeEvent * event);

    void openVideoFile(QString fName, TTVideoIndexList* viIndex=0, TTVideoHeaderList* viHeader=0);
    void openVideoStream(TTMpeg2VideoStream* v_stream);
    void closeVideoStream();

    // navigation
    void moveToFirstFrame(bool show = true);
    void moveToVideoFrame(int iFramePos);

    void showVideoFrame();
    void showFrameAt(int index);
    void showDecodedSlice();

    void saveCurrentFrame(QString fName, const char* format);

  protected:
  	void getFrameInfo();
    void decodeAndShowSlice();

  private:
    TFrameInfo*      frameInfo;
    quint8*          picBuffer;
    int              videoWidth;
    int              videoHeight;
    int              currentIndex;
    TTVideoStream*   mpVideoStream;
    TTMessageLogger* log;
    TTMpeg2Decoder*  mpeg2Decoder;
};

#endif //TTMPEG2WINDOW_H
