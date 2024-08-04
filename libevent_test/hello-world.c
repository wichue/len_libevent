/*
  This example program provides a trivial server program that listens for TCP
  connections on port 9995.  When they arrive, it writes a short message to
  each client connection, and closes each connection once it is flushed.
  此示例程序提供了一个简单的tcp服务器程序，用于侦听端口9995上的TCP连接。当有tcp客户端接入时，会向客户端发送一条消息，发送完成后关闭链接。

  Where possible, it exits cleanly in response to a SIGINT (ctrl-c).
  在可能的情况下，它会干净地退出以响应SIGINT（ctrl-c）。
*/


#include <string.h>
#include <errno.h>
#include <stdio.h>
#include <signal.h>
#ifndef _WIN32
#include <netinet/in.h>
# ifdef _XOPEN_SOURCE_EXTENDED
#  include <arpa/inet.h>
# endif
#include <sys/socket.h>
#endif

#include <event2/bufferevent.h>
#include <event2/buffer.h>
#include <event2/listener.h>
#include <event2/util.h>
#include <event2/event.h>

static const char MESSAGE[] = "Hello, World!\n";//发送给客户端的消息

static const int PORT = 9995;//监听的端口号

static void listener_cb(struct evconnlistener *, evutil_socket_t,
    struct sockaddr *, int socklen, void *);
static void conn_writecb(struct bufferevent *, void *);
static void conn_eventcb(struct bufferevent *, short, void *);
static void signal_cb(evutil_socket_t, short, void *);

int
main(int argc, char **argv)
{
	struct event_base *base;
	struct evconnlistener *listener;
	struct event *signal_event;

	struct sockaddr_in sin = {0};
#ifdef _WIN32
	WSADATA wsa_data;
	WSAStartup(0x0201, &wsa_data);
#endif

	base = event_base_new();//1.创建 event_base(include/event2/event.h)
	if (!base) {
		fprintf(stderr, "Could not initialize libevent!\n");
		return 1;
	}

	sin.sin_family = AF_INET;//IPv4 网络协议的套接字类型
	sin.sin_port = htons(PORT);//端口号
	//2.分配一个新的 evconnlistener 对象来监听给定地址上的传入TCP连接。(include/event2/listener.h)
	//LEV_OPT_CLOSE_ON_FREE:当连接监听器释放时，会自动关闭底层的socket。
	//LEV_OPT_REUSEABLE:关闭该socket后，其他socket可以马上使用同一个端口。
	listener = evconnlistener_new_bind(base, listener_cb, (void *)base,
	    LEV_OPT_REUSEABLE|LEV_OPT_CLOSE_ON_FREE, -1,
	    (struct sockaddr*)&sin,
	    sizeof(sin));

	if (!listener) {
		fprintf(stderr, "Could not create a listener!\n");
		return 1;
	}
	//3.创建一个信号事件，当执行ctrl-c时响应SIGINT事件，执行signal_cb回调，底层是执行了 event_new(include/event2/event.h)
	signal_event = evsignal_new(base, SIGINT, signal_cb, (void *)base);
	//4.将事件添加到待处理事件集中。(include/event2/event.h)
	if (!signal_event || event_add(signal_event, NULL)<0) {
		fprintf(stderr, "Could not create/add a signal event!\n");
		return 1;
	}
	//5.事件调度循环，此循环将运行事件库，底层运行 event_base_loop(include/event2/event.h)
	event_base_dispatch(base);

	evconnlistener_free(listener);//6.禁用并取消分配的 evconnlistener(include/event2/listener.h)
	event_free(signal_event);//7.释放 event_new 分配返回的结构体event*。(include/event2/event.h)
	event_base_free(base);//8.释放分配与事件库event_base关联的所有内存，并释放该库。(include/event2/event.h)

	printf("done\n");
	return 0;
}
//有tcp链接接入时执行的回调
//接口定义在(include/event2/bufferevent.h)
static void
listener_cb(struct evconnlistener *listener, evutil_socket_t fd,
    struct sockaddr *sa, int socklen, void *user_data)
{
	struct event_base *base = user_data;
	struct bufferevent *bev;
	//在现有的套接字上创建一个新的套接字缓冲区。
	//BEV_OPT_CLOSE_ON_FREE:当释放此缓冲区，我们关闭底层文件描述符
	bev = bufferevent_socket_new(base, fd, BEV_OPT_CLOSE_ON_FREE);
	if (!bev) {
		fprintf(stderr, "Error constructing bufferevent!");
		event_base_loopbreak(base);//立即中止活动的 event_base_loop
		return;
	}
	bufferevent_setcb(bev, NULL, conn_writecb, conn_eventcb, NULL);//更改缓冲器的回调。
	bufferevent_enable(bev, EV_WRITE);//启用bufferevent写。
	bufferevent_disable(bev, EV_READ);//禁用bufferevent读。

	bufferevent_write(bev, MESSAGE, strlen(MESSAGE));//将数据写入缓冲区buffer。
}
//已经写入足够的数据（输出缓冲数据低于阈值，由bufferevent_setwatermark设置）执行的回调
static void
conn_writecb(struct bufferevent *bev, void *user_data)
{
	struct evbuffer *output = bufferevent_get_output(bev);// 返回输出缓冲区。
	if (evbuffer_get_length(output) == 0) {// 返回存储在事件缓冲区中的总字节数
		printf("flushed answer\n");
		bufferevent_free(bev);//释放分配与缓冲器结构相关联的存储空间。
	}
}
//在文件描述符上有事件时调用的回调，主要是socket监听到错误时的回调
static void
conn_eventcb(struct bufferevent *bev, short events, void *user_data)
{
	if (events & BEV_EVENT_EOF) {
		printf("Connection closed.\n");
	} else if (events & BEV_EVENT_ERROR) {
		printf("Got an error on the connection: %s\n",
		    strerror(errno));/*XXX win32*/
	}
	/* None of the other events can happen here, since we haven't enabled
	 * timeouts	其他事件都不会在这里发生，因为我们没有启用超时 */
	bufferevent_free(bev);//释放分配与缓冲器结构相关联的存储空间。
}
//监听到某个信号时执行的回调(SIGINT)
static void
signal_cb(evutil_socket_t sig, short events, void *user_data)
{
	struct event_base *base = user_data;
	struct timeval delay = { 2, 0 };

	printf("Caught an interrupt signal; exiting cleanly in two seconds.\n");

	event_base_loopexit(base, &delay);//在指定时间后退出事件循环
}
