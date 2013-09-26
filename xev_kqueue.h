//
//  xev_kqueue.h
//  xhttpd
//
//  Created by Peng Yunchou on 13-9-7.
//  Copyright (c) 2013年 Peng Yunchou. All rights reserved.
//

#ifndef xhttpd_xev_kqueue_h
#define xhttpd_xev_kqueue_h
#include <sys/event.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#pragma mark -
#pragma mark private api

struct xev_queue * xev_queue_new(){
    int pollfd=kqueue();
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
    struct kevent ke;
	EV_SET(&ke, e->fd, EVFILT_READ, EV_ADD, 0, 0, e);
	if (kevent(queue->fd, &ke, 1, NULL, 0, NULL) == -1) {
		return 1;
	}
	EV_SET(&ke, e->fd, EVFILT_WRITE, EV_ADD, 0, 0, e);
	if (kevent(queue->fd, &ke, 1, NULL, 0, NULL) == -1) {
		EV_SET(&ke, e->fd, EVFILT_READ, EV_DELETE, 0, 0, NULL);
		kevent(queue->fd, &ke, 1, NULL, 0, NULL);
		return -1;
	}
	EV_SET(&ke, e->fd, EVFILT_WRITE, EV_DISABLE, 0, 0, e);
	if (kevent(queue->fd, &ke, 1, NULL, 0, NULL) == -1) {
        printf("%s %s error\n",__FILE__,__FUNCTION__);
		xev_queue_del(queue, e);
		return 1;
	}
	return 0;
}
int xev_queue_del(struct xev_queue *queue,struct xevent_parameter *e){
    struct kevent ke;
	EV_SET(&ke, e->fd, EVFILT_READ, EV_DELETE, 0, 0, NULL);
	kevent(queue->fd, &ke, 1, NULL, 0, NULL);
	EV_SET(&ke, e->fd, EVFILT_WRITE, EV_DELETE, 0, 0, NULL);
	kevent(queue->fd, &ke, 1, NULL, 0, NULL);
    return 0;
}
int xev_queue_write(struct xev_queue *queue,struct xevent_parameter *e,int enable){
    struct kevent ke;
    if (enable==1) {
        EV_SET(&ke, e->fd, EVFILT_WRITE, EV_ENABLE , 0, 0, e);
        if (kevent(queue->fd, &ke, 1, NULL, 0, NULL)==-1) {
            char msg[1024];
            sprintf(msg, "%s %s\n",__FILE__,__FUNCTION__);
            perror(msg);
        }
    }else{
        EV_SET(&ke, e->fd, EVFILT_WRITE, EV_DISABLE , 0, 0, e);
        if (kevent(queue->fd, &ke, 1, NULL, 0, NULL)==-1) {
            char msg[1024];
            sprintf(msg, "%s %s\n",__FILE__,__FUNCTION__);
            perror(msg);
        }
    }
	
    return 0;
}
int xev_queue_poll(struct xev_queue *queue,long timeout){
    struct kevent ev[XEVENT_MAX];
    int n=0;
    if (timeout<0) {
        n = kevent(queue->fd, NULL, 0, ev, XEVENT_MAX, NULL);
    }else{
        struct timespec timeoutspec;
        timeoutspec.tv_sec = timeout / 1000;
        timeoutspec.tv_nsec = (timeout % 1000) * 1000000;
        n = kevent(queue->fd, NULL, 0, ev, XEVENT_MAX, &timeoutspec);
    }
	int i;
	for (i=0;i<n;i++) {
        struct x_rw_ev *rwe=&(queue->rw_evs[i]);
        struct xevent_parameter *xe=ev[i].udata;
        rwe->fd=xe->fd;
        unsigned filter = ev[i].filter;
        rwe->write = (filter == EVFILT_WRITE);
        rwe->read= (filter == EVFILT_READ);
	}
	return n;
}
int xev_queue_destroy(struct xev_queue *queue){
    int ret=close(queue->fd);
    free(queue);
    return ret;
}

#endif
