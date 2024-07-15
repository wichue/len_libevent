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
#ifndef EVENT2_BUFFEREVENT_H_INCLUDED_
#define EVENT2_BUFFEREVENT_H_INCLUDED_

/**
   @file event2/bufferevent.h

  Functions for buffering data for network sending or receiving.  Bufferevents
  are higher level than evbuffers: each has an underlying evbuffer for reading
  and one for writing, and callbacks that are invoked under certain
  circumstances.
  用于缓冲网络发送或接收数据的功能。 
  Bufferevents 比 evbuffers 级别更高：每个 Bufferevent 都有一个底层用于读取的 evbuffer 和一个用于写入的 evbuffer ，以及在某些情况下被调用的回调。

  A bufferevent provides input and output buffers that get filled and
  drained automatically.  The user of a bufferevent no longer deals
  directly with the I/O, but instead is reading from input and writing
  to output buffers.
  bufferevent 提供了自动填充和排出的输入和输出缓冲区。 bufferevent 的用户不再直接处理I/O，而是从输入区读取和写入到输出缓冲区。

  Once initialized, the bufferevent structure can be used repeatedly
  with bufferevent_enable() and bufferevent_disable().
  一旦初始化， bufferevent 结构可以与bufferevent_enable（）和bufferevent_disable（）一起重复使用。

  When reading is enabled, the bufferevent will try to read from the
  file descriptor onto its input buffer, and call the read callback.
  When writing is enabled, the bufferevent will try to write data onto its
  file descriptor when the output buffer has enough data, and call the write
  callback when the output buffer is sufficiently drained.
  当启用读取时，缓冲区程序将尝试从文件描述符读取到其输入缓冲区上，并调用读取回调。
  当启用写入时，当输出缓冲区有足够的数据时，缓冲区将尝试将数据写入其文件描述符，并在输出缓冲区充分耗尽时调用写回调。

  Bufferevents come in several flavors, including:

  <dl>
    <dt>Socket-based bufferevents</dt>
      <dd>A bufferevent that reads and writes data onto a network
          socket. Created with bufferevent_socket_new().</dd> 一种读写数据到网络套接字的缓冲器。使用 bufferevent_socket_new （）创建的

    <dt>Paired bufferevents</dt>
      <dd>A pair of bufferevents that send and receive data to one
          another without touching the network.  Created with
          bufferevent_pair_new().</dd> 一对缓冲器，它们彼此发送和接收数据而不接触网络。这是使用 bufferevent_pair_new （）来创建的。

    <dt>Filtering bufferevents</dt>
       <dd>A bufferevent that transforms data, and sends or receives it
          over another underlying bufferevent.  Created with
          bufferevent_filter_new().</dd> 一种转换数据并通过另一个底层缓冲器发送或接收数据的缓冲器。这是使用 bufferevent_filter_new （）来创建的。

    <dt>SSL-backed bufferevents</dt>
      <dd>A bufferevent that uses the openssl library to send and
          receive data over an encrypted connection. Created with
	  bufferevent_openssl_socket_new() or
	  bufferevent_openssl_filter_new().</dd> 一种使用选项库通过加密连接发送和接收数据的缓冲器。使用bufferevent_openssl_socket_new（）或bufferevent_openssl_filter_new（）创建的
  </dl>
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

/* For int types. */
#include <event2/util.h>

/** @name Bufferevent event codes

    These flags are passed as arguments to a bufferevent's event callback.
    这些标志被作为参数传递到缓冲程序的事件回调中。
    @{
*/
#define BEV_EVENT_READING	0x01	/**< error encountered while reading 读取时遇到错误 */
#define BEV_EVENT_WRITING	0x02	/**< error encountered while writing 写入时遇到错误 */
#define BEV_EVENT_EOF		0x10	/**< eof file reached 已到达eof文件 */
#define BEV_EVENT_ERROR		0x20	/**< unrecoverable error encountered 遇到不可恢复的错误 */
#define BEV_EVENT_TIMEOUT	0x40	/**< user-specified timeout reached 已达到用户指定的超时 */
#define BEV_EVENT_CONNECTED	0x80	/**< connect operation finished. 连接操作完成 */
/**@}*/

/**
   An opaque type for handling buffered IO
   一种用于处理被缓冲的IO的不透明类型

   @see event2/bufferevent.h
 */
struct bufferevent
#ifdef EVENT_IN_DOXYGEN_
{}
#endif
;
struct event_base;
struct evbuffer;
struct sockaddr;

/**
   A read or write callback for a bufferevent.
   缓冲程序的读回调或写回调。

   The read callback is triggered when new data arrives in the input
   buffer and the amount of readable data exceed the low watermark
   which is 0 by default.
   当新数据到达输入缓冲区，并且可读数据量超过默认的低水位线0时，将触发读取回调。

   The write callback is triggered if the write buffer has been
   exhausted or fell below its low watermark.
   如果写缓冲区已经耗尽或低于其低水位线，则会触发写回调。

   @param bev the bufferevent that triggered the callback 触发回调的缓冲程序
   @param ctx the user-specified context for this bufferevent 此缓冲区的用户指定的上下文
 */
typedef void (*bufferevent_data_cb)(struct bufferevent *bev, void *ctx);

/**
   An event/error callback for a bufferevent.
   缓冲程序的事件/错误回调。

   The event callback is triggered if either an EOF condition or another
   unrecoverable error was encountered.
   如果遇到EOF条件或另一个不可恢复的错误，则将触发事件回调。

   For bufferevents with deferred callbacks, this is a bitwise OR of all errors
   that have happened on the bufferevent since the last callback invocation.
   对于具有延迟回调的缓冲区，这是自上次回调调用以来在缓冲区上发生的所有错误的按位或错误。

   @param bev the bufferevent for which the error condition was reached 达到错误条件的缓冲器
   @param what a conjunction of flags: BEV_EVENT_READING or BEV_EVENT_WRITING
	  to indicate if the error was encountered on the read or write path,
	  and one of the following flags: BEV_EVENT_EOF, BEV_EVENT_ERROR,
	  BEV_EVENT_TIMEOUT, BEV_EVENT_CONNECTED.
     一个标志的连接：  BEV_EVENT_READING 或 BEV_EVENT_WRITING ，以指示是否在读取或写路径上遇到错误，以及以下标志之一： 
     BEV_EVENT_EOF ， BEV_EVENT_ERROR ， BEV_EVENT_TIMEOUT ， BEV_EVENT_CONNECTED 。

   @param ctx the user-specified context for this bufferevent 此缓冲区的用户指定的上下文
*/
typedef void (*bufferevent_event_cb)(struct bufferevent *bev, short what, void *ctx);

