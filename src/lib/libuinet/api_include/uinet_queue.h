/*-
 * Copyright (c) 1991, 1993
 *	The Regents of the University of California.  All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 4. Neither the name of the University nor the names of its contributors
 *    may be used to endorse or promote products derived from this software
 *    without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE REGENTS AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE REGENTS OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 *
 *	@(#)queue.h	8.5 (Berkeley) 8/20/94
 * $FreeBSD: releng/10.0/sys/sys/queue.h 251887 2013-06-18 02:57:56Z lstewart $
 */

#ifndef _UINET_QUEUE_H_
#define	_UINET_QUEUE_H_


/*
 * Macro to test if we're using a specific version of gcc or later.
 */
#if defined(__GNUC__) && !defined(__INTEL_COMPILER)
#define __UINET_GNUC_PREREQ__(ma, mi) \
        (__GNUC__ > (ma) || __GNUC__ == (ma) && __GNUC_MINOR__ >= (mi))
#else
#define __UINET_GNUC_PREREQ__(ma, mi) 0
#endif

#ifndef __UINET_DEQUALIFY
#define __UINET_DEQUALIFY(type, var)  ((type)(uintptr_t)(const volatile void *)(var))
#endif

/*
 * We define this here since <stddef.h>, <sys/queue.h>, and <sys/types.h>
 * require it.
 */
#if __UINET_GNUC_PREREQ__(4, 1)
#define __uinet_offsetof(type, field)  __builtin_offsetof(type, field)
#else
#ifndef __cplusplus
#define __uinet_offsetof(type, field) \
        ((__size_t)(__uintptr_t)((const volatile void *)&((type *)0)->field))
#else
#define __uinet_offsetof(type, field)                           \
  (__offsetof__ (reinterpret_cast <__size_t>                    \
                 (&reinterpret_cast <const volatile char &>     \
                  (static_cast<type *> (0)->field))))
#endif
#endif

/*
 * Given the pointer x to the member m of the struct s, return
 * a pointer to the containing structure.  When using GCC, we first
 * assign pointer x to a local variable, to check that its type is
 * compatible with member m.
 */
#if __UINET_GNUC_PREREQ__(3, 1)
#define __uinet_containerof(x, s, m) ({                                 \
        const volatile __typeof(((s *)0)->m) *__x = (x);                \
        __UINET_DEQUALIFY(s *, (const volatile char *)__x - __uinet_offsetof(s, m));\
})
#else
#define __uinet_containerof(x, s, m)                                     \
        __UINET_DEQUALIFY(s *, (const volatile char *)(x) - __uinet_offsetof(s, m))
#endif


/*
 * This file defines four types of data structures: singly-linked lists,
 * singly-linked tail queues, lists and tail queues.
 *
 * A singly-linked list is headed by a single forward pointer. The elements
 * are singly linked for minimum space and pointer manipulation overhead at
 * the expense of O(n) removal for arbitrary elements. New elements can be
 * added to the list after an existing element or at the head of the list.
 * Elements being removed from the head of the list should use the explicit
 * macro for this purpose for optimum efficiency. A singly-linked list may
 * only be traversed in the forward direction.  Singly-linked lists are ideal
 * for applications with large datasets and few or no removals or for
 * implementing a LIFO queue.
 *
 * A singly-linked tail queue is headed by a pair of pointers, one to the
 * head of the list and the other to the tail of the list. The elements are
 * singly linked for minimum space and pointer manipulation overhead at the
 * expense of O(n) removal for arbitrary elements. New elements can be added
 * to the list after an existing element, at the head of the list, or at the
 * end of the list. Elements being removed from the head of the tail queue
 * should use the explicit macro for this purpose for optimum efficiency.
 * A singly-linked tail queue may only be traversed in the forward direction.
 * Singly-linked tail queues are ideal for applications with large datasets
 * and few or no removals or for implementing a FIFO queue.
 *
 * A list is headed by a single forward pointer (or an array of forward
 * pointers for a hash table header). The elements are doubly linked
 * so that an arbitrary element can be removed without a need to
 * traverse the list. New elements can be added to the list before
 * or after an existing element or at the head of the list. A list
 * may be traversed in either direction.
 *
 * A tail queue is headed by a pair of pointers, one to the head of the
 * list and the other to the tail of the list. The elements are doubly
 * linked so that an arbitrary element can be removed without a need to
 * traverse the list. New elements can be added to the list before or
 * after an existing element, at the head of the list, or at the end of
 * the list. A tail queue may be traversed in either direction.
 *
 * For details on the use of these macros, see the queue(3) manual page.
 *
 *
 *				SLIST	LIST	STAILQ	TAILQ
 * _HEAD			+	+	+	+
 * _HEAD_INITIALIZER		+	+	+	+
 * _ENTRY			+	+	+	+
 * _INIT			+	+	+	+
 * _EMPTY			+	+	+	+
 * _FIRST			+	+	+	+
 * _NEXT			+	+	+	+
 * _PREV			-	+	-	+
 * _LAST			-	-	+	+
 * _FOREACH			+	+	+	+
 * _FOREACH_FROM		+	+	+	+
 * _FOREACH_SAFE		+	+	+	+
 * _FOREACH_FROM_SAFE		+	+	+	+
 * _FOREACH_REVERSE		-	-	-	+
 * _FOREACH_REVERSE_FROM	-	-	-	+
 * _FOREACH_REVERSE_SAFE	-	-	-	+
 * _FOREACH_REVERSE_FROM_SAFE	-	-	-	+
 * _INSERT_HEAD			+	+	+	+
 * _INSERT_BEFORE		-	+	-	+
 * _INSERT_AFTER		+	+	+	+
 * _INSERT_TAIL			-	-	+	+
 * _CONCAT			-	-	+	+
 * _REMOVE_AFTER		+	-	+	-
 * _REMOVE_HEAD			+	-	+	-
 * _REMOVE			+	+	+	+
 * _SWAP			+	+	+	+
 *
 */
