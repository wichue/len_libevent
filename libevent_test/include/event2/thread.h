/*
 * Copyright (c) 2008-2012 Niels Provos and Nick Mathewson
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. The name of the author may not be used to endorse or promote products
 *    derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
 * OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
 * IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT
 * NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
 * THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */
#ifndef EVENT2_THREAD_H_INCLUDED_
#define EVENT2_THREAD_H_INCLUDED_

/** @file event2/thread.h

  Functions for multi-threaded applications using Libevent.
  使用Libevent的多线程应用程序的函数。

  When using a multi-threaded application in which multiple threads
  add and delete events from a single event base, Libevent needs to
  lock its data structures.
  当使用多线程应用程序，其中多个线程在单个事件库中添加和删除事件时，Libevent需要锁定其数据结构。

  Like the memory-management function hooks, all of the threading functions
  _must_ be set up before an event_base is created if you want the base to
  use them.
  与内存管理函数挂钩一样，如果你想让事件库使用所有线程函数_must_，那么必须在创建事件库之前设置它们。

  Most programs will either be using Windows threads or Posix threads.  You
  can configure Libevent to use one of these event_use_windows_threads() or
  event_use_pthreads() respectively.  If you're using another threading
  library, you'll need to configure threading functions manually using
  evthread_set_lock_callbacks() and evthread_set_condition_callbacks().
  大多数程序将使用Windows线程或Posix线程。您可以将Libevent配置为分别使用以下event_use_windows_threads（）或event_use_pthreads（）之一。
  如果您使用的是另一个线程库，则需要使用evthread_set_lock_callbacks（）和evthread_set_condition_callbacks（）手动配置线程函数。

 */

#include <event2/visibility.h>

