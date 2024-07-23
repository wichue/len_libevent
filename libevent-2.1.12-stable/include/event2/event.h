/*
 * Copyright (c) 2000-2007 Niels Provos <provos@citi.umich.edu>
 * Copyright (c) 2007-2012 Niels Provos and Nick Mathewson
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
#ifndef EVENT2_EVENT_H_INCLUDED_
#define EVENT2_EVENT_H_INCLUDED_

/**
   @mainpage

  @section intro Introduction 引言

  Libevent is an event notification library for developing scalable network
  servers.  The Libevent API provides a mechanism to execute a callback
  function when a specific event occurs on a file descriptor or after a
  timeout has been reached. Furthermore, Libevent also support callbacks due
  to signals or regular timeouts.
  Libevent是一个用于开发可扩展网络服务器的事件通知库。Libevent API提供了一种机制，用于在文件描述符上发生特定事件或达到超时后执行回调函数。
  此外，Libevent还支持由于信号或常规超时而产生的回调。

  Libevent is meant to replace the event loop found in event driven network
  servers. An application just needs to call event_base_dispatch() and then add or
  remove events dynamically without having to change the event loop.
  Libevent旨在替换事件驱动网络服务器中的事件循环。应用程序只需要调用event_base_dispatch（），然后动态添加或删除事件，而无需更改事件循环。


  Currently, Libevent supports /dev/poll, kqueue(2), select(2), poll(2),
  epoll(4), and evports. The internal event mechanism is completely
  independent of the exposed event API, and a simple update of Libevent can
  provide new functionality without having to redesign the applications. As a
  result, Libevent allows for portable application development and provides
  the most scalable event notification mechanism available on an operating
  system.  Libevent can also be used for multithreaded programs.  Libevent
  should compile on Linux, *BSD, Mac OS X, Solaris and, Windows.
  目前，Libevent支持/dev/poll、kqueue（2）、select（2）和poll（2），epoll（4）以及evports。
  内部事件机制完全独立于公开事件API，Libevent的简单更新可以提供新的功能，而无需重新设计应用程序。
  因此，Libevent允许便携式应用程序开发，并提供了操作系统上可用的最可扩展的事件通知机制。
  Libevent也可用于多线程程序。Libevent应该在Linux、*BSD、Mac OS X、Solaris和Windows上编译。


  @section usage Standard usage 标准用法

  Every program that uses Libevent must include the <event2/event.h>
  header, and pass the -levent flag to the linker.  (You can instead link
  -levent_core if you only want the main event and buffered IO-based code,
  and don't want to link any protocol code.)
  使用Libevent的每个程序都必须包含<event2/event.h>标头，并将-levent标志传递给链接器。
  （如果你只想要主事件和缓冲的基于IO的代码，而不想链接任何协议代码，你可以改为链接-levent_core。）

  @section setup Library setup 库设置

  Before you call any other Libevent functions, you need to set up the
  library.  If you're going to use Libevent from multiple threads in a
  multithreaded application, you need to initialize thread support --
  typically by using evthread_use_pthreads() or
  evthread_use_windows_threads().  See <event2/thread.h> for more
  information.
  在调用任何其他Libevent函数之前，您需要设置库。
  如果要在多线程应用程序中的多个线程中使用Libevent，则需要初始化线程支持，通常使用 evthread_use_pthreads （）或 evthread_use_windows_threads （）。
  有关更多信息，请参阅<event2/thread.h>。

  This is also the point where you can replace Libevent's memory
  management functions with event_set_mem_functions, and enable debug mode
  with event_enable_debug_mode().
  这也是您可以用 event_set_mem_functions 替换Libevent的内存管理函数，并用 event_enable_debug_mode （）启用调试模式的地方。

  @section base Creating an event base 创建事件库

  Next, you need to create an event_base structure, using event_base_new()
  or event_base_new_with_config().  The event_base is responsible for
  keeping track of which events are "pending" (that is to say, being
  watched to see if they become active) and which events are "active".
  Every event is associated with a single event_base.
  接下来，您需要使用 event_base_new （）或 event_base_new_with_config （）创建一个event_base结构。
  event_base负责跟踪哪些事件处于“待定”状态（也就是说，被监视以查看它们是否处于活动状态）以及哪些事件处于活动状态。
  每个事件都与一个event_base相关联。

  @section event Event notification 事件通知

  For each file descriptor that you wish to monitor, you must create an
  event structure with event_new().  (You may also declare an event
  structure and call event_assign() to initialize the members of the
  structure.)  To enable notification, you add the structure to the list
  of monitored events by calling event_add().  The event structure must
  remain allocated as long as it is active, so it should generally be
  allocated on the heap.
  对于要监视的每个文件描述符，必须使用event_new（）创建一个事件结构。
  （您还可以声明一个事件结构，并调用event_assign（）来初始化该结构的成员。）要启用通知，您可以通过调用event_add（）将该结构添加到监视的事件列表中。
  只要事件结构处于活动状态，它就必须保持分配状态，因此通常应该在堆上分配。

  @section loop Dispatching events. 调度事件。

  Finally, you call event_base_dispatch() to loop and dispatch events.
  You can also use event_base_loop() for more fine-grained control.
  最后，您调用event_base_dispatch（）来循环和分派事件。您还可以使用event_base_loop（）进行更细粒度的控制。

  Currently, only one thread can be dispatching a given event_base at a
  time.  If you want to run events in multiple threads at once, you can
  either have a single event_base whose events add work to a work queue,
  or you can create multiple event_base objects.
  目前，一次只能有一个线程调度给定的event_base。如果你想一次在多个线程中运行事件，你可以有一个event_base，其事件将工作添加到工作队列中，也可以创建多个event_base对象。

  @section bufferevent I/O Buffers I/O缓冲器

  Libevent provides a buffered I/O abstraction on top of the regular event
  callbacks. This abstraction is called a bufferevent. A bufferevent
  provides input and output buffers that get filled and drained
  automatically. The user of a buffered event no longer deals directly
  with the I/O, but instead is reading from input and writing to output
  buffers.
  Libevent在常规事件回调之上提供了一个缓冲I/O抽象。这种抽象称为bufferevent。bufferevent提供自动填充和排空的输入和输出缓冲区。
  缓冲事件的用户不再直接处理I/O，而是从输入缓冲区读取并写入输出缓冲区。

  Once initialized via bufferevent_socket_new(), the bufferevent structure
  can be used repeatedly with bufferevent_enable() and
  bufferevent_disable().  Instead of reading and writing directly to a
  socket, you would call bufferevent_read() and bufferevent_write().
  一旦通过bufferevent_socket_new（）初始化，bufferevent结构就可以与bufferevent_enable（）和bufferevent_disable（）重复使用。
  您将调用bufferevent_read（）和bufferevent_write（），而不是直接读写套接字。

  When read enabled the bufferevent will try to read from the file descriptor
  and call the read callback. The write callback is executed whenever the
  output buffer is drained below the write low watermark, which is 0 by
  default.
  启用读取后，bufferevent将尝试从文件描述符中读取并调用读取回调。
  每当输出缓冲区耗尽到低于写低水位线（默认为0）时，就会执行写回调。

  See <event2/bufferevent*.h> for more information.

  @section timers Timers 定时器

  Libevent can also be used to create timers that invoke a callback after a
  certain amount of time has expired. The evtimer_new() macro returns
  an event struct to use as a timer. To activate the timer, call
  evtimer_add(). Timers can be deactivated by calling evtimer_del().
  (These macros are thin wrappers around event_new(), event_add(),
  and event_del(); you can also use those instead.)
  Libevent还可以用于创建计时器，在一定时间到期后调用回调。evtimer_new（）宏返回一个事件结构体用作计时器。
  要激活计时器，请调用evtimer_add（）。可以通过调用evtimer_del（）来停用计时器。
  （这些宏是event_new（）、event_add（）和event_del（）的精简包装器；你也可以用这些来代替。）

  @section evdns Asynchronous DNS resolution 异步DNS解析

  Libevent provides an asynchronous DNS resolver that should be used instead
  of the standard DNS resolver functions.  See the <event2/dns.h>
  functions for more detail.
  Libevent提供了一个异步DNS解析器，应该使用它来代替标准的DNS解析器函数。有关更多详细信息，请参阅<event2/dns.h>函数。

  @section evhttp Event-driven HTTP servers 事件驱动的HTTP服务器

  Libevent provides a very simple event-driven HTTP server that can be
  embedded in your program and used to service HTTP requests.
  Libevent提供了一个非常简单的事件驱动HTTP服务器，可以嵌入到您的程序中，用于为HTTP请求提供服务。

  To use this capability, you need to include the <event2/http.h> header in your
  program.  See that header for more information.
  要使用此功能，您需要在程序中包含<event2/http.h>标头。有关更多信息，请参阅标题。

  @section evrpc A framework for RPC servers and clients RPC服务器和客户端的框架

  Libevent provides a framework for creating RPC servers and clients.  It
  takes care of marshaling and unmarshaling all data structures.
  Libevent提供了一个用于创建RPC服务器和客户端的框架。它负责编组和解组所有数据结构。

  @section api API Reference API参考

  To browse the complete documentation of the libevent API, click on any of
  the following links.
  要浏览libevent API的完整文档，请单击以下任何链接。

  event2/event.h
  The primary libevent header libevent主头文件

  event2/thread.h
  Functions for use by multithreaded programs 多线程程序使用的函数

  event2/buffer.h and event2/bufferevent.h
  Buffer management for network reading and writing 网络读写缓冲区管理

  event2/util.h
  Utility functions for portable nonblocking network code   可移植非阻塞网络代码的实用函数

  event2/dns.h
  Asynchronous DNS resolution 异步DNS解析

  event2/http.h
  An embedded libevent-based HTTP server  基于libevent的嵌入式HTTP服务器

  event2/rpc.h
  A framework for creating RPC servers and clients    用于创建RPC服务器和客户端的框架

 */

/** @file event2/event.h

  Core functions for waiting for and receiving events, and using event bases.
  等待和接收事件以及使用事件库的核心功能。
*/

#include <event2/visibility.h>

