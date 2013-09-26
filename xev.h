//
//  xev.h
//  xhttpd
//
//  Created by Peng Yunchou on 13-9-7.
//  Copyright (c) 2013年 Peng Yunchou. All rights reserved.
//

#ifndef xhttpd_xev_h
#define xhttpd_xev_h

#include "lua.h"
#include "lauxlib.h"
LUALIB_API int luaopen_xev (lua_State *L);
#define XEVENT_MAX 1024
struct xevent_parameter {
    int fd;
    //char buffer[1024*1024*10];
};
struct x_rw_ev {
    int fd;
    int read;
    int write;
};
struct xev_queue{
    int fd;
    void *ud;
    struct x_rw_ev rw_evs[XEVENT_MAX];
};
struct xev_queue * xev_queue_new();
int xev_queue_add(struct xev_queue *ctx,struct xevent_parameter *e);
int xev_queue_del(struct xev_queue *ctx,struct xevent_parameter *e);
int xev_queue_write(struct xev_queue *ctx,struct xevent_parameter *e,int enable);
int xev_queue_poll(struct xev_queue *ctx,long timeout);
int xev_queue_destroy(struct xev_queue *ctx);
#ifdef __linux__
    #include "xev_epoll.h"
#endif

#if defined(__APPLE__) || defined(__FreeBSD__) || defined(__OpenBSD__) || defined (__NetBSD__)
    #include "xev_kqueue.h"
#endif

#endif