/** Options that can be specified when creating a bufferevent 在创建缓冲程序时可以指定的选项 */
enum bufferevent_options {
	/** If set, we close the underlying file
	 * descriptor/bufferevent/whatever when this bufferevent is freed. 如果设置，当释放此缓冲程序时，我们关闭底层文件描述符/缓冲程序/任何。 */
	BEV_OPT_CLOSE_ON_FREE = (1<<0),

	/** If set, and threading is enabled, operations on this bufferevent
	 * are protected by a lock 如果设置并启用线程，则此缓冲器上的操作将由锁保护 */
	BEV_OPT_THREADSAFE = (1<<1),

	/** If set, callbacks are run deferred in the event loop. 如果设置，则在事件循环中延迟运行回调。 */
	BEV_OPT_DEFER_CALLBACKS = (1<<2),

	/** If set, callbacks are executed without locks being held on the
	* bufferevent.  This option currently requires that
	* BEV_OPT_DEFER_CALLBACKS also be set; a future version of Libevent
	* might remove the requirement. 如果设置，则在缓冲器上没有锁的情况下执行回调。此选项当前要求同时设置 BEV_OPT_DEFER_CALLBACKS ； Libevent 的未来版本可能会删除该要求。 */
	BEV_OPT_UNLOCK_CALLBACKS = (1<<3)
};

/**
  Create a new socket bufferevent over an existing socket.
  在现有的套接字上创建一个新的套接字缓冲器。

  @param base the event base to associate with the new bufferevent. 要与新的缓冲器相关联的事件基础。
  @param fd the file descriptor from which data is read and written to.
	    This file descriptor is not allowed to be a pipe(2).
	    It is safe to set the fd to -1, so long as you later
	    set it with bufferevent_setfd or bufferevent_socket_connect().
       从中阅读和写入数据的文件描述符。此文件描述符不允许是管道(2)。将fd设置为-1是安全的，只要您稍后将其设置为bufferevent_setfd或bufferevent_socket_connect（）。
  @param options Zero or more BEV_OPT_* flags 零个或更多的BEV_OPT_*标志
  @return a pointer to a newly allocated bufferevent struct, or NULL if an
	  error occurred 指向新分配的缓冲区结构的指针，如果发生错误，则为NULL
  @see bufferevent_free()
  */
EVENT2_EXPORT_SYMBOL
struct bufferevent *bufferevent_socket_new(struct event_base *base, evutil_socket_t fd, int options);

/**
   Launch a connect() attempt with a socket-based bufferevent.
   使用基于套接字的缓冲器启动 connect 尝试。

   When the connect succeeds, the eventcb will be invoked with
   BEV_EVENT_CONNECTED set.
   当连接成功时，将使用 BEV_EVENT_CONNECTED 集调用事件cb。

   If the bufferevent does not already have a socket set, we allocate a new
   socket here and make it nonblocking before we begin.
   如果缓冲器还没有一个套接字集，我们在这里分配一个新的套接字，并在我们开始之前使它不阻塞。

   If no address is provided, we assume that the socket is already connecting,
   and configure the bufferevent so that a BEV_EVENT_CONNECTED event will be
   yielded when it is done connecting.
   如果没有提供地址，我们假设套接字已经在连接，并配置缓冲器，以便在连接完成时将产生一个BEV_EVENT_CONNECTED事件。

   @param bufev an existing bufferevent allocated with
       bufferevent_socket_new(). 用bufferevent_socket_new（）分配的现有缓冲器。
   @param addr the address we should connect to 我们应该连接到的地址
   @param socklen The length of the address 该地址的长度
   @return 0 on success, -1 on failure.
 */
EVENT2_EXPORT_SYMBOL
int bufferevent_socket_connect(struct bufferevent *, const struct sockaddr *, int);

struct evdns_base;
/**
   Resolve the hostname 'hostname' and connect to it as with
   bufferevent_socket_connect().
   解析主机名“主机名”，并像使用bufferevent_socket_connect（）一样连接到它。

   @param bufev An existing bufferevent allocated with bufferevent_socket_new() 与bufferevent_socket_new（）一起分配的现有缓冲器
   @param evdns_base Optionally, an evdns_base to use for resolving hostnames
      asynchronously. May be set to NULL for a blocking resolve.
      或者，可以选择一个用于异步解析主机名的evdns_base。可以设置为NULL。
   @param family A preferred address family to resolve addresses to, or
      AF_UNSPEC for no preference.  Only AF_INET, AF_INET6, and AF_UNSPEC are
      supported. 首选地址族解析地址，或AF_UNSPEC没有首选。只支持AF_INET、AF_INET6和AF_UNSPEC。
   @param hostname The hostname to resolve; see below for notes on recognized
      formats 要解析的主机名；有关已识别格式的注释
   @param port The port to connect to on the resolved address. 在已解析的地址上要连接到的端口。
   @return 0 if successful, -1 on failure.

   Recognized hostname formats are:

       www.example.com	(hostname)
       1.2.3.4		(ipv4address)
       ::1		(ipv6address)
       [::1]		([ipv6address])

   Performance note: If you do not provide an evdns_base, this function
   may block while it waits for a DNS response.	 This is probably not
   what you want.
   性能注意：如果您不提供evdns_base，此函数可能会在等待 DNS 响应时被阻塞。这可能不是你想要的。
 */
EVENT2_EXPORT_SYMBOL
int bufferevent_socket_connect_hostname(struct bufferevent *,
    struct evdns_base *, int, const char *, int);

/**
   Return the error code for the last failed DNS lookup attempt made by
   bufferevent_socket_connect_hostname().
   返回 bufferevent_socket_connect_hostname 最后一次失败的DNS查找尝试的错误代码。

   @param bev The bufferevent object. 缓冲区对象。
   @return DNS error code.
   @see evutil_gai_strerror()
*/
EVENT2_EXPORT_SYMBOL
int bufferevent_socket_get_dns_error(struct bufferevent *bev);

/**
  Assign a bufferevent to a specific event_base.
  将缓冲程序分配给特定的event_base。

  NOTE that only socket bufferevents support this function.
  注意，只有套接字缓冲区支持此功能。

  @param base an event_base returned by event_init() 由event_init（）返回的 event_base
  @param bufev a bufferevent struct returned by bufferevent_new()
     or bufferevent_socket_new() 由bufferevent_new（）或bufferevent_socket_new（）返回的 bufferevent 结构
  @return 0 if successful, or -1 if an error occurred
  @see bufferevent_new()
 */