#ifdef __cplusplus
extern "C" {
#endif

#include <event2/event-config.h>

/**
   @name Flags passed to lock functions	传递给锁定函数的标志

   @{
*/
/** A flag passed to a locking callback when the lock was allocated as a
 * read-write lock, and we want to acquire or release the lock for writing. 
 * 当锁被分配为读写锁时，传递给锁回调的标志，我们希望获取或释放锁进行写入。*/
#define EVTHREAD_WRITE	0x04
/** A flag passed to a locking callback when the lock was allocated as a
 * read-write lock, and we want to acquire or release the lock for reading.
 * 当锁被分配为读写锁时，传递给锁回调的标志，我们希望获取或释放锁进行读取。 */
#define EVTHREAD_READ	0x08
/** A flag passed to a locking callback when we don't want to block waiting
 * for the lock; if we can't get the lock immediately, we will instead
 * return nonzero from the locking callback. 
 * 当我们不想阻塞等待锁时，传递给锁定回调的标志；如果我们不能立即获得锁，我们将从锁回调中返回非零。*/
#define EVTHREAD_TRY    0x10
/**@}*/

#if !defined(EVENT__DISABLE_THREAD_SUPPORT) || defined(EVENT_IN_DOXYGEN_)

#define EVTHREAD_LOCK_API_VERSION 1

/**
   @name Types of locks	线程锁类型

   @{*/
/** A recursive lock is one that can be acquired multiple times at once by the
 * same thread.  No other process can allocate the lock until the thread that
 * has been holding it has unlocked it as many times as it locked it.
 * 递归锁是同一线程可以同时多次获取的锁。在持有锁的线程解锁锁的次数与锁定锁的次数相同之前，任何其他进程都无法分配锁。 */
#define EVTHREAD_LOCKTYPE_RECURSIVE 1
/* A read-write lock is one that allows multiple simultaneous readers, but
 * where any one writer excludes all other writers and readers.
   读写锁是一种允许多个同时读取的锁，但任何一个写入器都排除了所有其他写入器和读取器。 */
#define EVTHREAD_LOCKTYPE_READWRITE 2
/**@}*/

/** This structure describes the interface a threading library uses for
 * locking.   It's used to tell evthread_set_lock_callbacks() how to use
 * locking on this platform.
 * 此结构描述了线程库用于锁定的接口。它用于告诉evthread_set_lock_callbacks（）如何在此平台上使用锁定。
 */
struct evthread_lock_callbacks {
	/** The current version of the locking API.  Set this to
	 * EVTHREAD_LOCK_API_VERSION	锁定API的当前版本。将其设置为EVTHREAD_LOCK_API_VERSION */
	int lock_api_version;
	/** Which kinds of locks does this version of the locking API
	 * support?  A bitfield of EVTHREAD_LOCKTYPE_RECURSIVE and
	 * EVTHREAD_LOCKTYPE_READWRITE.
	 * 此版本的锁定API支持哪些类型的锁？EVTHREAD_LOCKTYPE_RECURSIVE和EVTHREAD_LOCKTYPE_READWRITE的位字段。
	 *
	 * (Note that RECURSIVE locks are currently mandatory, and
	 * READWRITE locks are not currently used.)
	 * （请注意，RECURSIVE锁目前是强制性的，READWRITE锁目前尚未使用。）
	 **/
	unsigned supported_locktypes;
	/** Function to allocate and initialize new lock of type 'locktype'.
	 * Returns NULL on failure. 用于分配和初始化“locktype”类型的新锁的函数。失败时返回NULL。 */
	void *(*alloc)(unsigned locktype);
	/** Funtion to release all storage held in 'lock', which was created
	 * with type 'locktype'.	释放“锁”中所有存储的功能，该锁是使用“locktype”类型创建的。 */
	void (*free)(void *lock, unsigned locktype);
	/** Acquire an already-allocated lock at 'lock' with mode 'mode'.
	 * Returns 0 on success, and nonzero on failure.	以“mode”模式在“lock”处获取已分配的锁。成功时返回0，失败时返回非零。 */
	int (*lock)(unsigned mode, void *lock);
	/** Release a lock at 'lock' using mode 'mode'.  Returns 0 on success,
	 * and nonzero on failure.	使用模式“mode”在“lock”处解锁。成功时返回0，失败时返回非零。 */
	int (*unlock)(unsigned mode, void *lock);
};

/** Sets a group of functions that Libevent should use for locking.
 * For full information on the required callback API, see the
 * documentation for the individual members of evthread_lock_callbacks.
 * 设置Libevent应用于锁定的一组函数。有关所需回调API的完整信息，请参阅evthread_lock_callbacks各个成员的文档。
 *
 * Note that if you're using Windows or the Pthreads threading library, you
 * probably shouldn't call this function; instead, use
 * evthread_use_windows_threads() or evthread_use_posix_threads() if you can.
 * 请注意，如果您使用的是Windows或Pthreads线程库，则可能不应该调用此函数；相反，如果可以的话，请使用evthread_use_windowsthreads（）或evthread-use_posixthreades（）。
 */
EVENT2_EXPORT_SYMBOL
int evthread_set_lock_callbacks(const struct evthread_lock_callbacks *);

#define EVTHREAD_CONDITION_API_VERSION 1

struct timeval;

/** This structure describes the interface a threading library uses for
 * condition variables.  It's used to tell evthread_set_condition_callbacks
 * how to use locking on this platform.
 * 此结构描述了线程库用于条件变量的接口。它用于告诉evthread_set_condition_callbacks如何在此平台上使用锁定。
 */
struct evthread_condition_callbacks {
	/** The current version of the conditions API.  Set this to
	 * EVTHREAD_CONDITION_API_VERSION	条件API的当前版本。将其设置为EVTHREAD_CONDITION_API_VERSION */
	int condition_api_version;
	/** Function to allocate and initialize a new condition variable.
	 * Returns the condition variable on success, and NULL on failure.
	 * The 'condtype' argument will be 0 with this API version.
	 * 用于分配和初始化新条件变量的函数。成功时返回条件变量，失败时返回NULL。此API版本的“condtype”参数将为0。
	 */
	void *(*alloc_condition)(unsigned condtype);
	/** Function to free a condition variable.	函数用于释放条件变量。 */
	void (*free_condition)(void *cond);
	/** Function to signal a condition variable.  If 'broadcast' is 1, all
	 * threads waiting on 'cond' should be woken; otherwise, only on one
	 * thread is worken.  Should return 0 on success, -1 on failure.
	 * This function will only be called while holding the associated
	 * lock for the condition.
	 * 用于发出条件变量信号的函数。如果'broadcast'为1，则应唤醒所有等待'cond'的线程；否则，只有一个线程是worker。
	 * 成功时应返回0，失败时应返回-1。只有在持有该条件的相关锁时，才会调用此函数。
	 */
	int (*signal_condition)(void *cond, int broadcast);
	/** Function to wait for a condition variable.  The lock 'lock'
	 * will be held when this function is called; should be released
	 * while waiting for the condition to be come signalled, and
	 * should be held again when this function returns.
	 * If timeout is provided, it is interval of seconds to wait for
	 * the event to become signalled; if it is NULL, the function
	 * should wait indefinitely.
	 * 函数用于等待条件变量。调用此函数时，锁“lock”将保持不变；应在等待条件发出信号时释放，并在此函数返回时再次保持。
	 * 如果提供了超时，则是等待事件发出信号的秒间隔；如果它为NULL，则函数应无限期等待。
	 *
	 * The function should return -1 on error; 0 if the condition
	 * was signalled, or 1 on a timeout.	函数出错时应返回-1；如果条件已发出信号，则为0，如果超时，则为1。 */
	int (*wait_condition)(void *cond, void *lock,
	    const struct timeval *timeout);
};

/** Sets a group of functions that Libevent should use for condition variables.
 * For full information on the required callback API, see the
 * documentation for the individual members of evthread_condition_callbacks.
 * 设置Libevent应用于条件变量的一组函数。有关所需回调API的完整信息，请参阅evthread_condition_callbacks各个成员的文档。
 *
 * Note that if you're using Windows or the Pthreads threading library, you
 * probably shouldn't call this function; instead, use
 * evthread_use_windows_threads() or evthread_use_pthreads() if you can.
 * 请注意，如果您使用的是Windows或Pthreads线程库，则可能不应该调用此函数；相反，如果可以的话，请使用evthread_use_windowsthreads（）或evthread-use_phreads（）。
 */
EVENT2_EXPORT_SYMBOL
int evthread_set_condition_callbacks(
	const struct evthread_condition_callbacks *);

/**
   Sets the function for determining the thread id.	设置用于确定线程id的函数。

   @param base the event base for which to set the id function	要为其设置id函数的事件库
   @param id_fn the identify function Libevent should invoke to
     determine the identity of a thread.	应该调用identify函数Libevent来确定线程的身份。
*/
EVENT2_EXPORT_SYMBOL
void evthread_set_id_callback(
    unsigned long (*id_fn)(void));

#if (defined(_WIN32) && !defined(EVENT__DISABLE_THREAD_SUPPORT)) || defined(EVENT_IN_DOXYGEN_)
/** Sets up Libevent for use with Windows builtin locking and thread ID
    functions.  Unavailable if Libevent is not built for Windows.
	将Libevent设置为与Windows内置的锁定和线程ID函数一起使用。如果Libevent不是为Windows构建的，则不可用。

    @return 0 on success, -1 on failure. */
EVENT2_EXPORT_SYMBOL
int evthread_use_windows_threads(void);
/**
   Defined if Libevent was built with support for evthread_use_windows_threads()
   如果Libevent是在支持evthread_use_windows_threads（）的情况下构建的，则定义
*/
#define EVTHREAD_USE_WINDOWS_THREADS_IMPLEMENTED 1
#endif

#if defined(EVENT__HAVE_PTHREADS) || defined(EVENT_IN_DOXYGEN_)
/** Sets up Libevent for use with Pthreads locking and thread ID functions.
    Unavailable if Libevent is not build for use with pthreads.  Requires
    libraries to link against Libevent_pthreads as well as Libevent.
	设置Libevent以与Pthreads锁定和线程ID函数一起使用。如果Libevent不是为与pthreads一起使用而构建的，则不可用。要求库链接到Libevent_pthreads和Libevent。

    @return 0 on success, -1 on failure. */
EVENT2_EXPORT_SYMBOL
int evthread_use_pthreads(void);
/** Defined if Libevent was built with support for evthread_use_pthreads()	如果Libevent是在支持evthread_use_pthreads（）的情况下构建的，则定义 */
#define EVTHREAD_USE_PTHREADS_IMPLEMENTED 1

#endif

/** Enable debugging wrappers around the current lock callbacks.  If Libevent
 * makes one of several common locking errors, exit with an assertion failure.
 * 启用当前锁回调的调试包装器。如果Libevent发生了几个常见的锁定错误之一，请退出断言失败。
 *
 * If you're going to call this function, you must do so before any locks are
 * allocated.	如果你要调用这个函数，你必须在分配任何锁之前这样做。
 **/
EVENT2_EXPORT_SYMBOL
void evthread_enable_lock_debugging(void);

/* Old (misspelled) version: This is deprecated; use
 * evthread_enable_log_debugging instead.	旧（拼写错误）版本：已弃用；请改用evthread_enable_log_debugging。 */
EVENT2_EXPORT_SYMBOL
void evthread_enable_lock_debuging(void);

#endif /* EVENT__DISABLE_THREAD_SUPPORT */

struct event_base;
/** Make sure it's safe to tell an event base to wake up from another thread
    or a signal handler.
	确保告诉事件库从另一个线程或信号处理程序中唤醒是安全的。

    You shouldn't need to call this by hand; configuring the base with thread
    support should be necessary and sufficient.
	你不需要用手打电话；配置具有线程支持的基座应该是必要和充分的。

    @return 0 on success, -1 on failure.
 */
EVENT2_EXPORT_SYMBOL
int evthread_make_base_notifiable(struct event_base *base);

#ifdef __cplusplus
}
#endif

#endif /* EVENT2_THREAD_H_INCLUDED_ */
