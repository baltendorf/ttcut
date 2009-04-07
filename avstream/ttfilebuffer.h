/*----------------------------------------------------------------------------*/
/* COPYRIGHT: TriTime (c) 2003/2009 / www.tritime.org                         */
/*----------------------------------------------------------------------------*/
/* PROJEKT  : TTCUT 2008                                                      */
/* FILE     : ttfilebuffer.h                                                  */
/*----------------------------------------------------------------------------*/
/* AUTHOR  : b. altendorf (E-Mail: b.altendorf@tritime.de)   DATE: 02/23/2005 */
/* MODIFIED: b. altendorf (totally rewritten)                DATE: 11/12/2007 */
/* MODIFIED:                                                 DATE:            */
/*----------------------------------------------------------------------------*/

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

#ifndef TTFILEBUFFER_H
#define TTFILEBUFFER_H

// Qt header files
#include <QString>
#include <QFile>
#include <QByteArray>

// -----------------------------------------------------------------------------
// TTFileBuffer: class declaration
// -----------------------------------------------------------------------------
class TTFileBuffer
{
public:
  // create / delete
  TTFileBuffer(QString name, QIODevice::OpenMode mode);
  ~TTFileBuffer();

  // file stream
  bool    open();    
  void    close();
  QString fileName();
  quint64 size();
  bool    atEnd();
  quint64 position();
  bool    flush();

  // read / write
  void    readByte( quint8 &byte1 );
  int     readByte( quint8* byteArray, int length);

  // search
  void    nextStartCodeBF();
  void    initTSearch();
  void    nextStartCodeTS();
   
  // positioning
  void    seekForward(quint64 offset);
  void    seekBackward(quint64 offset);
  void    seekRelative(quint64 offset);
  void    seekAbsolute(quint64 offset);

  // migration stuff
  void    readUInt16(quint16 &byte2);
  void    readUInt32(quint32 &byte4);
  void    readUInt64(quint64 &byte8);

  quint64 directWrite(quint8 byte1);
  quint64 directWrite(const quint8* w_buffer, int w_length);
  quint64 directWriteUInt16(quint16 byte2);
  quint64 directWriteUInt32(quint32 byte4);
  quint64 directWriteUInt64(quint64 byte8);

 protected:
  void    initInstance();
  void    fillBuffer();
  void    fillBuffer(int length);
  quint8  readByte();
  bool    readArray(quint8* buffer, int length);

 private:
  QFile*     file;
  QIODevice::OpenMode mode;
  bool       isAtEnd;
  quint8*    cBuffer;
  int        bufferSize;
  int        bufferMask;
  qint64     writePos;
  qint64     readPos;
  int        readInc;

  quint8     tsStartCode[3];
  int        tsShift[256];
  int        tsLookAt;
};

class TTFileBufferException
{
  public:
    enum ExceptionType
    {
      StreamEOF,
      SeekError
    };

    TTFileBufferException(ExceptionType type);

    QString message();

  protected:
    ExceptionType ex_type;
};

#endif //TTFILEBUFFER_H