#ifdef UINET_QUEUE_MACRO_DEBUG
/* Store the last 2 places the queue element or head was altered */
struct uinet_qm_trace {
	unsigned long	 lastline;
	unsigned long	 prevline;
	const char	*lastfile;
	const char	*prevfile;
};

#define	UINET_TRACEBUF	struct uinet_qm_trace trace;
#define	UINET_TRACEBUF_INITIALIZER	{ __FILE__, __LINE__, NULL, 0 } ,
#define	UINET_TRASHIT(x)	do {(x) = (void *)-1;} while (0)
#define	UINET_QMD_SAVELINK(name, link)	void **name = (void *)&(link)

#define	UINET_QMD_TRACE_HEAD(head) do {					\
	(head)->trace.prevline = (head)->trace.lastline;		\
	(head)->trace.prevfile = (head)->trace.lastfile;		\
	(head)->trace.lastline = __LINE__;				\
	(head)->trace.lastfile = __FILE__;				\
} while (0)

#define	UINET_QMD_TRACE_ELEM(elem) do {					\
	(elem)->trace.prevline = (elem)->trace.lastline;		\
	(elem)->trace.prevfile = (elem)->trace.lastfile;		\
	(elem)->trace.lastline = __LINE__;				\
	(elem)->trace.lastfile = __FILE__;				\
} while (0)

#else
#define	UINET_QMD_TRACE_ELEM(elem)
#define	UINET_QMD_TRACE_HEAD(head)
#define	UINET_QMD_SAVELINK(name, link)
#define	UINET_TRACEBUF
#define	UINET_TRACEBUF_INITIALIZER
#define	UINET_TRASHIT(x)
#endif	/* UINET_QUEUE_MACRO_DEBUG */

/*
 * Singly-linked List declarations.
 */
#define	UINET_SLIST_HEAD(name, type)					\
struct name {								\
	struct type *slh_first;	/* first element */			\
}

#define	UINET_SLIST_HEAD_INITIALIZER(head)				\
	{ NULL }

#define	UINET_SLIST_ENTRY(type)						\
struct {								\
	struct type *sle_next;	/* next element */			\
}

/*
 * Singly-linked List functions.
 */
#define	UINET_SLIST_EMPTY(head)	((head)->slh_first == NULL)

#define	UINET_SLIST_FIRST(head)	((head)->slh_first)

#define	UINET_SLIST_FOREACH(var, head, field)				\
	for ((var) = UINET_SLIST_FIRST((head));				\
	    (var);							\
	    (var) = UINET_SLIST_NEXT((var), field))

#define	UINET_SLIST_FOREACH_FROM(var, head, field)			\
	for ((var) = ((var) ? (var) : UINET_SLIST_FIRST((head)));	\
	    (var);							\
	    (var) = UINET_SLIST_NEXT((var), field))

#define	UINET_SLIST_FOREACH_SAFE(var, head, field, tvar)		\
	for ((var) = UINET_SLIST_FIRST((head));				\
	    (var) && ((tvar) = UINET_SLIST_NEXT((var), field), 1);	\
	    (var) = (tvar))

