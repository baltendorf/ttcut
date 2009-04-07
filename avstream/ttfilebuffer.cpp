/*----------------------------------------------------------------------------*/
/* COPYRIGHT: TriTime (c) 2003/2009 / www.tritime.org                         */
/*----------------------------------------------------------------------------*/
/* PROJEKT  : TTCUT 2008                                                      */
/* FILE     : ttfilebuffer.cpp                                                */
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

// -----------------------------------------------------------------------------
// Buffered file I/O
// -----------------------------------------------------------------------------

#include "ttfilebuffer.h"
#include "ttcommon.h"

/* ////////////////////////////////////////////////////////////////////////////
 * Construct object
 */
TTFileBuffer::TTFileBuffer(QString name, QIODevice::OpenMode mode)
{
  this->file = new QFile(name);
  this->mode = mode;

  if (this->mode == QIODevice::WriteOnly)
    file->resize(0);

  initInstance();
  initTSearch();

  open();
}

/* ////////////////////////////////////////////////////////////////////////////
 * Destruct object
 */
TTFileBuffer::~TTFileBuffer()
{
  close();

  delete []cBuffer;

  if (file != NULL)
    delete file;
}

/* /////////////////////////////////////////////////////////////////////////////
 *
 */
void TTFileBuffer::initInstance()
{
  bufferSize = 16384;
  bufferMask = bufferSize-1;
  readInc    = bufferSize/2;
  isAtEnd    = false;
  readPos    = 0;
  writePos   = -1;

  cBuffer = new quint8[bufferSize];
}

/* /////////////////////////////////////////////////////////////////////////////
 * Open file
 */
bool TTFileBuffer::open()
{
  //qDebug("open file: %s", qPrintable(file->fileName()));

  if (file->isOpen())
    return true;

  file->open(this->mode);

  return (file->exists() & file->isOpen());
}

/* /////////////////////////////////////////////////////////////////////////////
 * Close file
 */
void TTFileBuffer::close()
{
  //qDebug("close file: %s", qPrintable(file->fileName()));

  if (file->openMode() == QIODevice::WriteOnly) {
    //qDebug("close -> waitForBytesWritten");
    //file->flush();
    //file->waitForBytesWritten(-1);
    //qDebug("all bytes written");
  }

  file->close();
}

/* /////////////////////////////////////////////////////////////////////////////
 * Returns the current file name
 */
QString TTFileBuffer::fileName()
{
  return file->fileName();
}

/* /////////////////////////////////////////////////////////////////////////////
 *Get file size in bytes
 */
quint64 TTFileBuffer::size()
{
  return file->size();
}

/* /////////////////////////////////////////////////////////////////////////////
 * Returns true, if the file is at the end positon
 */
bool TTFileBuffer::atEnd()
{
  return (isAtEnd && readPos > writePos);
}

/* /////////////////////////////////////////////////////////////////////////////
 * Returns the current read position
 */
quint64 TTFileBuffer::position()
{
  return (quint64)readPos;
}

/* /////////////////////////////////////////////////////////////////////////////
 * Flushes any buffered data to the file. Returns true if successful;
 * otherwise returns false.
 */
bool TTFileBuffer::flush()
{
  return file->flush();
}

/* /////////////////////////////////////////////////////////////////////////////
 *
 */
void TTFileBuffer::nextStartCodeBF()
{
}

/* /////////////////////////////////////////////////////////////////////////////
 *
 */
void TTFileBuffer::initTSearch()
{
  tsStartCode[0] = 0x00;
  tsStartCode[1] = 0x00;
  tsStartCode[2] = 0x01;

  int m = 3;

  for (int i = 0; i < 256; ++i)
    tsShift[i] = m+1;

  for (int i = 0; i < m; ++i)
    tsShift[tsStartCode[i]] = m-i;

  tsLookAt = 0;

  while (tsLookAt < m-1)
  {
    if (tsStartCode[m-1] == tsStartCode[m-(tsLookAt+2)])
      break;
    ++tsLookAt;
  }
}

/* /////////////////////////////////////////////////////////////////////////////
 *
 */
void TTFileBuffer::nextStartCodeTS()
{
  int i;
  int mask = bufferMask;
  int m    = 3;

  try
  {
    do
    {
      // FIXME: Quick workaround for passing sequence-header at the beginning of stream!
      //i = (position() == 0) ? 0 : m-1;
      // test it under os x!
      i = m-1;
      fillBuffer(2*m+1);

      while (cBuffer[(int)((position()+i)&mask)] != tsStartCode[i])
      {
        quint8 pos = cBuffer[(int)((position()+m)&mask)];
        seekRelative(tsShift[(int)pos]);
        fillBuffer(2*m+1);
      }

      --i;

      while(cBuffer[(int)((position()+i)&mask)] == tsStartCode[i])
      {
        if(--i < 0)
        {
          seekRelative(m);
          return;
        }
      }

      quint8 pos2 = cBuffer[(int)((position()+m+tsLookAt)&mask)];
      seekRelative(tsLookAt+tsShift[(int)pos2]);
    }while(-1);
  }catch(...)
  {
    return;
  }
}

/* /////////////////////////////////////////////////////////////////////////////
 * Seek forward offset bytes in stream
 */
void TTFileBuffer::seekForward(quint64 offset)
{
  //printf("seek forward: %lld / %lld\n", offset, readPos);
  readPos += offset;
}