EVENT2_EXPORT_SYMBOL
int bufferevent_base_set(struct event_base *base, struct bufferevent *bufev);

/**
   Return the event_base used by a bufferevent
   返回 bufferevent 使用的 event_base
*/
EVENT2_EXPORT_SYMBOL
struct event_base *bufferevent_get_base(struct bufferevent *bev);

/**
  Assign a priority to a bufferevent.
  为缓冲器分配优先级。

  Only supported for socket bufferevents.
  仅支持套接字缓冲区。

  @param bufev a bufferevent struct
  @param pri the priority to be assigned 要分配的优先级
  @return 0 if successful, or -1 if an error occurred
  */
EVENT2_EXPORT_SYMBOL
int bufferevent_priority_set(struct bufferevent *bufev, int pri);

/**
   Return the priority of a bufferevent.
   返回 bufferevent 的优先级

   Only supported for socket bufferevents
   仅支持套接字缓冲区
 */
EVENT2_EXPORT_SYMBOL
int bufferevent_get_priority(const struct bufferevent *bufev);

/**
  Deallocate the storage associated with a bufferevent structure.
  取消分配与缓冲器结构相关联的存储空间。

  If there is pending data to write on the bufferevent, it probably won't be
  flushed before the bufferevent is freed.
  如果在缓冲器上有待写的数据，那么在释放缓冲器之前，它可能不会被刷新。

  @param bufev the bufferevent structure to be freed.
  */
EVENT2_EXPORT_SYMBOL
void bufferevent_free(struct bufferevent *bufev);


/**
  Changes the callbacks for a bufferevent.
  更改缓冲器的回调。

  @param bufev the bufferevent object for which to change callbacks 要更改其回调的缓冲区对象
  @param readcb callback to invoke when there is data to be read, or NULL if
	 no callback is desired 当有读取数据时调用回调，如果不需要回调时为NULL
  @param writecb callback to invoke when the file descriptor is ready for
	 writing, or NULL if no callback is desired 当文件描述符准备好写入时调用的回调，或者如果不需要回调时为NULL
  @param eventcb callback to invoke when there is an event on the file
	 descriptor 在文件描述符上有事件时调用的回调
  @param cbarg an argument that will be supplied to each of the callbacks 一个将被提供给每个回调的参数
	 (readcb, writecb, and errorcb)
  @see bufferevent_new()
  */
EVENT2_EXPORT_SYMBOL
void bufferevent_setcb(struct bufferevent *bufev,
    bufferevent_data_cb readcb, bufferevent_data_cb writecb,
    bufferevent_event_cb eventcb, void *cbarg);

/**
 Retrieves the callbacks for a bufferevent.
 检索缓冲程序的回调。

 @param bufev the bufferevent to examine. 待检查的 bufferevent
 @param readcb_ptr if readcb_ptr is nonnull, *readcb_ptr is set to the current
    read callback for the bufferevent. 如果readcb_ptr是非空的，则*readcb_ptr被设置为缓冲区的当前读取回调。
 @param writecb_ptr if writecb_ptr is nonnull, *writecb_ptr is set to the
    current write callback for the bufferevent. 如果writecb_ptr是非空的，则*writecb_ptr被设置为缓冲区的当前写回调。
 @param eventcb_ptr if eventcb_ptr is nonnull, *eventcb_ptr is set to the
    current event callback for the bufferevent. 如果eventcb_ptr不为空，则*eventcb_ptr被设置为缓冲区的当前事件回调。
 @param cbarg_ptr if cbarg_ptr is nonnull, *cbarg_ptr is set to the current
    callback argument for the bufferevent. 如果cbarg_ptr是非空的，则*cbarg_ptr被设置为缓冲区的当前回调参数。
 @see buffervent_setcb()//todo
*/
EVENT2_EXPORT_SYMBOL
void bufferevent_getcb(struct bufferevent *bufev,
    bufferevent_data_cb *readcb_ptr,
    bufferevent_data_cb *writecb_ptr,
    bufferevent_event_cb *eventcb_ptr,
    void **cbarg_ptr);

/**
  Changes the file descriptor on which the bufferevent operates.
  Not supported for all bufferevent types.
  更改缓冲程序所要操作的文件描述符。
  不支持所有缓冲区类型。

  @param bufev the bufferevent object for which to change the file descriptor 要为其更改文件描述符的缓冲区对象
  @param fd the file descriptor to operate on 要进行操作的文件描述符
*/
EVENT2_EXPORT_SYMBOL
int bufferevent_setfd(struct bufferevent *bufev, evutil_socket_t fd);

/**
   Returns the file descriptor associated with a bufferevent, or -1 if
   no file descriptor is associated with the bufferevent.
   返回与缓冲程序关联的文件描述符，如果没有与缓冲关联的文件描述符，则返回-1。
 */
EVENT2_EXPORT_SYMBOL
evutil_socket_t bufferevent_getfd(struct bufferevent *bufev);

/**
   Returns the underlying bufferevent associated with a bufferevent (if
   the bufferevent is a wrapper), or NULL if there is no underlying bufferevent.
   返回与缓冲程序关联的底层缓冲程序（如果缓冲程序是包装器），如果没有底层缓冲程序，则返回NULL。
 */
EVENT2_EXPORT_SYMBOL
struct bufferevent *bufferevent_get_underlying(struct bufferevent *bufev);

/**
  Write data to a bufferevent buffer.
  将数据写入缓冲区buffer。

  The bufferevent_write() function can be used to write data to the file
  descriptor.  The data is appended to the output buffer and written to the
  descriptor automatically as it becomes available for writing.
  bufferevent_write （）函数可以用来将数据写入文件描述符。数据被附加到输出缓冲区中，并在它可供写入时自动写入描述符。

  @param bufev the bufferevent to be written to
  @param data a pointer to the data to be written 一个指向要写入的数据的指针
  @param size the length of the data, in bytes 数据的长度，单位为字节
  @return 0 if successful, or -1 if an error occurred
  @see bufferevent_write_buffer()
  */
EVENT2_EXPORT_SYMBOL
int bufferevent_write(struct bufferevent *bufev,
    const void *data, size_t size);


