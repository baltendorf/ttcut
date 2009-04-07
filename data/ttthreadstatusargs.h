
#ifndef TTTHREADSTATUSARGS_H
#define TTTHREADSTATUSARGS_H

class TTThreadStatusArgs
{
	public:
	  enum State
	  {
	    Started,
	    Finished,
	    Canceled,
	    Error
	  };
};

#endif
