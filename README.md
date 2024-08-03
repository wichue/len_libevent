# len_libevent
学习C语言网络库libevent-2.1.12。

# 编译，CMake (Unix)
`
mkdir build && cd build
cmake ..     # Default to Unix Makefiles.
make
make verify  # (optional)
`

# 头文件功能
include/event2/buffer_compat.h
buffer.h 中的过时和不用的版本：仅提供向后兼容。

include/event2/buffer.h
用于缓冲网络发送或接收数据的功能。

include/event2/bufferevent_compat.h
提供兼容性，包含被弃用的 bufferevent_new 等函数。

include/event2/bufferevent_ssl.h
bufferevent 的 OpenSSL 支持。

include/event2/bufferevent_struct.h
bufferevent_struct.h 的使用已完全弃用；这些结构只公开，以便与使用它们的Libevent 2.0之前编写的程序向后兼容。

include/event2/bufferevent.h
用于缓冲网络发送或接收数据的功能。Bufferevents 比 evbuffers 级别更高：每个 Bufferevent 都有一个底层用于读取的 evbuffer 和一个用于写入的 evbuffer ，以及在某些情况下被调用的回调

include/event2/dns_compat.h
dns.h中函数的潜在非线程安全版本：仅为向后兼容性而提供。

include/event2/dns_struct.h
dns的数据结构。使用这些结构可能会损害与更高版本的Libevent的前向兼容性：小心！

include/event2/dns.h
异步DNS解析

include/event2/event_compat.h
event.h中函数的潜在非线程安全版本：仅为向后兼容性而提供。

include/event2/event_struct.h
event.h使用的结构。定义了 event event_callback 结构体。

include/event2/event.h
等待和接收事件以及使用事件库的核心功能。

include/event2/http_compat.h
http.h中函数的潜在非线程安全版本仅用于向后兼容性。

include/event2/http_struct.h
http的数据结构。使用这些结构可能会损害与更高版本的Libevent的前向兼容性：小心！

include/event2/http.h
基于libevent的嵌入式HTTP服务器。

include/event2/keyvalq_struct.h
修复了人们不必使用<sys/queue.h>运行的问题，此代码与event_struct.h重复。

include/event2/listener.h
TCP监听器，监听接入的tcp链接。

include/event2/rpc_compat.h
rpc.h中函数的弃用版本：仅为向后兼容性而提供。

include/event2/rpc_struct.h
rpc.h使用的结构。直接使用这些结构可能会损害前向兼容性：小心！

include/event2/rpc.h
为RPC服务器和客户端提供基本支持。

include/event2/tag_compat.h
tag.h中的过时/弃用函数；仅提供向后兼容性。

include/event2/tag.h
用于将标记数据读取和写入缓冲区的辅助函数。

include/event2/thread.h
使用Libevent的多线程应用程序的函数。

include/event2/util.h
跨平台可移植性，相关套接字操作的常见便利功能，生成随机数。

include/event2/visibility.h
符号导出相关。

include/evdns.h
在Libevent 2.0及更高版本中，<evdns.h>标头已被弃用；请改用<event2/evdns.h>。

include/event.h
在Libevent 2.0及更高版本中，<event.h>标头已被弃用；请改用<event2/event.h>。

include/evhttp.h
在Libevent 2.0及更高版本中，<evhttp.h>标头已被弃用；请改用<event2/http.h>。

include/evrpc.h
在Libevent 2.0及更高版本中，<evrpc.h>标头已被弃用；请改用<event2/rpc.h>。

include/evutil.h
在Libevent 2.0及更高版本中，<evutil.h>标头已被弃用；请改用<event2/util.h>。