/**
  Write data from an evbuffer to a bufferevent buffer.	The evbuffer is
  being drained as a result.
  将数据从 evbuffer 写入到 bufferevent 缓冲区。结果，evbuffer正在被耗尽。

  @param bufev the bufferevent to be written to
  @param buf the evbuffer to be written
  @return 0 if successful, or -1 if an error occurred
  @see bufferevent_write()
 */
EVENT2_EXPORT_SYMBOL
int bufferevent_write_buffer(struct bufferevent *bufev, struct evbuffer *buf);


/**
  Read data from a bufferevent buffer.
  从bufferevent缓冲区读取数据。

  The bufferevent_read() function is used to read data from the input buffer.
  bufferevent_read（）函数用于从输入缓冲区中读取数据。

  @param bufev the bufferevent to be read from 要读取的bufferevent
  @param data pointer to a buffer that will store the data 指向将存储数据的缓冲区的指针
  @param size the size of the data buffer, in bytes 数据缓冲区的大小，以字节为单位
  @return the amount of data read, in bytes. 读取的数据量，以字节为单位。
 */
EVENT2_EXPORT_SYMBOL
size_t bufferevent_read(struct bufferevent *bufev, void *data, size_t size);

/**
  Read data from a bufferevent buffer into an evbuffer.	 This avoids
  memory copies.
  将数据从bufferevent读取到evbuffer中。这避免了内存副本。

  @param bufev the bufferevent to be read from 要读取的bufferevent
  @param buf the evbuffer to which to add data 要向其添加数据的evbuffer
  @return 0 if successful, or -1 if an error occurred.
 */
EVENT2_EXPORT_SYMBOL
int bufferevent_read_buffer(struct bufferevent *bufev, struct evbuffer *buf);

/**
   Returns the input buffer.
   返回输入缓冲区。

   The user MUST NOT set the callback on this buffer.
   用户不能在此缓冲区上设置回调。

   @param bufev the bufferevent from which to get the evbuffer 用来获得evbuffer的bufferevent
   @return the evbuffer object for the input buffer 输入缓冲区的evbuffer对象
 */

EVENT2_EXPORT_SYMBOL
struct evbuffer *bufferevent_get_input(struct bufferevent *bufev);

/**
   Returns the output buffer.
   返回输出缓冲区。

   The user MUST NOT set the callback on this buffer.
   用户不能在此缓冲区上设置回调。

   When filters are being used, the filters need to be manually
   triggered if the output buffer was manipulated.
   当使用过滤器时，如果操纵了输出缓冲区，则需要手动触发过滤器。

   @param bufev the bufferevent from which to get the evbuffer 用来获得evbuffer的bufferevent
   @return the evbuffer object for the output buffer 输出缓冲区的evbuffer对象
 */

EVENT2_EXPORT_SYMBOL
struct evbuffer *bufferevent_get_output(struct bufferevent *bufev);

/**
  Enable a bufferevent.
  启用bufferevent。

  @param bufev the bufferevent to be enabled
  @param event any combination of EV_READ | EV_WRITE. EV_READ | EV_WRITE的任何组合。
  @return 0 if successful, or -1 if an error occurred
  @see bufferevent_disable()
 */
EVENT2_EXPORT_SYMBOL
int bufferevent_enable(struct bufferevent *bufev, short event);

/**
  Disable a bufferevent.
  禁用bufferevent。

  @param bufev the bufferevent to be disabled
  @param event any combination of EV_READ | EV_WRITE. EV_READ | EV_WRITE的任何组合。
  @return 0 if successful, or -1 if an error occurred
  @see bufferevent_enable()
 */
EVENT2_EXPORT_SYMBOL
int bufferevent_disable(struct bufferevent *bufev, short event);

/**
   Return the events that are enabled on a given bufferevent.
   返回在给定的缓冲程序上启用的事件。

   @param bufev the bufferevent to inspect
   @return A combination of EV_READ | EV_WRITE EV_READ | EV_WRITE的组合
 */
EVENT2_EXPORT_SYMBOL
short bufferevent_get_enabled(struct bufferevent *bufev);

/**
  Set the read and write timeout for a bufferevent.
  设置bufferevent的读写超时。

  A bufferevent's timeout will fire the first time that the indicated
  amount of time has elapsed since a successful read or write operation,
  during which the bufferevent was trying to read or write.
  自成功的读或写操作后，指定的时间将第一次触发，在此期间缓冲程序尝试读或写。
  缓冲事件的超时将在自成功读取或写入操作以来第一次经过所指示的时间量时触发,在此期间，缓冲区事件正在尝试读取或写入。

  (In other words, if reading or writing is disabled, or if the
  bufferevent's read or write operation has been suspended because
  there's no data to write, or not enough bandwidth, or so on, the
  timeout isn't active.  The timeout only becomes active when we we're
  willing to actually read or write.)
  (换句话说，如果读或写被禁用，或者缓冲器的读或写操作因为没有要写的数据或没有足够的带宽而被暂停，则超时不处于活动状态。只有当我们愿意真正读或写时，超时才会变得活跃。)

  Calling bufferevent_enable or setting a timeout for a bufferevent
  whose timeout is already pending resets its timeout.
  调用bufferevent_enable或为超时的缓冲程序设置超时将重置其超时。

  If the timeout elapses, the corresponding operation (EV_READ or
  EV_WRITE) becomes disabled until you re-enable it again.  The
  bufferevent's event callback is called with the
  BEV_EVENT_TIMEOUT|BEV_EVENT_READING or
  BEV_EVENT_TIMEOUT|BEV_EVENT_WRITING.
  如果超时，则相应的操作（EV_READ或EV_WRITE）将被禁用，直到重新启用它。使用BEV_EVENT_TIMEOUT|BEV_EVENT_READING或BEV_EVENT_TIMEOUT|BEV_EVENT_WRITING调用缓冲器的事件回调。

  @param bufev the bufferevent to be modified
  @param timeout_read the read timeout, or NULL
  @param timeout_write the write timeout, or NULL
 */
EVENT2_EXPORT_SYMBOL
int bufferevent_set_timeouts(struct bufferevent *bufev,
    const struct timeval *timeout_read, const struct timeval *timeout_write);