#define	UINET_SLIST_FOREACH_FROM_SAFE(var, head, field, tvar)		\
	for ((var) = ((var) ? (var) : UINET_SLIST_FIRST((head)));	\
	    (var) && ((tvar) = UINET_SLIST_NEXT((var), field), 1);	\
	    (var) = (tvar))

#define	UINET_SLIST_FOREACH_PREVPTR(var, varp, head, field)		\
	for ((varp) = &UINET_SLIST_FIRST((head));			\
	    ((var) = *(varp)) != NULL;					\
	    (varp) = &UINET_SLIST_NEXT((var), field))

#define	UINET_SLIST_INIT(head) do {					\
	UINET_SLIST_FIRST((head)) = NULL;				\
} while (0)

#define	UINET_SLIST_INSERT_AFTER(slistelm, elm, field) do {		\
	UINET_SLIST_NEXT((elm), field) = UINET_SLIST_NEXT((slistelm), field);	\
	UINET_SLIST_NEXT((slistelm), field) = (elm);			\
} while (0)

#define	UINET_SLIST_INSERT_HEAD(head, elm, field) do {			\
	UINET_SLIST_NEXT((elm), field) = UINET_SLIST_FIRST((head));	\
	UINET_SLIST_FIRST((head)) = (elm);				\
} while (0)

#define	UINET_SLIST_NEXT(elm, field)	((elm)->field.sle_next)

#define	UINET_SLIST_REMOVE(head, elm, type, field) do {			\
	UINET_QMD_SAVELINK(oldnext, (elm)->field.sle_next);		\
	if (UINET_SLIST_FIRST((head)) == (elm)) {			\
		UINET_SLIST_REMOVE_HEAD((head), field);			\
	}								\
	else {								\
		struct type *curelm = UINET_SLIST_FIRST((head));	\
		while (UINET_SLIST_NEXT(curelm, field) != (elm))	\
			curelm = UINET_SLIST_NEXT(curelm, field);	\
		UINET_SLIST_REMOVE_AFTER(curelm, field);		\
	}								\
	UINET_TRASHIT(*oldnext);					\
} while (0)

#define UINET_SLIST_REMOVE_AFTER(elm, field) do {			\
	UINET_SLIST_NEXT(elm, field) =					\
	    UINET_SLIST_NEXT(UINET_SLIST_NEXT(elm, field), field);	\
} while (0)

#define	UINET_SLIST_REMOVE_HEAD(head, field) do {			\
	UINET_SLIST_FIRST((head)) = UINET_SLIST_NEXT(UINET_SLIST_FIRST((head)), field);	\
} while (0)

#define UINET_SLIST_SWAP(head1, head2, type) do {			\
	struct type *swap_first = UINET_SLIST_FIRST(head1);		\
	UINET_SLIST_FIRST(head1) = UINET_SLIST_FIRST(head2);		\
	UINET_SLIST_FIRST(head2) = swap_first;				\
} while (0)

/*
 * Singly-linked Tail queue declarations.
 */
#define	UINET_STAILQ_HEAD(name, type)					\
struct name {								\
	struct type *stqh_first;/* first element */			\
	struct type **stqh_last;/* addr of last next element */		\
}

#define	UINET_STAILQ_HEAD_INITIALIZER(head)				\
	{ NULL, &(head).stqh_first }

#define	UINET_STAILQ_ENTRY(type)					\
struct {								\
	struct type *stqe_next;	/* next element */			\
}

/*
 * Singly-linked Tail queue functions.
 */
#define	UINET_STAILQ_CONCAT(head1, head2) do {				\
	if (!UINET_STAILQ_EMPTY((head2))) {				\
		*(head1)->stqh_last = (head2)->stqh_first;		\
		(head1)->stqh_last = (head2)->stqh_last;		\
		UINET_STAILQ_INIT((head2));				\
	}								\
} while (0)

#define	UINET_STAILQ_EMPTY(head)	((head)->stqh_first == NULL)

#define	UINET_STAILQ_FIRST(head)	((head)->stqh_first)

#define	UINET_STAILQ_FOREACH(var, head, field)				\
	for((var) = UINET_STAILQ_FIRST((head));				\
	   (var);							\
	   (var) = UINET_STAILQ_NEXT((var), field))

#define	UINET_STAILQ_FOREACH_FROM(var, head, field)			\
	for ((var) = ((var) ? (var) : UINET_STAILQ_FIRST((head)));	\
	   (var);							\
	   (var) = UINET_STAILQ_NEXT((var), field))

