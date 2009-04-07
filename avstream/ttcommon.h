
#include <QString>
#include <QDateTime>

class TTTimeCode;

extern "C" bool    ttAssigned( const void* pointer );
extern "C" bool    ttFileExists( QString fName );
extern "C" bool    ttDeleteFile( QString fName );
extern "C" QString ttAddFileExt( QString fName, const char* cExt );
extern "C" QString ttChangeFileExt( QString fName, const char* cExt );
extern "C" QTime   ttMsecToTime( int msec );
extern "C" QTime   ttMsecToTimeD( double msec );
extern "C" QTime   ttFramesToTime(long lFrames, float fps);
extern "C" long    ttTimeToFrames(QTime timeCode, float fps);
extern "C" TTTimeCode ttFrameToTimeCode( int FrameNr, float fps);

#ifndef TTTIMECODE_H
#define TTTIMECODE_H

class TTTimeCode
{
 public:
  TTTimeCode();
  QString toString();

  bool drop_frame_flag;
  short hours;
  short minutes;
  short seconds;
  short pictures;
  bool marker_bit;

};
#endif