/**
  Sets the watermarks for read and write events.
  设置读和写事件的水印。

  On input, a bufferevent does not invoke the user read callback unless
  there is at least low watermark data in the buffer.	If the read buffer
  is beyond the high watermark, the bufferevent stops reading from the network.
  But be aware that bufferevent input/read buffer can overrun high watermark
  limit (typical example is openssl bufferevent), so you should not relay in
  this.
  在输入时，缓冲区不会调用用户读取回调，除非缓冲区中至少有低水位线数据。如果读取缓冲区超出高水位，则缓冲区停止从网络读取。
  但是请注意，缓冲区输入/读取缓冲区可以超过高水印限制（典型的例子是开放的缓冲区），所以您不应该在这里中继。

  On output, the user write callback is invoked whenever the buffered data
  falls below the low watermark.  Filters that write to this bufev will try
  not to write more bytes to this buffer than the high watermark would allow,
  except when flushing.
  在输出时，当缓冲数据低于低水位时，将调用用户写回调。写入此bufev的过滤器将尝试不写入此缓冲区的字节超过高水印所允许的字节，除非刷新时除外。

  @param bufev the bufferevent to be modified
  @param events EV_READ, EV_WRITE or both EV_READ、EV_WRITE或两者兼有
  @param lowmark the lower watermark to set 设置低水印
  @param highmark the high watermark to set 设置高水印
*/

EVENT2_EXPORT_SYMBOL
void bufferevent_setwatermark(struct bufferevent *bufev, short events,
    size_t lowmark, size_t highmark);

/**
  Retrieves the watermarks for read or write events.
  Returns non-zero if events contains not only EV_READ or EV_WRITE.
  Returns zero if events equal EV_READ or EV_WRITE
  检索读取或写事件的水印。
  如果事件不仅包含EV_READ或EV_WRITE，则返回非零。
  如果事件等于EV_READ或EV_WRITE，则返回零

  @param bufev the bufferevent to be examined 要查询的bufferevent
  @param events EV_READ or EV_WRITE
  @param lowmark receives the lower watermark if not NULL 如果不是空的，则接收到低的水印
  @param highmark receives the high watermark if not NULL 如果不是空的，则接收到高水位线
*/
EVENT2_EXPORT_SYMBOL
int bufferevent_getwatermark(struct bufferevent *bufev, short events,
    size_t *lowmark, size_t *highmark);

/**
   Acquire the lock on a bufferevent.  Has no effect if locking was not
   enabled with BEV_OPT_THREADSAFE.
   获得缓冲上的锁。如果没有使用BEV_OPT_THREADSAFE启用锁定，则无效。
 */
EVENT2_EXPORT_SYMBOL
void bufferevent_lock(struct bufferevent *bufev);

/**
   Release the lock on a bufferevent.  Has no effect if locking was not
   enabled with BEV_OPT_THREADSAFE.
   松开缓冲装置上的锁。如果没有使用BEV_OPT_THREADSAFE启用锁定，则无效。
 */
EVENT2_EXPORT_SYMBOL
void bufferevent_unlock(struct bufferevent *bufev);


/**
 * Public interface to manually increase the reference count of a bufferevent
 * this is useful in situations where a user may reference the bufferevent
 * somewhere else (unknown to libevent)
 * 公共接口可以手动增加缓冲器的引用计数，这在用户可能在其他地方引用缓冲器（自由事件未知）的情况下很有用
 *
 * @param bufev the bufferevent to increase the refcount on. 增加引用计数的bufferevent
 *
 */
EVENT2_EXPORT_SYMBOL
void bufferevent_incref(struct bufferevent *bufev);

/**
 * Public interface to manually decrement the reference count of a bufferevent
 * 手动减少缓冲器的引用计数的公共接口
 *
 * Warning: make sure you know what you're doing. This is mainly used in
 * conjunction with bufferevent_incref(). This will free up all data associated
 * with a bufferevent if the reference count hits 0.
 * 警告：确保你知道自己在做什么。这主要与bufferevent_incref（）一起使用。如果引用计数为0，这将释放与缓冲器相关联的所有数据。
 *
 * @param bufev the bufferevent to decrement the refcount on 减少引用计数的bufferevent
 *
 * @return 1 if the bufferevent was freed, otherwise 0 (still referenced) 1，如果缓冲器被释放，否则为0（仍被引用）
 */
EVENT2_EXPORT_SYMBOL
int bufferevent_decref(struct bufferevent *bufev);

/**
   Flags that can be passed into filters to let them know how to
   deal with the incoming data.
   可以传递到过滤器中的标志，让他们知道如何处理传入的数据。
*/
enum bufferevent_flush_mode {
	/** usually set when processing data 通常是在处理数据时设置的 */
	BEV_NORMAL = 0,

	/** want to checkpoint all data sent. 希望检查所有已发送的数据。 */
	BEV_FLUSH = 1,

	/** encountered EOF on read or done sending data 在读取或完成发送数据时遇到EOF */
	BEV_FINISHED = 2
};

/**
   Triggers the bufferevent to produce more data if possible.
   如果可能的话，触发缓冲器以产生更多的数据。

   @param bufev the bufferevent object
   @param iotype either EV_READ or EV_WRITE or both. 或是EV_READ或EV_WRITE，或两者都有。
   @param mode either BEV_NORMAL or BEV_FLUSH or BEV_FINISHED 或BEV_NORMAL或BEV_FLUSH或BEV_FINISHED
   @return -1 on failure, 0 if no data was produces, 1 if data was produced 失败时-1，如果没有数据产生0，如果数据产生1
 */
EVENT2_EXPORT_SYMBOL
int bufferevent_flush(struct bufferevent *bufev,
    short iotype,
    enum bufferevent_flush_mode mode);

/**
   Flags for bufferevent_trigger(_event) that modify when and how to trigger
   the callback.
   为修改何时以及如何触发回调的bufferevent_trigger（_事件）设置的标记。
*/
enum bufferevent_trigger_options {
	/** trigger the callback regardless of the watermarks 触发回调，而不管水印如何 */
	BEV_TRIG_IGNORE_WATERMARKS = (1<<16),

	/** defer even if the callbacks are not 推迟，即使回调没有 */
	BEV_TRIG_DEFER_CALLBACKS = BEV_OPT_DEFER_CALLBACKS

	/* (Note: for internal reasons, these need to be disjoint from
	 * bufferevent_options, except when they mean the same thing. 
      注意：由于内部原因，这些东西需要与bufferevent_options不相交，除非它们指的是相同的东西。*/
};

/**
   Triggers bufferevent data callbacks.
   触发bufferevent数据回调。

   The function will honor watermarks unless options contain
   BEV_TRIG_IGNORE_WATERMARKS. If the options contain BEV_OPT_DEFER_CALLBACKS,
   the callbacks are deferred.
   该功能将显示水印，除非选项包含BEV_TRIG_IGNORE_WATERMARKS。如果这些选项中包含BEV_OPT_DEFER_CALLBACKS，则会延迟回调。

   @param bufev the bufferevent object
   @param iotype either EV_READ or EV_WRITE or both. 或是EV_READ或EV_WRITE，或两者都有。
   @param options
 */