#define	UINET_STAILQ_FOREACH_SAFE(var, head, field, tvar)		\
	for ((var) = UINET_STAILQ_FIRST((head));			\
	    (var) && ((tvar) = UINET_STAILQ_NEXT((var), field), 1);	\
	    (var) = (tvar))

#define	UINET_STAILQ_FOREACH_FROM_SAFE(var, head, field, tvar)		\
	for ((var) = ((var) ? (var) : UINET_STAILQ_FIRST((head)));	\
	    (var) && ((tvar) = UINET_STAILQ_NEXT((var), field), 1);	\
	    (var) = (tvar))

#define	UINET_STAILQ_INIT(head) do {					\
	UINET_STAILQ_FIRST((head)) = NULL;				\
	(head)->stqh_last = &UINET_STAILQ_FIRST((head));		\
} while (0)

#define	UINET_STAILQ_INSERT_AFTER(head, tqelm, elm, field) do {		\
	if ((UINET_STAILQ_NEXT((elm), field) = UINET_STAILQ_NEXT((tqelm), field)) == NULL)\
		(head)->stqh_last = &UINET_STAILQ_NEXT((elm), field);	\
	UINET_STAILQ_NEXT((tqelm), field) = (elm);			\
} while (0)

#define	UINET_STAILQ_INSERT_HEAD(head, elm, field) do {			\
	if ((UINET_STAILQ_NEXT((elm), field) = UINET_STAILQ_FIRST((head))) == NULL)	\
		(head)->stqh_last = &UINET_STAILQ_NEXT((elm), field);	\
	UINET_STAILQ_FIRST((head)) = (elm);				\
} while (0)

#define	UINET_STAILQ_INSERT_TAIL(head, elm, field) do {			\
	UINET_STAILQ_NEXT((elm), field) = NULL;				\
	*(head)->stqh_last = (elm);					\
	(head)->stqh_last = &UINET_STAILQ_NEXT((elm), field);		\
} while (0)

#define	UINET_STAILQ_LAST(head, type, field)				\
	(UINET_STAILQ_EMPTY((head)) ? NULL :				\
	    __uinet_containerof((head)->stqh_last, struct type, field.stqe_next))

#define	UINET_STAILQ_NEXT(elm, field)	((elm)->field.stqe_next)

#define	UINET_STAILQ_REMOVE(head, elm, type, field) do {		\
	UINET_QMD_SAVELINK(oldnext, (elm)->field.stqe_next);		\
	if (UINET_STAILQ_FIRST((head)) == (elm)) {			\
		UINET_STAILQ_REMOVE_HEAD((head), field);		\
	}								\
	else {								\
		struct type *curelm = UINET_STAILQ_FIRST((head));	\
		while (UINET_STAILQ_NEXT(curelm, field) != (elm))	\
			curelm = UINET_STAILQ_NEXT(curelm, field);	\
		UINET_STAILQ_REMOVE_AFTER(head, curelm, field);		\
	}								\
	UINET_TRASHIT(*oldnext);					\
} while (0)

#define UINET_STAILQ_REMOVE_AFTER(head, elm, field) do {		\
	if ((UINET_STAILQ_NEXT(elm, field) =				\
	     UINET_STAILQ_NEXT(UINET_STAILQ_NEXT(elm, field), field)) == NULL)	\
		(head)->stqh_last = &UINET_STAILQ_NEXT((elm), field);	\
} while (0)

#define	UINET_STAILQ_REMOVE_HEAD(head, field) do {			\
	if ((UINET_STAILQ_FIRST((head)) =				\
	     UINET_STAILQ_NEXT(UINET_STAILQ_FIRST((head)), field)) == NULL)	\
		(head)->stqh_last = &UINET_STAILQ_FIRST((head));	\
} while (0)

#define UINET_STAILQ_SWAP(head1, head2, type) do {			\
	struct type *swap_first = UINET_STAILQ_FIRST(head1);		\
	struct type **swap_last = (head1)->stqh_last;			\
	UINET_STAILQ_FIRST(head1) = UINET_STAILQ_FIRST(head2);		\
	(head1)->stqh_last = (head2)->stqh_last;			\
	UINET_STAILQ_FIRST(head2) = swap_first;				\
	(head2)->stqh_last = swap_last;					\
	if (UINET_STAILQ_EMPTY(head1))					\
		(head1)->stqh_last = &UINET_STAILQ_FIRST(head1);	\
	if (UINET_STAILQ_EMPTY(head2))					\
		(head2)->stqh_last = &UINET_STAILQ_FIRST(head2);	\
} while (0)