#ifdef __cplusplus
extern "C" {
#endif

#include <event2/event-config.h>
#ifdef EVENT__HAVE_SYS_TYPES_H
#include <sys/types.h>
#endif
#ifdef EVENT__HAVE_SYS_TIME_H
#include <sys/time.h>
#endif

#include <stdio.h>

/* For int types. */
#include <event2/util.h>

/**
 * Structure to hold information and state for a Libevent dispatch loop.
 * 用于保存Libevent调度循环的信息和状态的结构。
 *
 * The event_base lies at the center of Libevent; every application will
 * have one.  It keeps track of all pending and active events, and
 * notifies your application of the active ones.
 * event_base 位于Libevent的中心；每个应用程序都有一个。它跟踪所有未决和活动事件，并将活动事件通知您的应用程序。
 *
 * This is an opaque structure; you can allocate one using
 * event_base_new() or event_base_new_with_config().
 * 这是一个不透明的结构；您可以使用event_base_new（）或event_base_new_with_config（）分配一个。
 *
 * @see event_base_new(), event_base_free(), event_base_loop(),
 *    event_base_new_with_config()
 */
struct event_base
#ifdef EVENT_IN_DOXYGEN_
{/*Empty body so that doxygen will generate documentation here.*/}
#endif
;

/**
 * @struct event
 *
 * Structure to represent a single event.
 * 表示单个事件的结构。
 *
 * An event can have some underlying condition it represents: a socket
 * becoming readable or writeable (or both), or a signal becoming raised.
 * (An event that represents no underlying condition is still useful: you
 * can use one to implement a timer, or to communicate between threads.)
 * 一个事件可以有它所代表的一些潜在条件：套接字变得可读或可写（或两者兼而有之），或者发出信号。
 * （表示没有底层条件的事件仍然有用：您可以使用一个来实现计时器，或在线程之间进行通信。）
 *
 * Generally, you can create events with event_new(), then make them
 * pending with event_add().  As your event_base runs, it will run the
 * callbacks of an events whose conditions are triggered.  When you no
 * longer want the event, free it with event_free().
 * 通常，您可以使用event_new（）创建事件，然后使用event_add（）使其挂起。
 * 当您的event_base运行时，它将运行条件被触发的事件的回调。当你不再需要这个事件时，用event_free（）释放它。
 *
 * In more depth: 更深入地说：
 *
 * An event may be "pending" (one whose condition we are watching),
 * "active" (one whose condition has triggered and whose callback is about
 * to run), neither, or both.  Events come into existence via
 * event_assign() or event_new(), and are then neither active nor pending.
 * 一个事件可能是“待定”（我们正在观察其状态）、“活动”（其状态已触发且其回调即将运行）、两者都不是或两者都是。
 * 事件通过event_assign（）或event_new（）产生，然后既不是活动的也不是挂起的。
 *
 * To make an event pending, pass it to event_add().  When doing so, you
 * can also set a timeout for the event.
 * 要使事件挂起，请将其传递给event_add（）。执行此操作时，您还可以为事件设置超时。
 *
 * Events become active during an event_base_loop() call when either their
 * condition has triggered, or when their timeout has elapsed.  You can
 * also activate an event manually using event_active().  The even_base
 * loop will run the callbacks of active events; after it has done so, it
 * marks them as no longer active.
 * 在event_base_loop（）调用期间，当事件的条件被触发或超时时，事件将变为活动状态。
 * 您还可以使用event_active（）手动激活事件。even_base循环将运行活动事件的回调；完成此操作后，它将它们标记为不再活动。
 *
 * You can make an event non-pending by passing it to event_del().  This
 * also makes the event non-active.
 * 您可以通过将事件传递给event_del（）来使其非挂起。这也使事件处于非活动状态。
 *
 * Events can be "persistent" or "non-persistent".  A non-persistent event
 * becomes non-pending as soon as it is triggered: thus, it only runs at
 * most once per call to event_add().  A persistent event remains pending
 * even when it becomes active: you'll need to event_del() it manually in
 * order to make it non-pending.  When a persistent event with a timeout
 * becomes active, its timeout is reset: this means you can use persistent
 * events to implement periodic timeouts.
 * 事件可以是“持久的”或“非持久的”。非持久性事件一经触发就变为非挂起：因此，每次调用event_add（）最多只运行一次。
 * 持久事件即使处于活动状态，也仍然处于挂起状态：您需要手动event_del（）将其设置为非挂起状态。
 * 当具有超时的持久事件变为活动状态时，其超时将被重置：这意味着您可以使用持久事件来实现定期超时。
 *
 * This should be treated as an opaque structure; you should never read or
 * write any of its fields directly.  For backward compatibility with old
 * code, it is defined in the event2/event_struct.h header; including this
 * header may make your code incompatible with other versions of Libevent.
 * 这应该被视为一个不透明的结构；你永远不应该直接读或写它的任何字段。
 * 为了与旧代码向后兼容，它在event2/event_struct.h标头中定义；包含此标头可能会使您的代码与其他版本的Libevent不兼容。
 *
 * @see event_new(), event_free(), event_assign(), event_get_assignment(),
 *    event_add(), event_del(), event_active(), event_pending(),
 *    event_get_fd(), event_get_base(), event_get_events(),
 *    event_get_callback(), event_get_callback_arg(),
 *    event_priority_set()
 */
struct event
#ifdef EVENT_IN_DOXYGEN_
{/*Empty body so that doxygen will generate documentation here. 清空正文，以便doxygen在此处生成文档。*/}
#endif
;

/**
 * Configuration for an event_base. event_base的配置。
 *
 * There are many options that can be used to alter the behavior and
 * implementation of an event_base.  To avoid having to pass them all in a
 * complex many-argument constructor, we provide an abstract data type
 * where you set up configuration information before passing it to
 * event_base_new_with_config().
 * 有许多选项可用于更改event_base的行为和实现。
 * 为了避免在复杂的多参数构造函数中传递所有参数，我们提供了一种抽象数据类型，在将配置信息传递给event_base_new_with_config（）之前，您可以在其中设置配置信息。
 *
 * @see event_config_new(), event_config_free(), event_base_new_with_config(),
 *   event_config_avoid_method(), event_config_require_features(),
 *   event_config_set_flag(), event_config_set_num_cpus_hint()
 */
struct event_config
#ifdef EVENT_IN_DOXYGEN_
{/*Empty body so that doxygen will generate documentation here.*/}
#endif
;

/**
 * Enable some relatively expensive debugging checks in Libevent that
 * would normally be turned off.  Generally, these checks cause code that
 * would otherwise crash mysteriously to fail earlier with an assertion
 * failure.  Note that this method MUST be called before any events or
 * event_bases have been created.
 * 在Libevent中启用一些通常会关闭的相对昂贵的调试检查。通常，这些检查会导致原本会神秘崩溃的代码在断言失败时提前失败。
 * 请注意，必须在创建任何事件或event_bases之前调用此方法。
 *
 * Debug mode can currently catch the following errors: 调试模式当前可以捕获以下错误：
 *    An event is re-assigned while it is added    添加事件时会重新分配事件
 *    Any function is called on a non-assigned event  在未指定的事件上调用任何函数
 *
 * Note that debugging mode uses memory to track every event that has been
 * initialized (via event_assign, event_set, or event_new) but not yet
 * released (via event_free or event_debug_unassign).  If you want to use
 * debug mode, and you find yourself running out of memory, you will need
 * to use event_debug_unassign to explicitly stop tracking events that
 * are no longer considered set-up.
 * 请注意，调试模式使用内存来跟踪已初始化（通过event_assign、event_set或event_new）但尚未发布（通过event_free或event_debug_unassign）的每个事件。
 * 如果您想使用调试模式，并且发现内存不足，则需要使用event_debug_unassign显式停止跟踪不再被视为设置的事件。
 *
 * @see event_debug_unassign()
 */
EVENT2_EXPORT_SYMBOL
void event_enable_debug_mode(void);

/**
 * When debugging mode is enabled, informs Libevent that an event should no
 * longer be considered as assigned. When debugging mode is not enabled, does
 * nothing.
 * 启用调试模式后，通知Libevent某个事件不应再被视为已分配。未启用调试模式时，不执行任何操作。
 *
 * This function must only be called on a non-added event.
 * 此函数只能在未添加的事件上调用。
 *
 * @see event_enable_debug_mode()
 */
EVENT2_EXPORT_SYMBOL
void event_debug_unassign(struct event *);

/**
 * Create and return a new event_base to use with the rest of Libevent.
 * 创建并返回一个新的event_base，以与Libevent的其余部分一起使用。
 *
 * @return a new event_base on success, or NULL on failure.
 *
 * @see event_base_free(), event_base_new_with_config()
 */
EVENT2_EXPORT_SYMBOL
struct event_base *event_base_new(void);

/**
  Reinitialize the event base after a fork
  分叉后重新初始化事件库

  Some event mechanisms do not survive across fork.   The event base needs
  to be reinitialized with the event_reinit() function.
  某些事件机制在fork中无法生存。需要使用event_reinit（）函数重新初始化事件库。

  @param base the event base that needs to be re-initialized
  @return 0 if successful, or -1 if some events could not be re-added.
  @see event_base_new()
*/
EVENT2_EXPORT_SYMBOL
int event_reinit(struct event_base *base);

/**
   Event dispatching loop 事件调度循环

  This loop will run the event base until either there are no more pending or
  active, or until something calls event_base_loopbreak() or
  event_base_loopexit().
  此循环将运行事件库，直到不再有挂起或活动的事件，或者直到有东西调用event_base_loopbreak（）或event_base_loopexit（）。

  @param base the event_base structure returned by event_base_new() or
     event_base_new_with_config()
     event_base_new（）或event_base_new_with_config（）返回的event_base结构
  @return 0 if successful, -1 if an error occurred, or 1 if we exited because
     no events were pending or active.
  @see event_base_loop()
 */
EVENT2_EXPORT_SYMBOL
int event_base_dispatch(struct event_base *);

/**
 Get the kernel event notification mechanism used by Libevent.
 获取Libevent使用的内核事件通知机制。

 @param eb the event_base structure returned by event_base_new() event_base_new（）返回的event_base结构
 @return a string identifying the kernel event mechanism (kqueue, epoll, etc.)   标识内核事件机制（kqueue、epoll等）的字符串
 */
EVENT2_EXPORT_SYMBOL
const char *event_base_get_method(const struct event_base *);

/**
   Gets all event notification mechanisms supported by Libevent.
   获取Libevent支持的所有事件通知机制。

   This functions returns the event mechanism in order preferred by
   Libevent.  Note that this list will include all backends that
   Libevent has compiled-in support for, and will not necessarily check
   your OS to see whether it has the required resources.
   此函数按Libevent首选的顺序返回事件机制。请注意，此列表将包括Libevent为支持而编译的所有后端，并且不一定会检查您的操作系统是否具有所需的资源。

   @return an array with pointers to the names of support methods.
     The end of the array is indicated by a NULL pointer.  If an
     error is encountered NULL is returned.
     一个带有指向支持方法名称的指针的数组。数组的末尾由NULL指针表示。如果遇到错误，则返回NULL。
*/
EVENT2_EXPORT_SYMBOL
const char **event_get_supported_methods(void);

/** Query the current monotonic time from a the timer for a struct
 * event_base. 从结构event_base的计时器中查询当前单调时间。
 */
EVENT2_EXPORT_SYMBOL
int event_gettime_monotonic(struct event_base *base, struct timeval *tp);

/**
   @name event type flag   事件类型标志

   Flags to pass to event_base_get_num_events() to specify the kinds of events
   we want to aggregate counts for
   传递给event_base_get_num_events（）的标志，用于指定我们要聚合计数的事件类型
*/
/**@{*/
/** count the number of active events, which have been triggered. 统计已触发的活动事件的数量。 */
#define EVENT_BASE_COUNT_ACTIVE                1U
/** count the number of virtual events, which is used to represent an internal
 * condition, other than a pending event, that keeps the loop from exiting. 
 * 统计虚拟事件的数量，该数量用于表示阻止循环退出的内部条件，而不是挂起的事件。 */
#define EVENT_BASE_COUNT_VIRTUAL       2U
/** count the number of events which have been added to event base, including
 * internal events. 统计已添加到事件库中的事件数，包括内部事件。 */
#define EVENT_BASE_COUNT_ADDED         4U
/**@}*/

/**
   Gets the number of events in event_base, as specified in the flags.
   获取event_base中的事件数，如标志中所指定。

   Since event base has some internal events added to make some of its
   functionalities work, EVENT_BASE_COUNT_ADDED may return more than the
   number of events you added using event_add().
   由于事件库添加了一些内部事件以使其某些功能正常工作，EVENT_BASE_COUNT_ADDED可能会返回的事件数超过您使用event_add（）添加的事件数。

   If you pass EVENT_BASE_COUNT_ACTIVE and EVENT_BASE_COUNT_ADDED together, an
   active event will be counted twice. However, this might not be the case in
   future libevent versions.  The return value is an indication of the work
   load, but the user shouldn't rely on the exact value as this may change in
   the future.
   如果同时传递EVENT_BASE_COUNT_ACTIVE和EVENT_BASE_COUNT_ADDED，则活动事件将被计数两次。
   然而，在未来的libevent版本中可能不会出现这种情况。
   返回值是工作量的指示，但用户不应依赖确切的值，因为这可能会在未来发生变化。

   @param eb the event_base structure returned by event_base_new()
   @param flags a bitwise combination of the kinds of events to aggregate
       counts for    按位组合事件类型以聚合计数
   @return the number of events specified in the flags   标志中指定的事件数
*/
EVENT2_EXPORT_SYMBOL
int event_base_get_num_events(struct event_base *, unsigned int);

/**
  Get the maximum number of events in a given event_base as specified in the
  flags.
  获取标志中指定的给定event_base中的最大事件数。

  @param eb the event_base structure returned by event_base_new()
  @param flags a bitwise combination of the kinds of events to aggregate
         counts for  按位组合事件类型以聚合计数
  @param clear option used to reset the maximum count.   用于重置最大计数的选项
  @return the number of events specified in the flags 标志中指定的事件数
 */
EVENT2_EXPORT_SYMBOL
int event_base_get_max_events(struct event_base *, unsigned int, int);

/**
   Allocates a new event configuration object.
   分配新的事件配置对象。

   The event configuration object can be used to change the behavior of
   an event base.
   事件配置对象可用于更改事件库的行为。

   @return an event_config object that can be used to store configuration, or
     NULL if an error is encountered.、
     可用于存储配置的event_config对象，如果遇到错误，则为NULL。
   @see event_base_new_with_config(), event_config_free(), event_config
*/
EVENT2_EXPORT_SYMBOL
struct event_config *event_config_new(void);

/**
   Deallocates all memory associated with an event configuration object
   取消分配与事件配置对象关联的所有内存

   @param cfg the event configuration object to be freed.   要释放的事件配置对象。
*/
EVENT2_EXPORT_SYMBOL
void event_config_free(struct event_config *cfg);

/**
   Enters an event method that should be avoided into the configuration.
   在配置中输入应避免的事件方法。

   This can be used to avoid event mechanisms that do not support certain
   file descriptor types, or for debugging to avoid certain event
   mechanisms.  An application can make use of multiple event bases to
   accommodate incompatible file descriptor types.
   这可用于避免不支持某些文件描述符类型的事件机制，或用于调试以避免某些事件机制。应用程序可以利用多个事件库来容纳不兼容的文件描述符类型。

   @param cfg the event configuration object    事件配置对象
   @param method the name of the event method to avoid   要避免的事件方法的名称
   @return 0 on success, -1 on failure.
*/
EVENT2_EXPORT_SYMBOL
int event_config_avoid_method(struct event_config *cfg, const char *method);

/**
   A flag used to describe which features an event_base (must) provide.
   用于描述event_base（必须）提供哪些功能的标志。

   Because of OS limitations, not every Libevent backend supports every
   possible feature.  You can use this type with
   event_config_require_features() to tell Libevent to only proceed if your
   event_base implements a given feature, and you can receive this type from
   event_base_get_features() to see which features are available.
   由于操作系统的限制，并非每个Libevent后端都支持所有可能的功能。您可以将此类型与event_config_require_features（）一起使用，
   以告诉Libevent只有在您的event_base实现了给定的功能时才能继续，并且您可以从event_base_get_features（）接收此类型，以查看哪些功能可用。
*/
enum event_method_feature {
    /** Require an event method that allows edge-triggered events with EV_ET. 需要一种允许EV_ET发生边缘触发事件的事件方法。 */
    EV_FEATURE_ET = 0x01,
    /** Require an event method where having one event triggered among
     * many is [approximately] an O(1) operation. This excludes (for
     * example) select and poll, which are approximately O(N) for N
     * equal to the total number of possible events. 
     * 需要一种事件方法，其中在许多事件中触发一个事件[大约]是O（1）操作。这不包括（例如）选择和轮询，对于可能事件的总数而言，N l大约为O（N）。*/
    EV_FEATURE_O1 = 0x02,
    /** Require an event method that allows file descriptors as well as
     * sockets. 需要一个允许文件描述符和套接字的事件方法。 */
    EV_FEATURE_FDS = 0x04,
    /** Require an event method that allows you to use EV_CLOSED to detect
     * connection close without the necessity of reading all the pending data.
     * 需要一种事件方法，允许您使用EV_CLOSED检测连接关闭，而无需读取所有挂起的数据。
     *
     * Methods that do support EV_CLOSED may not be able to provide support on
     * all kernel versions.
     * 支持EV_CLOSED的方法可能无法在所有内核版本上提供支持。
     **/
    EV_FEATURE_EARLY_CLOSE = 0x08
};

/**
   A flag passed to event_config_set_flag().
   传递给event_config_set_flag（）的标志。

    These flags change the behavior of an allocated event_base.
    这些标志会改变已分配事件库的行为。

    @see event_config_set_flag(), event_base_new_with_config(),
       event_method_feature
 */
enum event_base_config_flag {
	/** Do not allocate a lock for the event base, even if we have
	    locking set up.
       即使我们设置了锁，也不要为事件库分配锁。

	    Setting this option will make it unsafe and nonfunctional to call
	    functions on the base concurrently from multiple threads.
       设置此选项将使从多个线程并发调用基上的函数变得不安全且不起作用。
	*/
	EVENT_BASE_FLAG_NOLOCK = 0x01,
	/** Do not check the EVENT_* environment variables when configuring
	    an event_base    配置EVENT_base时不要检查EVENT_*环境变量 */
	EVENT_BASE_FLAG_IGNORE_ENV = 0x02,
	/** Windows only: enable the IOCP dispatcher at startup
    * 仅限Windows：启动时启用IOCP调度程序

	    If this flag is set then bufferevent_socket_new() and
	    evconn_listener_new() will use IOCP-backed implementations
	    instead of the usual select-based one on Windows.
       如果设置了此标志，则bufferevent_socket_new（）和evconn_listener_new（）将使用IOCP支持的实现，而不是Windows上通常的基于选择的实现。
	 */
	EVENT_BASE_FLAG_STARTUP_IOCP = 0x04,
	/** Instead of checking the current time every time the event loop is
	    ready to run timeout callbacks, check after each timeout callback.
       不要每次事件循环准备运行超时回调时都检查当前时间，而是在每次超时回调后进行检查。
	 */
	EVENT_BASE_FLAG_NO_CACHE_TIME = 0x08,

	/** If we are using the epoll backend, this flag says that it is
	    safe to use Libevent's internal change-list code to batch up
	    adds and deletes in order to try to do as few syscalls as
	    possible.  Setting this flag can make your code run faster, but
	    it may trigger a Linux bug: it is not safe to use this flag
	    if you have any fds cloned by dup() or its variants.  Doing so
	    will produce strange and hard-to-diagnose bugs.
       如果我们使用epoll后端，此标志表示使用Libevent的内部更改列表代码批量添加和删除是安全的，以便尽量减少系统调用。
       设置此标志可以使您的代码运行得更快，但它可能会触发Linux错误：如果您有任何由dup（）或其变体克隆的fds，则使用此标志是不安全的。这样做会产生奇怪且难以诊断的错误。

	    This flag can also be activated by setting the
	    EVENT_EPOLL_USE_CHANGELIST environment variable.
       此标志也可以通过设置EVENT_EPOLL_USE_CHANGELIST环境变量来激活。

	    This flag has no effect if you wind up using a backend other than
	    epoll.
       如果您最终使用epoll以外的后端，则此标志无效。
	 */
	EVENT_BASE_FLAG_EPOLL_USE_CHANGELIST = 0x10,

	/** Ordinarily, Libevent implements its time and timeout code using
	    the fastest monotonic timer that we have.  If this flag is set,
	    however, we use less efficient more precise timer, assuming one is
	    present.
       通常，Libevent使用我们拥有的最快的单调计时器来实现其时间和超时代码。然而，如果设置了此标志，我们将使用效率较低但更精确的计时器，假设存在计时器。
	 */
	EVENT_BASE_FLAG_PRECISE_TIMER = 0x20
};

/**
   Return a bitmask of the features implemented by an event base.  This
   will be a bitwise OR of one or more of the values of
   event_method_feature
   返回事件库实现的功能的位掩码。这将是event_method_feature的一个或多个值的按位OR

   @see event_method_feature
 */
EVENT2_EXPORT_SYMBOL
int event_base_get_features(const struct event_base *base);

/**
   Enters a required event method feature that the application demands.
   输入应用程序所需的事件方法功能。

   Note that not every feature or combination of features is supported
   on every platform.  Code that requests features should be prepared
   to handle the case where event_base_new_with_config() returns NULL, as in:
   请注意，并非每个平台都支持每个功能或功能组合。请求功能的代码应该准备好处理event_base_new_with_config（）返回NULL的情况，如下所示：
   <pre>
     event_config_require_features(cfg, EV_FEATURE_ET);
     base = event_base_new_with_config(cfg);
     if (base == NULL) {
       // We can't get edge-triggered behavior here.
       event_config_require_features(cfg, 0);
       base = event_base_new_with_config(cfg);
     }
   </pre>

   @param cfg the event configuration object
   @param feature a bitfield of one or more event_method_feature values.
          Replaces values from previous calls to this function.
          一个或多个event_method_feature值的位字段。替换以前调用此函数时的值。
   @return 0 on success, -1 on failure.
   @see event_method_feature, event_base_new_with_config()
*/
EVENT2_EXPORT_SYMBOL
int event_config_require_features(struct event_config *cfg, int feature);

/**
 * Sets one or more flags to configure what parts of the eventual event_base
 * will be initialized, and how they'll work.
 * 设置一个或多个标志，以配置最终事件库的哪些部分将被初始化，以及它们将如何工作。
 *
 * @see event_base_config_flags, event_base_new_with_config()
 **/
EVENT2_EXPORT_SYMBOL
int event_config_set_flag(struct event_config *cfg, int flag);

/**
 * Records a hint for the number of CPUs in the system. This is used for
 * tuning thread pools, etc, for optimal performance.  In Libevent 2.0,
 * it is only on Windows, and only when IOCP is in use.
 * 记录系统中CPU数量的提示。这用于调整线程池等，以获得最佳性能。在Libevent 2.0中，它仅在Windows上运行，并且仅在使用IOCP时运行。
 *
 * @param cfg the event configuration object
 * @param cpus the number of cpus
 * @return 0 on success, -1 on failure.
 */
EVENT2_EXPORT_SYMBOL
int event_config_set_num_cpus_hint(struct event_config *cfg, int cpus);

/**
 * Record an interval and/or a number of callbacks after which the event base
 * should check for new events.  By default, the event base will run as many
 * events are as activated at the highest activated priority before checking
 * for new events.  If you configure it by setting max_interval, it will check
 * the time after each callback, and not allow more than max_interval to
 * elapse before checking for new events.  If you configure it by setting
 * max_callbacks to a value >= 0, it will run no more than max_callbacks
 * callbacks before checking for new events.
 * 记录一个时间间隔和/或多次回调，之后事件库应检查新事件。默认情况下，在检查新事件之前，事件库将以最高激活优先级运行尽可能多的事件。
 * 如果您通过设置max_interval来配置它，它将在每次回调后检查时间，并且在检查新事件之前不允许超过max_interval。
 * 如果通过将max_callbacks设置为>=0来配置它，则在检查新事件之前，它将运行不超过max_accolls回调。
 *
 * This option can decrease the latency of high-priority events, and
 * avoid priority inversions where multiple low-priority events keep us from
 * polling for high-priority events, but at the expense of slightly decreasing
 * the throughput.  Use it with caution!
 * 此选项可以减少高优先级事件的延迟，并避免优先级反转，即多个低优先级事件使我们无法轮询高优先级事件，但以略微降低吞吐量为代价。小心使用！
 *
 * @param cfg The event_base configuration object.
 * @param max_interval An interval after which Libevent should stop running
 *     callbacks and check for more events, or NULL if there should be
 *     no such interval.   Libevent应停止运行回调并检查更多事件的间隔，如果没有这样的间隔，则为NULL。
 * @param max_callbacks A number of callbacks after which Libevent should
 *     stop running callbacks and check for more events, or -1 if there
 *     should be no such limit.  多个回调，之后Libevent应停止运行回调并检查更多事件，如果没有这样的限制，则为-1。
 * @param min_priority A priority below which max_interval and max_callbacks
 *     should not be enforced.  If this is set to 0, they are enforced
 *     for events of every priority; if it's set to 1, they're enforced
 *     for events of priority 1 and above, and so on.
 *     一个优先级，低于该优先级，不应强制执行max_interval和max_accalls。
 *     如果将其设置为0，则对每个优先级的事件都强制执行；如果设置为1，则对优先级为1及以上的事件强制执行，依此类推。
 * @return 0 on success, -1 on failure.
 **/
EVENT2_EXPORT_SYMBOL
int event_config_set_max_dispatch_interval(struct event_config *cfg,
    const struct timeval *max_interval, int max_callbacks,
    int min_priority);

/**
  Initialize the event API.   初始化事件API。

  Use event_base_new_with_config() to initialize a new event base, taking
  the specified configuration under consideration.  The configuration object
  can currently be used to avoid certain event notification mechanisms.
  使用event_base_new_with_config（）初始化新的事件库，同时考虑指定的配置。配置对象当前可用于避免某些事件通知机制。

  @param cfg the event configuration object
  @return an initialized event_base that can be used to registering events,
     or NULL if no event base can be created with the requested event_config.
     一个初始化的event_base，可用于注册事件，如果无法使用请求的event_config创建事件库，则为NULL。
  @see event_base_new(), event_base_free(), event_init(), event_assign()
*/
EVENT2_EXPORT_SYMBOL
struct event_base *event_base_new_with_config(const struct event_config *);

/**
  Deallocate all memory associated with an event_base, and free the base.
  取消分配与事件库关联的所有内存，并释放该库。

  Note that this function will not close any fds or free any memory passed
  to event_new as the argument to callback.
  请注意，此函数不会关闭任何fds或释放作为回调参数传递给event_new的任何内存。

  If there are any pending finalizer callbacks, this function will invoke
  them.
  如果有任何未决的终结器回调，此函数将调用它们。

  @param eb an event_base to be freed
 */
EVENT2_EXPORT_SYMBOL
void event_base_free(struct event_base *);

/**
   As event_base_free, but do not run finalizers.  作为event_base_free，但不运行终结器。
 */
EVENT2_EXPORT_SYMBOL
void event_base_free_nofinalize(struct event_base *);

/** @name Log severities
 */
/**@{*/
#define EVENT_LOG_DEBUG 0
#define EVENT_LOG_MSG   1
#define EVENT_LOG_WARN  2
#define EVENT_LOG_ERR   3
/**@}*/

/* Obsolete names: these are deprecated, but older programs might use them.   过时的名称：这些名称已被弃用，但旧程序可能会使用它们。
 * They violate the reserved-identifier namespace. 它们违反了保留的标识符命名空间。 */
#define _EVENT_LOG_DEBUG EVENT_LOG_DEBUG
#define _EVENT_LOG_MSG EVENT_LOG_MSG
#define _EVENT_LOG_WARN EVENT_LOG_WARN
#define _EVENT_LOG_ERR EVENT_LOG_ERR

/**
  A callback function used to intercept Libevent's log messages.
  用于拦截Libevent日志消息的回调函数。

  @see event_set_log_callback
 */
typedef void (*event_log_cb)(int severity, const char *msg);
/**
  Redirect Libevent's log messages.
  重定向Libevent的日志消息。

  @param cb a function taking two arguments: an integer severity between
     EVENT_LOG_DEBUG and EVENT_LOG_ERR, and a string.  If cb is NULL,
	 then the default log is used.
    一个接受两个参数的函数：一个介于EVENT_LOG_DEBUG和EVENT_LOG_ERR之间的整数级别，以及一个字符串。如果cb为NULL，则使用默认日志。

  NOTE: The function you provide *must not* call any other libevent
  functionality.  Doing so can produce undefined behavior.
  您提供的函数*不得*调用任何其他libevent函数。这样做可能会产生未定义的行为。
  */
EVENT2_EXPORT_SYMBOL
void event_set_log_callback(event_log_cb cb);

/**
   A function to be called if Libevent encounters a fatal internal error.
   当Libevent遇到致命的内部错误时调用的函数。

   @see event_set_fatal_callback
 */
typedef void (*event_fatal_cb)(int err);

/**
 Override Libevent's behavior in the event of a fatal internal error.
 在发生致命内部错误时覆盖Libevent的行为。

 By default, Libevent will call exit(1) if a programming error makes it
 impossible to continue correct operation.  This function allows you to supply
 another callback instead.  Note that if the function is ever invoked,
 something is wrong with your program, or with Libevent: any subsequent calls
 to Libevent may result in undefined behavior.
 默认情况下，如果编程错误导致无法继续正确操作，Libevent将调用exit（1）。此函数允许您提供另一个回调。
 请注意，如果调用了该函数，则表明您的程序或Libevent出现了问题：任何后续对Libevent的调用都可能导致未定义的行为。

 Libevent will (almost) always log an EVENT_LOG_ERR message before calling
 this function; look at the last log message to see why Libevent has died.
 Libevent在调用此函数之前（几乎）总是会记录一条EVENT_log_ERR消息；查看最后一条日志消息，了解Libevent死亡的原因。
 */
EVENT2_EXPORT_SYMBOL
void event_set_fatal_callback(event_fatal_cb cb);

#define EVENT_DBG_ALL 0xffffffffu
#define EVENT_DBG_NONE 0

/**
 Turn on debugging logs and have them sent to the default log handler.
 打开调试日志并将其发送到默认日志处理程序。

 This is a global setting; if you are going to call it, you must call this
 before any calls that create an event-base.  You must call it before any
 multithreaded use of Libevent.
 这是一个全球性的环境；如果要调用它，则必须在任何创建事件库的调用之前调用它。在多线程使用Libevent之前，必须先调用它。

 Debug logs are verbose.   调试日志很冗长。

 @param which Controls which debug messages are turned on.  This option is
   unused for now; for forward compatibility, you must pass in the constant
   "EVENT_DBG_ALL" to turn debugging logs on, or "EVENT_DBG_NONE" to turn
   debugging logs off.
   控制启用哪些调试消息。此选项暂时不使用；为了实现前向兼容性，您必须传入常量“EVENT_DBG_ALL”以打开调试日志，或传入“EVENT_BG_NONE”以关闭调试日志。
 */
EVENT2_EXPORT_SYMBOL
void event_enable_debug_logging(ev_uint32_t which);

/**
  Associate a different event base with an event.  将不同的事件库与事件相关联。

  The event to be associated must not be currently active or pending.
  要关联的事件当前不得处于活动状态或挂起状态。

  @param eb the event base
  @param ev the event
  @return 0 on success, -1 on failure.
 */
EVENT2_EXPORT_SYMBOL
int event_base_set(struct event_base *, struct event *);

/** @name Loop flags 循环标志

    These flags control the behavior of event_base_loop().
    这些标志控制event_base_loop（）的行为。
 */
/**@{*/
/** Block until we have an active event, then exit once all active events
 * have had their callbacks run. 阻塞直到我们有一个活动事件，然后在所有活动事件都运行了回调后退出。 */
#define EVLOOP_ONCE	0x01
/** Do not block: see which events are ready now, run the callbacks
 * of the highest-priority ones, then exit. 不要阻塞：查看哪些事件现在已准备就绪，运行最高优先级事件的回调，然后退出。 */
#define EVLOOP_NONBLOCK	0x02
/** Do not exit the loop because we have no pending events.  Instead, keep
 * running until event_base_loopexit() or event_base_loopbreak() makes us
 * stop.
 * 不要退出循环，因为我们没有未决事件。相反，请继续运行，直到event_base_loopexit（）或event_base_loopbreak（）使我们停止。
 */
#define EVLOOP_NO_EXIT_ON_EMPTY 0x04
/**@}*/

/**
  Wait for events to become active, and run their callbacks.
  等待事件变为活动状态，并运行它们的回调。

  This is a more flexible version of event_base_dispatch().
  这是event_base_dispatch（）的一个更灵活的版本。

  By default, this loop will run the event base until either there are no more
  pending or active events, or until something calls event_base_loopbreak() or
  event_base_loopexit().  You can override this behavior with the 'flags'
  argument.
  默认情况下，此循环将运行事件库，直到没有更多挂起或活动的事件，或者直到有东西调用event_base_loopbreak（）或event_base_loopexit（）。您可以使用“flags”参数覆盖此行为。

  @param eb the event_base structure returned by event_base_new() or
     event_base_new_with_config()
     event_base_new（）或event_base_new_with_config（）返回的event_base结构
  @param flags any combination of EVLOOP_ONCE | EVLOOP_NONBLOCK   EVLOOP_ONCE|EVLOOP_NONBLOCK的任意组合
  @return 0 if successful, -1 if an error occurred, or 1 if we exited because
     no events were pending or active.
  @see event_base_loopexit(), event_base_dispatch(), EVLOOP_ONCE,
     EVLOOP_NONBLOCK
  */
EVENT2_EXPORT_SYMBOL
int event_base_loop(struct event_base *, int);

/**
  Exit the event loop after the specified time
  在指定时间后退出事件循环

  The next event_base_loop() iteration after the given timer expires will
  complete normally (handling all queued events) then exit without
  blocking for events again.
  给定计时器到期后的下一个event_base_loop（）迭代将正常完成（处理所有排队的事件），然后退出，不再阻塞事件。

  Subsequent invocations of event_base_loop() will proceed normally.
  后续对event_base_loop（）的调用将正常进行。

  @param eb the event_base structure returned by event_init()
  @param tv the amount of time after which the loop should terminate,
    or NULL to exit after running all currently active events.
    循环应终止的时间量，或在运行所有当前活动事件后退出NULL。
  @return 0 if successful, or -1 if an error occurred
  @see event_base_loopbreak()
 */
EVENT2_EXPORT_SYMBOL
int event_base_loopexit(struct event_base *, const struct timeval *);

/**
  Abort the active event_base_loop() immediately.
  立即中止活动的event_base_loop（）。

  event_base_loop() will abort the loop after the next event is completed;
  event_base_loopbreak() is typically invoked from this event's callback.
  This behavior is analogous to the "break;" statement.
  event_base_loop（）将在下一个事件完成后中止循环；event_base_loopbreak（）通常从该事件的回调调用。这种行为类似于“break；”语句。

  Subsequent invocations of event_base_loop() will proceed normally.
  后续对event_base_loop（）的调用将正常进行。

  @param eb the event_base structure returned by event_init()
  @return 0 if successful, or -1 if an error occurred
  @see event_base_loopexit()
 */
EVENT2_EXPORT_SYMBOL
int event_base_loopbreak(struct event_base *);

/**
  Tell the active event_base_loop() to scan for new events immediately.
  告诉活动的event_base_loop（）立即扫描新事件。

  Calling this function makes the currently active event_base_loop()
  start the loop over again (scanning for new events) after the current
  event callback finishes.  If the event loop is not running, this
  function has no effect.
  调用此函数会使当前活动的event_base_loop（）在当前事件回调完成后再次启动循环（扫描新事件）。如果事件循环未运行，则此函数无效。

  event_base_loopbreak() is typically invoked from this event's callback.
  This behavior is analogous to the "continue;" statement.
  event_base_loopbreak（）通常从该事件的回调调用。这种行为类似于“continue；”语句。

  Subsequent invocations of event loop will proceed normally.
  事件循环的后续调用将正常进行。

  @param eb the event_base structure returned by event_init()
  @return 0 if successful, or -1 if an error occurred
  @see event_base_loopbreak()
 */
EVENT2_EXPORT_SYMBOL
int event_base_loopcontinue(struct event_base *);

/**
  Checks if the event loop was told to exit by event_base_loopexit().
  检查事件循环是否被event_base_loopexit（）告知退出。

  This function will return true for an event_base at every point after
  event_loopexit() is called, until the event loop is next entered.
  此函数将在调用event_loopexit（）后的每个点为event_base返回true，直到下一次进入事件循环。

  @param eb the event_base structure returned by event_init()
  @return true if event_base_loopexit() was called on this event base,
    or 0 otherwise  如果在此事件库上调用了event_base_loopexit（），则为true，否则为0
  @see event_base_loopexit()
  @see event_base_got_break()
 */
EVENT2_EXPORT_SYMBOL
int event_base_got_exit(struct event_base *);

/**
  Checks if the event loop was told to abort immediately by event_base_loopbreak().
  检查事件循环是否被event_base_loopbreak（）告知立即中止。

  This function will return true for an event_base at every point after
  event_base_loopbreak() is called, until the event loop is next entered.
  此函数将在调用event_base_loopbreak（）后的每个点为event_base返回true，直到下一次进入事件循环。

  @param eb the event_base structure returned by event_init()
  @return true if event_base_loopbreak() was called on this event base,
    or 0 otherwise  如果在此事件库上调用了event_base_loopbreak（），则为true，否则为0
  @see event_base_loopbreak()
  @see event_base_got_exit()
 */
EVENT2_EXPORT_SYMBOL
int event_base_got_break(struct event_base *);

/**
 * @name event flags
 *
 * Flags to pass to event_new(), event_assign(), event_pending(), and
 * anything else with an argument of the form "short events"
 * 传递给event_new（）、event_assign（）、events_pending（）和任何其他具有“短事件”形式参数的标记
 */
/**@{*/
/** Indicates that a timeout has occurred.  It's not necessary to pass
 * this flag to event_for new()/event_assign() to get a timeout. 
 * 表示发生了超时。没有必要将此标志传递给event_for new（）/event_assign（）以获得超时。*/
#define EV_TIMEOUT	0x01
/** Wait for a socket or FD to become readable 等待套接字或FD变得可读 */
#define EV_READ		0x02
/** Wait for a socket or FD to become writeable 等待套接字或FD变为可写 */
#define EV_WRITE	0x04
/** Wait for a POSIX signal to be raised  等待POSIX信号发出*/
#define EV_SIGNAL	0x08
/**
 * Persistent event: won't get removed automatically when activated.
 * 持久事件：激活后不会自动删除。
 *
 * When a persistent event with a timeout becomes activated, its timeout
 * is reset to 0. 当具有超时的持久事件被激活时，其超时时间将重置为0。
 */
#define EV_PERSIST	0x10
/** Select edge-triggered behavior, if supported by the backend. 如果后端支持，请选择边缘触发行为。 */
#define EV_ET		0x20
/**
 * If this option is provided, then event_del() will not block in one thread
 * while waiting for the event callback to complete in another thread.
 * 如果提供了此选项，则event_del（）在等待事件回调在另一个线程中完成时，不会在一个线程中阻塞。
 *
 * To use this option safely, you may need to use event_finalize() or
 * event_free_finalize() in order to safely tear down an event in a
 * multithreaded application.  See those functions for more information.
 * 为了安全地使用此选项，您可能需要使用event_finalize（）或event_free_finalize（）来安全地删除多线程应用程序中的事件。有关更多信息，请参阅这些功能。
 **/
#define EV_FINALIZE     0x40
/**
 * Detects connection close events.  You can use this to detect when a
 * connection has been closed, without having to read all the pending data
 * from a connection.
 * 检测连接关闭事件。您可以使用此功能检测连接何时关闭，而无需从连接中读取所有挂起的数据。
 *
 * Not all backends support EV_CLOSED.  To detect or require it, use the
 * feature flag EV_FEATURE_EARLY_CLOSE.
 * 并非所有后端都支持EV_CLOSED。要检测或需要它，请使用功能标志EV_feature_EARLY_CLOSE。
 **/
#define EV_CLOSED	0x80
/**@}*/

/**
   @name evtimer_* macros 宏

   Aliases for working with one-shot timer events
   If you need EV_PERSIST timer use event_*() functions.
   用于处理单次定时器事件的宏，如果需要EV_PERSIST定时器，请使用event_*（）函数。
 */
/**@{*/
#define evtimer_assign(ev, b, cb, arg) \
	event_assign((ev), (b), -1, 0, (cb), (arg))
#define evtimer_new(b, cb, arg)		event_new((b), -1, 0, (cb), (arg))
#define evtimer_add(ev, tv)		event_add((ev), (tv))
#define evtimer_del(ev)			event_del(ev)
#define evtimer_pending(ev, tv)		event_pending((ev), EV_TIMEOUT, (tv))
#define evtimer_initialized(ev)		event_initialized(ev)
/**@}*/

/**
   @name evsignal_* macros 宏

   Aliases for working with signal events 用于处理信号事件的宏
 */
/**@{*/
#define evsignal_add(ev, tv)		event_add((ev), (tv))
#define evsignal_assign(ev, b, x, cb, arg)			\
	event_assign((ev), (b), (x), EV_SIGNAL|EV_PERSIST, cb, (arg))
#define evsignal_new(b, x, cb, arg)				\
	event_new((b), (x), EV_SIGNAL|EV_PERSIST, (cb), (arg))
#define evsignal_del(ev)		event_del(ev)
#define evsignal_pending(ev, tv)	event_pending((ev), EV_SIGNAL, (tv))
#define evsignal_initialized(ev)	event_initialized(ev)
/**@}*/

/**
   @name evuser_* macros

   Aliases for working with user-triggered events
   If you need EV_PERSIST event use event_*() functions.
   用于处理用户触发事件的宏，如果需要EV_PERSIST事件，请使用event_*（）函数。
 */
/**@{*/
#define evuser_new(b, cb, arg)		event_new((b), -1, 0, (cb), (arg))
#define evuser_del(ev)			event_del(ev)
#define evuser_pending(ev, tv)		event_pending((ev), 0, (tv))
#define evuser_initialized(ev)		event_initialized(ev)
#define evuser_trigger(ev)		event_active((ev), 0, 0)
/**@}*/

/**
   A callback function for an event.  事件的回调函数。

   It receives three arguments: 它接收三个参数：

   @param fd An fd or signal  fd或信号
   @param events One or more EV_* flags
   @param arg A user-supplied argument. 用户提供的参数。

   @see event_new()
 */
typedef void (*event_callback_fn)(evutil_socket_t, short, void *);

/**
  Return a value used to specify that the event itself must be used as the callback argument.
  返回一个值，用于指定事件本身必须用作回调参数。

  The function event_new() takes a callback argument which is passed
  to the event's callback function. To specify that the argument to be
  passed to the callback function is the event that event_new() returns,
  pass in the return value of event_self_cbarg() as the callback argument
  for event_new().
  函数event_new（）接受一个回调参数，该参数被传递给事件的回调函数。
  要指定传递给回调函数的参数是event_new（）返回的事件，请将event_self_cbarg（）的返回值作为event_new的回调参数传递。

  For example:
  <pre>
      struct event *ev = event_new(base, sock, events, callback, %event_self_cbarg());
  </pre>

  For consistency with event_new(), it is possible to pass the return value
  of this function as the callback argument for event_assign() &ndash; this
  achieves the same result as passing the event in directly.
  为了与event_new（）保持一致，可以将此函数的返回值作为event_assign（）的回调参数传递&ndash；这实现了与直接传递事件相同的结果。 

  @return a value to be passed as the callback argument to event_new() or
  event_assign(). 一个值，作为回调参数传递给eventnew（）或eventassign（）。
  @see event_new(), event_assign()
 */
EVENT2_EXPORT_SYMBOL
void *event_self_cbarg(void);

/**
  Allocate and assign a new event structure, ready to be added.
  分配一个新的事件结构，准备添加。

  The function event_new() returns a new event that can be used in
  future calls to event_add() and event_del().  The fd and events
  arguments determine which conditions will trigger the event; the
  callback and callback_arg arguments tell Libevent what to do when the
  event becomes active.
  函数event_new（）返回一个新事件，可以在将来调用event_add（）和event_del（）时使用。
  fd和events参数决定了哪些条件将触发事件；callback和callback_arg参数告诉Libevent在事件变为活动状态时该做什么。

  If events contains one of EV_READ, EV_WRITE, or EV_READ|EV_WRITE, then
  fd is a file descriptor or socket that should get monitored for
  readiness to read, readiness to write, or readiness for either operation
  (respectively).  If events contains EV_SIGNAL, then fd is a signal
  number to wait for.  If events contains none of those flags, then the
  event can be triggered only by a timeout or by manual activation with
  event_active(): In this case, fd must be -1.
  如果事件包含EV_READ、EV_WRITE或EV_READ|EV_WRITE之一，则fd是一个文件描述符或套接字，应分别监视其是否准备好读取、是否准备好写入或是否准备好进行任何操作。
  如果事件包含EV_SIGNAL，则fd是要等待的信号号。
  如果事件不包含这些标志，则只能通过超时或使用event_active（）手动激活来触发事件：在这种情况下，fd必须为-1。

  The EV_PERSIST flag can also be passed in the events argument: it makes
  event_add() persistent until event_del() is called.
  EV_PERSIST标志也可以在events参数中传递：它使event_add（）持久化，直到调用event_del（）。

  The EV_ET flag is compatible with EV_READ and EV_WRITE, and supported
  only by certain backends.  It tells Libevent to use edge-triggered
  events.
  EV_ET标志与EV_READ和EV_WRITE兼容，并且仅由某些后端支持。它告诉Libevent使用边缘触发事件。

  The EV_TIMEOUT flag has no effect here.
  EV_TIMEOUT标志在此无效。

  It is okay to have multiple events all listening on the same fds; but
  they must either all be edge-triggered, or all not be edge triggered.
  让多个事件都在同一个fds上监听是可以的；但它们要么都是边缘触发的，要么都不是边缘触发的。

  When the event becomes active, the event loop will run the provided
  callback function, with three arguments.  The first will be the provided
  fd value.  The second will be a bitfield of the events that triggered:
  EV_READ, EV_WRITE, or EV_SIGNAL.  Here the EV_TIMEOUT flag indicates
  that a timeout occurred, and EV_ET indicates that an edge-triggered
  event occurred.  The third event will be the callback_arg pointer that
  you provide.
  当事件激活时，事件循环将运行提供的回调函数，该函数有三个参数。
  第一个将是提供的fd值。第二个将是触发事件的位字段：EV_READ、EV_WRITE或EV_SIGNAL。
  这里，EV_TIMEOUT标志表示发生了超时，EV_ET表示发生了边缘触发事件。第三个事件将是您提供的callback_arg指针。

  @param base the event base to which the event should be attached. 事件应附加到的事件库。
  @param fd the file descriptor or signal to be monitored, or -1. 要监视的文件描述符或信号，或-1。
  @param events desired events to monitor: bitfield of EV_READ, EV_WRITE,
      EV_SIGNAL, EV_PERSIST, EV_ET. 需要监视的事件：EV_READ、EV_WRITE、EV_SIGNAL、EV_PERSIST、EV_ET的位字段。
  @param callback callback function to be invoked when the event occurs 事件发生时调用的回调函数
  @param callback_arg an argument to be passed to the callback function 要传递给回调函数的参数

  @return a newly allocated struct event that must later be freed with
    event_free() or NULL if an error occurred.  一个新分配的结构事件，如果发生错误，稍后必须用event_free（）或NULL释放。
  @see event_free(), event_add(), event_del(), event_assign()
 */
EVENT2_EXPORT_SYMBOL
struct event *event_new(struct event_base *, evutil_socket_t, short, event_callback_fn, void *);


/**
  Prepare a new, already-allocated event structure to be added.
  准备一个新的、已分配的要添加的事件结构。

  The function event_assign() prepares the event structure ev to be used
  in future calls to event_add() and event_del().  Unlike event_new(), it
  doesn't allocate memory itself: it requires that you have already
  allocated a struct event, probably on the heap.  Doing this will
  typically make your code depend on the size of the event structure, and
  thereby create incompatibility with future versions of Libevent.
  函数event_assign（）准备事件结构ev，以便在将来调用event_add（）和event_del（）时使用。
  与event_new（）不同，它本身不分配内存：它要求您已经分配了一个结构事件，可能是在堆上。
  这样做通常会使您的代码依赖于事件结构的大小，从而与Libevent的未来版本不兼容。

  The easiest way to avoid this problem is just to use event_new() and
  event_free() instead.
  避免这个问题的最简单方法就是使用event_new（）和event_free（）。

  A slightly harder way to future-proof your code is to use
  event_get_struct_event_size() to determine the required size of an event
  at runtime.
  一种稍微困难的方法来证明你的代码是在运行时使用event_get_struct_event_size（）来确定事件所需的大小。

  Note that it is NOT safe to call this function on an event that is
  active or pending.  Doing so WILL corrupt internal data structures in
  Libevent, and lead to strange, hard-to-diagnose bugs.  You _can_ use
  event_assign to change an existing event, but only if it is not active
  or pending!
  请注意，在活动或挂起的事件上调用此函数是不安全的。这样做会破坏Libevent中的内部数据结构，并导致奇怪的、难以诊断的错误。
  您可以使用event_assign更改现有事件，但前提是它不是活动的或挂起的！

  The arguments for this function, and the behavior of the events that it
  makes, are as for event_new().
  此函数的参数及其产生的事件的行为与event_new（）相同。

  @param ev an event struct to be modified  要修改的事件结构
  @param base the event base to which ev should be attached.  ev应该附加到的事件库。
  @param fd the file descriptor to be monitored 要监视的文件描述符
  @param events desired events to monitor; can be EV_READ and/or EV_WRITE 需要监控的事件；可以是EV_READ和/或EV_WRITE
  @param callback callback function to be invoked when the event occurs 事件发生时调用的回调函数
  @param callback_arg an argument to be passed to the callback function 要传递给回调函数的参数

  @return 0 if success, or -1 on invalid arguments.

  @see event_new(), event_add(), event_del(), event_base_once(),
    event_get_struct_event_size()
  */
EVENT2_EXPORT_SYMBOL
int event_assign(struct event *, struct event_base *, evutil_socket_t, short, event_callback_fn, void *);

/**
   Deallocate a struct event * returned by event_new().
   取消分配event_new（）返回的结构体event*。

   If the event is pending or active, this function makes it non-pending
   and non-active first.
   如果事件处于挂起或活动状态，此功能将使其首先处于非挂起和非活动状态。
 */
EVENT2_EXPORT_SYMBOL
void event_free(struct event *);

/**
 * Callback type for event_finalize and event_free_finalize().
 * event_finalize和event_free_finalize（）的回调类型。
 **/
typedef void (*event_finalize_callback_fn)(struct event *, void *);
/**
   @name Finalization functions

   These functions are used to safely tear down an event in a multithreaded
   application.  If you construct your events with EV_FINALIZE to avoid
   deadlocks, you will need a way to remove an event in the certainty that
   it will definitely not be running its callback when you deallocate it
   and its callback argument.
   这些函数用于安全地删除多线程应用程序中的事件。如果使用EV_FINALIZE构造事件以避免死锁，则需要一种方法来删除事件，以确保在释放事件及其回调参数时，它肯定不会运行回调。

   To do this, call one of event_finalize() or event_free_finalize with
   0 for its first argument, the event to tear down as its second argument,
   and a callback function as its third argument.  The callback will be
   invoked as part of the event loop, with the event's priority.
   为此，调用event_finalize（）或event_free_finalize中的一个，第一个参数为0，第二个参数为要删除的事件，第三个参数为回调函数。
   回调将作为事件循环的一部分被调用，具有事件的优先级。

   After you call a finalizer function, event_add() and event_active() will
   no longer work on the event, and event_del() will produce a no-op. You
   must not try to change the event's fields with event_assign() or
   event_set() while the finalize callback is in progress.  Once the
   callback has been invoked, you should treat the event structure as
   containing uninitialized memory.
   调用finalizer函数后，event_add（）和event_active（）将不再对事件起作用，event_del（）将产生no-op。
   在finalize回调过程中，您不得尝试使用event_assign（）或event_set（）更改事件的字段。一旦调用了回调，您应该将事件结构视为包含未初始化的内存。

   The event_free_finalize() function frees the event after it's finalized;
   event_finalize() does not.
   event_free_finalize（）函数在事件完成后释放事件；event_finalize（）没有。

   A finalizer callback must not make events pending or active.  It must not
   add events, activate events, or attempt to "resuscitate" the event being
   finalized in any way.
   终结器回调不得使事件挂起或处于活动状态。它不得添加事件、激活事件或试图以任何方式“恢复”正在最终确定的事件。

   @return 0 on success, -1 on failure.
 */
/**@{*/
EVENT2_EXPORT_SYMBOL
int event_finalize(unsigned, struct event *, event_finalize_callback_fn);
EVENT2_EXPORT_SYMBOL
int event_free_finalize(unsigned, struct event *, event_finalize_callback_fn);
/**@}*/

/**
  Schedule a one-time event
  安排一次性活动

  The function event_base_once() is similar to event_new().  However, it
  schedules a callback to be called exactly once, and does not require the
  caller to prepare an event structure.
  函数event_base_once（）类似于event_new（）。但是，它会安排一个回调被精确地调用一次，并且不需要调用者准备事件结构。

  Note that in Libevent 2.0 and earlier, if the event is never triggered, the
  internal memory used to hold it will never be freed.  In Libevent 2.1,
  the internal memory will get freed by event_base_free() if the event
  is never triggered.  The 'arg' value, however, will not get freed in either
  case--you'll need to free that on your own if you want it to go away.
  请注意，在Libevent 2.0及更早版本中，如果事件从未被触发，则用于保存它的内部内存将永远不会被释放。
  在Libevent 2.1中，如果事件从未被触发，则内部内存将通过event_base_free（）释放。然而，在任何一种情况下，arg值都不会被释放——如果你想让它消失，你需要自己释放它。

  @param base an event_base
  @param fd a file descriptor to monitor, or -1 for no fd.  要监视的文件描述符，或者-1表示没有fd。
  @param events event(s) to monitor; can be any of EV_READ |
         EV_WRITE, or EV_TIMEOUT  要监控的事件；可以是EV_READ|EV_WRITE或EV_TIMEOUT中的任何一个
  @param callback callback function to be invoked when the event occurs 事件发生时调用的回调函数
  @param arg an argument to be passed to the callback function  要传递给回调函数的参数
  @param timeout the maximum amount of time to wait for the event. NULL
         makes an EV_READ/EV_WRITE event make forever; NULL makes an
        EV_TIMEOUT event success immediately.
        等待事件的最长时间。NULL使EV_READ/EV_WRITE事件永久化；NULL会立即使EV_TIMEOUT事件成功。
  @return 0 if successful, or -1 if an error occurred
 */
EVENT2_EXPORT_SYMBOL
int event_base_once(struct event_base *, evutil_socket_t, short, event_callback_fn, void *, const struct timeval *);

/**
  Add an event to the set of pending events.
  将事件添加到待处理事件集中。

  The function event_add() schedules the execution of the event 'ev' when the
  condition specified by event_assign() or event_new() occurs, or when the time
  specified in timeout has elapsed.  If a timeout is NULL, no timeout
  occurs and the function will only be
  called if a matching event occurs.  The event in the
  ev argument must be already initialized by event_assign() or event_new()
  and may not be used
  in calls to event_assign() until it is no longer pending.
  函数event_add（）在event_assign（）或event_new（）指定的条件发生时，或者在超时中指定的时间已过时，调度事件“ev”的执行。
  如果超时为NULL，则不会发生超时，并且只有在发生匹配事件时才会调用该函数。
  ev参数中的事件必须已经由event_assign（）或event_new（）初始化，并且在不再挂起之前，不能在调用event_assignd（）时使用。

  If the event in the ev argument already has a scheduled timeout, calling
  event_add() replaces the old timeout with the new one if tv is non-NULL.
  如果ev参数中的事件已经有一个计划的超时，如果tv不是NULL，则调用event_add（）将用新的超时替换旧的超时。

  @param ev an event struct initialized via event_assign() or event_new() 通过event_assign（）或event_new（）初始化的事件结构
  @param timeout the maximum amount of time to wait for the event, or NULL
         to wait forever  等待事件的最长时间，或NULL表示永远等待
  @return 0 if successful, or -1 if an error occurred
  @see event_del(), event_assign(), event_new()
  */
EVENT2_EXPORT_SYMBOL
int event_add(struct event *ev, const struct timeval *timeout);

/**
   Remove a timer from a pending event without removing the event itself.
   从挂起的事件中删除计时器，而不删除事件本身。

   If the event has a scheduled timeout, this function unschedules it but
   leaves the event otherwise pending.
   如果事件有计划的超时，此函数会取消计划，但会让事件处于挂起状态。

   @param ev an event struct initialized via event_assign() or event_new() 通过event_assign（）或event_new（）初始化的事件结构
   @return 0 on success, or -1 if an error occurred.
*/
EVENT2_EXPORT_SYMBOL
int event_remove_timer(struct event *ev);

/**
  Remove an event from the set of monitored events.
  从监视的事件集中删除事件。

  The function event_del() will cancel the event in the argument ev.  If the
  event has already executed or has never been added the call will have no
  effect.
  函数event_del（）将取消参数ev中的事件。如果事件已经执行或从未添加，则调用将无效。

  @param ev an event struct to be removed from the working set  要从工作集中删除的事件结构
  @return 0 if successful, or -1 if an error occurred
  @see event_add()
 */
EVENT2_EXPORT_SYMBOL
int event_del(struct event *);

/**
   As event_del(), but never blocks while the event's callback is running
   in another thread, even if the event was constructed without the
   EV_FINALIZE flag.
   作为event_del（），但当事件的回调在另一个线程中运行时，即使事件是在没有EV_FINALIZE标志的情况下构造的，也不会阻塞。
 */
EVENT2_EXPORT_SYMBOL
int event_del_noblock(struct event *ev);
/**
   As event_del(), but always blocks while the event's callback is running
   in another thread, even if the event was constructed with the
   EV_FINALIZE flag.
   作为event_del（），但在事件的回调在另一个线程中运行时总是阻塞，即使该事件是用EV_FINALIZE标志构造的。
 */
EVENT2_EXPORT_SYMBOL
int event_del_block(struct event *ev);

/**
  Make an event active.
  激活事件。

  You can use this function on a pending or a non-pending event to make it
  active, so that its callback will be run by event_base_dispatch() or
  event_base_loop().
  您可以在挂起或非挂起的事件上使用此函数将其激活，以便其回调将由event_base_dispatch（）或event_base_loop（）运行。

  One common use in multithreaded programs is to wake the thread running
  event_base_loop() from another thread.
  多线程程序中的一个常见用法是从另一个线程中唤醒运行event_base_loop（）的线程。

  @param ev an event to make active.  一个需要激活的事件。
  @param res a set of flags to pass to the event's callback.  传递给事件回调的一组标志。
  @param ncalls an obsolete argument: this is ignored.  一个过时的论点：这被忽略了。
 **/
EVENT2_EXPORT_SYMBOL
void event_active(struct event *ev, int res, short ncalls);

/**
  Checks if a specific event is pending or scheduled.
  检查特定事件是否处于待处理状态或已计划状态。

  @param ev an event struct previously passed to event_add()  之前传递给event_add（）的事件结构
  @param events the requested event type; any of EV_TIMEOUT|EV_READ|
         EV_WRITE|EV_SIGNAL 所请求的事件类型；EV_TIMEOUT | EV_READ | EV_WRITE | EV_SIGNAL中的任何一个
  @param tv if this field is not NULL, and the event has a timeout,
         this field is set to hold the time at which the timeout will
	 expire.  如果此字段不为NULL，并且事件有超时，则此字段设置为保存超时到期的时间。

  @return true if the event is pending on any of the events in 'what', (that
  is to say, it has been added), or 0 if the event is not added.
  如果事件在“what”中的任何事件上处于挂起状态（也就是说，它已被添加），则返回true，如果事件未被添加，则返回0。
 */
EVENT2_EXPORT_SYMBOL
int event_pending(const struct event *ev, short events, struct timeval *tv);

/**
   If called from within the callback for an event, returns that event.
   如果从事件的回调函数中调用，则返回该事件。

   The behavior of this function is not defined when called from outside the
   callback function for an event.
   当从事件的回调函数外部调用时，此函数的行为未定义。
 */
EVENT2_EXPORT_SYMBOL
struct event *event_base_get_running_event(struct event_base *base);

/**
  Test if an event structure might be initialized.
  测试事件结构是否可以初始化。

  The event_initialized() function can be used to check if an event has been
  initialized.
  event_initialized（）函数可用于检查事件是否已初始化。

  Warning: This function is only useful for distinguishing a zeroed-out
    piece of memory from an initialized event, it can easily be confused by
    uninitialized memory.  Thus, it should ONLY be used to distinguish an
    initialized event from zero.
    警告：此函数仅用于区分清零的内存和初始化的事件，它很容易被未初始化的内存混淆。因此，它只能用于区分初始化事件和零。

  @param ev an event structure to be tested 待测试的事件结构
  @return 1 if the structure might be initialized, or 0 if it has not been
          initialized 如果结构可能已初始化，则返回1，如果尚未初始化，则为0
 */
EVENT2_EXPORT_SYMBOL
int event_initialized(const struct event *ev);

/**
   Get the signal number assigned to a signal event
   获取分配给信号事件的信号编号
*/
#define event_get_signal(ev) ((int)event_get_fd(ev))

/**
   Get the socket or signal assigned to an event, or -1 if the event has
   no socket.
   获取分配给事件的套接字或信号，如果事件没有套接字，则获取-1。
*/
EVENT2_EXPORT_SYMBOL
evutil_socket_t event_get_fd(const struct event *ev);

/**
   Get the event_base associated with an event.
   获取与事件关联的event_base。
*/
EVENT2_EXPORT_SYMBOL
struct event_base *event_get_base(const struct event *ev);

/**
   Return the events (EV_READ, EV_WRITE, etc) assigned to an event.
   返回分配给事件的事件（EV_READ、EV_WRITE等）。
*/
EVENT2_EXPORT_SYMBOL
short event_get_events(const struct event *ev);

/**
   Return the callback assigned to an event.
   返回分配给事件的回调。
*/
EVENT2_EXPORT_SYMBOL
event_callback_fn event_get_callback(const struct event *ev);

/**
   Return the callback argument assigned to an event.
   返回分配给事件的回调参数。
*/
EVENT2_EXPORT_SYMBOL
void *event_get_callback_arg(const struct event *ev);

/**
   Return the priority of an event.
   返回事件的优先级。
   @see event_priority_init(), event_get_priority()
*/
EVENT2_EXPORT_SYMBOL
int event_get_priority(const struct event *ev);

/**
   Extract _all_ of arguments given to construct a given event.  The
   event_base is copied into *base_out, the fd is copied into *fd_out, and so
   on.
   提取用于构造给定事件的所有参数。event_base被复制到*base_out，fd被复制到*fd_out，以此类推。

   If any of the "_out" arguments is NULL, it will be ignored.
   如果任何“_out”参数为NULL，则将忽略它。
 */
EVENT2_EXPORT_SYMBOL
void event_get_assignment(const struct event *event,
    struct event_base **base_out, evutil_socket_t *fd_out, short *events_out,
    event_callback_fn *callback_out, void **arg_out);

/**
   Return the size of struct event that the Libevent library was compiled
   with.

   This will be NO GREATER than sizeof(struct event) if you're running with
   the same version of Libevent that your application was built with, but
   otherwise might not.

   Note that it might be SMALLER than sizeof(struct event) if some future
   version of Libevent adds extra padding to the end of struct event.
   We might do this to help ensure ABI-compatibility between different
   versions of Libevent.
 */
EVENT2_EXPORT_SYMBOL
size_t event_get_struct_event_size(void);

/**
   Get the Libevent version.

   Note that this will give you the version of the library that you're
   currently linked against, not the version of the headers that you've
   compiled against.

   @return a string containing the version number of Libevent
*/
EVENT2_EXPORT_SYMBOL
const char *event_get_version(void);

/**
   Return a numeric representation of Libevent's version.

   Note that this will give you the version of the library that you're
   currently linked against, not the version of the headers you've used to
   compile.

   The format uses one byte each for the major, minor, and patchlevel parts of
   the version number.  The low-order byte is unused.  For example, version
   2.0.1-alpha has a numeric representation of 0x02000100
*/
EVENT2_EXPORT_SYMBOL
ev_uint32_t event_get_version_number(void);

/** As event_get_version, but gives the version of Libevent's headers.  作为event_get_version，但给出了Libevent头文件的版本。 */
#define LIBEVENT_VERSION EVENT__VERSION
/** As event_get_version_number, but gives the version number of Libevent's
 * headers. 作为event_get_version_number，但给出了Libevent标头的版本号。 */
#define LIBEVENT_VERSION_NUMBER EVENT__NUMERIC_VERSION

/** Largest number of priorities that Libevent can support. Libevent可以支持的最大数量的优先事项。 */
#define EVENT_MAX_PRIORITIES 256
/**
  Set the number of different event priorities
  设置不同事件优先级的数量

  By default Libevent schedules all active events with the same priority.
  However, some time it is desirable to process some events with a higher
  priority than others.  For that reason, Libevent supports strict priority
  queues.  Active events with a lower priority are always processed before
  events with a higher priority.
  默认情况下，Libevent以相同的优先级安排所有活动事件。然而，有时需要以比其他事件更高的优先级处理一些事件。
  因此，Libevent支持严格的优先级队列。优先级较低的活动事件总是在优先级较高的事件之前处理。

  The number of different priorities can be set initially with the
  event_base_priority_init() function.  This function should be called
  before the first call to event_base_dispatch().  The
  event_priority_set() function can be used to assign a priority to an
  event.  By default, Libevent assigns the middle priority to all events
  unless their priority is explicitly set.
  最初可以使用event_base_priority_init（）函数设置不同优先级的数量。
  此函数应在第一次调用event_base_dispatch（）之前调用。event_priority_set（）函数可用于为事件分配优先级。
  默认情况下，除非明确设置了优先级，否则Libevent会为所有事件分配中间优先级。

  Note that urgent-priority events can starve less-urgent events: after
  running all urgent-priority callbacks, Libevent checks for more urgent
  events again, before running less-urgent events.  Less-urgent events
  will not have their callbacks run until there are no events more urgent
  than them that want to be active.
  请注意，紧急优先级事件可能会饿死不太紧急的事件：在运行所有紧急优先级回调后，Libevent会在运行不太紧急事件之前再次检查更紧急的事件。
  不太紧急的事件将不会运行回调，直到没有比它们更紧急的事件想要激活。

  @param eb the event_base structure returned by event_base_new() event_base_new（）返回的event_base结构
  @param npriorities the maximum number of priorities 优先级的最大数量
  @return 0 if successful, or -1 if an error occurred
  @see event_priority_set()
 */
EVENT2_EXPORT_SYMBOL
int	event_base_priority_init(struct event_base *, int);

/**
  Get the number of different event priorities. 获取不同事件优先级的数量。

  @param eb the event_base structure returned by event_base_new() event_base_new（）返回的event_base结构
  @return Number of different event priorities  不同事件优先级的数量
  @see event_base_priority_init()
*/
EVENT2_EXPORT_SYMBOL
int	event_base_get_npriorities(struct event_base *eb);

/**
  Assign a priority to an event.  为事件分配优先级。

  @param ev an event struct 事件结构体
  @param priority the new priority to be assigned 要分配的新优先级
  @return 0 if successful, or -1 if an error occurred
  @see event_priority_init(), event_get_priority()
  */
EVENT2_EXPORT_SYMBOL
int	event_priority_set(struct event *, int);

/**
   Prepare an event_base to use a large number of timeouts with the same
   duration.
   准备一个事件库，以使用大量具有相同持续时间的超时。

   Libevent's default scheduling algorithm is optimized for having a large
   number of timeouts with their durations more or less randomly
   distributed.  But if you have a large number of timeouts that all have
   the same duration (for example, if you have a large number of
   connections that all have a 10-second timeout), then you can improve
   Libevent's performance by telling Libevent about it.
   Libevent的默认调度算法针对大量超时进行了优化，这些超时的持续时间或多或少是随机分布的。
   但是，如果你有大量的超时，并且所有超时的持续时间都相同（例如，如果你的大量连接都有10秒的超时），那么你可以通过告诉Libevent来提高Libevent的性能。

   To do this, call this function with the common duration.  It will return a
   pointer to a different, opaque timeout value.  (Don't depend on its actual
   contents!)  When you use this timeout value in event_add(), Libevent will
   schedule the event more efficiently.
   为此，请使用公共持续时间调用此函数。它将返回一个指向不同的不透明超时值的指针。（不依赖于其实际内容！）当您在event_add（）中使用此超时值时，Libevent将更有效地调度事件。

   (This optimization probably will not be worthwhile until you have thousands
   or tens of thousands of events with the same timeout.)
   （除非有数千或数万个具有相同超时的事件，否则这种优化可能不值得。）
 */
EVENT2_EXPORT_SYMBOL
const struct timeval *event_base_init_common_timeout(struct event_base *base,
    const struct timeval *duration);

#if !defined(EVENT__DISABLE_MM_REPLACEMENT) || defined(EVENT_IN_DOXYGEN_)
/**
 Override the functions that Libevent uses for memory management.
 重写Libevent用于内存管理的函数。

 Usually, Libevent uses the standard libc functions malloc, realloc, and
 free to allocate memory.  Passing replacements for those functions to
 event_set_mem_functions() overrides this behavior.
 通常，Libevent使用标准的libc函数malloc、realloc和free来分配内存。将这些函数的替换传递给event_set_mem_functions（）会覆盖此行为。

 Note that all memory returned from Libevent will be allocated by the
 replacement functions rather than by malloc() and realloc().  Thus, if you
 have replaced those functions, it will not be appropriate to free() memory
 that you get from Libevent.  Instead, you must use the free_fn replacement
 that you provided.
 请注意，从Libevent返回的所有内存将由替换函数分配，而不是由malloc（）和realloc（）分配。
 因此，如果你已经替换了这些函数，那么从Libevent中释放（）内存是不合适的。相反，您必须使用您提供的free_fn替换项。

 Note also that if you are going to call this function, you should do so
 before any call to any Libevent function that does allocation.
 Otherwise, those functions will allocate their memory using malloc(), but
 then later free it using your provided free_fn.
 另请注意，如果要调用此函数，则应在调用任何执行分配的Libevent函数之前执行此操作。否则，这些函数将使用malloc（）分配内存，但随后使用您提供的free_fn释放内存。

 @param malloc_fn A replacement for malloc. malloc的替代品
 @param realloc_fn A replacement for realloc  realloc的替代品
 @param free_fn A replacement for free. free的替代品
 **/
EVENT2_EXPORT_SYMBOL
void event_set_mem_functions(
	void *(*malloc_fn)(size_t sz),
	void *(*realloc_fn)(void *ptr, size_t sz),
	void (*free_fn)(void *ptr));
/** This definition is present if Libevent was built with support for
    event_set_mem_functions() 如果Libevent是在支持event_set_mem_functions（）的情况下构建的，则存在此定义 */
#define EVENT_SET_MEM_FUNCTIONS_IMPLEMENTED
#endif

/**
   Writes a human-readable description of all inserted and/or active
   events to a provided stdio stream.
   将所有插入和/或活动事件的人类可读描述写入提供的stdio流。

   This is intended for debugging; its format is not guaranteed to be the same
   between libevent versions.
   这是用于调试的；不能保证libevent版本之间的格式相同。

   @param base An event_base on which to scan the events. 用于扫描事件的事件库。
   @param output A stdio file to write on.  要写入的stdio文件。
 */
EVENT2_EXPORT_SYMBOL
void event_base_dump_events(struct event_base *, FILE *);


/**
   Activates all pending events for the given fd and event mask.
   激活给定fd和事件掩码的所有未决事件。

   This function activates pending events only.  Events which have not been
   added will not become active.
   此功能仅激活未决事件。未添加的事件将不会处于活动状态。

   @param base the event_base on which to activate the events.  激活事件的事件库。
   @param fd An fd to active events on. fd用于激活事件。
   @param events One or more of EV_{READ,WRITE,TIMEOUT}.  EV_{读取、写入、超时}中的一个或多个。
 */
EVENT2_EXPORT_SYMBOL
void event_base_active_by_fd(struct event_base *base, evutil_socket_t fd, short events);

/**
   Activates all pending signals with a given signal number
   激活具有给定信号编号的所有待处理信号

   This function activates pending events only.  Events which have not been
   added will not become active.
   此功能仅激活未决事件。未添加的事件将不会处于活动状态。

   @param base the event_base on which to activate the events.  激活事件的事件库。
   @param fd The signal to active events on.  激活事件的信号。
 */
EVENT2_EXPORT_SYMBOL
void event_base_active_by_signal(struct event_base *base, int sig);

/**
 * Callback for iterating events in an event base via event_base_foreach_event
 * 通过event_base_foreach_event回调事件库中的迭代事件
 */
typedef int (*event_base_foreach_event_cb)(const struct event_base *, const struct event *, void *);

/**
   Iterate over all added or active events events in an event loop, and invoke
   a given callback on each one.
   在事件循环中迭代所有已添加或活动的事件，并对每个事件调用给定的回调。

   The callback must not call any function that modifies the event base, that
   modifies any event in the event base, or that adds or removes any event to
   the event base.  Doing so is unsupported and will lead to undefined
   behavior -- likely, to crashes.
   回调函数不得调用任何修改事件库、修改事件库中的任何事件或向事件库添加或删除任何事件的函数。这样做是不受支持的，并将导致未定义的行为——很可能导致崩溃。

   event_base_foreach_event() holds a lock on the event_base() for the whole
   time it's running: slow callbacks are not advisable.
   event_base_foreach_event（）在整个运行过程中对event_base（）保持锁定：不建议使用慢速回调。

   Note that Libevent adds some events of its own to make pieces of its
   functionality work.  You must not assume that the only events you'll
   encounter will be the ones you added yourself.
   请注意，Libevent添加了一些自己的事件，以使其功能正常工作。您不能假设您将遇到的唯一事件是您自己添加的事件。

   The callback function must return 0 to continue iteration, or some other
   integer to stop iterating.
   回调函数必须返回0以继续迭代，或返回其他整数以停止迭代。

   @param base An event_base on which to scan the events. 用于扫描事件的事件库。
   @param fn   A callback function to receive the events. 用于接收事件的回调函数。
   @param arg  An argument passed to the callback function. 传递给回调函数的参数。
   @return 0 if we iterated over every event, or the value returned by the
      callback function if the loop exited early.
      如果我们对每个事件进行迭代，则返回0；如果循环提前退出，则返回回调函数返回的值。
*/
EVENT2_EXPORT_SYMBOL
int event_base_foreach_event(struct event_base *base, event_base_foreach_event_cb fn, void *arg);


/** Sets 'tv' to the current time (as returned by gettimeofday()),
    looking at the cached value in 'base' if possible, and calling
    gettimeofday() or clock_gettime() as appropriate if there is no
    cached time.
    将'tv'设置为当前时间（由gettimeofday（）返回），如果可能的话，查看'base'中的缓存值，如果没有缓存时间，则根据需要调用gettimeofday（）或clock_gettime（）。

    Generally, this value will only be cached while actually
    processing event callbacks, and may be very inaccurate if your
    callbacks take a long time to execute.
    通常，只有在实际处理事件回调时才会缓存此值，如果回调需要很长时间才能执行，则可能会非常不准确。

    Returns 0 on success, negative on failure.  成功时返回0，失败时返回负数。
 */
EVENT2_EXPORT_SYMBOL
int event_base_gettimeofday_cached(struct event_base *base,
    struct timeval *tv);

/** Update cached_tv in the 'base' to the current time  将“base”中的cached_tv更新为当前时间
 *
 * You can use this function is useful for selectively increasing
 * the accuracy of the cached time value in 'base' during callbacks
 * that take a long time to execute.
 * 您可以使用此函数，在执行需要很长时间的回调期间，有选择地提高“base”中缓存时间值的准确性。
 *
 * This function has no effect if the base is currently not in its
 * event loop, or if timeval caching is disabled via
 * EVENT_BASE_FLAG_NO_CACHE_TIME.
 * 如果基当前不在其事件循环中，或者通过event_base_FLAG_no_CACHE_TIME禁用了时间值缓存，则此函数无效。
 *
 * @return 0 on success, -1 on failure
 */
EVENT2_EXPORT_SYMBOL
int event_base_update_cache_time(struct event_base *base);

/** Release up all globally-allocated resources allocated by Libevent.
 * 释放Libevent分配的所有全球资源。

    This function does not free developer-controlled resources like
    event_bases, events, bufferevents, listeners, and so on.  It only releases
    resources like global locks that there is no other way to free.
    此函数不会释放开发人员控制的资源，如事件库、事件、缓冲区事件、监听器等。它只会释放全局锁等资源，而这些资源是没有其他释放方式的。

    It is not actually necessary to call this function before exit: every
    resource that it frees would be released anyway on exit.  It mainly exists
    so that resource-leak debugging tools don't see Libevent as holding
    resources at exit.
    实际上，在退出之前没有必要调用此函数：它释放的每个资源都会在退出时释放。它的存在主要是为了使资源泄漏调试工具不会将Libevent视为在出口处持有资源。

    You should only call this function when no other Libevent functions will
    be invoked -- e.g., when cleanly exiting a program.
    您应该只在没有其他Libevent函数被调用时调用此函数，例如，当干净地退出程序时。
 */
EVENT2_EXPORT_SYMBOL
void libevent_global_shutdown(void);

#ifdef __cplusplus
}
#endif

#endif /* EVENT2_EVENT_H_INCLUDED_ */