EVENT2_EXPORT_SYMBOL
void bufferevent_trigger(struct bufferevent *bufev, short iotype,
    int options);

/**
   Triggers the bufferevent event callback.
   触发缓冲区事件回调。

   If the options contain BEV_OPT_DEFER_CALLBACKS, the callbacks are deferred.
   如果这些选项中包含BEV_OPT_DEFER_CALLBACKS，则会延迟回调。

   @param bufev the bufferevent object
   @param what the flags to pass onto the event callback 要传递到事件回调中的标志
   @param options
 */
EVENT2_EXPORT_SYMBOL
void bufferevent_trigger_event(struct bufferevent *bufev, short what,
    int options);

/**
   @name Filtering support 过滤支持

   @{
*/
/**
   Values that filters can return. 过滤器可以返回的值。
 */
enum bufferevent_filter_result {
	/** everything is okay 一切都好 */
	BEV_OK = 0,

	/** the filter needs to read more data before output 过滤器需要在输出前读取更多的数据 */
	BEV_NEED_MORE = 1,

	/** the filter encountered a critical error, no further data
	    can be processed. 过滤器遇到严重错误，无法处理进一步的数据。 */
	BEV_ERROR = 2
};

/** A callback function to implement a filter for a bufferevent. 一个用来实现一个缓冲器的过滤器的回调函数。

    @param src An evbuffer to drain data from. 用于排出数据的事件缓冲区。
    @param dst An evbuffer to add data to. 要向其中添加数据的ev缓冲区。
    @param limit A suggested upper bound of bytes to write to dst.
       The filter may ignore this value, but doing so means that
       it will overflow the high-water mark associated with dst.
       -1 means "no limit".建议写入dst的字节的上界。过滤器可能会忽略这个值，但这样做意味着它将溢出与dst相关联的高水位标记。-1表示无限制。
    @param mode Whether we should write data as may be convenient
       (BEV_NORMAL), or flush as much data as we can (BEV_FLUSH),
       or flush as much as we can, possibly including an end-of-stream
       marker (BEV_FINISH).我们是应该按照方便的方式编写数据（BEV_NORMAL），还是尽可能多地刷新数据（BEV_FLUSH），还是尽可能多地刷新数据，可能包括流的结束标记（BEV_FINISH）。
    @param ctx A user-supplied pointer. 用户提供的指针。

    @return BEV_OK if we wrote some data; BEV_NEED_MORE if we can't
       produce any more output until we get some input; and BEV_ERROR
       on an error. 返回BEV_OK，如果我们写了一些数据；BEV_NEED_MORE，如果我们不能产生更多的输出，直到我们得到一些输入；和BEV_ERROR的错误。
 */
typedef enum bufferevent_filter_result (*bufferevent_filter_cb)(
    struct evbuffer *src, struct evbuffer *dst, ev_ssize_t dst_limit,
    enum bufferevent_flush_mode mode, void *ctx);

/**
   Allocate a new filtering bufferevent on top of an existing bufferevent.
   在现有的缓冲器之上分配一个新的过滤缓冲器。

   @param underlying the underlying bufferevent. 底层的bufferevent
   @param input_filter The filter to apply to data we read from the underlying
     bufferevent 应用于我们从底层缓冲区读取的数据的过滤器
   @param output_filter The filer to apply to data we write to the underlying
     bufferevent 要应用于我们写入底层缓冲区的数据的文件管理程序
   @param options A bitfield of bufferevent options. 缓冲选项的位场。
   @param free_context A function to use to free the filter context when
     this bufferevent is freed. 在释放此缓冲区时用于释放过滤器上下文的函数。
   @param ctx A context pointer to pass to the filter functions. 一个要传递给过滤器函数的上下文指针。
 */
EVENT2_EXPORT_SYMBOL
struct bufferevent *
bufferevent_filter_new(struct bufferevent *underlying,
		       bufferevent_filter_cb input_filter,
		       bufferevent_filter_cb output_filter,
		       int options,
		       void (*free_context)(void *),
		       void *ctx);
/**@}*/

/**
   Allocate a pair of linked bufferevents.  The bufferevents behave as would
   two bufferevent_sock instances connected to opposite ends of a
   socketpair(), except that no internal socketpair is allocated.
   分配一对已链接的缓冲器。缓冲剂的行为与两个连接到socketpair（）的相对两端的bufferevent_sock实例一样，除了没有分配内部的容器对。

   @param base The event base to associate with the socketpair. 与socketpair关联的事件基础。
   @param options A set of options for this bufferevent 这个缓冲器的一组选项
   @param pair A pointer to an array to hold the two new bufferevent objects. 一个指向数组的指针，用来保存两个新的缓冲对象。
   @return 0 on success, -1 on failure.
 */
EVENT2_EXPORT_SYMBOL
int bufferevent_pair_new(struct event_base *base, int options,
    struct bufferevent *pair[2]);

/**
   Given one bufferevent returned by bufferevent_pair_new(), returns the
   other one if it still exists.  Otherwise returns NULL.
   给定bufferevent_pair_new（）返回的一个缓冲区，如果另一个仍然存在，则返回另一个。否则返回NULL。
 */
EVENT2_EXPORT_SYMBOL
struct bufferevent *bufferevent_pair_get_partner(struct bufferevent *bev);

/**
   Abstract type used to configure rate-limiting on a bufferevent or a group
   of bufferevents.
   抽象类型，用于配置一个缓冲器或一组缓冲器上的速率限制。
 */
struct ev_token_bucket_cfg;

/**
   A group of bufferevents which are configured to respect the same rate
   limit.
   一组缓冲器，它们被配置为尊重相同的速率限制。
*/
struct bufferevent_rate_limit_group;

/** Maximum configurable rate- or burst-limit. 最大可配置速率或突发限制。 */
#define EV_RATE_LIMIT_MAX EV_SSIZE_MAX