/*
 * List declarations.
 */
#define	UINET_LIST_HEAD(name, type)					\
struct name {								\
	struct type *lh_first;	/* first element */			\
}

#define	UINET_LIST_HEAD_INITIALIZER(head)				\
	{ NULL }

#define	UINET_LIST_ENTRY(type)						\
struct {								\
	struct type *le_next;	/* next element */			\
	struct type **le_prev;	/* address of previous next element */	\
}

/*
 * List functions.
 */

#if (defined(_KERNEL) && defined(INVARIANTS))
#define	UINET_QMD_LIST_CHECK_HEAD(head, field) do {			\
	if (UINET_LIST_FIRST((head)) != NULL &&				\
	    UINET_LIST_FIRST((head))->field.le_prev !=			\
	     &UINET_LIST_FIRST((head)))					\
		panic("Bad list head %p first->prev != head", (head));	\
} while (0)

#define	UINET_QMD_LIST_CHECK_NEXT(elm, field) do {			\
	if (UINET_LIST_NEXT((elm), field) != NULL &&			\
	    UINET_LIST_NEXT((elm), field)->field.le_prev !=		\
	     &((elm)->field.le_next))					\
	     	panic("Bad link elm %p next->prev != elm", (elm));	\
} while (0)

#define	UINET_QMD_LIST_CHECK_PREV(elm, field) do {			\
	if (*(elm)->field.le_prev != (elm))				\
		panic("Bad link elm %p prev->next != elm", (elm));	\
} while (0)
#else
#define	UINET_QMD_LIST_CHECK_HEAD(head, field)
#define	UINET_QMD_LIST_CHECK_NEXT(elm, field)
#define	UINET_QMD_LIST_CHECK_PREV(elm, field)
#endif /* (_KERNEL && INVARIANTS) */

#define	UINET_LIST_EMPTY(head)	((head)->lh_first == NULL)

#define	UINET_LIST_FIRST(head)	((head)->lh_first)

#define	UINET_LIST_FOREACH(var, head, field)				\
	for ((var) = UINET_LIST_FIRST((head));				\
	    (var);							\
	    (var) = UINET_LIST_NEXT((var), field))

#define	UINET_LIST_FOREACH_FROM(var, head, field)			\
	for ((var) = ((var) ? (var) : UINET_LIST_FIRST((head)));	\
	    (var);							\
	    (var) = UINET_LIST_NEXT((var), field))

#define	UINET_LIST_FOREACH_SAFE(var, head, field, tvar)			\
	for ((var) = UINET_LIST_FIRST((head));				\
	    (var) && ((tvar) = UINET_LIST_NEXT((var), field), 1);	\
	    (var) = (tvar))

#define	UINET_LIST_FOREACH_FROM_SAFE(var, head, field, tvar)		\
	for ((var) = ((var) ? (var) : UINET_LIST_FIRST((head)));	\
	    (var) && ((tvar) = UINET_LIST_NEXT((var), field), 1);	\
	    (var) = (tvar))

#define	UINET_LIST_INIT(head) do {					\
	UINET_LIST_FIRST((head)) = NULL;				\
} while (0)

#define	UINET_LIST_INSERT_AFTER(listelm, elm, field) do {		\
	UINET_QMD_LIST_CHECK_NEXT(listelm, field);			\
	if ((UINET_LIST_NEXT((elm), field) = UINET_LIST_NEXT((listelm), field)) != NULL)\
		UINET_LIST_NEXT((listelm), field)->field.le_prev =	\
		    &UINET_LIST_NEXT((elm), field);			\
	UINET_LIST_NEXT((listelm), field) = (elm);			\
	(elm)->field.le_prev = &UINET_LIST_NEXT((listelm), field);	\
} while (0)

#define	UINET_LIST_INSERT_BEFORE(listelm, elm, field) do {		\
	UINET_QMD_LIST_CHECK_PREV(listelm, field);			\
	(elm)->field.le_prev = (listelm)->field.le_prev;		\
	UINET_LIST_NEXT((elm), field) = (listelm);			\
	*(listelm)->field.le_prev = (elm);				\
	(listelm)->field.le_prev = &UINET_LIST_NEXT((elm), field);	\
} while (0)

