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
#ifndef EVENT2_BUFFER_H_INCLUDED_
#define EVENT2_BUFFER_H_INCLUDED_

/** @file event2/buffer.h

  Functions for buffering data for network sending or receiving.
  用于缓冲网络发送或接收数据的功能。

  An evbuffer can be used for preparing data before sending it to
  the network or conversely for reading data from the network.
  Evbuffers try to avoid memory copies as much as possible.  As a
  result, evbuffers can be used to pass data around without actually
  incurring the overhead of copying the data.
  ev缓冲器可以用于在将数据发送到网络之前准备数据，或者相反地用于从网络中读取数据。
  事件缓冲器会尽量避免使用内存拷贝。因此，可以使用缓冲区传递数据，而不会产生复制数据的开销。

  A new evbuffer can be allocated with evbuffer_new(), and can be
  freed with evbuffer_free().  Most users will be using evbuffers via
  the bufferevent interface.  To access a bufferevent's evbuffers, use
  bufferevent_get_input() and bufferevent_get_output().
  可以用 evbuffer_new()分配一个新的事件缓冲区，也可以用evbuffer_free（）进行释放。
  大多数用户将通过缓冲区界面使用事件缓冲区。
  要访问缓冲区的外缓冲区，请使用 bufferevent_get_input()和 bufferevent_get_output()。

  There are several guidelines for using evbuffers.
  有几种关于使用事件缓冲区的指导原则。

  - if you already know how much data you are going to add as a result
    of calling evbuffer_add() multiple times, it makes sense to use
    evbuffer_expand() first to make sure that enough memory is allocated
    before hand.
  - 如果你已经知道你在多次调用 evbuffer_add 后会添加多少数据，那么首先使用evbuffer_expand（）来确保事先分配了足够的内存是有意义的。

  - evbuffer_add_buffer() adds the contents of one buffer to the other
    without incurring any unnecessary memory copies.
  - evbuffer_add_buffer 将一个缓冲区的内容添加到另一个缓冲区，而不会产生任何不必要的内存副本。

  - evbuffer_add() and evbuffer_add_buffer() do not mix very well:
    if you use them, you will wind up with fragmented memory in your
	buffer.
  - evbuffer_add 和evbuffer_add_buffer 混合得不太好：
    如果您使用它们，您将在缓冲区中出现碎片化的内存。

  - For high-performance code, you may want to avoid copying data into and out
    of buffers.  You can skip the copy step by using
    evbuffer_reserve_space()/evbuffer_commit_space() when writing into a
    buffer, and evbuffer_peek() when reading.
  -对于高性能代码，您可能希望避免将数据复制到或退出缓冲区。
   您可以通过在写入缓冲区时使用 evbuffer_reserve_space （）/evbuffer_commit_space （）和在读取时使用 evbuffer_peek （）来跳过复制步骤。

  In Libevent 2.0 and later, evbuffers are represented using a linked
  list of memory chunks, with pointers to the first and last chunk in
  the chain.
  在Libevent 2.0及更高版本中，事件缓冲区使用内存块的链接列表来表示，以及指向链中的第一个和最后一个块的指针。

  As the contents of an evbuffer can be stored in multiple different
  memory blocks, it cannot be accessed directly.  Instead, evbuffer_pullup()
  can be used to force a specified number of bytes to be contiguous. This
  will cause memory reallocation and memory copies if the data is split
  across multiple blocks.  It is more efficient, however, to use
  evbuffer_peek() if you don't require that the memory to be contiguous.
  由于一个数据缓冲区的内容可以存储在多个不同的内存块中，因此不能直接访问它。
  相反， evbuffer_pullup （）可以用于强制指定数量的字节保持连续。
  如果数据被分成多个块，这将导致内存重新分配和内存复制。
  但是，如果您不需要内存是连续的，那么使用 evbuffer_peek （）会更有效。
 */

#include <event2/visibility.h>