/* /////////////////////////////////////////////////////////////////////////////
 * Seek backward offset bytes in stream
 */
void TTFileBuffer::seekBackward(quint64 offset)
{
  //printf("seek backward: %lld / %lld\n", offset, readPos);
  readPos -= offset;

  seekAbsolute(readPos);
}

/* /////////////////////////////////////////////////////////////////////////////
 *
 */
void TTFileBuffer::seekRelative(quint64 offset)
{
  readPos += offset;
}

/* /////////////////////////////////////////////////////////////////////////////
 *
 */
void TTFileBuffer::seekAbsolute(quint64 offset)
{
  isAtEnd  = false;
  readPos  = offset;
  writePos = (readPos&~bufferMask)-1;

  file->seek(writePos+1);
}

/* /////////////////////////////////////////////////////////////////////////////
 *
 */
void TTFileBuffer::readByte(quint8 &byte1)
{
  byte1 = readByte();
}

/* /////////////////////////////////////////////////////////////////////////////
 *
 */
int TTFileBuffer::readByte(quint8* byteArray, int length)
{
  quint64 startReadPos = readPos;

  try
  {
    for (int i = 0; i < length; i++) {
      //readByte(byteArray[i]);
      byteArray[i] = readByte();
      //quint8 by = readByte();
      //byteArray[i] = by;
    }
  }
  catch(TTFileBufferException)
  {
    qDebug("StreamEOF during readByte!");
    qDebug("length: %d / start: %lld / end: %lld", length, startReadPos, readPos-startReadPos);
    readPos = writePos;
    return (readPos - startReadPos);
  }
  return (readPos - startReadPos);
}

/* /////////////////////////////////////////////////////////////////////////////
 *
 */
void TTFileBuffer::readUInt16(quint16 &byte2)
{
  readByte((quint8*)&byte2, 2);
}

/* /////////////////////////////////////////////////////////////////////////////
 *
 */
void TTFileBuffer::readUInt32(quint32 &byte4)
{
  readByte((quint8*)&byte4, 4);
}

/* /////////////////////////////////////////////////////////////////////////////
 *
 */
void TTFileBuffer::readUInt64(quint64 &byte8)
{
  readByte((quint8*)&byte8, 8);
}

/* /////////////////////////////////////////////////////////////////////////////
 * Writes w_buffer with size w_length direct to stream and returns the number
 * of bytes that were actually written, or -1 if an error occured.
 */
quint64 TTFileBuffer::directWrite(const quint8* w_buffer, int w_length)
{
  quint64 result = file->write((char*)w_buffer, w_length);

  return result;
}

/* /////////////////////////////////////////////////////////////////////////////
 * Writes an quint8 to stream
 */
quint64 TTFileBuffer::directWrite(quint8 byte1)
{
  return directWrite((quint8*)&byte1, 1);
}

/* /////////////////////////////////////////////////////////////////////////////
 * Writes an quint16 to stream
 */
quint64 TTFileBuffer::directWriteUInt16(quint16 byte2)
{
  return directWrite((quint8*)&byte2, 2);
}

/* /////////////////////////////////////////////////////////////////////////////
 * Writes an quint32 to stream
 */
quint64 TTFileBuffer::directWriteUInt32(quint32 byte4)
{
  return directWrite((quint8*)&byte4, 4);
}

/* /////////////////////////////////////////////////////////////////////////////
 * Write an quint64 to stream
 */
quint64 TTFileBuffer::directWriteUInt64(quint64 byte8)
{
  return directWrite((quint8*)&byte8, 8);
}

/* /////////////////////////////////////////////////////////////////////////////
 *
 */
void TTFileBuffer::fillBuffer()
{
  if (isAtEnd)
    throw TTFileBufferException(TTFileBufferException::StreamEOF);

  qint64 rLength = file->read((char*)&cBuffer[((writePos+1)&bufferMask)], readInc);

  if (rLength > 0)
    writePos += rLength;

  if (rLength < readInc)
    isAtEnd = true;
}

/* /////////////////////////////////////////////////////////////////////////////
 *
 */
void TTFileBuffer::fillBuffer(int length)
{
    while(readPos > (writePos-length) && !isAtEnd)
      fillBuffer();
}

/* /////////////////////////////////////////////////////////////////////////////
 *
 */
quint8 TTFileBuffer::readByte()
{
  while (readPos > writePos && !isAtEnd)
    fillBuffer();

  return cBuffer[(int)(readPos++&bufferMask)];
}

/* /////////////////////////////////////////////////////////////////////////////
 *
 */
bool TTFileBuffer::readArray(quint8* buffer, int length)
{
  while(readPos > (writePos-length))
    fillBuffer();

  buffer = cBuffer;
  return true;
}

/* /////////////////////////////////////////////////////////////////////////////
 * Exception class for TTFileBuffer
 */
TTFileBufferException::TTFileBufferException(ExceptionType type)
{
  ex_type = type;
}

/* /////////////////////////////////////////////////////////////////////////////
 * Returns the exception message
 */
QString TTFileBufferException::message()
{
  switch(ex_type)
  {
    case TTFileBufferException::StreamEOF:
      return "Stream EOF reached!";
    case TTFileBufferException::SeekError:
      return "Error during seek!";
	default:
	  return "Unknown exception type!";
  }
}
