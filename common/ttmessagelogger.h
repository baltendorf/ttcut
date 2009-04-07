/*----------------------------------------------------------------------------*/
/* COPYRIGHT: TriTime (c) 2003/2006 / www.tritime.org                         */
/*----------------------------------------------------------------------------*/
/* PROJEKT  : TTCUT 2006                                                      */
/* FILE     : ttmessagelogger.h                                               */
/*----------------------------------------------------------------------------*/
/* AUTHOR  : b. altendorf (E-Mail: b.altendorf@tritime.de)   DATE: 01/28/2006 */
/* MODIFIED:                                                 DATE:            */
/*----------------------------------------------------------------------------*/

// -----------------------------------------------------------------------------
// TTMESSAGELOGGER
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


#ifndef TTMESSAGELOGGER_H
#define TTMESSAGELOGGER_H

#include <QString>
#include <QFile>
#include <QFileInfo>
#include <QTextStream>
#include <QDateTime>

class TTMessageLogger
{
  private:
    TTMessageLogger(int mode=STD_LOG_MODE);

  public:
    static TTMessageLogger* getInstance(int mode=STD_LOG_MODE);
    ~TTMessageLogger();

    void enableLogFile(bool enable);
    void setLogModeConsole(bool console);
    void setLogModeExtended(bool extended);
    
    void setLogMode(int mode);
    void setLogLevel(int level);

    // deprecated calls
    void infoMsg(QString caller, QString msgString, int line=-1);
    void warningMsg(QString caller, QString msgString, int line=-1);
    void errorMsg(QString caller, QString msgString, int line=-1);
    void fatalMsg(QString caller, QString msgString, int line=-1);
    void debugMsg(QString caller, QString msgString, int line=-1);
    
    // new signature!
    void infoMsg(QString caller, int line, QString msgString);
    void warningMsg(QString caller, int line, QString msgString);
    void errorMsg(QString caller, int line, QString msgString);
    void fatalMsg(QString caller, int line, QString msgString);
    void debugMsg(QString caller, int line, QString msgString);

    // deprecated calls
    void infoMsg(QString caller, int line, const char* msg, ...);
    void warningMsg(QString caller, int line, const char* msg, ...);
    void errorMsg(QString caller, int line, const char* msg, ...);
    void fatalMsg(QString caller, int line, const char* msg, ...);
    void debugMsg(QString caller, int line, const char* msg, ...);
    void showErrorMsg(QString caller, int line, const char* msg, ...);
    void showFatalMsg(QString caller, int line, const char* msg, ...);

    enum MsgType
    {
      INFO,
      WARNING,
      ERROR,
      FATAL,
      DEBUG
    };

    enum LogMode
    {
      INDIVIDUAL = 0x01,
      SUMMARIZE  = 0x02,
      CONSOLE    = 0x04
    };

    enum LogLevel
    {
      ALL,         // FATAL+ERROR+WARNING+INFO+DEBUG
      EXTENDED,    // FATAL+ERROR+WARNING+INFO
      MINIMAL,     // FATAL+ERROR+WARNING
      NONE         // FATAL+ERROR
    };

    void logMsg( MsgType type, QString caller, int line, QString msgString, bool show=false);
    void writeMsg(QString msgString);

  private:
    QFile* logfile;
    static TTMessageLogger* loggerInstance;
    static QString stdLogFilePath;
    bool   logEnabled;
    bool   logConsole;
    bool   logExtended;

    static       int   logMode;
    static       int   logLevel;
    static const int   STD_LOG_MODE;
    static const char* INFO_FILE_NAME;
    static const char* WARNING_FILE_NAME;
    static const char* ERROR_FILE_NAME;
    static const char* DEBUG_FILE_NAME;
    static const char* SUM_FILE_NAME;
};
#endif //TTMESSAGELOGGER_H
   