#define	UINET_LIST_INSERT_HEAD(head, elm, field) do {			\
	UINET_QMD_LIST_CHECK_HEAD((head), field);			\
	if ((UINET_LIST_NEXT((elm), field) = UINET_LIST_FIRST((head))) != NULL)	\
		UINET_LIST_FIRST((head))->field.le_prev = &UINET_LIST_NEXT((elm), field);\
	UINET_LIST_FIRST((head)) = (elm);				\
	(elm)->field.le_prev = &UINET_LIST_FIRST((head));		\
} while (0)

#define	UINET_LIST_NEXT(elm, field)	((elm)->field.le_next)

#define	UINET_LIST_PREV(elm, head, type, field)				\
	((elm)->field.le_prev == &UINET_LIST_FIRST((head)) ? NULL :	\
	    __uinet_containerof((elm)->field.le_prev, struct type, field.le_next))

#define	UINET_LIST_REMOVE(elm, field) do {				\
	UINET_QMD_SAVELINK(oldnext, (elm)->field.le_next);		\
	UINET_QMD_SAVELINK(oldprev, (elm)->field.le_prev);		\
	UINET_QMD_LIST_CHECK_NEXT(elm, field);				\
	UINET_QMD_LIST_CHECK_PREV(elm, field);				\
	if (UINET_LIST_NEXT((elm), field) != NULL)			\
		UINET_LIST_NEXT((elm), field)->field.le_prev = 		\
		    (elm)->field.le_prev;				\
	*(elm)->field.le_prev = UINET_LIST_NEXT((elm), field);		\
	UINET_TRASHIT(*oldnext);					\
	UINET_TRASHIT(*oldprev);					\
} while (0)

#define UINET_LIST_SWAP(head1, head2, type, field) do {			\
	struct type *swap_tmp = UINET_LIST_FIRST((head1));		\
	UINET_LIST_FIRST((head1)) = UINET_LIST_FIRST((head2));		\
	UINET_LIST_FIRST((head2)) = swap_tmp;				\
	if ((swap_tmp = UINET_LIST_FIRST((head1))) != NULL)		\
		swap_tmp->field.le_prev = &UINET_LIST_FIRST((head1));	\
	if ((swap_tmp = UINET_LIST_FIRST((head2))) != NULL)		\
		swap_tmp->field.le_prev = &UINET_LIST_FIRST((head2));	\
} while (0)

/*
 * Tail queue declarations.
 */
#define	UINET_TAILQ_HEAD(name, type)					\
struct name {								\
	struct type *tqh_first;	/* first element */			\
	struct type **tqh_last;	/* addr of last next element */		\
	UINET_TRACEBUF							\
}

#define	UINET_TAILQ_HEAD_INITIALIZER(head)				\
	{ NULL, &(head).tqh_first, UINET_TRACEBUF_INITIALIZER }

#define	UINET_TAILQ_ENTRY(type)						\
struct {								\
	struct type *tqe_next;	/* next element */			\
	struct type **tqe_prev;	/* address of previous next element */	\
	UINET_TRACEBUF							\
}

/*
 * Tail queue functions.
 */
#if (defined(_KERNEL) && defined(INVARIANTS))
#define	UINET_QMD_TAILQ_CHECK_HEAD(head, field) do {			\
	if (!UINET_TAILQ_EMPTY(head) &&					\
	    UINET_TAILQ_FIRST((head))->field.tqe_prev !=		\
	     &UINET_TAILQ_FIRST((head)))				\
		panic("Bad tailq head %p first->prev != head", (head));	\
} while (0)

#define	UINET_QMD_TAILQ_CHECK_TAIL(head, field) do {			\
	if (*(head)->tqh_last != NULL)					\
	    	panic("Bad tailq NEXT(%p->tqh_last) != NULL", (head)); 	\
} while (0)

#define	UINET_QMD_TAILQ_CHECK_NEXT(elm, field) do {			\
	if (UINET_TAILQ_NEXT((elm), field) != NULL &&			\
	    UINET_TAILQ_NEXT((elm), field)->field.tqe_prev !=		\
	     &((elm)->field.tqe_next))					\
		panic("Bad link elm %p next->prev != elm", (elm));	\
} while (0)

#define	UINET_QMD_TAILQ_CHECK_PREV(elm, field) do {			\
	if (*(elm)->field.tqe_prev != (elm))				\
		panic("Bad link elm %p prev->next != elm", (elm));	\
} while (0)
#else
#define	UINET_QMD_TAILQ_CHECK_HEAD(head, field)
#define	UINET_QMD_TAILQ_CHECK_TAIL(head, headname)
#define	UINET_QMD_TAILQ_CHECK_NEXT(elm, field)
#define	UINET_QMD_TAILQ_CHECK_PREV(elm, field)
#endif /* (_KERNEL && INVARIANTS) */