/**
   Initialize and return a new object to configure the rate-limiting behavior
   of bufferevents.
   初始化并返回一个新对象，以配置缓冲区的速率限制行为。

   @param read_rate The maximum number of bytes to read per tick on
     average. 平均每刻度要读取的最大字节数。
   @param read_burst The maximum number of bytes to read in any single tick. 要读取的最大字节数。
   @param write_rate The maximum number of bytes to write per tick on
     average. 平均每刻写入的最大字节数。
   @param write_burst The maximum number of bytes to write in any single tick. 要写入的最大字节数。
   @param tick_len The length of a single tick.	 Defaults to one second.
     Any fractions of a millisecond are ignored. 一个刻度的长度。默认为一秒。一毫秒内的任何分数都会被忽略。

   Note that all rate-limits hare are currently best-effort: future versions
   of Libevent may implement them more tightly.
   请注意，所有的速率限制目前都是最大努力：Libevent的未来版本可能会更严格地实现它们。
 */
EVENT2_EXPORT_SYMBOL
struct ev_token_bucket_cfg *ev_token_bucket_cfg_new(
	size_t read_rate, size_t read_burst,
	size_t write_rate, size_t write_burst,
	const struct timeval *tick_len);

/** Free all storage held in 'cfg'.

    Note: 'cfg' is not currently reference-counted; it is not safe to free it
    until no bufferevent is using it.
    注意：“cfg”当前没有引用计数；释放它是不安全的，直到没有bufferevent使用它。
 */
EVENT2_EXPORT_SYMBOL
void ev_token_bucket_cfg_free(struct ev_token_bucket_cfg *cfg);

/**
   Set the rate-limit of a the bufferevent 'bev' to the one specified in
   'cfg'.  If 'cfg' is NULL, disable any per-bufferevent rate-limiting on
   'bev'.
   将缓冲器“bev”的速率限制设置为“cfg”中指定的限制。如果“cfg”为NULL，则禁用“bev”上的任何每个缓冲区速率限制。

   Note that only some bufferevent types currently respect rate-limiting.
   They are: socket-based bufferevents (normal and IOCP-based), and SSL-based
   bufferevents.
   请注意，目前只有一些bufferevent类型尊重速率限制。它们是：基于套接字的缓冲程序（普通的和基于IOCP的）和基于ssl的缓冲程序。

   Return 0 on success, -1 on failure.
 */
EVENT2_EXPORT_SYMBOL
int bufferevent_set_rate_limit(struct bufferevent *bev,
    struct ev_token_bucket_cfg *cfg);

/**
   Create a new rate-limit group for bufferevents.  A rate-limit group
   constrains the maximum number of bytes sent and received, in toto,
   by all of its bufferevents.
   为缓冲器创建一个新的速率限制组。速率限制组限制其所有缓冲程序总共发送和接收的最大字节数。

   @param base An event_base to run any necessary timeouts for the group.
      Note that all bufferevents in the group do not necessarily need to share
      this event_base. 用于为组运行任何必要的超时时间的event_base。请注意，组中的所有缓冲区都不一定需要共享此event_base。
   @param cfg The rate-limit for this group. 这个组的速率限制。

   Note that all rate-limits hare are currently best-effort: future versions
   of Libevent may implement them more tightly.
   请注意，所有的速率限制目前都是最大努力：Libevent的未来版本可能会更严格地实现它们。

   Note also that only some bufferevent types currently respect rate-limiting.
   They are: socket-based bufferevents (normal and IOCP-based), and SSL-based
   bufferevents.
   还要注意，目前只有一些bufferevent类型尊重速率限制。它们是：基于套接字的缓冲程序（普通的和基于IOCP的）和基于ssl的缓冲程序。
 */
EVENT2_EXPORT_SYMBOL
struct bufferevent_rate_limit_group *bufferevent_rate_limit_group_new(
	struct event_base *base,
	const struct ev_token_bucket_cfg *cfg);
/**
   Change the rate-limiting settings for a given rate-limiting group. 更改给定的速率限制组的速率限制设置。

   Return 0 on success, -1 on failure.
*/
EVENT2_EXPORT_SYMBOL
int bufferevent_rate_limit_group_set_cfg(
	struct bufferevent_rate_limit_group *,
	const struct ev_token_bucket_cfg *);

/**
   Change the smallest quantum we're willing to allocate to any single
   bufferevent in a group for reading or writing at a time.
   更改我们愿意分配给组中任何单个bufferevent的最小量子，以便一次阅读或写作。

   The rationale is that, because of TCP/IP protocol overheads and kernel
   behavior, if a rate-limiting group is so tight on bandwidth that you're
   only willing to send 1 byte per tick per bufferevent, you might instead
   want to batch up the reads and writes so that you send N bytes per
   1/N of the bufferevents (chosen at random) each tick, so you still wind
   up send 1 byte per tick per bufferevent on average, but you don't send
   so many tiny packets.
   其基本原理是，由于TCP/IP协议开销和内核行为，如果一个速率限制组在带宽上非常紧张，以至于你只愿意在每个缓冲区事件中每时刻发送1个字节，你可能会想批量处理读写操作，
   这样你每时刻每1/N缓冲区事件（随机选择）发送N个字节，所以你仍然会平均每缓冲区事件发送1个比特，但你不会发送太多的小数据包。

   The default min-share is currently 64 bytes. 默认最小共享当前为64字节。

   Returns 0 on success, -1 on failure.
 */
EVENT2_EXPORT_SYMBOL
int bufferevent_rate_limit_group_set_min_share(
	struct bufferevent_rate_limit_group *, size_t);

/**
   Free a rate-limiting group.  The group must have no members when
   this function is called. 释放一个限速组。调用此函数时，组必须没有成员。
*/
EVENT2_EXPORT_SYMBOL
void bufferevent_rate_limit_group_free(struct bufferevent_rate_limit_group *);

/**
   Add 'bev' to the list of bufferevents whose aggregate reading and writing
   is restricted by 'g'.  If 'g' is NULL, remove 'bev' from its current group.
   将'bev'添加到其总读写受到'g'限制的缓冲事件列表中。如果'g'为NULL，则从其当前组中删除'bev'。

   A bufferevent may belong to no more than one rate-limit group at a time.
   If 'bev' is already a member of a group, it will be removed from its old
   group before being added to 'g'.
   缓冲事件一次只能属于一个速率限制组。如果'bev'已经是某个组的成员，它将在添加到'g'之前从其旧组中删除。

   Return 0 on success and -1 on failure.
 */
EVENT2_EXPORT_SYMBOL
int bufferevent_add_to_rate_limit_group(struct bufferevent *bev,
    struct bufferevent_rate_limit_group *g);

/** Remove 'bev' from its current rate-limit group (if any). 从当前的速率限制组中删除“bev”（如果有的话）。 */
EVENT2_EXPORT_SYMBOL
int bufferevent_remove_from_rate_limit_group(struct bufferevent *bev);

