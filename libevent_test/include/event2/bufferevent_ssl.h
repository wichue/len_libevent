/*
 * Copyright (c) 2009-2012 Niels Provos and Nick Mathewson
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
#ifndef EVENT2_BUFFEREVENT_SSL_H_INCLUDED_
#define EVENT2_BUFFEREVENT_SSL_H_INCLUDED_

/** @file event2/bufferevent_ssl.h

    OpenSSL support for bufferevents.
    bufferevent的OpenSSL支持。
 */
#include <event2/visibility.h>
#include <event2/event-config.h>
#include <event2/bufferevent.h>
#include <event2/util.h>

#ifdef __cplusplus
extern "C" {
#endif

/* This is what openssl's SSL objects are underneath. 下面是openssl's SSL对象 */
struct ssl_st;

/**
   The state of an SSL object to be used when creating a new
   SSL bufferevent.
   在创建新的SSL缓冲区时要使用的SSL对象的状态。
 */
enum bufferevent_ssl_state {
	BUFFEREVENT_SSL_OPEN = 0,
	BUFFEREVENT_SSL_CONNECTING = 1,
	BUFFEREVENT_SSL_ACCEPTING = 2
};

#if defined(EVENT__HAVE_OPENSSL) || defined(EVENT_IN_DOXYGEN_)
/**
   Create a new SSL bufferevent to send its data over another bufferevent.
   创建一个新的SSL缓冲器，以通过另一个缓冲器发送其数据。

   @param base An event_base to use to detect reading and writing.  It
      must also be the base for the underlying bufferevent. 用于检测读取和写的事件基。它也必须是底层缓冲剂的基础。
   @param underlying A socket to use for this SSL 一个可用于此SSL的套接字
   @param ssl A SSL* object from openssl. 一个来自openssl的SSL*对象。
   @param state The current state of the SSL connection SSL连接的当前状态
   @param options One or more bufferevent_options 一个或多个bufferevent_options
   @return A new bufferevent on success, or NULL on failure
*/
EVENT2_EXPORT_SYMBOL
struct bufferevent *
bufferevent_openssl_filter_new(struct event_base *base,
    struct bufferevent *underlying,
    struct ssl_st *ssl,
    enum bufferevent_ssl_state state,
    int options);

/**
   Create a new SSL bufferevent to send its data over an SSL * on a socket.
   创建一个新的SSL缓冲器，以便通过套接字上的SSL *发送其数据。

   @param base An event_base to use to detect reading and writing 用于检测读写的事件基
   @param fd A socket to use for this SSL 一个可用于此SSL的套接字
   @param ssl A SSL* object from openssl. 一个来自openssl的SSL*对象。
   @param state The current state of the SSL connection SSL连接的当前状态
   @param options One or more bufferevent_options 一个或多个bufferevent_options
   @return A new bufferevent on success, or NULL on failure.
*/
EVENT2_EXPORT_SYMBOL
struct bufferevent *
bufferevent_openssl_socket_new(struct event_base *base,
    evutil_socket_t fd,
    struct ssl_st *ssl,
    enum bufferevent_ssl_state state,
    int options);

/** Control how to report dirty SSL shutdowns.
 *  控制如何报告被污染的SSL关闭情况。

    If the peer (or the network, or an attacker) closes the TCP
    connection before closing the SSL channel, and the protocol is SSL >= v3,
    this is a "dirty" shutdown.  If allow_dirty_shutdown is 0 (default),
    this is reported as BEV_EVENT_ERROR.
    如果对等端（或网络或攻击者）在关闭SSL通道之前关闭TCP连接，并且协议为SSL>=v3，则为“脏”关闭。如果alew_dirth_关闭为0（默认），则报告为BEV_EVENT_ERROR。

    If instead allow_dirty_shutdown=1, a dirty shutdown is reported as
    BEV_EVENT_EOF.
    如果 allow_dirty_shutdown=1，则将脏关闭报告为BEV_EVENT_EOF。

    (Note that if the protocol is < SSLv3, you will always receive
    BEV_EVENT_EOF, since SSL 2 and earlier cannot distinguish a secure
    connection close from a dirty one.  This is one reason (among many)
    not to use SSL 2.)
    (注意，如果协议<SSLv3，您将始终接收BEV_EVENT_EOF，因为SSL 2和更早的版本无法区分安全连接关闭和脏连接。这是不使用SSL2的原因之一（众多原因之一）。
*/

EVENT2_EXPORT_SYMBOL
int bufferevent_openssl_get_allow_dirty_shutdown(struct bufferevent *bev);
EVENT2_EXPORT_SYMBOL
void bufferevent_openssl_set_allow_dirty_shutdown(struct bufferevent *bev,
    int allow_dirty_shutdown);

/** Return the underlying openssl SSL * object for an SSL bufferevent. */
EVENT2_EXPORT_SYMBOL
struct ssl_st *
bufferevent_openssl_get_ssl(struct bufferevent *bufev);

/** Tells a bufferevent to begin SSL renegotiation. 告诉一个缓冲者开始SSL重新谈判。 */
EVENT2_EXPORT_SYMBOL
int bufferevent_ssl_renegotiate(struct bufferevent *bev);

/** Return the most recent OpenSSL error reported on an SSL bufferevent. 返回在SSL缓冲程序上报告的最新的OpenSSL错误。 */
EVENT2_EXPORT_SYMBOL
unsigned long bufferevent_get_openssl_error(struct bufferevent *bev);

#endif

#ifdef __cplusplus
}
#endif

#endif /* EVENT2_BUFFEREVENT_SSL_H_INCLUDED_ */
