
#ifndef ISTATUSREPORTER_H
#define ISTATUSREPORTER_H

#include <QObject>

//! Status reporter interface
class IStatusReporter : public QObject
{ 
  Q_OBJECT

  public:
    IStatusReporter();
    virtual ~IStatusReporter() {}

  signals:
    void init();
    void started(const QString& msg, quint64 value);
    void step(const QString& msg, quint64 value);
    void finished(const QString& msg, quint64 value);
    void aborted(const QString& msg, quint64 value);
    void error(const QString& msg, quint64 value);
    void exit();
    void statusReport(int state, const QString& msg, quint64 value);
};

//! Status reporter arguments
class StatusReportArgs
{
  public:
    enum State
    {
      Init,
      Start,
      Step,
      Finished,
      Exit,
      Canceled,
      Error,
      ShowProcessForm,
      AddProcessLine,
      HideProcessForm
    };
};

#endif //ISTATUSREPORTER_H