#define	UINET_TAILQ_CONCAT(head1, head2, field) do {			\
	if (!UINET_TAILQ_EMPTY(head2)) {				\
		*(head1)->tqh_last = (head2)->tqh_first;		\
		(head2)->tqh_first->field.tqe_prev = (head1)->tqh_last;	\
		(head1)->tqh_last = (head2)->tqh_last;			\
		UINET_TAILQ_INIT((head2));				\
		UINET_QMD_TRACE_HEAD(head1);				\
		UINET_QMD_TRACE_HEAD(head2);				\
	}								\
} while (0)

#define	UINET_TAILQ_EMPTY(head)	((head)->tqh_first == NULL)

#define	UINET_TAILQ_FIRST(head)	((head)->tqh_first)

#define	UINET_TAILQ_FOREACH(var, head, field)				\
	for ((var) = UINET_TAILQ_FIRST((head));				\
	    (var);							\
	    (var) = UINET_TAILQ_NEXT((var), field))

#define	UINET_TAILQ_FOREACH_FROM(var, head, field)			\
	for ((var) = ((var) ? (var) : UINET_TAILQ_FIRST((head)));	\
	    (var);							\
	    (var) = UINET_TAILQ_NEXT((var), field))

#define	UINET_TAILQ_FOREACH_SAFE(var, head, field, tvar)		\
	for ((var) = UINET_TAILQ_FIRST((head));				\
	    (var) && ((tvar) = UINET_TAILQ_NEXT((var), field), 1);	\
	    (var) = (tvar))

#define	UINET_TAILQ_FOREACH_FROM_SAFE(var, head, field, tvar)		\
	for ((var) = ((var) ? (var) : UINET_TAILQ_FIRST((head)));	\
	    (var) && ((tvar) = UINET_TAILQ_NEXT((var), field), 1);	\
	    (var) = (tvar))

#define	UINET_TAILQ_FOREACH_REVERSE(var, head, headname, field)		\
	for ((var) = UINET_TAILQ_LAST((head), headname);		\
	    (var);							\
	    (var) = UINET_TAILQ_PREV((var), headname, field))

#define	UINET_TAILQ_FOREACH_REVERSE_FROM(var, head, headname, field)	\
	for ((var) = ((var) ? (var) : UINET_TAILQ_LAST((head), headname));	\
	    (var);							\
	    (var) = UINET_TAILQ_PREV((var), headname, field))

#define	UINET_TAILQ_FOREACH_REVERSE_SAFE(var, head, headname, field, tvar)	\
	for ((var) = UINET_TAILQ_LAST((head), headname);			\
	    (var) && ((tvar) = UINET_TAILQ_PREV((var), headname, field), 1);	\
	    (var) = (tvar))

#define	UINET_TAILQ_FOREACH_REVERSE_FROM_SAFE(var, head, headname, field, tvar) \
	for ((var) = ((var) ? (var) : UINET_TAILQ_LAST((head), headname));	\
	    (var) && ((tvar) = UINET_TAILQ_PREV((var), headname, field), 1);	\
	    (var) = (tvar))

#define	UINET_TAILQ_INIT(head) do {					\
	UINET_TAILQ_FIRST((head)) = NULL;				\
	(head)->tqh_last = &UINET_TAILQ_FIRST((head));			\
	UINET_QMD_TRACE_HEAD(head);					\
} while (0)

#define	UINET_TAILQ_INSERT_AFTER(head, listelm, elm, field) do {	\
	UINET_QMD_TAILQ_CHECK_NEXT(listelm, field);			\
	if ((UINET_TAILQ_NEXT((elm), field) = UINET_TAILQ_NEXT((listelm), field)) != NULL)\
		UINET_TAILQ_NEXT((elm), field)->field.tqe_prev = 	\
		    &UINET_TAILQ_NEXT((elm), field);			\
	else {								\
		(head)->tqh_last = &UINET_TAILQ_NEXT((elm), field);	\
		UINET_QMD_TRACE_HEAD(head);				\
	}								\
	UINET_TAILQ_NEXT((listelm), field) = (elm);			\
	(elm)->field.tqe_prev = &UINET_TAILQ_NEXT((listelm), field);	\
	UINET_QMD_TRACE_ELEM(&(elm)->field);				\
	UINET_QMD_TRACE_ELEM(&listelm->field);				\
} while (0)

