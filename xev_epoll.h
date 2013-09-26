//
//  xev_epoll.h
//  xhttpd
//
//  Created by Peng Yunchou on 13-9-7.
//  Copyright (c) 2013年 Peng Yunchou. All rights reserved.
//

#ifndef xhttpd_xev_epoll_h
#define xhttpd_xev_epoll_h
#include <netdb.h>
#include <unistd.h>
#include <sys/epoll.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <fcntl.h>
#include <string.h>
struct xev_queue * xev_queue_new(){
    int pollfd=epoll_create(1024);
    if (pollfd>0) {
        struct xev_queue *queue=malloc(sizeof(struct xev_queue));
        memset(queue,0x0,sizeof(struct xev_queue));
        queue->fd=pollfd;
        return queue;
    }else{
        return NULL;
    }
}
int xev_queue_add(struct xev_queue *queue,struct xevent_parameter *e){
    struct epoll_event ev;
	ev.events = EPOLLIN;
	ev.data.ptr = e;
	if (epoll_ctl(queue->fd, EPOLL_CTL_ADD, e->fd, &ev) == -1) {
		return -1;
	}
	return 0;
}
int xev_queue_del(struct xev_queue *queue,struct xevent_parameter *e){
    epoll_ctl(queue->fd, EPOLL_CTL_DEL, e->fd , NULL);
    return 0;
}
int xev_queue_write(struct xev_queue *queue,struct xevent_parameter *e,int enable){
    struct epoll_event ev;
	ev.events = EPOLLIN | (enable ? EPOLLOUT : 0);
	ev.data.ptr = e;
	epoll_ctl(queue->fd, EPOLL_CTL_MOD, e->fd, &ev);
}
int xev_queue_poll(struct xev_queue *queue,long timeout){
    struct epoll_event ev[XEVENT_MAX];
	int n = epoll_wait(queue->fd , ev, XEVENT_MAX, timeout);
	int i;
	for (i=0;i<n;i++) {        
        struct x_rw_ev *rwe=&(queue->rw_evs[i]);
		unsigned flag = ev[i].events;
		int write= (flag & EPOLLOUT) != 0;
		int read = (flag & EPOLLIN) != 0;
        struct xevent_parameter *xe=ev[i].data.ptr;
        rwe->fd=xe->fd;
        //printf("set event index:%d,fd:%d\n",i,xe->fd);
        rwe->write=write;
        rwe->read=read;
	}
	return n;
}
int xev_queue_destroy(struct xev_queue *queue){
    int ret=close(queue->fd);
    free(queue);
    return ret;
}
#endif
