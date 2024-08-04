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
#ifndef EVENT2_LISTENER_H_INCLUDED_
#define EVENT2_LISTENER_H_INCLUDED_

#include <event2/visibility.h>

#ifdef __cplusplus
extern "C" {
#endif

#include <event2/event.h>

struct sockaddr;
struct evconnlistener;

/**
   A callback that we invoke when a listener has a new connection.
   当侦听器有新连接时调用的回调。

   @param listener The evconnlistener
   @param fd The new file descriptor   新的文件描述符
   @param addr The source address of the connection   连接的源地址
   @param socklen The length of addr   addr的长度
   @param user_arg the pointer passed to evconnlistener_new()  指针传递给 evconnlistener_new
 */
typedef void (*evconnlistener_cb)(struct evconnlistener *, evutil_socket_t, struct sockaddr *, int socklen, void *);

/**
   A callback that we invoke when a listener encounters a non-retriable error.
   当侦听器遇到不可重试的错误时，我们调用的回调。

   @param listener The evconnlistener
   @param user_arg the pointer passed to evconnlistener_new()  指针传递给 evconnlistener_new
 */
typedef void (*evconnlistener_errorcb)(struct evconnlistener *, void *);

/** Flag: Indicates that we should not make incoming sockets nonblocking
 * before passing them to the callback. 默认情况下，当连接监听器接收到新的客户端socket连接后，会把该socket设置为非阻塞的。如果设置该选项，那么就把之客户端socket保留为阻塞的 */
#define LEV_OPT_LEAVE_SOCKETS_BLOCKING	(1u<<0)
/** Flag: Indicates that freeing the listener should close the underlying
 * socket.  标志：当连接监听器释放时，会自动关闭底层的socket。 */
#define LEV_OPT_CLOSE_ON_FREE		(1u<<1)
/** Flag: Indicates that we should set the close-on-exec flag, if possible Flag：表示如果可能的话，为底层的socket设置close-on-exec标志 */
#define LEV_OPT_CLOSE_ON_EXEC		(1u<<2)
/** Flag: Indicates that we should disable the timeout (if any) between when
 * this socket is closed and when we can listen again on the same port. 
 * 标志：在某些平台，默认情况下当一个监听socket被关闭时，其他socket不能马上绑定到同一个端口，要等一会儿才行。
 * 设置该标志后，Libevent会把该socket设置成reuseable。这样，关闭该socket后，其他socket就能马上使用同一个端口 */
#define LEV_OPT_REUSEABLE		(1u<<3)
/** Flag: Indicates that the listener should be locked so it's safe to use
 * from multiple threadcs at once.  标志：为连接监听器分配锁。这样可以确保线程安全。 */
#define LEV_OPT_THREADSAFE		(1u<<4)
/** Flag: Indicates that the listener should be created in disabled
 * state. Use evconnlistener_enable() to enable it later. 
 * 标志：表示侦听器应在禁用状态下创建。稍后使用 evconnlistener_enable 启用它。 */
#define LEV_OPT_DISABLED		(1u<<5)
/** Flag: Indicates that the listener should defer accept() until data is
 * available, if possible.  Ignored on platforms that do not support this.
 * 标志：表示如果可能的话，侦听器应将accept（）推迟到数据可用为止。在不支持此功能的平台上被忽略。
 *
 * This option can help performance for protocols where the client transmits
 * immediately after connecting.  Do not use this option if your protocol
 * _doesn't_ start out with the client transmitting data, since in that case
 * this option will sometimes cause the kernel to never tell you about the
 * connection.
 * 此选项可以帮助客户端在连接后立即传输的协议的性能。如果你的协议 _doesn't_ 从客户端传输数据开始，不要使用此选项，因为在这种情况下，此选项有时会导致内核永远不会告诉你连接的情况。
 *
 * This option is only supported by evconnlistener_new_bind(): it can't
 * work with evconnlistener_new_fd(), since the listener needs to be told
 * to use the option before it is actually bound.
 * 此选项仅受 evconnlistener_new_bind （）支持：它不能与 evconnlistener_new_fd （）一起使用，因为在实际绑定之前需要告诉监听器使用该选项。
 */
#define LEV_OPT_DEFERRED_ACCEPT		(1u<<6)
/** Flag: Indicates that we ask to allow multiple servers (processes or
 * threads) to bind to the same port if they each set the option. 
 * 标志：表示我们要求允许多个服务器（进程或线程）绑定到同一端口，如果它们都设置了该选项。
 * 
 * SO_REUSEPORT is what most people would expect SO_REUSEADDR to be, however
 * SO_REUSEPORT does not imply SO_REUSEADDR.
 * SO_REUSEPORT 是大多数人所期望的SO_REUSEADDR，但SO_REUSEPORT并不意味着SO_REUSEADD。
 *
 * This is only available on Linux and kernel 3.9+ 这仅在Linux和内核3.9+上可用
 */
#define LEV_OPT_REUSEABLE_PORT		(1u<<7)
/** Flag: Indicates that the listener wants to work only in IPv6 socket.
 *  标志：表示侦听器只想在IPv6套接字中工作。
 *
 * According to RFC3493 and most Linux distributions, default value is to
 * work in IPv4-mapped mode. If there is a requirement to bind same port
 * on same ip addresses but different handlers for both IPv4 and IPv6,
 * it is required to set IPV6_V6ONLY socket option to be sure that the
 * code works as expected without affected by bindv6only sysctl setting in
 * system.
 * 根据RFC3493和大多数Linux发行版，默认值是在IPv4映射模式下工作。
 * 如果需要在相同的ip地址上绑定相同的端口，但IPv4和IPv6的处理程序不同，则需要设置IPv6_V6ONLY套接字选项，以确保代码按预期工作，而不受系统中 bindv6only sysctl 设置的影响。
 *
 * This socket option also supported by Windows.   Windows也支持此套接字选项。
 */
#define LEV_OPT_BIND_IPV6ONLY		(1u<<8)

/**
   Allocate a new evconnlistener object to listen for incoming TCP connections
   on a given file descriptor.
   分配一个新的evconnliester对象，以监听给定文件描述符上的传入TCP连接。

   @param base The event base to associate the listener with.  与侦听器关联的事件库。
   @param cb A callback to be invoked when a new connection arrives.  If the
      callback is NULL, the listener will be treated as disabled until the
      callback is set.
      当新连接到达时调用的回调。如果回调为NULL，则侦听器将被视为已禁用，直到设置回调为止。
   @param ptr A user-supplied pointer to give to the callback. 用户提供的指向回调的指针。
   @param flags Any number of LEV_OPT_* flags   任意数量的LEV_OPT_*标志
   @param backlog Passed to the listen() call to determine the length of the
      acceptable connection backlog.  Set to -1 for a reasonable default.
      Set to 0 if the socket is already listening.
      传递给listen（）调用，以确定可接受的连接积压的长度。设置为-1表示合理的默认值。如果套接字已在侦听，则设置为0。
   @param fd The file descriptor to listen on.  It must be a nonblocking
      file descriptor, and it should already be bound to an appropriate
      port and address.
      要监听的文件描述符。它必须是非阻塞文件描述符，并且应该已经绑定到适当的端口和地址。
*/
EVENT2_EXPORT_SYMBOL
struct evconnlistener *evconnlistener_new(struct event_base *base,
    evconnlistener_cb cb, void *ptr, unsigned flags, int backlog,
    evutil_socket_t fd);
/**
   Allocate a new evconnlistener object to listen for incoming TCP connections
   on a given address.
   分配一个新的 evconnlistener 对象来监听给定地址上的传入TCP连接。

   @param base The event base to associate the listener with. 与侦听器关联的事件库。
   @param cb A callback to be invoked when a new connection arrives. If the
      callback is NULL, the listener will be treated as disabled until the
      callback is set.
      当新连接到达时调用的回调。如果回调为NULL，则侦听器将被视为已禁用，直到设置回调为止。
   @param ptr A user-supplied pointer to give to the callback. 用户提供的指向回调的指针。
   @param flags Any number of LEV_OPT_* flags   任意数量的LEV_OPT_*标志
   @param backlog Passed to the listen() call to determine the length of the
      acceptable connection backlog.  Set to -1 for a reasonable default.
      传递给listen（）调用，以确定可接受的连接积压的长度。设置为-1表示合理的默认值。
   @param addr The address to listen for connections on. 用于监听连接的地址。
   @param socklen The length of the address. 地址的长度。
 */
EVENT2_EXPORT_SYMBOL
struct evconnlistener *evconnlistener_new_bind(struct event_base *base,
    evconnlistener_cb cb, void *ptr, unsigned flags, int backlog,
    const struct sockaddr *sa, int socklen);
/**
   Disable and deallocate an evconnlistener. 禁用并取消分配 evconnlistener
 */
EVENT2_EXPORT_SYMBOL
void evconnlistener_free(struct evconnlistener *lev);
/**
   Re-enable an evconnlistener that has been disabled.   重新启用已禁用的 evconnlistener
 */
EVENT2_EXPORT_SYMBOL
int evconnlistener_enable(struct evconnlistener *lev);
/**
   Stop listening for connections on an evconnlistener.  停止监听evconnlistener上的连接。
 */
EVENT2_EXPORT_SYMBOL
int evconnlistener_disable(struct evconnlistener *lev);

/** Return an evconnlistener's associated event_base. 返回evconnlistener的关联事件库。 */
EVENT2_EXPORT_SYMBOL
struct event_base *evconnlistener_get_base(struct evconnlistener *lev);

/** Return the socket that an evconnlistner is listening on. 返回evconnlistner正在监听的套接字。 */
EVENT2_EXPORT_SYMBOL
evutil_socket_t evconnlistener_get_fd(struct evconnlistener *lev);

/** Change the callback on the listener to cb and its user_data to arg.
 *  将侦听器上的回调更改为cb，将其user_data更改为arg。
 */
EVENT2_EXPORT_SYMBOL
void evconnlistener_set_cb(struct evconnlistener *lev,
    evconnlistener_cb cb, void *arg);

/** Set an evconnlistener's error callback.  设置evconnlistener的错误回调。 */
EVENT2_EXPORT_SYMBOL
void evconnlistener_set_error_cb(struct evconnlistener *lev,
    evconnlistener_errorcb errorcb);

#ifdef __cplusplus
}
#endif

#endif
