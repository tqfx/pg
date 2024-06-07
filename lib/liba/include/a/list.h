/*!
 @file list.h
 @brief circular doubly linked list implementation
*/

#ifndef LIBA_LIST_H
#define LIBA_LIST_H

#include "a.h"

/*!
 @ingroup liba
 @addtogroup a_list circular doubly linked list
 @{
*/

// clang-format off
#define A_LIST_INIT(node) {&(node), &(node)}
// clang-format on

/*!
 @brief instance structure for circular doubly linked list
*/
typedef struct a_list
{
    struct a_list *next, *prev;
} a_list;

/*!
 @brief cast a list pointer from another type pointer
 @param[in] _ additional attributes of specified type
 @param[in] x points to circular doubly linked list
 @return a pointer to circular doubly linked list
*/
#define a_list_(_, x) a_cast_s(a_list _, a_cast_s(void _, x))

/*!
 @brief access the struct for this entry
 @param ptr the &a_list pointer
 @param type the type of the struct this is embedded in
 @param member the name of the a_list within the struct
*/
#define a_list_entry(ptr, type, member) a_container_of(ptr, type, member)
#define a_list_entry_next(ptr, type, member) a_list_entry((ptr)->next, type, member)
#define a_list_entry_prev(ptr, type, member) a_list_entry((ptr)->prev, type, member)

/*!
 @brief iterate over a list
 @param it the &a_list to use as a loop counter
 @param ctx points to circular doubly linked list
 @param next the direction of loop iteration
  @arg next the backward iteration
  @arg prev the forward iteration
*/
#define a_list_foreach_(it, ctx, next) \
    for (a_list *it = (ctx)->next; it != (ctx); it = it->next)
#define a_list_foreach_next(it, ctx) a_list_foreach_(it, ctx, next)
#define a_list_foreach_prev(it, ctx) a_list_foreach_(it, ctx, prev)

/*!
 @brief iterate over a list safe against removal of list entry
 @param it the &a_list to use as a loop counter
 @param at another &a_list to use as temporary storage
 @param ctx points to circular doubly linked list
 @param next the direction of loop iteration
  @arg next the backward iteration
  @arg prev the forward iteration
*/
#define a_list_forsafe_(it, at, ctx, next) \
    for (a_list *it = (ctx)->next, *at = it->next; it != (ctx); it = at, at = it->next)
#define a_list_forsafe_next(it, at, ctx) a_list_forsafe_(it, at, ctx, next)
#define a_list_forsafe_prev(it, at, ctx) a_list_forsafe_(it, at, ctx, prev)

/*!
 @brief constructor for circular doubly linked list
 @param[in,out] ctx points to circular doubly linked list
*/
A_INTERN void a_list_ctor(a_list *ctx) { ctx->prev = ctx->next = ctx; }

/*!
 @brief initialize for circular doubly linked list
 @param[in,out] ctx points to circular doubly linked list
*/
A_INTERN void a_list_init(a_list *ctx) { ctx->prev = ctx->next = ctx; }

/*!
 @brief destructor for circular doubly linked list
 @param[in,out] ctx points to circular doubly linked list
*/
A_INTERN void a_list_dtor(a_list *ctx) { ctx->prev = ctx->next = ctx; }

/*!
 @brief link head node and tail node
 @dot
 digraph a_list_link {
     node[shape="record"]
     nodehead[label="{<prev>prev|<addr>head|<next>next}"]
     nodetail[label="{<prev>prev|<addr>tail|<next>next}"]
     nodehead:next -> nodetail:addr [color=green]
     nodetail:prev -> nodehead:addr [color=green]
     nodehead -> "..." -> nodetail
 }
 @enddot
 @param[in,out] head the head node of a list
 @param[in,out] tail the tail node of a list
*/
A_INTERN void a_list_link(a_list *head, a_list *tail)
{
    head->next = tail;
    tail->prev = head;
}

