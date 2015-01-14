/*----------------------------------------------------------------------------*/
/* COPYRIGHT: TriTime (c) 2003/2010 / www.tritime.org                         */
/*----------------------------------------------------------------------------*/
/* PROJEKT  : TTCUT 2008                                                      */
/* FILE     : ttaciwriter.h                                                   */
/*----------------------------------------------------------------------------*/
/* AUTHOR  : b. altendorf (E-Mail: b.altendorf@tritime.de)   DATE: 08/06/2005 */
/* MODIFIED: b. altendorf                                    DATE: 11/08/2008 */
/*----------------------------------------------------------------------------*/

// ----------------------------------------------------------------------------
// TTAVIWRITER
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


#ifndef TTAVIWRITER_H
#define TTAVIWRITER_H

#include <QString>

#include "avstream/ttavstream.h"
#include "mpeg2decoder/ttmpeg2decoder.h"

class QFileInfo;
class TTMessageLogger;

extern "C"
{
#include "avilib/avilib.h"
}

/* /////////////////////////////////////////////////////////////////////////////
 * AVI writer class
 */
class TTAVIWriter
{
 public:
   TTAVIWriter(TTVideoStream* v_stream);
   ~TTAVIWriter();

   int  writeAVI(int start_frame_pos, int end_frame_pos, const QFileInfo& avi_fInfo);
   bool closeAVI();

 protected:
   void initAVIWriter(TTVideoStream* v_stream);
   bool initDecoder();
   long compareFrames();

 private:
   TTMessageLogger*   log;
   TTMpeg2Decoder*    decoder;
   QString            video_file_name;
   TTVideoHeaderList* header_list;
   TTVideoIndexList*  index_list;
   double             mFrameRate;
   int                current_frame;
   qint64             file_size;
   quint64            file_offset;
   quint8*            ref_data;
   avi_t*             avi_file;
};

#endif //TTAVIWRITER_H


