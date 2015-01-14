
#include <QString>
#include <QDateTime>

class TTTimeCode;

extern bool    ttAssigned( const void* pointer );
extern bool    ttFileExists( QString fName );
extern bool    ttDeleteFile( QString fName );
extern QString ttAddFileExt( QString fName, const char* cExt );
extern QString ttChangeFileExt( QString fName, const char* cExt );
extern QTime   ttMsecToTime( int msec );
extern QTime   ttMsecToTimeD( double msec );
extern QTime   ttFramesToTime(long lFrames, float fps);
extern long    ttTimeToFrames(QTime timeCode, float fps);
extern TTTimeCode ttFrameToTimeCode( int FrameNr, float fps);

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