/*!
 @brief loop head node to tail node
 @dot
 digraph a_list_loop {
     node[shape="record"]
     nodehead[label="{<prev>prev|<addr>head|<next>next}"]
     nodetail[label="{<prev>prev|<addr>tail|<next>next}"]
     nodehead:prev -> nodetail:addr [color=green]
     nodetail:next -> nodehead:addr [color=green]
     nodehead -> "..." -> nodetail
 }
 @enddot
 @param[in,out] head the head node of a list
 @param[in,out] tail the tail node of a list
*/
A_INTERN void a_list_loop(a_list *head, a_list *tail)
{
    head->prev = tail;
    tail->next = head;
}

/*!
 @brief connect list1 and list2
 @dot
 digraph a_list_add_ {
     node[shape="record"]
     subgraph cluster0 {
         head1[label="<prev>prev|<addr>head1|<next>next"]
         tail1[label="<prev>prev|<addr>tail1|<next>next"]
     }
     subgraph cluster1 {
         head2[label="<prev>prev|<addr>head2|<next>next"]
         tail2[label="<prev>prev|<addr>tail2|<next>next"]
     }
     tail1:next -> head2:addr [color=green]
     head2:prev -> tail1:addr [color=green]
     tail2:next -> head1:addr [color=blue]
     head1:prev -> tail2:addr [color=blue]
 }
 @enddot
 @param[in,out] head1 the head node of the list1
 @param[in,out] tail1 the tail node of the list1
 @param[in,out] head2 the head node of the list2
 @param[in,out] tail2 the tail node of the list2
*/
A_INTERN void a_list_add_(a_list *head1, a_list *tail1, a_list *head2, a_list *tail2)
{
    a_list_link(tail1, head2);
    a_list_link(tail2, head1);
}

/*!
 @brief insert a node to a list
 @dot
 digraph a_list_add_node {
     node[shape="record"]
     subgraph cluster0 {
         0[label="<node>node"]
         1[label="<head>head|<tail>tail"]
     }
     subgraph cluster1 {
         2[label="<head>head|<tail>tail|<node>node"]
     }
     1 -> 2
 }
 @enddot
 @param[in,out] head the head node of a list
 @param[in,out] tail the tail node of a list
 @param[in] node a circular doubly linked list node
*/
A_INTERN void a_list_add_node(a_list *head, a_list *tail, a_list *node)
{
    a_list_add_(head, tail, node, node);
}

/*!
 @brief append a node to a list forward
 @dot
 digraph a_list_add_next {
     node[shape="record"]
     subgraph cluster0 {
         0[label="<0>0"]
         1[label="<prev>prev|<node>node|<next>next"]
     }
     subgraph cluster1 {
         2[label="<prev>prev|<node>node|<next>next|<0>0"]
     }
     1 -> 2
 }
 @enddot
 @param[in,out] ctx points to circular doubly linked list
 @param[in] node a circular doubly linked list node
*/
A_INTERN void a_list_add_next(a_list *ctx, a_list *node)
{
    a_list_add_(ctx->next, ctx, node, node);
}

/*!
 @brief append a node to a list backward
 @dot
 digraph a_list_add_prev {
     node[shape="record"]
     subgraph cluster0 {
         1[label="<prev>prev|<node>node|<next>next"]
         0[label="<0>0"]
     }
     subgraph cluster1 {
         2[label="<0>0|<prev>prev|<node>node|<next>next"]
     }
     1 -> 2
 }
 @enddot
 @param[in,out] ctx points to circular doubly linked list
 @param[in] node a circular doubly linked list node
*/
A_INTERN void a_list_add_prev(a_list *ctx, a_list *node)
{
    a_list_add_(ctx, ctx->prev, node, node);
}

