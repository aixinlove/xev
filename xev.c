//
//  xev.c
//  xhttpd
//
//  Created by Peng Yunchou on 13-9-7.
//  Copyright (c) 2013年 Peng Yunchou. All rights reserved.
//
#include "xev.h"
#include <stdio.h>
#include "xlualib.h"

#pragma mark -
#pragma mark lua
LUA_API int l_xev_queue_new(lua_State* L);
LUA_API int l_xev_queue_add(lua_State* L);
LUA_API int l_xev_queue_del(lua_State* L);
LUA_API int l_xev_queue_wait(lua_State* L);
LUA_API int l_xev_queue_destroy(lua_State* L);
LUA_API int l_xev_queue_write(lua_State *L);
LUA_API int l_xev_queue_get(lua_State* L);
LUA_API int l_xev_queue_destroy(lua_State* L);

LUA_API int l_xev_queue_add(lua_State* L){
    struct xev_queue *queue=lua_touserdata(L, 1);
    struct xevent_parameter *e=lua_touserdata(L, 2);
    lua_pushinteger(L, xev_queue_add(queue, e));
    return 1;
}
LUA_API int l_xev_queue_del(lua_State* L){
    struct xev_queue *queue=lua_touserdata(L, 1);
    struct xevent_parameter *e=lua_touserdata(L, 2);
    lua_pushinteger(L, xev_queue_del(queue, e));
    return 1;
}
LUA_API int l_xev_queue_write(lua_State *L){
    struct xev_queue *queue=lua_touserdata(L, 1);
    struct xevent_parameter *e=lua_touserdata(L, 2);
    int enable=luaL_checkint(L, 3);
    lua_pushnumber(L, xev_queue_write(queue, e, enable));
    return 1;
}
LUA_API int l_xev_queue_poll(lua_State* L){
    struct xev_queue *queue=lua_touserdata(L, 1);
    long timeout=luaL_checklong(L, 2);
    lua_pushinteger(L, xev_queue_poll(queue,timeout));
    return 1;
}
LUA_API int l_xev_queue_destroy(lua_State* L){
    struct xev_queue *queue=lua_touserdata(L, 1);
    lua_pushinteger(L, xev_queue_destroy(queue));
    return 1;
};
LUA_API int l_xev_queue_get(lua_State* L){
    struct xev_queue *queue=lua_touserdata(L, 1);
    int index=luaL_checkint(L, 2);
    index=index-1;//lua start with 1
    if (index<0||index>XEVENT_MAX) {
        lua_pushnil(L);
    }else{
        //printf("get event index:%d\n",index);
        struct x_rw_ev *e=&queue->rw_evs[index];
        lua_newtable(L);
        l_pushtablenumber(L, "fd", e->fd);
        l_pushtablenumber(L, "read", e->read);
        l_pushtablenumber(L, "write", e->write);
    }
    return 1;
}

LUA_API int l_xev_queue_new(lua_State* L){
    struct xev_queue *queue=xev_queue_new();
    if (queue==NULL) {
        lua_pushnil(L);
        return 1;
    }else{
        lua_pushlightuserdata(L, queue);
        return 1;
    }
}
LUA_API int l_xev_event_parameter_new(lua_State* L){
    int fd=luaL_checkint(L, 1);
    struct xevent_parameter *ev=malloc(sizeof(struct xevent_parameter));
    memset(ev, 0x0, sizeof(struct xevent_parameter));
    ev->fd=fd;
    lua_pushlightuserdata(L, ev);
    //printf("create ev parameter.\n");
    return 1;
}
LUA_API int l_xev_event_parameter_destroy(lua_State* L){
    struct xevent_parameter *ev=lua_touserdata(L, 1);
    free(ev);
    //printf("free ev parameter.\n");
    return 0;
}
static luaL_Reg xev_lib_methods[]={
    {"new",l_xev_queue_new},
    {"add",l_xev_queue_add},
    {"del",l_xev_queue_del},
    {"poll",l_xev_queue_poll},
    {"write",l_xev_queue_write},
    {"get",l_xev_queue_get},
    {"destroy",l_xev_queue_destroy},
    {NULL,NULL}
};
static luaL_Reg xevent_parameter_lib_methods[]={
    {"new",l_xev_event_parameter_new},
    {"destroy",l_xev_event_parameter_destroy},
    {NULL,NULL}
};
LUALIB_API int luaopen_xevent_parameter(lua_State *L){
    luaL_newlib(L, xevent_parameter_lib_methods);
    return 1;
}
LUALIB_API int luaopen_xev (lua_State *L){
    luaL_requiref(L, "xev.eventparameter", luaopen_xevent_parameter, 0);
    lua_pop(L, 1);
    luaL_newlib(L, xev_lib_methods);
    return 1;
}