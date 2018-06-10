#ifndef WALLE_SYS_RUNNABLE_H_
#define WALLE_SYS_RUNNABLE_H_
#include <memory>

namespace walle {

/**
 * The Runnable interface with the run() method
 * must be implemented by classes that provide
 * an entry point for a thread.
 */
class  Runnable {
public:	
	Runnable();
	virtual ~Runnable();

	/*!
	 * Do whatever the thread needs to do. Must
	 * be overridden by subclasses.
	 */
	virtual void run() = 0;

};

typedef std::shared_ptr<Runnable> RunnablePtr;
} //namespace walle


#endif 