/*!
 @brief delete a section of a list
 @dot
 digraph a_list_del_ {
     node[shape="record"]
     head[label="<prev>prev|<addr>head-prev|<next>next"]
     tail[label="<prev>prev|<addr>tail-next|<next>next"]
     nodehead[label="<prev>prev|<addr>head|<next>next"]
     nodetail[label="<prev>prev|<addr>tail|<next>next"]
     head:addr -> nodehead:addr -> "..." -> nodetail:addr -> tail:addr [dir=both]
     head:next -> tail:addr [color=green]
     tail:prev -> head:addr [color=green]
 }
 @enddot
 @param[in] head the head node of a list
 @param[in] tail the tail node of a list
*/
A_INTERN void a_list_del_(a_list const *head, a_list const *tail)
{
    a_list_link(head->prev, tail->next);
}

/*!
 @brief delete a node from a list
 @dot
 digraph a_list_del_node {
     node[shape="record"]
     subgraph cluster0 {
         1[label="<head>head|<node>node|<tail>tail"]
     }
     subgraph cluster1 {
         0[label="<node>node"]
         2[label="<head>head|<tail>tail"]
     }
     1 -> 2
 }
 @enddot
 @param[in] node a circular doubly linked list node
*/
A_INTERN void a_list_del_node(a_list const *node) { a_list_del_(node, node); }

/*!
 @brief remove a node from a list forward
 @dot
 digraph a_list_del_next {
     node[shape="record"]
     subgraph cluster0 {
         1[label="<prev>prev|<node>node|<next>next|<0>0"]
     }
     subgraph cluster1 {
         0[label="<0>0"]
         2[label="<prev>prev|<node>node|<next>next"]
     }
     1 -> 2
 }
 @enddot
 @param[in] node a circular doubly linked list node
*/
A_INTERN void a_list_del_next(a_list const *node) { a_list_del_(node->next, node->next); }

/*!
 @brief remove a node from a list backward
 @dot
 digraph a_list_del_prev {
     node[shape="record"]
     subgraph cluster0 {
         1[label="<0>0|<prev>prev|<node>node|<next>next"]
     }
     subgraph cluster1 {
         2[label="<prev>prev|<node>node|<next>next"]
         0[label="<0>0"]
     }
     1 -> 2
 }
 @enddot
 @param[in] node a circular doubly linked list node
*/
A_INTERN void a_list_del_prev(a_list const *node) { a_list_del_(node->prev, node->prev); }

/*!
 @brief modify a section of a list
 @dot
 digraph a_list_set_ {
     node[shape="record"]
     subgraph cluster0 {
         1[label="<prev>prev|<head>head|<tail>tail|<next>next"]
     }
     subgraph cluster1 {
         2[label="<prev>prev|<head>head|<tail>tail|<next>next"]
     }
     1:prev -> 2:head [color=green]
     2:tail -> 1:next [color=green]
 }
 @enddot
 @param[in] head1 the head node of the list1
 @param[in] tail1 the tail node of the list1
 @param[in,out] head2 the head node of the list2
 @param[in,out] tail2 the tail node of the list2
*/
A_INTERN void a_list_set_(a_list const *head1, a_list const *tail1, a_list *head2, a_list *tail2)
{
    a_list_add_(tail1->next, head1->prev, head2, tail2);
}

/*!
 @brief modify a node of a list
 @param[in] ctx the current node
 @param[in,out] rhs the new node
*/
A_INTERN void a_list_set_node(a_list const *ctx, a_list *rhs)
{
    a_list_add_(ctx->next, ctx->prev, rhs, rhs);
}

/*!
 @brief moving a list from another list forward
 @dot
 digraph a_list_mov_next {
     node[shape="record"]
     subgraph cluster0 {
         0[label="<0>0|<1>1"]
         1[label="<prev>prev|<node>node|<next>next"]
     }
     subgraph cluster1 {
         2[label="<prev>prev|<node>node|<next>next|<0>0|<1>1"]
     }
     1 -> 2
 }
 @enddot
 @param[in,out] ctx points to circular doubly linked list
 @param[in] rhs another circular doubly linked list
*/
A_INTERN void a_list_mov_next(a_list *ctx, a_list const *rhs)
{
    a_list_add_(ctx->next, ctx, rhs->next, rhs->prev);
}

