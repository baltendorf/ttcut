/*----------------------------------------------------------------------------*/
/* COPYRIGHT: TriTime (c) 2003/2006/2010 / www.tritime.org                    */
/*----------------------------------------------------------------------------*/
/* PROJEKT  : TTCUT 2008                                                      */
/* FILE     : ttexception.h                                                   */
/*----------------------------------------------------------------------------*/
/* AUTHOR  : b. altendorf (E-Mail: b.altendorf@tritime.de)   DATE: 06/15/2008 */
/* MODIFIED:                                                 DATE:            */
/*----------------------------------------------------------------------------*/

// -----------------------------------------------------------------------------
// TTEXCEPTION
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


#ifndef TTEXCEPTION_H
#define TTEXCEPTION_H

#include <QString>

/* /////////////////////////////////////////////////////////////////////////////
 * Generell base class for all exception types
 */
class TTException
{
  public:
    TTException();
    TTException(const QString& message);
    TTException(const QString& caller, int line, const QString& message);
    virtual ~TTException();

    virtual QString getClassName() {return "TTException";};
    QString getMessage();

  protected:
    QString message;
};

class TTCommonException : public TTException
{
  public:
    TTCommonException(const QString& msg) : TTException(msg){};
    TTCommonException(const QString& caller, int line, const QString& msg) : TTException(caller, line, msg){};
  protected:
    virtual QString getClassName() { return "TTCommonException"; };
};

class TTMemoryAllocationException : public TTException
{
  public:
    TTMemoryAllocationException(const QString&msg) : TTException(msg){};
    TTMemoryAllocationException(const QString& caller, int line, const QString& msg) : TTException(caller, line, msg){};
   protected:
    virtual QString getClassName() {return "TTMemoryAllocationException";}
};

class TTMethodNotImplementedException : public TTException
{
  public:
    TTMethodNotImplementedException(const QString&msg) : TTException(msg){};
    TTMethodNotImplementedException(const QString& caller, int line, const QString& msg) : TTException(caller, line, msg){};
   protected:
    virtual QString getClassName() {return "TTMethodNotImplementedException";};
};

class TTMissingMethodException : public TTException
{
  public:
    TTMissingMethodException(const QString&msg) : TTException(msg){};
    TTMissingMethodException(const QString& caller, int line, const QString& msg) : TTException(caller, line, msg){};
   protected:
    virtual QString getClassName() {return "TTMissingMethodException";};
};

class TTIOException : public TTException
{
  public:
    TTIOException(const QString& msg) : TTException(msg){};
    TTIOException(const QString& caller, int line, const QString& msg) : TTException(caller, line, msg){};
   protected:
    virtual QString getClassName() {return "TTIOException";};
};

class TTDataFormatException : public TTException
{
  public:
    TTDataFormatException(const QString& msg) : TTException(msg){};
    TTDataFormatException(const QString& caller, int line, const QString& msg) : TTException(caller, line, msg){};
   protected:
    virtual QString getClassName() {return "TTDataFormatException";};
};

class TTInvalidOperationException : public TTException
{
  public:
    TTInvalidOperationException(const QString& msg) : TTException(msg){};
    TTInvalidOperationException(const QString& caller, int line, const QString& msg) : TTException(caller, line, msg){};
   protected:
    virtual QString getClassName() {return "TTInvalidOperationException";};
};

class TTArgumentException : public TTException
{
  public:
    TTArgumentException(const QString& msg) : TTException(msg){};
    TTArgumentException(const QString& caller, int line, const QString& msg) : TTException(caller, line, msg){};
   protected:
    virtual QString getClassName() {return "TTArgumentException";};
};

class TTArgumentNullException : public TTException
{
  public:
    TTArgumentNullException(const QString& msg) : TTException(msg) {};
    TTArgumentNullException(const QString& caller, int line, const QString& msg) : TTException(caller, line, msg){};
   protected:
    virtual QString getClassName() {return "TTArgumentNullException";};
};

class TTArgumentOutOfRangeException : public TTException
{
  public:
    TTArgumentOutOfRangeException(const QString& msg) : TTException(msg) {};
    TTArgumentOutOfRangeException(const QString& caller, int line, const QString& msg) : TTException(caller, line, msg){};
   protected:
    virtual QString getClassName() {return "TTArgumentOutOfRangeException";};
};

class TTIndexOutOfRangeException : public TTException
{
  public:
    TTIndexOutOfRangeException(const QString& msg) : TTException(msg) {};
    TTIndexOutOfRangeException(const QString& caller, int line, const QString& msg) : TTException(caller, line, msg){};
   protected:
    virtual QString getClassName() {return "TTIndexOutOfRangeException";};
};

class TTFileNotFoundException : public TTException
{
	public:
		TTFileNotFoundException(const QString& msg) : TTException(msg) {};
    TTFileNotFoundException(const QString& caller, int line, const QString& msg) : TTException(caller, line, msg){};
 	protected:
		virtual QString getClassName() {return "TTFileNotFoundException";};
};

class TTAbortException : public TTException
{
	public:
		TTAbortException(const QString& msg) : TTException(msg) {};
    TTAbortException(const QString& caller, int line, const QString& msg) : TTException(caller, line, msg){};
 	protected:
		virtual QString getClassName() {return "TTAbortException";};
};
#endif