#ifdef __cplusplus
extern "C" {
#endif

#include <event2/event-config.h>
#include <stdarg.h>
#ifdef EVENT__HAVE_SYS_TYPES_H
#include <sys/types.h>
#endif
#ifdef EVENT__HAVE_SYS_UIO_H
#include <sys/uio.h>
#endif
#include <event2/util.h>

/**
   An evbuffer is an opaque data type for efficiently buffering data to be
   sent or received on the network.
   ev缓冲区是一种不透明的数据类型，用于有效地缓冲要在网络上发送或接收的数据。

   @see event2/event.h for more information
*/
struct evbuffer
#ifdef EVENT_IN_DOXYGEN_
{}
#endif
;

/**
    Pointer to a position within an evbuffer.
    指向事件缓冲区中的位置的指针。

    Used when repeatedly searching through a buffer.  Calling any function
    that modifies or re-packs the buffer contents may invalidate all
    evbuffer_ptrs for that buffer.  Do not modify or contruct these values
    except with evbuffer_ptr_set.
    在通过缓冲区进行重复搜索时使用。
    调用任何修改或重新打包缓冲区内容的函数都可能会使该缓冲区的所有evbuffer_ptrs无效。
    不要修改或构造这些值，除非使用 evbuffer_ptr_set

    An evbuffer_ptr can represent any position from the start of a buffer up
    to a position immediately after the end of a buffer.
    evbuffer_ptr 可以表示从缓冲区开始到缓冲区结束后的一个位置的任何位置。

    @see evbuffer_ptr_set()
 */
struct evbuffer_ptr {
	ev_ssize_t pos;

	/* Do not alter or rely on the values of fields: they are for internal
	 * use */
  // 不要更改或依赖于字段的值：它们将用于内部使用
	struct {
		void *chain;
		size_t pos_in_chain;
	} internal_;
};

/** Describes a single extent of memory inside an evbuffer.  Used for
    direct-access functions.
    描述事件缓冲区中的单个内存范围。用于直接访问函数。

    @see evbuffer_reserve_space, evbuffer_commit_space, evbuffer_peek
 */
#ifdef EVENT__HAVE_SYS_UIO_H
#define evbuffer_iovec iovec
/* Internal use -- defined only if we are using the native struct iovec */
// 内部使用——仅在我们使用本机结构体iovec时才定义
#define EVBUFFER_IOVEC_IS_NATIVE_
#else
struct evbuffer_iovec {
	/** The start of the extent of memory. */
	void *iov_base;
	/** The length of the extent of memory. */
	size_t iov_len;
};
#endif

/**
  Allocate storage for a new evbuffer.
  为新的数据缓冲区分配存储空间。

  @return a pointer to a newly allocated evbuffer struct, or NULL if an error
	occurred
  指向新分配的ev缓冲区结构的指针，如果发生错误，则为空
 */
EVENT2_EXPORT_SYMBOL
struct evbuffer *evbuffer_new(void);
/**
  Deallocate storage for an evbuffer.
  取消对事件缓冲区的存储空间的分配。

  @param buf pointer to the evbuffer to be freed
 */
EVENT2_EXPORT_SYMBOL
void evbuffer_free(struct evbuffer *buf);

/**
   Enable locking on an evbuffer so that it can safely be used by multiple
   threads at the same time.
   启用锁定ev缓冲区，以便它可以安全地被多个线程同时使用。

   NOTE: when locking is enabled, the lock will be held when callbacks are
   invoked.  This could result in deadlock if you aren't careful.  Plan
   accordingly!
   注意：当启用锁定时，在调用回调时将保持锁定。如果您不小心，这可能会导致死锁。相应的计划！

   @param buf An evbuffer to make lockable. 一个使可锁定的事件缓冲区。
   @param lock A lock object, or NULL if we should allocate our own. 一个锁对象，或者是空，如果我们应该分配我们自己的对象。
   @return 0 on success, -1 on failure.
 */
EVENT2_EXPORT_SYMBOL
int evbuffer_enable_locking(struct evbuffer *buf, void *lock);

/**
   Acquire the lock on an evbuffer.  Has no effect if locking was not enabled
   with evbuffer_enable_locking.
   获取事件缓冲区上的锁。如果没有使用 evbuffer_enable_locking 启用锁定，则无效。
*/
EVENT2_EXPORT_SYMBOL
void evbuffer_lock(struct evbuffer *buf);

/**
   Release the lock on an evbuffer.  Has no effect if locking was not enabled
   with evbuffer_enable_locking.
   释放缓冲区上的锁。如果没有使用 evbuffer_enable_locking 启用锁定，则无效
*/
EVENT2_EXPORT_SYMBOL
void evbuffer_unlock(struct evbuffer *buf);


/** If this flag is set, then we will not use evbuffer_peek(),
 * evbuffer_remove(), evbuffer_remove_buffer(), and so on to read bytes
 * from this buffer: we'll only take bytes out of this buffer by
 * writing them to the network (as with evbuffer_write_atmost), by
 * removing them without observing them (as with evbuffer_drain),
 * or by copying them all out at once (as with evbuffer_add_buffer).
 * 如果设置这个标志，那么我们不会使用 evbuffer_peek （）， evbuffer_remove （）， evbuffer_remove_buffer （）等等读取字节从这个缓冲区：
 * 我们只会把字节的缓冲区写入网络（ evbuffer_write_atmost ），通过删除他们没有观察他们（ evbuffer_drain ），或通过复制它们（ evbuffer_add_buffer ）。
 *
 * Using this option allows the implementation to use sendfile-based
 * operations for evbuffer_add_file(); see that function for more
 * information.
 * 使用此选项允许实现对 evbuffer_add_file （）使用基于发送文件的操作；有关更多信息，请参阅该函数。
 *
 * This flag is on by default for bufferevents that can take advantage
 * of it; you should never actually need to set it on a bufferevent's
 * output buffer.
 * 默认情况下，对于可以利用它的缓冲区，这个标志是打开的；实际上不需要在缓冲器的输出缓冲区上设置它。
 */
#define EVBUFFER_FLAG_DRAINS_TO_FD 1

/** Change the flags that are set for an evbuffer by adding more.
 *  通过添加更多内容来更改为事件缓冲区设置的标志。
 *
 * @param buffer the evbuffer that the callback is watching. 回调正在监视的事件缓冲区。
 * @param cb the callback whose status we want to change. 我们想更改其状态的回调函。
 * @param flags One or more EVBUFFER_FLAG_* options 一个或多个 EVBUFFER_FLAG_*选项
 * @return 0 on success, -1 on failure.
 */
EVENT2_EXPORT_SYMBOL
int evbuffer_set_flags(struct evbuffer *buf, ev_uint64_t flags);
/** Change the flags that are set for an evbuffer by removing some.
 *  通过删除一些标志来更改为事件缓冲区设置的标志。
 *
 * @param buffer the evbuffer that the callback is watching.
 * @param cb the callback whose status we want to change.
 * @param flags One or more EVBUFFER_FLAG_* options
 * @return 0 on success, -1 on failure.
 */
EVENT2_EXPORT_SYMBOL
int evbuffer_clear_flags(struct evbuffer *buf, ev_uint64_t flags);

/**
  Returns the total number of bytes stored in the evbuffer
  返回存储在事件缓冲区中的总字节数

  @param buf pointer to the evbuffer
  @return the number of bytes stored in the evbuffer
*/
EVENT2_EXPORT_SYMBOL
size_t evbuffer_get_length(const struct evbuffer *buf);

/**
   Returns the number of contiguous available bytes in the first buffer chain.
   返回第一个缓冲区链中连续的可用字节数。

   This is useful when processing data that might be split into multiple
   chains, or that might all be in the first chain.  Calls to
   evbuffer_pullup() that cause reallocation and copying of data can thus be
   avoided.
   当处理可能被分割成多个链或可能都在第一个链中的数据时，这很有用。因此，可以避免对导致重新分配和复制数据的 evbuffer_pullup （）的调用。

   @param buf pointer to the evbuffer
   @return 0 if no data is available, otherwise the number of available bytes
     in the first buffer chain.
     如果没有可用的数据返回0，否则返回第一个缓冲区链中可用的字节数
*/
EVENT2_EXPORT_SYMBOL
size_t evbuffer_get_contiguous_space(const struct evbuffer *buf);

/**
  Expands the available space in an evbuffer.
  扩展事件缓冲区中的可用空间。

  Expands the available space in the evbuffer to at least datlen, so that
  appending datlen additional bytes will not require any new allocations.
  将数据缓冲区中的可用空间扩展到至少数据集，这样附加数据字节将不需要任何新的分配。

  @param buf the evbuffer to be expanded 要扩展的事件缓冲区
  @param datlen the new minimum length requirement 新的最低长度要求
  @return 0 if successful, or -1 if an error occurred
*/
EVENT2_EXPORT_SYMBOL
int evbuffer_expand(struct evbuffer *buf, size_t datlen);

/**
   Reserves space in the last chain or chains of an evbuffer.
   在事件缓冲区的最后一个链中保留空间。

   Makes space available in the last chain or chains of an evbuffer that can
   be arbitrarily written to by a user.  The space does not become
   available for reading until it has been committed with
   evbuffer_commit_space().
   在用户可以任意写入的事件缓冲区的最后一个链中提供可用空间。该空间只有在提交给 evbuffer_commit_space （）后，才能用于阅读。

   The space is made available as one or more extents, represented by
   an initial pointer and a length.  You can force the memory to be
   available as only one extent.  Allowing more extents, however, makes the
   function more efficient.
   该空间可作为一个或多个区段使用，由一个初始指针和一个长度表示。您可以强制内存只能作为一个范围可用。然而，允许更多的范围，可以使该功能更有效。

   Multiple subsequent calls to this function will make the same space
   available until evbuffer_commit_space() has been called.
   对此函数的多次后续调用将使相同的空间可用，直到调用 evbuffer_commit_space （）为止。

   It is an error to do anything that moves around the buffer's internal
   memory structures before committing the space.
   在提交空间之前，执行在缓冲区内部内存结构中移动的任何操作都是错误的。

   NOTE: The code currently does not ever use more than two extents.
   This may change in future versions.
   注意：该代码当前使用的区段从来不超过两个区段。这种情况在未来的版本中可能会有所改变。

   @param buf the evbuffer in which to reserve space. 要在其中保留空间的事件缓冲区。
   @param size how much space to make available, at minimum.  The
      total length of the extents may be greater than the requested
      length. 至少可以提供多少空间。区段的总长度可能大于请求的长度。
   @param vec an array of one or more evbuffer_iovec structures to
      hold pointers to the reserved extents of memory. 一个或多个 evbuffer_iovec 结构的数组，用于保存指向内存保留区域的指针。
   @param n_vec The length of the vec array.  Must be at least 1;
       2 is more efficient. vec数组的长度。必须至少是1；2是更有效。
   @return the number of provided extents, or -1 on error. 提供的扩展数，或错误时为-1。
   @see evbuffer_commit_space()
*/
EVENT2_EXPORT_SYMBOL
int
evbuffer_reserve_space(struct evbuffer *buf, ev_ssize_t size,
    struct evbuffer_iovec *vec, int n_vec);

/**
   Commits previously reserved space.
   提交先前保留的空间。

   Commits some of the space previously reserved with
   evbuffer_reserve_space().  It then becomes available for reading.
   提交一些以前使用 evbuffer_reserve_space （）保留的空格。然后它就可以用来阅读了。

   This function may return an error if the pointer in the extents do
   not match those returned from evbuffer_reserve_space, or if data
   has been added to the buffer since the space was reserved.
   如果区段中的指针与 evbuffer_reserve_space 返回的指针不匹配，或者由于保留了空间，数据已添加到缓冲区，此函数可能返回错误。

   If you want to commit less data than you got reserved space for,
   modify the iov_len pointer of the appropriate extent to a smaller
   value.  Note that you may have received more space than you
   requested if it was available!
   如果要提交的数据少于保留的空间，请将适当范围的 iov_len 指针修改为更小的值。请注意，如果可用，您可能收到了比您要求的更多的空间！

   @param buf the evbuffer in which to reserve space. 要在其中保留空间的事件缓冲区
   @param vec one or two extents returned by evbuffer_reserve_space. 由 evbuffer_reserve_space 返回的一个或两个区段。
   @param n_vecs the number of extents. 范围的数目。
   @return 0 on success, -1 on error
   @see evbuffer_reserve_space()
*/
EVENT2_EXPORT_SYMBOL
int evbuffer_commit_space(struct evbuffer *buf,
    struct evbuffer_iovec *vec, int n_vecs);

/**
  Append data to the end of an evbuffer. 将数据附加到事件缓冲区的末尾。

  @param buf the evbuffer to be appended to 要附加到的事件缓冲区
  @param data pointer to the beginning of the data buffer 指向数据缓冲区开头的指针
  @param datlen the number of bytes to be copied from the data buffer 要从数据缓冲区中复制的字节数
  @return 0 on success, -1 on failure.
 */
EVENT2_EXPORT_SYMBOL
int evbuffer_add(struct evbuffer *buf, const void *data, size_t datlen);


/**
  Read data from an evbuffer and drain the bytes read.
  从事件缓冲区中读取数据，并消耗已读取的字节。

  If more bytes are requested than are available in the evbuffer, we
  only extract as many bytes as were available.
  如果请求的字节比ev缓冲区中可用的字节多，我们只提取可用的字节。

  @param buf the evbuffer to be read from 要从中读取的事件缓冲区
  @param data the destination buffer to store the result 要存储结果的目标缓冲区
  @param datlen the maximum size of the destination buffer 目标缓冲区的最大大小
  @return the number of bytes read, or -1 if we can't drain the buffer. 读取的字节数，或者，如果我们不能耗尽缓冲区时为-1。
 */
EVENT2_EXPORT_SYMBOL
int evbuffer_remove(struct evbuffer *buf, void *data, size_t datlen);

/**
  Read data from an evbuffer, and leave the buffer unchanged.
  从ev缓冲区中读取数据，并保持缓冲区不变。

  If more bytes are requested than are available in the evbuffer, we
  only extract as many bytes as were available.
  如果请求的字节比ev缓冲区中可用的字节多，我们只提取可用的字节。

  @param buf the evbuffer to be read from
  @param data_out the destination buffer to store the result
  @param datlen the maximum size of the destination buffer
  @return the number of bytes read, or -1 if we can't drain the buffer.
 */
EVENT2_EXPORT_SYMBOL
ev_ssize_t evbuffer_copyout(struct evbuffer *buf, void *data_out, size_t datlen);

/**
  Read data from the middle of an evbuffer, and leave the buffer unchanged.
  从事件缓冲区的中间读取数据，并保持缓冲区不变。

  If more bytes are requested than are available in the evbuffer, we
  only extract as many bytes as were available.

  @param buf the evbuffer to be read from
  @param pos the position to start reading from 要开始阅读的位置
  @param data_out the destination buffer to store the result
  @param datlen the maximum size of the destination buffer
  @return the number of bytes read, or -1 if we can't drain the buffer.
 */
EVENT2_EXPORT_SYMBOL
ev_ssize_t evbuffer_copyout_from(struct evbuffer *buf, const struct evbuffer_ptr *pos, void *data_out, size_t datlen);

/**
  Read data from an evbuffer into another evbuffer, draining
  the bytes from the source buffer.  This function avoids copy
  operations to the extent possible.
  将数据从一个ev缓冲区读入到另一个ev缓冲区中，从而从源缓冲区中消耗字节。此函数尽可能避免复制操作。

  If more bytes are requested than are available in src, the src
  buffer is drained completely.
  如果请求的字节超过src中可用的字节，则src缓冲区将完全耗尽。

  @param src the evbuffer to be read from 要从中读取的事件缓冲区
  @param dst the destination evbuffer to store the result into 要存储结果到的目标事件缓冲区
  @param datlen the maximum numbers of bytes to transfer 要传输的最大字节数
  @return the number of bytes read 已读取的字节数
 */
EVENT2_EXPORT_SYMBOL
int evbuffer_remove_buffer(struct evbuffer *src, struct evbuffer *dst,
    size_t datlen);

/** Used to tell evbuffer_readln what kind of line-ending to look for.
 *  用来告诉 evbuffer_readln 要找什么样的行结尾。
 */
enum evbuffer_eol_style {
	/** Any sequence of CR and LF characters is acceptable as an
	 * EOL.
   * 任何CR和LF字符序列都可以接受作为EOL。
	 *
	 * Note that this style can produce ambiguous results: the
	 * sequence "CRLF" will be treated as a single EOL if it is
	 * all in the buffer at once, but if you first read a CR from
	 * the network and later read an LF from the network, it will
	 * be treated as two EOLs.
   * 注意，这种样式可能会产生模糊的结果：如果序列"CRLF"同时在缓冲区中，它将被视为一个EOL，但如果您首先从网络读取CR，然后从网络读取LF，它将被视为两个EOL。
	 */
	EVBUFFER_EOL_ANY,
	/** An EOL is an LF, optionally preceded by a CR.  This style is
	 * most useful for implementing text-based internet protocols.
   * EOL是LF，可选先有CR。这种风格对于实现基于文本的互联网协议最为有用。 */
	EVBUFFER_EOL_CRLF,
	/** An EOL is a CR followed by an LF. EOL是CR后跟LF。 */
	EVBUFFER_EOL_CRLF_STRICT,
	/** An EOL is a LF. 一个EOL是一个LF。 */
	EVBUFFER_EOL_LF,
	/** An EOL is a NUL character (that is, a single byte with value 0) EOL是一个NUL字符（即，一个值为0的单个字节） */
	EVBUFFER_EOL_NUL
};

/**
 * Read a single line from an evbuffer.
 * 从事件缓冲区中读取一行数据。
 *
 * Reads a line terminated by an EOL as determined by the evbuffer_eol_style
 * argument.  Returns a newly allocated nul-terminated string; the caller must
 * free the returned value.  The EOL is not included in the returned string.
 * 读取由 evbuffer_eol_style 参数确定的由EOL终止的行。返回新分配的以无节点结尾的字符串；调用者必须释放返回的值。在返回的字符串中不包含该EOL。
 *
 * @param buffer the evbuffer to read from 要从中读取的事件缓冲区
 * @param n_read_out if non-NULL, points to a size_t that is set to the
 *       number of characters in the returned string.  This is useful for
 *       strings that can contain NUL characters.如果非NULL，则指向设置为返回字符串中的字符数的size_t。这对于可以包含NUL字符的字符串非常有用。
 * @param eol_style the style of line-ending to use. 要使用的行结尾的样式。
 * @return pointer to a single line, or NULL if an error occurred 指向一行的指针，如果发生错误则为空
 */
EVENT2_EXPORT_SYMBOL
char *evbuffer_readln(struct evbuffer *buffer, size_t *n_read_out,
    enum evbuffer_eol_style eol_style);

/**
  Move all data from one evbuffer into another evbuffer.
  将所有数据从一个数据缓冲区移动到另一个数据缓冲区中。

  This is a destructive add.  The data from one buffer moves into
  the other buffer.  However, no unnecessary memory copies occur.
  这是一个具有破坏性的补充。来自一个缓冲区的数据将移动到另一个缓冲区中。但是，不会发生不必要的内存副本。

  @param outbuf the output buffer 输出缓冲区
  @param inbuf the input buffer 输入缓冲区
  @return 0 if successful, or -1 if an error occurred

  @see evbuffer_remove_buffer()
 */
EVENT2_EXPORT_SYMBOL
int evbuffer_add_buffer(struct evbuffer *outbuf, struct evbuffer *inbuf);

/**
  Copy data from one evbuffer into another evbuffer.
  将数据从一个数据缓冲区复制到另一个事件缓冲区中。

  This is a non-destructive add.  The data from one buffer is copied
  into the other buffer.  However, no unnecessary memory copies occur.
  这是一个无损的添加。来自一个缓冲区的数据将被复制到另一个缓冲区中。但是，不会发生不必要的内存副本。

  Note that buffers already containing buffer references can't be added
  to other buffers.
  请注意，不能将已经包含缓冲区引用的缓冲区添加到其他缓冲区中。

  @param outbuf the output buffer
  @param inbuf the input buffer
  @return 0 if successful, or -1 if an error occurred
 */
EVENT2_EXPORT_SYMBOL
int evbuffer_add_buffer_reference(struct evbuffer *outbuf,
    struct evbuffer *inbuf);

/**
   A cleanup function for a piece of memory added to an evbuffer by
   reference.
   通过引用添加到事件缓冲区中的一段内存的清理函数。

   @see evbuffer_add_reference()
 */
typedef void (*evbuffer_ref_cleanup_cb)(const void *data,
    size_t datalen, void *extra);

/**
  Reference memory into an evbuffer without copying.
  将内存引用到ev缓冲区中，而不进行复制。

  The memory needs to remain valid until all the added data has been
  read.  This function keeps just a reference to the memory without
  actually incurring the overhead of a copy.
  内存需要保持有效，直到读取了所有添加的数据。这个函数只保留对内存的引用，而不会实际引起副本的开销。

  @param outbuf the output buffer 输出缓冲区
  @param data the memory to reference 要引用的内存
  @param datlen how memory to reference 要引用的内存大小
  @param cleanupfn callback to be invoked when the memory is no longer
	referenced by this evbuffer. 当内存不再被此事件缓冲区引用时要调用的回调。
  @param cleanupfn_arg optional argument to the cleanup callback 清除回调的可选参数
  @return 0 if successful, or -1 if an error occurred
 */
EVENT2_EXPORT_SYMBOL
int evbuffer_add_reference(struct evbuffer *outbuf,
    const void *data, size_t datlen,
    evbuffer_ref_cleanup_cb cleanupfn, void *cleanupfn_arg);

/**
  Copy data from a file into the evbuffer for writing to a socket.
  将数据从文件复制到ev缓冲区中，以便写入套接字。

  This function avoids unnecessary data copies between userland and
  kernel.  If sendfile is available and the EVBUFFER_FLAG_DRAINS_TO_FD
  flag is set, it uses those functions.  Otherwise, it tries to use
  mmap (or CreateFileMapping on Windows).
  这个函数避免了用户用地和内核之间不必要的数据复制。
  如果发送文件可用，并且设置了 EVBUFFER_FLAG_DRAINS_TO_FD 标志，那么它将使用这些函数。否则，它将尝试使用mmap（或在窗口上创建文件映射）。

  The function owns the resulting file descriptor and will close it
  when finished transferring data.
  该函数拥有生成的文件描述符，并将在完成传输数据时关闭它。

  The results of using evbuffer_remove() or evbuffer_pullup() on
  evbuffers whose data was added using this function are undefined.
  在使用此函数添加数据的事件缓冲区上使用 evbuffer_remove （）或 evbuffer_pullup （）的结果未定义。

  For more fine-grained control, use evbuffer_add_file_segment.
  对于更细粒度的控制，请使用 evbuffer_add_file_segment 。

  @param outbuf the output buffer 输出缓冲区
  @param fd the file descriptor
  @param offset the offset from which to read data 要从中读取数据的偏移量
  @param length how much data to read, or -1 to read as much as possible.
    (-1 requires that 'fd' support fstat.) 要读取多少数据，或者-1尽可能多地读取。（-1要求“fd”支持fstat。）
  @return 0 if successful, or -1 if an error occurred
*/

EVENT2_EXPORT_SYMBOL
int evbuffer_add_file(struct evbuffer *outbuf, int fd, ev_off_t offset,
    ev_off_t length);

/**
  An evbuffer_file_segment holds a reference to a range of a file --
  possibly the whole file! -- for use in writing from an evbuffer to a
  socket.  It could be implemented with mmap, sendfile, splice, or (if all
  else fails) by just pulling all the data into RAM.  A single
  evbuffer_file_segment can be added more than once, and to more than one
  evbuffer.
  一个 evbuffer_file_segment 包含对一个文件范围的引用——可能是整个文件！--，用于从数据缓冲区写入到套接字。
  它可以通过mmap、发送文件、拼接或（如果所有其他方法都失败了）仅通过将所有数据拉入RAM来实现。单个 evbuffer_file_segment 可以添加多次，并添加到多个缓冲区。
  define: evbuffer-internal.h
 */
struct evbuffer_file_segment;

/**
    Flag for creating evbuffer_file_segment: If this flag is set, then when
    the evbuffer_file_segment is freed and no longer in use by any
    evbuffer, the underlying fd is closed.
    创建 evbuffer_file_segment 的标志：如果设置了此标志，那么当 evbuffer_file_segment 被释放并且不再被任何事件缓冲区使用时，底层的fd将被关闭。
 */
#define EVBUF_FS_CLOSE_ON_FREE    0x01
/**
   Flag for creating evbuffer_file_segment: Disable memory-map based
   implementations.
   创建 evbuffer_file_segment 的标志：禁用基于内存映射的实现。
 */
#define EVBUF_FS_DISABLE_MMAP     0x02
/**
   Flag for creating evbuffer_file_segment: Disable direct fd-to-fd
   implementations (including sendfile and splice).
   创建 evbuffer_file_segment 的标志：禁用直接的fd到fd实现（包括发送文件和拼接）。

   You might want to use this option if data needs to be taken from the
   evbuffer by any means other than writing it to the network: the sendfile
   backend is fast, but it only works for sending files directly to the
   network.
   如果需要通过任何方式从ev缓冲区中获取数据，您可能希望使用此选项：发送文件后端快速，但它只用于将文件直接发送到网络。
 */
#define EVBUF_FS_DISABLE_SENDFILE 0x04
/**
   Flag for creating evbuffer_file_segment: Do not allocate a lock for this
   segment.  If this option is set, then neither the segment nor any
   evbuffer it is added to may ever be accessed from more than one thread
   at a time.
   创建 evbuffer_file_segment 的标志：不要为这个段分配一个锁。如果设置了此选项，则每次都不能从多个线程访问数据段或将其添加到的任何数据缓冲区。
 */
#define EVBUF_FS_DISABLE_LOCKING  0x08

/**
   A cleanup function for a evbuffer_file_segment added to an evbuffer
   for reference.
   对添加到事件缓冲区以供引用的 evbuffer_file_segment 的清理函数。
 */
typedef void (*evbuffer_file_segment_cleanup_cb)(
    struct evbuffer_file_segment const* seg, int flags, void* arg);

/**
   Create and return a new evbuffer_file_segment for reading data from a
   file and sending it out via an evbuffer.
   创建并返回一个新的 evbuffer_file_segment ，用于从文件中读取数据，并通过数据缓冲区发送出去。

   This function avoids unnecessary data copies between userland and
   kernel.  Where available, it uses sendfile or splice.
   这个函数避免了用户用地和内核之间不必要的数据复制。在可用的地方，它使用发送文件或拼接。

   The file descriptor must not be closed so long as any evbuffer is using
   this segment.
   只要任何ev缓冲区正在使用此段，就不能关闭文件描述符。

   The results of using evbuffer_remove() or evbuffer_pullup() or any other
   function that reads bytes from an evbuffer on any evbuffer containing
   the newly returned segment are undefined, unless you pass the
   EVBUF_FS_DISABLE_SENDFILE flag to this function.
   在包含新返回的段的任何ev缓冲区上，使用evbuffer_remove（）或evbuffer_pullup（）或任何其他从ev缓冲区读取字节的函数的结果都是未定义的，
   除非您将 EVBUF_FS_DISABLE_SENDFILE 标志传递给此函数。

   @param fd an open file to read from. 要读取的打开文件。
   @param offset an index within the file at which to start reading 要开始读取的文件中的一个索引
   @param length how much data to read, or -1 to read as much as possible.
      (-1 requires that 'fd' support fstat.) 要读取多少数据，或者-1尽可能多地读取。（-1要求“fd”支持fstat。）
   @param flags any number of the EVBUF_FS_* flags 任意数量的 EVBUF_FS_* 标志
   @return a new evbuffer_file_segment, or NULL on failure.
 **/
EVENT2_EXPORT_SYMBOL
struct evbuffer_file_segment *evbuffer_file_segment_new(
	int fd, ev_off_t offset, ev_off_t length, unsigned flags);

/**
   Free an evbuffer_file_segment 释放一个 evbuffer_file_segment

   It is safe to call this function even if the segment has been added to
   one or more evbuffers.  The evbuffer_file_segment will not be freed
   until no more references to it exist.
   即使段已添加到一个或多个缓冲区中，也可以调用此函数是安全的。在没有对它的引用存在之前，evbuffer_file_segment将不会被释放。
 */
EVENT2_EXPORT_SYMBOL
void evbuffer_file_segment_free(struct evbuffer_file_segment *seg);

/**
   Add cleanup callback and argument for the callback to an
   evbuffer_file_segment.
   将清除回调和该回调的参数添加到evbuffer_file_segment中。

   The cleanup callback will be invoked when no more references to the
   evbuffer_file_segment exist.
   当没有对evbuffer_file_segment的引用存在时，将调用清理回调。
 **/
EVENT2_EXPORT_SYMBOL
void evbuffer_file_segment_add_cleanup_cb(struct evbuffer_file_segment *seg,
	evbuffer_file_segment_cleanup_cb cb, void* arg);

/**
   Insert some or all of an evbuffer_file_segment at the end of an evbuffer
   在事件缓冲区的末尾插入部分或全部 evbuffer_file_segment

   Note that the offset and length parameters of this function have a
   different meaning from those provided to evbuffer_file_segment_new: When
   you create the segment, the offset is the offset _within the file_, and
   the length is the length _of the segment_, whereas when you add a
   segment to an evbuffer, the offset is _within the segment_ and the
   length is the length of the _part of the segment you want to use.
   请注意，此函数的偏移量和长度参数与evbuffer_file_segment_new提供的含义不同：创建段时，偏移量为文件中的偏移量，长度为段的长度，而添加段时，偏移量为段内，长度为要使用的段的长度。

   In other words, if you have a 10 KiB file, and you create an
   evbuffer_file_segment for it with offset 20 and length 1000, it will
   refer to bytes 20..1019 inclusive.  If you then pass this segment to
   evbuffer_add_file_segment and specify an offset of 20 and a length of
   50, you will be adding bytes 40..99 inclusive.
   换句话说，如果您有一个10 KiB文件，并且您为它创建一个偏移量为20和长度为1000的 evbuffer_file_segment ，它将引用字节20..包括1019。
   如果您将此段传递给 evbuffer_add_file_segment 并指定偏移量为20和长度为50，您将添加字节40..99。

   @param buf the evbuffer to append to 要附加到的ev缓冲区
   @param seg the segment to add 要添加的段
   @param offset the offset within the segment to start from 段内的偏移量
   @param length the amount of data to add, or -1 to add it all. 要添加的数据量，或-1将其全部添加到的数据量。
   @return 0 on success, -1 on failure.
 */
EVENT2_EXPORT_SYMBOL
int evbuffer_add_file_segment(struct evbuffer *buf,
    struct evbuffer_file_segment *seg, ev_off_t offset, ev_off_t length);

/**
  Append a formatted string to the end of an evbuffer.
  将格式化的字符串附加到事件缓冲区的结尾。

  The string is formated as printf.
  该字符串被格式化为printf。

  @param buf the evbuffer that will be appended to 将被附加到的ev缓冲区
  @param fmt a format string 格式化的字符串
  @param ... arguments that will be passed to printf(3) 将被传递给printf (3)的参数
  @return The number of bytes added if successful, or -1 if an error occurred. 如果成功则返回添加的字节数，如果发生错误则返回-1。

  @see evutil_printf(), evbuffer_add_vprintf()
 */
EVENT2_EXPORT_SYMBOL
int evbuffer_add_printf(struct evbuffer *buf, const char *fmt, ...)
#ifdef __GNUC__
  __attribute__((format(printf, 2, 3)))
#endif
;

/**
  Append a va_list formatted string to the end of an evbuffer.
  将一个va_list格式的字符串附加到事件缓冲区的末尾。

  @param buf the evbuffer that will be appended to 将被附加到的ev缓冲区
  @param fmt a format string
  @param ap a varargs va_list argument array that will be passed to vprintf(3) 将传递给vprintf (3)的变量va_list参数数组
  @return The number of bytes added if successful, or -1 if an error occurred. 如果成功则返回添加的字节数，如果发生错误则返回-1。
 */
EVENT2_EXPORT_SYMBOL
int evbuffer_add_vprintf(struct evbuffer *buf, const char *fmt, va_list ap)
#ifdef __GNUC__
	__attribute__((format(printf, 2, 0)))
#endif
;


/**
  Remove a specified number of bytes data from the beginning of an evbuffer.
  从事件缓冲区的开头删除指定数量的字节数据。

  @param buf the evbuffer to be drained 要操作的事件缓冲区
  @param len the number of bytes to drain from the beginning of the buffer 从缓冲区开始要删除的字节数
  @return 0 on success, -1 on failure.
 */
EVENT2_EXPORT_SYMBOL
int evbuffer_drain(struct evbuffer *buf, size_t len);


/**
  Write the contents of an evbuffer to a file descriptor.
  将ev缓冲区的内容写入文件描述符。

  The evbuffer will be drained after the bytes have been successfully written.
  在成功写入字节后，事件缓冲区将被耗尽。

  @param buffer the evbuffer to be written and drained 要写入和排出的事件缓冲区
  @param fd the file descriptor to be written to 要写入的文件描述符
  @return the number of bytes written, or -1 if an error occurred 已写入的字节数，如果发生错误，则为-1
  @see evbuffer_read()
 */
EVENT2_EXPORT_SYMBOL
int evbuffer_write(struct evbuffer *buffer, evutil_socket_t fd);

/**
  Write some of the contents of an evbuffer to a file descriptor.
  将ev缓冲区的一些内容写入文件描述符。

  The evbuffer will be drained after the bytes have been successfully written.
  在成功写入字节后，事件缓冲区将被耗尽。

  @param buffer the evbuffer to be written and drained 要写入和排出的事件缓冲区
  @param fd the file descriptor to be written to 要写入的文件描述符
  @param howmuch the largest allowable number of bytes to write, or -1
	to write as many bytes as we can. 允许写的最大字节数，或-1写入尽可能多的字节。
  @return the number of bytes written, or -1 if an error occurred 已写入的字节数，如果发生错误，则为-1
  @see evbuffer_read()
 */
EVENT2_EXPORT_SYMBOL
int evbuffer_write_atmost(struct evbuffer *buffer, evutil_socket_t fd,
						  ev_ssize_t howmuch);

/**
  Read from a file descriptor and store the result in an evbuffer.
  从文件描述符中读取，并将结果存储在事件缓冲区中。

  @param buffer the evbuffer to store the result 用来存储结果的ev缓冲区
  @param fd the file descriptor to read from 要从中读取的文件描述符
  @param howmuch the number of bytes to be read. If the given number is negative
  or out of maximum bytes per one read, as many bytes as we can will be read.
         要读取的字节数。如果给定的数字为负数或每次读取最大字节，将读取尽可能多的字节。
  @return the number of bytes read, or -1 if an error occurred
  @see evbuffer_write()
 */
EVENT2_EXPORT_SYMBOL
int evbuffer_read(struct evbuffer *buffer, evutil_socket_t fd, int howmuch);

/**
   Search for a string within an evbuffer.
   在ev缓冲区中搜索字符串。

   @param buffer the evbuffer to be searched 要搜索的事件缓冲区
   @param what the string to be searched for 要搜索的字符串
   @param len the length of the search string 搜索字符串的长度
   @param start NULL or a pointer to a valid struct evbuffer_ptr. 为空或指向有效结构体evbuffer_ptr的指针。
   @return a struct evbuffer_ptr whose 'pos' field has the offset of the
     first occurrence of the string in the buffer after 'start'.  The 'pos'
     field of the result is -1 if the string was not found.
     一个结构 evbuffer_ptr ，其“pos”字段具有“start”之后缓冲区中字符串的第一次出现的偏移量。如果没有找到该字符串，则结果的“pos”字段为-1。
 */
EVENT2_EXPORT_SYMBOL
struct evbuffer_ptr evbuffer_search(struct evbuffer *buffer, const char *what, size_t len, const struct evbuffer_ptr *start);

/**
   Search for a string within part of an evbuffer.
   在事件缓冲区的一部分中搜索一个字符串。

   @param buffer the evbuffer to be searched
   @param what the string to be searched for 要搜索的事件缓冲区
   @param len the length of the search string 搜索字符串的长度
   @param start NULL or a pointer to a valid struct evbuffer_ptr that
     indicates where we should start searching. NULL或指向有效结构evbuffer_ptr的指针，指示我们应该开始搜索。
   @param end NULL or a pointer to a valid struct evbuffer_ptr that
     indicates where we should stop searching. NULL或指向有效结构evbuffer_ptr的指针，指示我们应该停止搜索。
   @return a struct evbuffer_ptr whose 'pos' field has the offset of the
     first occurrence of the string in the buffer after 'start'.  The 'pos'
     field of the result is -1 if the string was not found.
     一个结构 evbuffer_ptr ，其“pos”字段具有“start”之后缓冲区中字符串的第一次出现的偏移量。如果没有找到该字符串，则结果的“pos”字段为-1。
 */
EVENT2_EXPORT_SYMBOL
struct evbuffer_ptr evbuffer_search_range(struct evbuffer *buffer, const char *what, size_t len, const struct evbuffer_ptr *start, const struct evbuffer_ptr *end);

/**
   Defines how to adjust an evbuffer_ptr by evbuffer_ptr_set()
   定义如何通过 evbuffer_ptr_set （）调整 evbuffer_ptr

   @see evbuffer_ptr_set() */
enum evbuffer_ptr_how {
	/** Sets the pointer to the position; can be called on with an
	    uninitialized evbuffer_ptr. 将指针设置为位置；可以使用未初始化的evbuffer_ptr进行调用。 */
	EVBUFFER_PTR_SET,
	/** Advances the pointer by adding to the current position. 通过添加到当前位置来推进指针。 */
	EVBUFFER_PTR_ADD
};

/**
   Sets the search pointer in the buffer to position.
   将缓冲区中的搜索指针设置为位置。

   There are two ways to use this function: you can call 有两种方法可以使用这个函数：您可以调用
      evbuffer_ptr_set(buf, &pos, N, EVBUFFER_PTR_SET)
   to move 'pos' to a position 'N' bytes after the start of the buffer, or 在缓冲区启动后将“pos”移动到位置“N”字节，或者
      evbuffer_ptr_set(buf, &pos, N, EVBUFFER_PTR_ADD)
   to move 'pos' forward by 'N' bytes. 将“pos”向前移动“N”字节。

   If evbuffer_ptr is not initialized, this function can only be called
   with EVBUFFER_PTR_SET.
   如果evbuffer_ptr未被初始化，则只能用 EVBUFFER_PTR_SET 来调用此函数。

   An evbuffer_ptr can represent any position from the start of the buffer to
   a position immediately after the end of the buffer.
   evbuffer_ptr 可以表示从缓冲区开始到缓冲区结束后的一个位置的任何位置。

   @param buffer the evbuffer to be search 要搜索的事件缓冲区
   @param ptr a pointer to a struct evbuffer_ptr 一个指向一个结构体evbuffer_ptr的指针
   @param position the position at which to start the next search 开始下一次搜索的位置
   @param how determines how the pointer should be manipulated. 确定应如何操纵指针。
   @returns 0 on success or -1 otherwise
*/
EVENT2_EXPORT_SYMBOL
int
evbuffer_ptr_set(struct evbuffer *buffer, struct evbuffer_ptr *ptr,
    size_t position, enum evbuffer_ptr_how how);

/**
   Search for an end-of-line string within an evbuffer.
   在事件缓冲区中搜索行尾字符串。

   @param buffer the evbuffer to be searched 要搜索的事件缓冲区
   @param start NULL or a pointer to a valid struct evbuffer_ptr to start
      searching at. NULL或一个指针，指向一个有效的结构体evbuffer_ptr，以开始搜索。
   @param eol_len_out If non-NULL, the pointed-to value will be set to
      the length of the end-of-line string. 如果非NULL，则指向值将设置为行末端字符串的长度。
   @param eol_style The kind of EOL to look for; see evbuffer_readln() for
      more information 要查找的EOL类型；有关更多信息，请参见 evbuffer_readln （）
   @return a struct evbuffer_ptr whose 'pos' field has the offset of the
     first occurrence EOL in the buffer after 'start'.  The 'pos'
     field of the result is -1 if the string was not found.
     一个结构evbuffer_ptr，其“pos”字段在“start”之后缓冲区中第一个出现的EOL的偏移量。如果没有找到该字符串，则结果的“pos”字段为-1。
 */
EVENT2_EXPORT_SYMBOL
struct evbuffer_ptr evbuffer_search_eol(struct evbuffer *buffer,
    struct evbuffer_ptr *start, size_t *eol_len_out,
    enum evbuffer_eol_style eol_style);

/** Function to peek at data inside an evbuffer without removing it or
    copying it out.
    函数来窥视数据在一个事件缓冲区内的数据，而不删除它或复制它。

    Pointers to the data are returned by filling the 'vec_out' array
    with pointers to one or more extents of data inside the buffer.
    通过向缓冲区内的一个或多个数据范围的指针填充‘ vec_out ’数组来返回指向数据的指针。

    The total data in the extents that you get back may be more than
    you requested (if there is more data last extent than you asked
    for), or less (if you do not provide enough evbuffer_iovecs, or if
    the buffer does not have as much data as you asked to see).
    您返回的范围中的总数据可能超过您请求的数据（如果最后范围的数据比您要求的多），或者更少（如果您没有提供足够的evbuffer_iovecs，或者缓冲区没有您要求看到的那么多数据）。

    @param buffer the evbuffer to peek into, 可以窥视的事件缓冲区
    @param len the number of bytes to try to peek.  If len is negative, we
       will try to fill as much of vec_out as we can.  If len is negative
       and vec_out is not provided, we return the number of evbuffer_iovecs
       that would be needed to get all the data in the buffer.
       要尝试查看的字节数。如果len是负的，我们将尝试尽可能多地填充vec_out。如果len为负数，并且没有提供vec_out，则我们返回获取缓冲区中所有数据所需的evbuffer_iovecs数。
    @param start_at an evbuffer_ptr indicating the point at which we
       should start looking for data.  NULL means, "At the start of the
       buffer." 一个指示我们应该开始寻找数据的点的evbuffer_ptr。空的意思是，在缓冲区的开始处。
    @param vec_out an array of evbuffer_iovec evbuffer_iovec 数组
    @param n_vec the length of vec_out.  If 0, we only count how many
       extents would be necessary to point to the requested amount of
       data. vec_out的长度。如果为0，则我们只计算需要多少区段来指向所请求的数据量。
    @return The number of extents needed.  This may be less than n_vec
       if we didn't need all the evbuffer_iovecs we were given, or more
       than n_vec if we would need more to return all the data that was
       requested.
       所需的扩展区数。如果我们不需要所有的 evbuffer_iovecs ，这可能会超过n_vec；如果我们需要更多来返回所请求的所有数据，这可能会超过n_vec。
 */
EVENT2_EXPORT_SYMBOL
int evbuffer_peek(struct evbuffer *buffer, ev_ssize_t len,
    struct evbuffer_ptr *start_at,
    struct evbuffer_iovec *vec_out, int n_vec);


/** Structure passed to an evbuffer_cb_func evbuffer callback
 *  传递给evbuffer_cb_func数据缓冲区回调的结构

    @see evbuffer_cb_func, evbuffer_add_cb()
 */
struct evbuffer_cb_info {
	/** The number of bytes in this evbuffer when callbacks were last
	 * invoked. 上次调用回调时此数据缓冲区中的字节数 */
	size_t orig_size;
	/** The number of bytes added since callbacks were last invoked. 自上次调用回调以来所添加的字节数。 */
	size_t n_added;
	/** The number of bytes removed since callbacks were last invoked. 自上次调用回调以来被删除的字节数。 */
	size_t n_deleted;
};

/** Type definition for a callback that is invoked whenever data is added or
    removed from an evbuffer.
    每当从ev缓冲区中添加或删除数据时，就会调用的回调的类型定义。

    An evbuffer may have one or more callbacks set at a time.  The order
    in which they are executed is undefined.
    一个事件缓冲区一次可以设置一个或多个回调。它们的执行顺序尚未定义。

    A callback function may add more callbacks, or remove itself from the
    list of callbacks, or add or remove data from the buffer.  It may not
    remove another callback from the list.
    回调函数可以添加更多的回调，或者从回调列表中删除自己，或者从缓冲区中添加或删除数据。它可能不会从列表中删除另一个回调。

    If a callback adds or removes data from the buffer or from another
    buffer, this can cause a recursive invocation of your callback or
    other callbacks.  If you ask for an infinite loop, you might just get
    one: watch out!
    如果回调从缓冲区或其他缓冲区中添加或删除数据，这可能会导致对回调或其他回调的递归调用。如果你要求一个无限循环，你可能会得到一个：小心！

    @param buffer the buffer whose size has changed 其大小已发生更改的缓冲区
    @param info a structure describing how the buffer changed. 一种描述缓冲区如何变化的结构。
    @param arg a pointer to user data 一个指向用户数据的指针
*/
typedef void (*evbuffer_cb_func)(struct evbuffer *buffer, const struct evbuffer_cb_info *info, void *arg);

struct evbuffer_cb_entry;
/** Add a new callback to an evbuffer.
 * 向事件缓冲区添加一个新的回调。

  Subsequent calls to evbuffer_add_cb() add new callbacks.  To remove this
  callback, call evbuffer_remove_cb or evbuffer_remove_cb_entry.
  对evbuffer_add_cb（）的后续调用将添加新的回调。若要删除此回调，请调用evbuffer_remove_cb或evbuffer_remove_cb_entry。

  @param buffer the evbuffer to be monitored 要监视的事件缓冲区
  @param cb the callback function to invoke when the evbuffer is modified,
	or NULL to remove all callbacks. 在ev缓冲区被修改时用来调用的回调函数，或NULL用来删除所有回调。
  @param cbarg an argument to be provided to the callback function 要提供给回调函数的参数
  @return a handle to the callback on success, or NULL on failure.
 */
EVENT2_EXPORT_SYMBOL
struct evbuffer_cb_entry *evbuffer_add_cb(struct evbuffer *buffer, evbuffer_cb_func cb, void *cbarg);

/** Remove a callback from an evbuffer, given a handle returned from
    evbuffer_add_cb.
    给定从evbuffer_add_cb返回的句柄，从事件缓冲区中删除回调。

    Calling this function invalidates the handle.
    调用此函数会使句柄无效。

    @return 0 if a callback was removed, or -1 if no matching callback was
    found.
 */
EVENT2_EXPORT_SYMBOL
int evbuffer_remove_cb_entry(struct evbuffer *buffer,
			     struct evbuffer_cb_entry *ent);

/** Remove a callback from an evbuffer, given the function and argument
    used to add it.
    从用于添加回调的缓冲区中删除函数和参数。

    @return 0 if a callback was removed, or -1 if no matching callback was
    found.
 */
EVENT2_EXPORT_SYMBOL
int evbuffer_remove_cb(struct evbuffer *buffer, evbuffer_cb_func cb, void *cbarg);

/** If this flag is not set, then a callback is temporarily disabled, and
 * should not be invoked.
 * 如果未设置此标志，则暂时禁用回调，不应调用。
 *
 * @see evbuffer_cb_set_flags(), evbuffer_cb_clear_flags()
 */
#define EVBUFFER_CB_ENABLED 1

/** Change the flags that are set for a callback on a buffer by adding more.
 *  通过添加更多内容，更改为缓冲区上的回调设置的标志。

    @param buffer the evbuffer that the callback is watching. 回调正在监视的事件缓冲区。
    @param cb the callback whose status we want to change. 我们想更改其状态的回调函数。
    @param flags EVBUFFER_CB_ENABLED to re-enable the callback. 使用EVBUFFER_CB_ENABLED来重新启用回调。
    @return 0 on success, -1 on failure.
 */
EVENT2_EXPORT_SYMBOL
int evbuffer_cb_set_flags(struct evbuffer *buffer,
			  struct evbuffer_cb_entry *cb, ev_uint32_t flags);

/** Change the flags that are set for a callback on a buffer by removing some
 *  通过删除一些回调来更改为缓冲区上的回调设置的标志

    @param buffer the evbuffer that the callback is watching. 回调正在监视的事件缓冲区。
    @param cb the callback whose status we want to change. 我们想更改其状态的回调函数。
    @param flags EVBUFFER_CB_ENABLED to disable the callback. 使用EVBUFFER_CB_ENABLED来禁用回调。
    @return 0 on success, -1 on failure.
 */
EVENT2_EXPORT_SYMBOL
int evbuffer_cb_clear_flags(struct evbuffer *buffer,
			  struct evbuffer_cb_entry *cb, ev_uint32_t flags);

#if 0
/** Postpone calling a given callback until unsuspend is called later.

    This is different from disabling the callback, since the callback will get
	invoked later if the buffer size changes between now and when we unsuspend
	it.

	@param the buffer that the callback is watching.
	@param cb the callback we want to suspend.
 */
EVENT2_EXPORT_SYMBOL
void evbuffer_cb_suspend(struct evbuffer *buffer, struct evbuffer_cb_entry *cb);
/** Stop postponing a callback that we postponed with evbuffer_cb_suspend.

	If data was added to or removed from the buffer while the callback was
	suspended, the callback will get called once now.

	@param the buffer that the callback is watching.
	@param cb the callback we want to stop suspending.
 */
EVENT2_EXPORT_SYMBOL
void evbuffer_cb_unsuspend(struct evbuffer *buffer, struct evbuffer_cb_entry *cb);
#endif

/**
  Makes the data at the beginning of an evbuffer contiguous.
  使事件缓冲区开头的数据连续。

  @param buf the evbuffer to make contiguous 要使之连续的事件缓冲区
  @param size the number of bytes to make contiguous, or -1 to make the
	entire buffer contiguous. 要使之连续的字节数，或要使整个缓冲区连续的字节数为-1。
  @return a pointer to the contiguous memory array, or NULL if param size
	requested more data than is present in the buffer.
  指向连续内存数组的指针，如果参数大小请求的数据比缓冲区中存在的数据更多，则为NULL。
*/

EVENT2_EXPORT_SYMBOL
unsigned char *evbuffer_pullup(struct evbuffer *buf, ev_ssize_t size);

/**
  Prepends data to the beginning of the evbuffer
  将数据准备到事件缓冲区的开头

  @param buf the evbuffer to which to prepend data 要准备数据的事件缓冲区
  @param data a pointer to the memory to prepend 一个指向要前置的内存的指针
  @param size the number of bytes to prepend 要前置的字节数
  @return 0 if successful, or -1 otherwise
*/

EVENT2_EXPORT_SYMBOL
int evbuffer_prepend(struct evbuffer *buf, const void *data, size_t size);

/**
  Prepends all data from the src evbuffer to the beginning of the dst
  evbuffer.
  准备从src缓冲区到dst缓冲区开始的所有数据。

  @param dst the evbuffer to which to prepend data 要准备数据的事件缓冲区
  @param src the evbuffer to prepend; it will be emptied as a result 要前置的事件缓冲区；因此，它将被清空
  @return 0 if successful, or -1 otherwise
*/
EVENT2_EXPORT_SYMBOL
int evbuffer_prepend_buffer(struct evbuffer *dst, struct evbuffer* src);

/**
   Prevent calls that modify an evbuffer from succeeding. A buffer may
   frozen at the front, at the back, or at both the front and the back.
   防止修改ev缓冲区的调用成功。缓冲器可能在前面、后面或前后都冻结。

   If the front of a buffer is frozen, operations that drain data from
   the front of the buffer, or that prepend data to the buffer, will
   fail until it is unfrozen.   If the back a buffer is frozen, operations
   that append data from the buffer will fail until it is unfrozen.
   如果缓冲区的前端被冻结，则将缓冲区前端的数据输出到缓冲区的操作将失败，直到解除冻结。如果缓冲区被冻结，从缓冲区附加数据的操作将失败，直到解除冻结。

   @param buf The buffer to freeze 要冻结的缓冲区
   @param at_front If true, we freeze the front of the buffer.  If false,
      we freeze the back. 如果为真，我们将冻结缓冲区的前面。如果是假的，我们就冻结背部。
   @return 0 on success, -1 on failure.
*/
EVENT2_EXPORT_SYMBOL
int evbuffer_freeze(struct evbuffer *buf, int at_front);
/**
   Re-enable calls that modify an evbuffer.
   重新启用可修改ev缓冲区的调用。

   @param buf The buffer to un-freeze 解除冻结的缓冲区
   @param at_front If true, we unfreeze the front of the buffer.  If false,
      we unfreeze the back. 如果为真，我们将解冻缓冲区的前面。如果是假的，我们就解冻背部。
   @return 0 on success, -1 on failure.
 */
EVENT2_EXPORT_SYMBOL
int evbuffer_unfreeze(struct evbuffer *buf, int at_front);

struct event_base;
/**
   Force all the callbacks on an evbuffer to be run, not immediately after
   the evbuffer is altered, but instead from inside the event loop.
   强制执行事件缓冲区上的所有回调，而不是在更改事件缓冲区后立即运行，而是从事件循环内部运行。

   This can be used to serialize all the callbacks to a single thread
   of execution.
   这可以用于序列化所有对一个执行线程的回调。
 */
EVENT2_EXPORT_SYMBOL
int evbuffer_defer_callbacks(struct evbuffer *buffer, struct event_base *base);

/**
  Append data from 1 or more iovec's to an evbuffer
  将一个或多个iovec的数据附加到事件缓冲区中

  Calculates the number of bytes needed for an iovec structure and guarantees
  all data will fit into a single chain. Can be used in lieu of functionality
  which calls evbuffer_add() constantly before being used to increase
  performance.
  计算iovec结构所需的字节数，并保证所有数据都适合到单个链中。可以用来代替在用于提高性能之前不断调用evbuffer_add（）的功能。

  @param buffer the destination buffer 目标缓冲区
  @param vec the source iovec
  @param n_vec the number of iovec structures. iovec 结构的数量。
  @return the number of bytes successfully written to the output buffer. 已成功写入输出缓冲区的字节数。
*/
EVENT2_EXPORT_SYMBOL
size_t evbuffer_add_iovec(struct evbuffer * buffer, struct evbuffer_iovec * vec, int n_vec);

#ifdef __cplusplus
}
#endif

#endif /* EVENT2_BUFFER_H_INCLUDED_ */