/*!
 @brief moving a list from another list backward
 @dot
 digraph a_list_mov_prev {
     node[shape="record"]
     subgraph cluster0 {
         0[label="<0>0|<1>1"]
         1[label="<prev>prev|<node>node|<next>next"]
     }
     subgraph cluster1 {
         2[label="<0>0|<1>1|<prev>prev|<node>node|<next>next"]
     }
     1 -> 2
 }
 @enddot
 @param[in,out] ctx points to circular doubly linked list
 @param[in] rhs another circular doubly linked list
*/
A_INTERN void a_list_mov_prev(a_list *ctx, a_list const *rhs)
{
    a_list_add_(ctx, ctx->prev, rhs->next, rhs->prev);
}

/*!
 @brief rotate a node in the list backward
 @dot
 digraph a_list_rot_next {
     node[shape="record"]
     subgraph cluster0 {
         1[label="<0>0|<1>1|<2>2|<3>3|<prev>prev|<node>node|<next>next"]
     }
     subgraph cluster1 {
         2[label="<0>0|<1>1|<2>2|<prev>prev|<node>node|<next>next|<3>3"]
     }
     1:prev -> 1:3 [color=green]
     1:next -> 1:0 [color=green]
     2:prev -> 2:2 [color=blue]
     2:next -> 2:3 [color=blue]
     1 -> 2
 }
 @enddot
 @param[in,out] ctx points to circular doubly linked list
*/
A_INTERN void a_list_rot_next(a_list *ctx)
{
    a_list *const node = ctx->prev;
    a_list_del_(node, node);
    a_list_add_(ctx->next, ctx, node, node);
}

/*!
 @brief rotate a node in the list forward
 @dot
 digraph a_list_rot_prev {
     node[shape="record"]
     subgraph cluster0 {
         1[label="<prev>prev|<node>node|<next>next|<0>0|<1>1|<2>2|<3>3"]
     }
     subgraph cluster1 {
         2[label="<0>0|<prev>prev|<node>node|<next>next|<1>1|<2>2|<3>3"]
     }
     1:prev -> 1:3 [color=green]
     1:next -> 1:0 [color=green]
     2:prev -> 2:0 [color=blue]
     2:next -> 2:1 [color=blue]
     1 -> 2
 }
 @enddot
 @param[in,out] ctx points to circular doubly linked list
*/
A_INTERN void a_list_rot_prev(a_list *ctx)
{
    a_list *const node = ctx->next;
    a_list_del_(node, node);
    a_list_add_(ctx, ctx->prev, node, node);
}

/*!
 @brief swap a section of one list and a section of another list
 @dot
 digraph a_list_swap_ {
     node[shape="record"]
     subgraph cluster0 {
         1[label="<prev>prev|<head>head|<tail>tail|<next>next"]
     }
     subgraph cluster1 {
         2[label="<prev>prev|<head>head|<tail>tail|<next>next"]
     }
     1:prev -> 2:head [color=green]
     2:tail -> 1:next [color=green]
     2:prev -> 1:head [color=blue]
     1:tail -> 2:next [color=blue]
 }
 @enddot
 @param[in,out] head1 the head node of the list1
 @param[in,out] tail1 the tail node of the list1
 @param[in,out] head2 the head node of the list2
 @param[in,out] tail2 the tail node of the list2
*/
A_INTERN void a_list_swap_(a_list *head1, a_list *tail1, a_list *head2, a_list *tail2)
{
    a_list *const head = tail2->next, *const tail = head2->prev;
    a_list_add_(tail1->next, head1->prev, head2, tail2);
    a_list_add_(head, tail, head1, tail1);
}

/*!
 @brief swap the node lhs and the node rhs
 @param[in,out] lhs the node on the left
 @param[in,out] rhs the node on the right
*/
A_INTERN void a_list_swap_node(a_list *lhs, a_list *rhs)
{
    a_list_swap_(lhs, lhs, rhs, rhs);
}

/*! @} a_list */

#endif /* a/list.h */