/**
   Set the size limit for single read operation.

   Set to 0 for a reasonable default.

   Return 0 on success and -1 on failure.
 */
EVENT2_EXPORT_SYMBOL
int bufferevent_set_max_single_read(struct bufferevent *bev, size_t size);

/**
   Set the size limit for single write operation.
   设置单次读取操作的大小限制。

   Set to 0 for a reasonable default.
   设置为0作为合理的默认值。

   Return 0 on success and -1 on failure.
 */
EVENT2_EXPORT_SYMBOL
int bufferevent_set_max_single_write(struct bufferevent *bev, size_t size);

/** Get the current size limit for single read operation. 获取单次读取操作的当前大小限制。 */
EVENT2_EXPORT_SYMBOL
ev_ssize_t bufferevent_get_max_single_read(struct bufferevent *bev);

/** Get the current size limit for single write operation. 获取单次写入操作的当前大小限制。 */
EVENT2_EXPORT_SYMBOL
ev_ssize_t bufferevent_get_max_single_write(struct bufferevent *bev);

/**
   @name Rate limit inspection 速率限制检查

   Return the current read or write bucket size for a bufferevent.
   If it is not configured with a per-bufferevent ratelimit, return
   EV_SSIZE_MAX.  This function does not inspect the group limit, if any.
   Note that it can return a negative value if the bufferevent has been
   made to read or write more than its limit.
   返回缓冲事件的当前读写桶大小。如果没有配置每个缓冲区事件的速率限制，则返回EV_SSIZE_MAX。此函数不会检查组限制（如果有的话）。
   请注意，如果缓冲区事件的读写次数超过其限制，则它可能会返回负值。

   @{
 */
EVENT2_EXPORT_SYMBOL
ev_ssize_t bufferevent_get_read_limit(struct bufferevent *bev);
EVENT2_EXPORT_SYMBOL
ev_ssize_t bufferevent_get_write_limit(struct bufferevent *bev);
/*@}*/

EVENT2_EXPORT_SYMBOL
ev_ssize_t bufferevent_get_max_to_read(struct bufferevent *bev);
EVENT2_EXPORT_SYMBOL
ev_ssize_t bufferevent_get_max_to_write(struct bufferevent *bev);

EVENT2_EXPORT_SYMBOL
const struct ev_token_bucket_cfg *bufferevent_get_token_bucket_cfg(const struct bufferevent * bev);

/**
   @name Group Rate limit inspection 组的速率限制检查

   Return the read or write bucket size for a bufferevent rate limit
   group.  Note that it can return a negative value if bufferevents in
   the group have been made to read or write more than their limits.
   返回缓冲区事件速率限制组的读写桶大小。请注意，如果组中的缓冲区事件的读写次数超过了其限制，则它可能会返回负值。

   @{
 */
EVENT2_EXPORT_SYMBOL
ev_ssize_t bufferevent_rate_limit_group_get_read_limit(
	struct bufferevent_rate_limit_group *);
EVENT2_EXPORT_SYMBOL
ev_ssize_t bufferevent_rate_limit_group_get_write_limit(
	struct bufferevent_rate_limit_group *);
/*@}*/

/**
   @name Rate limit manipulation 速率限制操纵

   Subtract a number of bytes from a bufferevent's read or write bucket.
   The decrement value can be negative, if you want to manually refill
   the bucket.	If the change puts the bucket above or below zero, the
   bufferevent will resume or suspend reading writing as appropriate.
   These functions make no change in the buckets for the bufferevent's
   group, if any.
   从缓冲区事件的读或写桶中减去一定数量的字节。
   如果您想手动重新填充桶，减量值可以是负值。如果更改使桶高于或低于零，缓冲事件将根据需要恢复或暂停读写。
   这些函数不会对bufferevent组的bucket进行任何更改（如果有的话）。

   Returns 0 on success, -1 on internal error.

   @{
 */
EVENT2_EXPORT_SYMBOL
int bufferevent_decrement_read_limit(struct bufferevent *bev, ev_ssize_t decr);
EVENT2_EXPORT_SYMBOL
int bufferevent_decrement_write_limit(struct bufferevent *bev, ev_ssize_t decr);
/*@}*/

/**
   @name Group rate limit manipulation 组的速率限制操纵

   Subtract a number of bytes from a bufferevent rate-limiting group's
   read or write bucket.  The decrement value can be negative, if you
   want to manually refill the bucket.	If the change puts the bucket
   above or below zero, the bufferevents in the group will resume or
   suspend reading writing as appropriate.
   从缓冲区事件速率限制组的读或写桶中减去一定数量的字节。如果您想手动重新填充桶，减量值可以是负值。如果更改使桶高于或低于零，组中的缓冲事件将根据需要恢复或暂停读写。

   Returns 0 on success, -1 on internal error.

   @{
 */
EVENT2_EXPORT_SYMBOL
int bufferevent_rate_limit_group_decrement_read(
	struct bufferevent_rate_limit_group *, ev_ssize_t);
EVENT2_EXPORT_SYMBOL
int bufferevent_rate_limit_group_decrement_write(
	struct bufferevent_rate_limit_group *, ev_ssize_t);
/*@}*/


/**
 * Inspect the total bytes read/written on a group.
 * 检查组上读取/写入的总字节数。
 *
 * Set the variable pointed to by total_read_out to the total number of bytes
 * ever read on grp, and the variable pointed to by total_written_out to the
 * total number of bytes ever written on grp.
 * 将total_read_out指向的变量设置为grp上读取的字节总数，将total_writen_out指向的变量设为grp中写入的字节总数。 */
EVENT2_EXPORT_SYMBOL
void bufferevent_rate_limit_group_get_totals(
    struct bufferevent_rate_limit_group *grp,
    ev_uint64_t *total_read_out, ev_uint64_t *total_written_out);

/**
 * Reset the total bytes read/written on a group.
 * 重置组上读取/写入的总字节数。
 *
 * Reset the number of bytes read or written on grp as given by
 * bufferevent_rate_limit_group_reset_totals(). 重置grp上读取或写入的字节数，如bufferevent_rate_limit_group_Reset_totals（）所示。 */
EVENT2_EXPORT_SYMBOL
void
bufferevent_rate_limit_group_reset_totals(
	struct bufferevent_rate_limit_group *grp);

#ifdef __cplusplus
}
#endif

#endif /* EVENT2_BUFFEREVENT_H_INCLUDED_ */
