/*
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
#ifndef EVENT2_UTIL_H_INCLUDED_
#define EVENT2_UTIL_H_INCLUDED_

/** @file event2/util.h

  Common convenience functions for cross-platform portability and
  related socket manipulations.
  跨平台可移植性和相关套接字操作的常见便利功能。

 */
#include <event2/visibility.h>

#ifdef __cplusplus
extern "C" {
#endif

#include <event2/event-config.h>
#ifdef EVENT__HAVE_SYS_TIME_H
#include <sys/time.h>
#endif
#ifdef EVENT__HAVE_STDINT_H
#include <stdint.h>
#elif defined(EVENT__HAVE_INTTYPES_H)
#include <inttypes.h>
#endif
#ifdef EVENT__HAVE_SYS_TYPES_H
#include <sys/types.h>
#endif
#ifdef EVENT__HAVE_STDDEF_H
#include <stddef.h>
#endif
#ifdef _MSC_VER
#include <BaseTsd.h>
#endif
#include <stdarg.h>
#ifdef EVENT__HAVE_NETDB_H
#include <netdb.h>
#endif

#ifdef _WIN32
#include <winsock2.h>
#ifdef EVENT__HAVE_GETADDRINFO
/* for EAI_* definitions. */
#include <ws2tcpip.h>
#endif
#else
#ifdef EVENT__HAVE_ERRNO_H
#include <errno.h>
#endif
#include <sys/socket.h>
#endif

#include <time.h>

/* Some openbsd autoconf versions get the name of this macro wrong. 某些openbsd autoconf版本错误地使用了此宏的名称。 */
#if defined(EVENT__SIZEOF_VOID__) && !defined(EVENT__SIZEOF_VOID_P)
#define EVENT__SIZEOF_VOID_P EVENT__SIZEOF_VOID__
#endif

/**
 * @name Standard integer types.    标准整数类型。
 *
 * Integer type definitions for types that are supposed to be defined in the
 * C99-specified stdint.h.  Shamefully, some platforms do not include
 * stdint.h, so we need to replace it.  (If you are on a platform like this,
 * your C headers are now over 10 years out of date.  You should bug them to
 * do something about this.)
 * 应该在C99指定的stdint.h中定义的类型的整数类型定义。令人遗憾的是，有些平台不包括stdint.h，所以我们需要替换它。
 * （如果你在这样的平台上，你的C头文件现在已经过时10多年了。你应该让他们做点什么。）
 *
 * We define:
 *
 * <dl>
 *   <dt>ev_uint64_t, ev_uint32_t, ev_uint16_t, ev_uint8_t</dt>
 *      <dd>unsigned integer types of exactly 64, 32, 16, and 8 bits
 *          respectively.</dd>
 *    <dt>ev_int64_t, ev_int32_t, ev_int16_t, ev_int8_t</dt>
 *      <dd>signed integer types of exactly 64, 32, 16, and 8 bits
 *          respectively.</dd>
 *    <dt>ev_uintptr_t, ev_intptr_t</dt>
 *      <dd>unsigned/signed integers large enough
 *      to hold a pointer without loss of bits.</dd>
 *    <dt>ev_ssize_t</dt>
 *      <dd>A signed type of the same size as size_t</dd>
 *    <dt>ev_off_t</dt>
 *      <dd>A signed type typically used to represent offsets within a
 *      (potentially large) file</dd>
 *
 * @{
 */
#ifdef EVENT__HAVE_UINT64_T
#define ev_uint64_t uint64_t
#define ev_int64_t int64_t
#elif defined(_WIN32)
#define ev_uint64_t unsigned __int64
#define ev_int64_t signed __int64
#elif EVENT__SIZEOF_LONG_LONG == 8
#define ev_uint64_t unsigned long long
#define ev_int64_t long long
#elif EVENT__SIZEOF_LONG == 8
#define ev_uint64_t unsigned long
#define ev_int64_t long
#elif defined(EVENT_IN_DOXYGEN_)
#define ev_uint64_t ...
#define ev_int64_t ...
#else
#error "No way to define ev_uint64_t"
#endif

#ifdef EVENT__HAVE_UINT32_T
#define ev_uint32_t uint32_t
#define ev_int32_t int32_t
#elif defined(_WIN32)
#define ev_uint32_t unsigned int
#define ev_int32_t signed int
#elif EVENT__SIZEOF_LONG == 4
#define ev_uint32_t unsigned long
#define ev_int32_t signed long
#elif EVENT__SIZEOF_INT == 4
#define ev_uint32_t unsigned int
#define ev_int32_t signed int
#elif defined(EVENT_IN_DOXYGEN_)
#define ev_uint32_t ...
#define ev_int32_t ...
#else
#error "No way to define ev_uint32_t"
#endif

#ifdef EVENT__HAVE_UINT16_T
#define ev_uint16_t uint16_t
#define ev_int16_t  int16_t
#elif defined(_WIN32)
#define ev_uint16_t unsigned short
#define ev_int16_t  signed short
#elif EVENT__SIZEOF_INT == 2
#define ev_uint16_t unsigned int
#define ev_int16_t  signed int
#elif EVENT__SIZEOF_SHORT == 2
#define ev_uint16_t unsigned short
#define ev_int16_t  signed short
#elif defined(EVENT_IN_DOXYGEN_)
#define ev_uint16_t ...
#define ev_int16_t ...
#else
#error "No way to define ev_uint16_t"
#endif

#ifdef EVENT__HAVE_UINT8_T
#define ev_uint8_t uint8_t
#define ev_int8_t int8_t
#elif defined(EVENT_IN_DOXYGEN_)
#define ev_uint8_t ...
#define ev_int8_t ...
#else
#define ev_uint8_t unsigned char
#define ev_int8_t signed char
#endif

#ifdef EVENT__HAVE_UINTPTR_T
#define ev_uintptr_t uintptr_t
#define ev_intptr_t intptr_t
#elif EVENT__SIZEOF_VOID_P <= 4
#define ev_uintptr_t ev_uint32_t
#define ev_intptr_t ev_int32_t
#elif EVENT__SIZEOF_VOID_P <= 8
#define ev_uintptr_t ev_uint64_t
#define ev_intptr_t ev_int64_t
#elif defined(EVENT_IN_DOXYGEN_)
#define ev_uintptr_t ...
#define ev_intptr_t ...
#else
#error "No way to define ev_uintptr_t"
#endif

#ifdef EVENT__ssize_t
#define ev_ssize_t EVENT__ssize_t
#else
#define ev_ssize_t ssize_t
#endif

/* Note that we define ev_off_t based on the compile-time size of off_t that
 * we used to build Libevent, and not based on the current size of off_t.
 * (For example, we don't define ev_off_t to off_t.).  We do this because
 * some systems let you build your software with different off_t sizes
 * at runtime, and so putting in any dependency on off_t would risk API
 * mismatch.
 * 请注意，我们根据用于构建Libevent的off_t的编译时大小定义ev_off_t，而不是根据off_t当前的大小（例如，我们没有将ev_off_time定义为off_t）。
 * 我们这样做是因为有些系统允许您在运行时构建具有不同off_t大小的软件，因此对off_t的任何依赖都有API不匹配的风险。
 */
#ifdef _WIN32
#define ev_off_t ev_int64_t
#elif EVENT__SIZEOF_OFF_T == 8
#define ev_off_t ev_int64_t
#elif EVENT__SIZEOF_OFF_T == 4
#define ev_off_t ev_int32_t
#elif defined(EVENT_IN_DOXYGEN_)
#define ev_off_t ...
#else
#define ev_off_t off_t
#endif
/**@}*/

/* Limits for integer types.    整数类型的限制。

   We're making two assumptions here:   我们在这里做两个假设：
     - The compiler does constant folding properly. 编译器正确执行常量折叠。
     - The platform does signed arithmetic in two's complement. 该平台在二进制补码中执行有符号算术。
*/

/**
   @name Limits for integer types   整数类型的限制。

   These macros hold the largest or smallest values possible for the
   ev_[u]int*_t types.
   这些宏包含ev_[u]int*_t类型可能的最大或最小值。

   @{
*/
#ifndef EVENT__HAVE_STDINT_H
#define EV_UINT64_MAX ((((ev_uint64_t)0xffffffffUL) << 32) | 0xffffffffUL)
#define EV_INT64_MAX  ((((ev_int64_t) 0x7fffffffL) << 32) | 0xffffffffL)
#define EV_INT64_MIN  ((-EV_INT64_MAX) - 1)
#define EV_UINT32_MAX ((ev_uint32_t)0xffffffffUL)
#define EV_INT32_MAX  ((ev_int32_t) 0x7fffffffL)
#define EV_INT32_MIN  ((-EV_INT32_MAX) - 1)
#define EV_UINT16_MAX ((ev_uint16_t)0xffffUL)
#define EV_INT16_MAX  ((ev_int16_t) 0x7fffL)
#define EV_INT16_MIN  ((-EV_INT16_MAX) - 1)
#define EV_UINT8_MAX  255
#define EV_INT8_MAX   127
#define EV_INT8_MIN   ((-EV_INT8_MAX) - 1)
#else
#define EV_UINT64_MAX UINT64_MAX
#define EV_INT64_MAX  INT64_MAX
#define EV_INT64_MIN  INT64_MIN
#define EV_UINT32_MAX UINT32_MAX
#define EV_INT32_MAX  INT32_MAX
#define EV_INT32_MIN  INT32_MIN
#define EV_UINT16_MAX UINT16_MAX
#define EV_INT16_MIN  INT16_MIN
#define EV_INT16_MAX  INT16_MAX
#define EV_UINT8_MAX  UINT8_MAX
#define EV_INT8_MAX   INT8_MAX
#define EV_INT8_MIN   INT8_MIN
/** @} */
#endif


/**
   @name Limits for SIZE_T and SSIZE_T  SIZE_T和SSIZE_T的限制

   @{
*/
#if EVENT__SIZEOF_SIZE_T == 8
#define EV_SIZE_MAX EV_UINT64_MAX
#define EV_SSIZE_MAX EV_INT64_MAX
#elif EVENT__SIZEOF_SIZE_T == 4
#define EV_SIZE_MAX EV_UINT32_MAX
#define EV_SSIZE_MAX EV_INT32_MAX
#elif defined(EVENT_IN_DOXYGEN_)
#define EV_SIZE_MAX ...
#define EV_SSIZE_MAX ...
#else
#error "No way to define SIZE_MAX"
#endif

#define EV_SSIZE_MIN ((-EV_SSIZE_MAX) - 1)
/**@}*/

#ifdef _WIN32
#define ev_socklen_t int
#elif defined(EVENT__socklen_t)
#define ev_socklen_t EVENT__socklen_t
#else
#define ev_socklen_t socklen_t
#endif

#ifdef EVENT__HAVE_STRUCT_SOCKADDR_STORAGE___SS_FAMILY
#if !defined(EVENT__HAVE_STRUCT_SOCKADDR_STORAGE_SS_FAMILY) \
 && !defined(ss_family)
#define ss_family __ss_family
#endif
#endif

/**
 * A type wide enough to hold the output of "socket()" or "accept()".  On
 * Windows, this is an intptr_t; elsewhere, it is an int.
 * 一个足够宽的类型，可以容纳“socket（）”或“accept（）”的输出。在Windows上，这是一个intptr_t；在其他地方，它是一个整数。 */
#ifdef _WIN32
#define evutil_socket_t intptr_t
#else
#define evutil_socket_t int
#endif

/**
 * Structure to hold information about a monotonic timer
 * 用于保存单调定时器信息的结构
 *
 * Use this with evutil_configure_monotonic_time() and
 * evutil_gettime_monotonic().
 * 将其与evutil_configure_monotonic_time（）和evutil_gettime_monotonic（）一起使用。
 *
 * This is an opaque structure; you can allocate one using
 * evutil_monotonic_timer_new().
 * 这是一个不透明的结构；您可以使用evutil_monotonic_timer_new（）分配一个。
 *
 * @see evutil_monotonic_timer_new(), evutil_monotonic_timer_free(),
 * evutil_configure_monotonic_time(), evutil_gettime_monotonic()
 */
struct evutil_monotonic_timer
#ifdef EVENT_IN_DOXYGEN_
{/*Empty body so that doxygen will generate documentation here.*/}
#endif
;

#define EV_MONOT_PRECISE  1
#define EV_MONOT_FALLBACK 2

/** Format a date string using RFC 1123 format (used in HTTP).  使用RFC 1123格式（用于HTTP）格式化日期字符串。
 * If `tm` is NULL, current system's time will be used. 如果“tm”为NULL，则将使用当前系统的时间。
 * The number of characters written will be returned.   将返回写入的字符数。
 * One should check if the return value is smaller than `datelen` to check if
 * the result is truncated or not.  应该检查返回值是否小于“datelen”，以检查结果是否被截断。
 */
EVENT2_EXPORT_SYMBOL int
evutil_date_rfc1123(char *date, const size_t datelen, const struct tm *tm);

/** Allocate a new struct evutil_monotonic_timer for use with the
 * evutil_configure_monotonic_time() and evutil_gettime_monotonic()
 * functions.  You must configure the timer with
 * evutil_configure_monotonic_time() before using it.
 * 为evutil_configure_monotonic_time（）和evutil_gettime_monotonic（）函数分配一个新的结构体evutil_monotonic_timer。
 * 在使用计时器之前，必须使用evutil_configure_monotonic_time（）配置计时器。
 */
EVENT2_EXPORT_SYMBOL
struct evutil_monotonic_timer * evutil_monotonic_timer_new(void);

/** Free a struct evutil_monotonic_timer that was allocated using
 * evutil_monotonic_timer_new().
 * 释放使用evutil_monotonic_timer_new（）分配的结构体evutil_munotonic_timer。
 */
EVENT2_EXPORT_SYMBOL
void evutil_monotonic_timer_free(struct evutil_monotonic_timer *timer);

/** Set up a struct evutil_monotonic_timer; flags can include
 * EV_MONOT_PRECISE and EV_MONOT_FALLBACK.
 * 设置一个结构体evutil_monotonic_timer；标志可以包括EV_MONOT_PRECISE和EV_MONOT_FALLBACK。
 */
EVENT2_EXPORT_SYMBOL
int evutil_configure_monotonic_time(struct evutil_monotonic_timer *timer,
                                    int flags);

/** Query the current monotonic time from a struct evutil_monotonic_timer
 * previously configured with evutil_configure_monotonic_time().  Monotonic
 * time is guaranteed never to run in reverse, but is not necessarily epoch-
 * based, or relative to any other definite point.  Use it to make reliable
 * measurements of elapsed time between events even when the system time
 * may be changed.
 * 从以前使用evutil_configure_monotonic_time（）配置的结构体evutil_monotonic_timer查询当前单调时间。
 * 单调时间保证永远不会反向运行，但不一定是基于纪元的，也不一定是相对于任何其他确定点的。即使系统时间可能发生变化，也可以使用它对事件之间的经过时间进行可靠的测量。
 *
 * It is not safe to use this funtion on the same timer from multiple
 * threads. 在多个线程的同一计时器上使用此功能是不安全的。
 */
EVENT2_EXPORT_SYMBOL
int evutil_gettime_monotonic(struct evutil_monotonic_timer *timer,
                             struct timeval *tp);

/** Create two new sockets that are connected to each other.
 * 创建两个相互连接的新套接字。

    On Unix, this simply calls socketpair().  On Windows, it uses the
    loopback network interface on 127.0.0.1, and only
    AF_INET,SOCK_STREAM are supported.
    在Unix上，这只是调用socketpair（）。在Windows上，它使用127.0.0.1上的环回网络接口，只支持AF_INET、SOCK_STREAM。

    (This may fail on some Windows hosts where firewall software has cleverly
    decided to keep 127.0.0.1 from talking to itself.)
    （在某些Windows主机上，这可能会失败，因为防火墙软件巧妙地决定阻止127.0.0.1与自己通信。）

    Parameters and return values are as for socketpair()
    参数和返回值与socketpair（）相同
*/
EVENT2_EXPORT_SYMBOL
int evutil_socketpair(int d, int type, int protocol, evutil_socket_t sv[2]);
/** Do platform-specific operations as needed to make a socket nonblocking.
 *  根据需要执行特定于平台的操作，使套接字非阻塞。

    @param sock The socket to make nonblocking  使套接字不阻塞
    @return 0 on success, -1 on failure
 */
EVENT2_EXPORT_SYMBOL
int evutil_make_socket_nonblocking(evutil_socket_t sock);

/** Do platform-specific operations to make a listener socket reusable.
 *  执行特定于平台的操作，使侦听器套接字可重用。

    Specifically, we want to make sure that another program will be able
    to bind this address right after we've closed the listener.
    具体来说，我们希望确保在关闭侦听器后，另一个程序能够立即绑定此地址。

    This differs from Windows's interpretation of "reusable", which
    allows multiple listeners to bind the same address at the same time.
    这与Windows对“可重用”的解释不同，后者允许多个侦听器同时绑定同一地址。

    @param sock The socket to make reusable 可复用的socket
    @return 0 on success, -1 on failure
 */
EVENT2_EXPORT_SYMBOL
int evutil_make_listen_socket_reuseable(evutil_socket_t sock);

/** Do platform-specific operations to make a listener port reusable.
 *  执行特定于平台的操作，使侦听器端口可重用。

    Specifically, we want to make sure that multiple programs which also
    set the same socket option will be able to bind, listen at the same time.
    具体来说，我们希望确保也设置了相同套接字选项的多个程序能够同时绑定、监听。

    This is a feature available only to Linux 3.9+
    此功能仅适用于Linux 3.9+

    @param sock The socket to make reusable 可复用的socket
    @return 0 on success, -1 on failure
 */
EVENT2_EXPORT_SYMBOL
int evutil_make_listen_socket_reuseable_port(evutil_socket_t sock);

/** Set ipv6 only bind socket option to make listener work only in ipv6 sockets.
 *  设置仅绑定ipv6套接字选项，使侦听器仅在ipv6套接字中工作。

    According to RFC3493 and most Linux distributions, default value for the
    sockets is to work in IPv4-mapped mode. In IPv4-mapped mode, it is not possible
    to bind same port from different IPv4 and IPv6 handlers.
    根据RFC3493和大多数Linux发行版，套接字的默认值是在IPv4映射模式下工作。在IPv4映射模式下，无法绑定来自不同IPv4和IPv6处理程序的同一端口。

    @param sock The socket to make in ipv6only working mode 仅在ipv6工作模式下创建的套接字
    @return 0 on success, -1 on failure
 */
EVENT2_EXPORT_SYMBOL
int evutil_make_listen_socket_ipv6only(evutil_socket_t sock);

/** Do platform-specific operations as needed to close a socket upon a
    successful execution of one of the exec*() functions.
    根据需要执行特定于平台的操作，以便在成功执行exec*（）函数之一时关闭套接字。

    @param sock The socket to be closed
    @return 0 on success, -1 on failure
 */
EVENT2_EXPORT_SYMBOL
int evutil_make_socket_closeonexec(evutil_socket_t sock);

/** Do the platform-specific call needed to close a socket returned from
    socket() or accept().
    执行关闭从socket（）或accept（）返回的套接字所需的特定于平台的调用。

    @param sock The socket to be closed
    @return 0 on success (whether the operation is supported or not 是否支持该操作),
            -1 on failure
 */
EVENT2_EXPORT_SYMBOL
int evutil_closesocket(evutil_socket_t sock);
#define EVUTIL_CLOSESOCKET(s) evutil_closesocket(s)

/** Do platform-specific operations, if possible, to make a tcp listener
 *  socket defer accept()s until there is data to read.
 *  如果可能的话，执行特定于平台的操作，使tcp侦听器套接字推迟accept（），直到有数据要读取。
 *  
 *  Not all platforms support this.  You don't want to do this for every
 *  listener socket: only the ones that implement a protocol where the
 *  client transmits before the server needs to respond.
 *  并非所有平台都支持此功能。你不想对每个侦听器套接字都这样做：只有那些实现了客户端在服务器需要响应之前传输的协议的套接字。
 *
 *  @param sock The listening socket to to make deferred    监听插座被推迟
 *  @return 0 on success (whether the operation is supported or not),
 *       -1 on failure
*/ 
EVENT2_EXPORT_SYMBOL
int evutil_make_tcp_listen_socket_deferred(evutil_socket_t sock);

#ifdef _WIN32
/** Return the most recent socket error.  Not idempotent on all platforms.  返回最近的套接字错误。并非在所有平台上都是幂等的。 */
#define EVUTIL_SOCKET_ERROR() WSAGetLastError()
/** Replace the most recent socket error with errcode   将最近的套接字错误替换为errcode */
#define EVUTIL_SET_SOCKET_ERROR(errcode)		\
	do { WSASetLastError(errcode); } while (0)
/** Return the most recent socket error to occur on sock.   返回socket上发生的最新套接字错误。 */
EVENT2_EXPORT_SYMBOL
int evutil_socket_geterror(evutil_socket_t sock);
/** Convert a socket error to a string. 将套接字错误转换为字符串。 */
EVENT2_EXPORT_SYMBOL
const char *evutil_socket_error_to_string(int errcode);
#define EVUTIL_INVALID_SOCKET INVALID_SOCKET
#elif defined(EVENT_IN_DOXYGEN_)
/**
   @name Socket error functions  套接字错误函数

   These functions are needed for making programs compatible between
   Windows and Unix-like platforms.
   这些功能是使程序在Windows和类Unix平台之间兼容所必需的。

   You see, Winsock handles socket errors differently from the rest of
   the world.  Elsewhere, a socket error is like any other error and is
   stored in errno.  But winsock functions require you to retrieve the
   error with a special function, and don't let you use strerror for
   the error codes.  And handling EWOULDBLOCK is ... different.
   你看，Winsock处理套接字错误的方式与世界其他地方不同。在其他地方，套接字错误与任何其他错误一样，存储在errno中。
   但是winsock函数要求您使用特殊函数检索错误，并且不允许您对错误代码使用strerror。处理EWOULDBLOCK是。。。不同。

   @{
*/
/** Return the most recent socket error.  Not idempotent on all platforms. 返回最近的套接字错误。并非在所有平台上都是幂等的。 */
#define EVUTIL_SOCKET_ERROR() ...
/** Replace the most recent socket error with errcode 将最近的套接字错误替换为errcode */
#define EVUTIL_SET_SOCKET_ERROR(errcode) ...
/** Return the most recent socket error to occur on sock.   返回socket上发生的最新套接字错误。 */
#define evutil_socket_geterror(sock) ...
/** Convert a socket error to a string.   将套接字错误转换为字符串。 */
#define evutil_socket_error_to_string(errcode) ...
#define EVUTIL_INVALID_SOCKET -1
/**@}*/
#else /** !EVENT_IN_DOXYGEN_ && !_WIN32 */
#define EVUTIL_SOCKET_ERROR() (errno)
#define EVUTIL_SET_SOCKET_ERROR(errcode)		\
		do { errno = (errcode); } while (0)
#define evutil_socket_geterror(sock) (errno)
#define evutil_socket_error_to_string(errcode) (strerror(errcode))
#define EVUTIL_INVALID_SOCKET -1
#endif /** !_WIN32 */


/**
 * @name Manipulation macros for struct timeval.   结构timeval的操作宏。
 *
 * We define replacements
 * for timeradd, timersub, timerclear, timercmp, and timerisset.
 * 我们定义了timeradd、timersub、timerclear、timercmp和timerisset的替换。
 *
 * @{
 */
#ifdef EVENT__HAVE_TIMERADD
#define evutil_timeradd(tvp, uvp, vvp) timeradd((tvp), (uvp), (vvp))
#define evutil_timersub(tvp, uvp, vvp) timersub((tvp), (uvp), (vvp))
#else
#define evutil_timeradd(tvp, uvp, vvp)					\
	do {								\
		(vvp)->tv_sec = (tvp)->tv_sec + (uvp)->tv_sec;		\
		(vvp)->tv_usec = (tvp)->tv_usec + (uvp)->tv_usec;       \
		if ((vvp)->tv_usec >= 1000000) {			\
			(vvp)->tv_sec++;				\
			(vvp)->tv_usec -= 1000000;			\
		}							\
	} while (0)
#define	evutil_timersub(tvp, uvp, vvp)					\
	do {								\
		(vvp)->tv_sec = (tvp)->tv_sec - (uvp)->tv_sec;		\
		(vvp)->tv_usec = (tvp)->tv_usec - (uvp)->tv_usec;	\
		if ((vvp)->tv_usec < 0) {				\
			(vvp)->tv_sec--;				\
			(vvp)->tv_usec += 1000000;			\
		}							\
	} while (0)
#endif /* !EVENT__HAVE_TIMERADD */

#ifdef EVENT__HAVE_TIMERCLEAR
#define evutil_timerclear(tvp) timerclear(tvp)
#else
#define	evutil_timerclear(tvp)	(tvp)->tv_sec = (tvp)->tv_usec = 0
#endif
/**@}*/

/** Return true iff the tvp is related to uvp according to the relational
 * operator cmp.  Recognized values for cmp are ==, <=, <, >=, and >.
 * 根据关系运算符cmp，如果tvp与uvp相关，则返回true。cmp的公认值为==、<=、<、>=和>。 */
#define	evutil_timercmp(tvp, uvp, cmp)					\
	(((tvp)->tv_sec == (uvp)->tv_sec) ?				\
	 ((tvp)->tv_usec cmp (uvp)->tv_usec) :				\
	 ((tvp)->tv_sec cmp (uvp)->tv_sec))

#ifdef EVENT__HAVE_TIMERISSET
#define evutil_timerisset(tvp) timerisset(tvp)
#else
#define	evutil_timerisset(tvp)	((tvp)->tv_sec || (tvp)->tv_usec)
#endif

/** Replacement for offsetof on platforms that don't define it.   在没有定义它的平台上替换offsetof。 */
#ifdef offsetof
#define evutil_offsetof(type, field) offsetof(type, field)
#else
#define evutil_offsetof(type, field) ((off_t)(&((type *)0)->field))
#endif

/* big-int related functions  big int相关函数 */
/** Parse a 64-bit value from a string.  Arguments are as for strtol.   从字符串中解析64位值。参数与strtol相同。 */
EVENT2_EXPORT_SYMBOL
ev_int64_t evutil_strtoll(const char *s, char **endptr, int base);

/** Replacement for gettimeofday on platforms that lack it. 在缺乏gettimeofday的平台上取代它。 */
#ifdef EVENT__HAVE_GETTIMEOFDAY
#define evutil_gettimeofday(tv, tz) gettimeofday((tv), (tz))
#else
struct timezone;
EVENT2_EXPORT_SYMBOL
int evutil_gettimeofday(struct timeval *tv, struct timezone *tz);
#endif

/** Replacement for snprintf to get consistent behavior on platforms for
    which the return value of snprintf does not conform to C99.
    替换snprintf，以便在snprintf的返回值不符合C99的平台上获得一致的行为。
 */
EVENT2_EXPORT_SYMBOL
int evutil_snprintf(char *buf, size_t buflen, const char *format, ...)
#ifdef __GNUC__
	__attribute__((format(printf, 3, 4)))
#endif
;
/** Replacement for vsnprintf to get consistent behavior on platforms for
    which the return value of snprintf does not conform to C99.
    替换vsnprintf，以便在snprintf的返回值不符合C99的平台上获得一致的行为。
 */
EVENT2_EXPORT_SYMBOL
int evutil_vsnprintf(char *buf, size_t buflen, const char *format, va_list ap)
#ifdef __GNUC__
	__attribute__((format(printf, 3, 0)))
#endif
;

/** Replacement for inet_ntop for platforms which lack it.  对于缺少inet_ntop的平台，可以替换它。 */
EVENT2_EXPORT_SYMBOL
const char *evutil_inet_ntop(int af, const void *src, char *dst, size_t len);
/** Variation of inet_pton that also parses IPv6 scopes. Public for
    unit tests. No reason to call this directly.
    inet_pton的变体，也解析IPv6作用域。公开进行单元测试。没有理由直接这么说。
 */
EVENT2_EXPORT_SYMBOL
int evutil_inet_pton_scope(int af, const char *src, void *dst,
	unsigned *indexp);
/** Replacement for inet_pton for platforms which lack it.  对于缺少inet_pton的平台，可以替换它。 */
EVENT2_EXPORT_SYMBOL
int evutil_inet_pton(int af, const char *src, void *dst);
struct sockaddr;

/** Parse an IPv4 or IPv6 address, with optional port, from a string.
 *  从字符串中解析具有可选端口的IPv4或IPv6地址。

    Recognized formats are:
    - [IPv6Address]:port
    - [IPv6Address]
    - IPv6Address
    - IPv4Address:port
    - IPv4Address

    If no port is specified, the port in the output is set to 0.
    如果未指定端口，则输出中的端口设置为0。

    @param str The string to parse. 要解析的字符串。
    @param out A struct sockaddr to hold the result.  This should probably be
       a struct sockaddr_storage.   一个用于保存结果的结构体sockaddr。这可能是一个结构体sockaddr_storage。
    @param outlen A pointer to the number of bytes that that 'out' can safely
       hold.  Set to the number of bytes used in 'out' on success.
       指向“out”可以安全保存的字节数的指针。设置为成功时“out”中使用的字节数。
    @return -1 if the address is not well-formed, if the port is out of range,
       or if out is not large enough to hold the result.  Otherwise returns
       0 on success.
       如果地址格式不正确、端口超出范围或out不足以容纳结果，则返回-1。否则，成功时返回0。
*/
EVENT2_EXPORT_SYMBOL
int evutil_parse_sockaddr_port(const char *str, struct sockaddr *out, int *outlen);

/** Compare two sockaddrs; return 0 if they are equal, or less than 0 if sa1
 * preceeds sa2, or greater than 0 if sa1 follows sa2.  If include_port is
 * true, consider the port as well as the address.  Only implemented for
 * AF_INET and AF_INET6 addresses. The ordering is not guaranteed to remain
 * the same between Libevent versions.
 * 比较两个sockaddrs；如果它们相等，则返回0；如果sa1在sa2之前，则返回小于0的值；如果sa2在sa1之后，则返回大于0的值。
 * 如果include_port为true，请考虑端口和地址。仅对AF_INET和AF_INET6地址实现。不能保证Libevent版本之间的顺序保持不变。 */
EVENT2_EXPORT_SYMBOL
int evutil_sockaddr_cmp(const struct sockaddr *sa1, const struct sockaddr *sa2,
    int include_port);

/** As strcasecmp, but always compares the characters in locale-independent
    ASCII.  That's useful if you're handling data in ASCII-based protocols.
    与strcasecmp一样，但始终比较独立于区域设置的ASCII中的字符。如果您在基于ASCII的协议中处理数据，这很有用。
 */
EVENT2_EXPORT_SYMBOL
int evutil_ascii_strcasecmp(const char *str1, const char *str2);
/** As strncasecmp, but always compares the characters in locale-independent
    ASCII.  That's useful if you're handling data in ASCII-based protocols.
    与strncasecmp一样，但始终比较独立于区域设置的ASCII中的字符。如果您在基于ASCII的协议中处理数据，这很有用。
 */
EVENT2_EXPORT_SYMBOL
int evutil_ascii_strncasecmp(const char *str1, const char *str2, size_t n);

/* Here we define evutil_addrinfo to the native addrinfo type, or redefine it
 * if this system has no getaddrinfo().
   在这里，我们将evutil_addrinfo定义为本机addrinfo类型，或者如果此系统没有getaddrinfo（），则重新定义它。 */
#ifdef EVENT__HAVE_STRUCT_ADDRINFO
#define evutil_addrinfo addrinfo
#else
/** A definition of struct addrinfo for systems that lack it.  缺少结构addrinfo的系统的结构addrinfo定义。

    (This is just an alias for struct addrinfo if the system defines
    struct addrinfo.)   （如果系统定义了结构addrinfo，这只是结构addrinfo的别名。）
*/
struct evutil_addrinfo {
	int     ai_flags;     /* AI_PASSIVE, AI_CANONNAME, AI_NUMERICHOST */
	int     ai_family;    /* PF_xxx */
	int     ai_socktype;  /* SOCK_xxx */
	int     ai_protocol;  /* 0 or IPPROTO_xxx for IPv4 and IPv6 */
	size_t  ai_addrlen;   /* length of ai_addr */
	char   *ai_canonname; /* canonical name for nodename */
	struct sockaddr  *ai_addr; /* binary address */
	struct evutil_addrinfo  *ai_next; /* next structure in linked list */
};
#endif
/** @name evutil_getaddrinfo() error codes   evutil_getaddrinfo（）错误代码

    These values are possible error codes for evutil_getaddrinfo() and
    related functions.  这些值可能是evutil_getaddinfo（）和相关函数的错误代码。

    @{
*/
#if defined(EAI_ADDRFAMILY) && defined(EVENT__HAVE_GETADDRINFO)
#define EVUTIL_EAI_ADDRFAMILY EAI_ADDRFAMILY
#else
#define EVUTIL_EAI_ADDRFAMILY -901
#endif
#if defined(EAI_AGAIN) && defined(EVENT__HAVE_GETADDRINFO)
#define EVUTIL_EAI_AGAIN EAI_AGAIN
#else
#define EVUTIL_EAI_AGAIN -902
#endif
#if defined(EAI_BADFLAGS) && defined(EVENT__HAVE_GETADDRINFO)
#define EVUTIL_EAI_BADFLAGS EAI_BADFLAGS
#else
#define EVUTIL_EAI_BADFLAGS -903
#endif
#if defined(EAI_FAIL) && defined(EVENT__HAVE_GETADDRINFO)
#define EVUTIL_EAI_FAIL EAI_FAIL
#else
#define EVUTIL_EAI_FAIL -904
#endif
#if defined(EAI_FAMILY) && defined(EVENT__HAVE_GETADDRINFO)
#define EVUTIL_EAI_FAMILY EAI_FAMILY
#else
#define EVUTIL_EAI_FAMILY -905
#endif
#if defined(EAI_MEMORY) && defined(EVENT__HAVE_GETADDRINFO)
#define EVUTIL_EAI_MEMORY EAI_MEMORY
#else
#define EVUTIL_EAI_MEMORY -906
#endif
/* This test is a bit complicated, since some MS SDKs decide to
 * remove NODATA or redefine it to be the same as NONAME, in a
 * fun interpretation of RFC 2553 and RFC 3493. 
 * 这个测试有点复杂，因为一些MS SDK决定删除NODATA或将其重新定义为与NONAME相同，这是对RFC 2553和RFC 3493的有趣解释。 */
#if defined(EAI_NODATA) && defined(EVENT__HAVE_GETADDRINFO) && (!defined(EAI_NONAME) || EAI_NODATA != EAI_NONAME)
#define EVUTIL_EAI_NODATA EAI_NODATA
#else
#define EVUTIL_EAI_NODATA -907
#endif
#if defined(EAI_NONAME) && defined(EVENT__HAVE_GETADDRINFO)
#define EVUTIL_EAI_NONAME EAI_NONAME
#else
#define EVUTIL_EAI_NONAME -908
#endif
#if defined(EAI_SERVICE) && defined(EVENT__HAVE_GETADDRINFO)
#define EVUTIL_EAI_SERVICE EAI_SERVICE
#else
#define EVUTIL_EAI_SERVICE -909
#endif
#if defined(EAI_SOCKTYPE) && defined(EVENT__HAVE_GETADDRINFO)
#define EVUTIL_EAI_SOCKTYPE EAI_SOCKTYPE
#else
#define EVUTIL_EAI_SOCKTYPE -910
#endif
#if defined(EAI_SYSTEM) && defined(EVENT__HAVE_GETADDRINFO)
#define EVUTIL_EAI_SYSTEM EAI_SYSTEM
#else
#define EVUTIL_EAI_SYSTEM -911
#endif

#define EVUTIL_EAI_CANCEL -90001

#if defined(AI_PASSIVE) && defined(EVENT__HAVE_GETADDRINFO)
#define EVUTIL_AI_PASSIVE AI_PASSIVE
#else
#define EVUTIL_AI_PASSIVE 0x1000
#endif
#if defined(AI_CANONNAME) && defined(EVENT__HAVE_GETADDRINFO)
#define EVUTIL_AI_CANONNAME AI_CANONNAME
#else
#define EVUTIL_AI_CANONNAME 0x2000
#endif
#if defined(AI_NUMERICHOST) && defined(EVENT__HAVE_GETADDRINFO)
#define EVUTIL_AI_NUMERICHOST AI_NUMERICHOST
#else
#define EVUTIL_AI_NUMERICHOST 0x4000
#endif
#if defined(AI_NUMERICSERV) && defined(EVENT__HAVE_GETADDRINFO)
#define EVUTIL_AI_NUMERICSERV AI_NUMERICSERV
#else
#define EVUTIL_AI_NUMERICSERV 0x8000
#endif
#if defined(AI_V4MAPPED) && defined(EVENT__HAVE_GETADDRINFO)
#define EVUTIL_AI_V4MAPPED AI_V4MAPPED
#else
#define EVUTIL_AI_V4MAPPED 0x10000
#endif
#if defined(AI_ALL) && defined(EVENT__HAVE_GETADDRINFO)
#define EVUTIL_AI_ALL AI_ALL
#else
#define EVUTIL_AI_ALL 0x20000
#endif
#if defined(AI_ADDRCONFIG) && defined(EVENT__HAVE_GETADDRINFO)
#define EVUTIL_AI_ADDRCONFIG AI_ADDRCONFIG
#else
#define EVUTIL_AI_ADDRCONFIG 0x40000
#endif
/**@}*/

struct evutil_addrinfo;
/**
 * This function clones getaddrinfo for systems that don't have it.  For full
 * details, see RFC 3493, section 6.1.
 * 此函数为没有getaddrinfo的系统克隆getaddrinfo。有关完整详细信息，请参阅RFC 3493第6.1节。
 *
 * Limitations:   局限性
 * - When the system has no getaddrinfo, we fall back to gethostbyname_r or
 *   gethostbyname, with their attendant issues.   当系统没有getaddrinfo时，我们会退回到gethostbyname_r或gethostbyname，以及随之而来的问题。
 * - The AI_V4MAPPED and AI_ALL flags are not currently implemented. AI_V4MAPPED和AI_ALL标志目前尚未实现。
 *
 * For a nonblocking variant, see evdns_getaddrinfo.  有关非阻塞变体，请参阅evdns_getaddrinfo。
 */
EVENT2_EXPORT_SYMBOL
int evutil_getaddrinfo(const char *nodename, const char *servname,
    const struct evutil_addrinfo *hints_in, struct evutil_addrinfo **res);

/** Release storage allocated by evutil_getaddrinfo or evdns_getaddrinfo.  释放由evutil_getaddrinfo或evdns_getaddrinfo分配的存储空间。 */
EVENT2_EXPORT_SYMBOL
void evutil_freeaddrinfo(struct evutil_addrinfo *ai);

EVENT2_EXPORT_SYMBOL
const char *evutil_gai_strerror(int err);

/** Generate n bytes of secure pseudorandom data, and store them in buf.
 *  生成n字节的安全伪随机数据，并将其存储在buf中。
 *
 * Current versions of Libevent use an ARC4-based random number generator,
 * seeded using the platform's entropy source (/dev/urandom on Unix-like
 * systems; CryptGenRandom on Windows).  This is not actually as secure as it
 * should be: ARC4 is a pretty lousy cipher, and the current implementation
 * provides only rudimentary prediction- and backtracking-resistance.  Don't
 * use this for serious cryptographic applications.
 * 当前版本的Libevent使用基于ARC4的随机数生成器，该生成器使用平台的熵源（类Unix系统上的/dev/urandom；Windows上的CryptGenRandom）播种。
 * 这实际上并不像它应该的那样安全：ARC4是一个非常糟糕的密码，目前的实现只提供了基本的预测和回溯阻力。不要将其用于严重的加密应用程序。
 */
EVENT2_EXPORT_SYMBOL
void evutil_secure_rng_get_bytes(void *buf, size_t n);

/**
 * Seed the secure random number generator if needed, and return 0 on
 * success or -1 on failure.
 * 如果需要，为安全随机数生成器添加种子，成功时返回0，失败时返回-1。
 *
 * It is okay to call this function more than once; it will still return
 * 0 if the RNG has been successfully seeded and -1 if it can't be
 * seeded. 可以多次调用此函数；如果RNG已成功播种，它仍将返回0，如果无法播种，它将返回-1。
 *
 * Ordinarily you don't need to call this function from your own code;
 * Libevent will seed the RNG itself the first time it needs good random
 * numbers.  You only need to call it if (a) you want to double-check
 * that one of the seeding methods did succeed, or (b) you plan to drop
 * the capability to seed (by chrooting, or dropping capabilities, or
 * whatever), and you want to make sure that seeding happens before your
 * program loses the ability to do it.
 * 通常，您不需要从自己的代码中调用此函数；Libevent将在第一次需要好的随机数时为RNG本身播种。
 * 您只需要在以下情况下调用它：（a）您想仔细检查其中一个种子方法是否成功，或（b）您计划放弃种子功能（通过chrooting、丢弃功能或其他方式），并且您想确保在程序失去种子功能之前进行种子播。
 */
EVENT2_EXPORT_SYMBOL
int evutil_secure_rng_init(void);

/**
 * Set a filename to use in place of /dev/urandom for seeding the secure
 * PRNG. Return 0 on success, -1 on failure.
 * 设置一个文件名来代替/dev/urandom为安全PRNG设置种子。成功时返回0，失败时返回-1。
 *
 * Call this function BEFORE calling any other initialization or RNG
 * functions.
 * 在调用任何其他初始化或RNG函数之前，请先调用此函数。
 *
 * (This string will _NOT_ be copied internally. Do not free it while any
 * user of the secure RNG might be running. Don't pass anything other than a
 * real /dev/...random device file here, or you might lose security.)
 * （此字符串_NOT_将在内部复制。当安全RNG的任何用户可能正在运行时，不要释放它。不要在此处传递除真实/dev/…随机设备文件以外的任何文件，否则您可能会失去安全性。）
 *
 * This API is unstable, and might change in a future libevent version. 此API不稳定，可能会在将来的libevent版本中更改。
 */
EVENT2_EXPORT_SYMBOL
int evutil_secure_rng_set_urandom_device_file(char *fname);

#if !defined(EVENT__HAVE_ARC4RANDOM) || defined(EVENT__HAVE_ARC4RANDOM_ADDRANDOM)
/** Seed the random number generator with extra random bytes.
 *  为随机数生成器添加额外的随机字节。

    You should almost never need to call this function; it should be
    sufficient to invoke evutil_secure_rng_init(), or let Libevent take
    care of calling evutil_secure_rng_init() on its own.
    您几乎不需要调用此函数；调用evutil_secure_ring_init（）就足够了，或者让Libevent自己负责调用evutil.secure_ring.init（）。

    If you call this function as a _replacement_ for the regular
    entropy sources, then you need to be sure that your input
    contains a fairly large amount of strong entropy.  Doing so is
    notoriously hard: most people who try get it wrong.  Watch out!
    如果你将此函数称为常规熵源的_replacement_，那么你需要确保你的输入包含相当大的强熵。众所周知，这样做很难：大多数试图犯错的人。小心！

    @param dat a buffer full of a strong source of random numbers 充满强随机数源的缓冲区
    @param datlen the number of bytes to read from datlen   从datlen读取的字节数
 */
EVENT2_EXPORT_SYMBOL
void evutil_secure_rng_add_bytes(const char *dat, size_t datlen);
#endif

#ifdef __cplusplus
}
#endif

#endif /* EVENT1_EVUTIL_H_INCLUDED_ */