#define	UINET_TAILQ_INSERT_BEFORE(listelm, elm, field) do {		\
	UINET_QMD_TAILQ_CHECK_PREV(listelm, field);			\
	(elm)->field.tqe_prev = (listelm)->field.tqe_prev;		\
	UINET_TAILQ_NEXT((elm), field) = (listelm);			\
	*(listelm)->field.tqe_prev = (elm);				\
	(listelm)->field.tqe_prev = &UINET_TAILQ_NEXT((elm), field);	\
	UINET_QMD_TRACE_ELEM(&(elm)->field);				\
	UINET_QMD_TRACE_ELEM(&listelm->field);				\
} while (0)

#define	UINET_TAILQ_INSERT_HEAD(head, elm, field) do {			\
	UINET_QMD_TAILQ_CHECK_HEAD(head, field);			\
	if ((UINET_TAILQ_NEXT((elm), field) = UINET_TAILQ_FIRST((head))) != NULL)	\
		UINET_TAILQ_FIRST((head))->field.tqe_prev =		\
		    &UINET_TAILQ_NEXT((elm), field);			\
	else								\
		(head)->tqh_last = &UINET_TAILQ_NEXT((elm), field);	\
	UINET_TAILQ_FIRST((head)) = (elm);				\
	(elm)->field.tqe_prev = &UINET_TAILQ_FIRST((head));		\
	UINET_QMD_TRACE_HEAD(head);					\
	UINET_QMD_TRACE_ELEM(&(elm)->field);				\
} while (0)

#define	UINET_TAILQ_INSERT_TAIL(head, elm, field) do {			\
	UINET_QMD_TAILQ_CHECK_TAIL(head, field);			\
	UINET_TAILQ_NEXT((elm), field) = NULL;				\
	(elm)->field.tqe_prev = (head)->tqh_last;			\
	*(head)->tqh_last = (elm);					\
	(head)->tqh_last = &UINET_TAILQ_NEXT((elm), field);		\
	UINET_QMD_TRACE_HEAD(head);					\
	UINET_QMD_TRACE_ELEM(&(elm)->field);				\
} while (0)

#define	UINET_TAILQ_LAST(head, headname)				\
	(*(((struct headname *)((head)->tqh_last))->tqh_last))

#define	UINET_TAILQ_NEXT(elm, field) ((elm)->field.tqe_next)

#define	UINET_TAILQ_PREV(elm, headname, field)				\
	(*(((struct headname *)((elm)->field.tqe_prev))->tqh_last))

#define	UINET_TAILQ_REMOVE(head, elm, field) do {			\
	UINET_QMD_SAVELINK(oldnext, (elm)->field.tqe_next);		\
	UINET_QMD_SAVELINK(oldprev, (elm)->field.tqe_prev);		\
	UINET_QMD_TAILQ_CHECK_NEXT(elm, field);				\
	UINET_QMD_TAILQ_CHECK_PREV(elm, field);				\
	if ((UINET_TAILQ_NEXT((elm), field)) != NULL)			\
		UINET_TAILQ_NEXT((elm), field)->field.tqe_prev = 	\
		    (elm)->field.tqe_prev;				\
	else {								\
		(head)->tqh_last = (elm)->field.tqe_prev;		\
		UINET_QMD_TRACE_HEAD(head);				\
	}								\
	*(elm)->field.tqe_prev = UINET_TAILQ_NEXT((elm), field);	\
	UINET_TRASHIT(*oldnext);					\
	UINET_TRASHIT(*oldprev);					\
	UINET_QMD_TRACE_ELEM(&(elm)->field);				\
} while (0)

#define UINET_TAILQ_SWAP(head1, head2, type, field) do {		\
	struct type *swap_first = (head1)->tqh_first;			\
	struct type **swap_last = (head1)->tqh_last;			\
	(head1)->tqh_first = (head2)->tqh_first;			\
	(head1)->tqh_last = (head2)->tqh_last;				\
	(head2)->tqh_first = swap_first;				\
	(head2)->tqh_last = swap_last;					\
	if ((swap_first = (head1)->tqh_first) != NULL)			\
		swap_first->field.tqe_prev = &(head1)->tqh_first;	\
	else								\
		(head1)->tqh_last = &(head1)->tqh_first;		\
	if ((swap_first = (head2)->tqh_first) != NULL)			\
		swap_first->field.tqe_prev = &(head2)->tqh_first;	\
	else								\
		(head2)->tqh_last = &(head2)->tqh_first;		\
} while (0)

#endif /* !_UINET_QUEUE_H_ */
