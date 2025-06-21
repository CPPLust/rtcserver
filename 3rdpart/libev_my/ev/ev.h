#ifndef MY_EV_FUNC_H_H
#define MY_EV_FUNC_H_H

#ifdef __cplusplus
extern "C" {
#endif

#include <signal.h>

typedef double ev_tstamp;

/* flag bits for ev_default_loop and ev_loop_new */
enum {
    EVFLAG_AUTO = 0x00000000U, /* not quite a mask */
    EVFLAG_NOENV = 0x01000000U, /* do NOT consult environment */
};

enum {
    EVBREAK_CANCEL = 0, /* undo unloop */
    EVBREAK_ONE = 1, /* unloop once */
    EVBREAK_ALL = 2 /* unloop all loops */
};

struct ev_loop;

#define EV_MINPRI -2
#define EV_MAXPRI +2
#define EV_ATOMIC_T sig_atomic_t volatile

#define EV_CB_DECLARE(type) void (*cb)(struct ev_loop * loop, struct type * w, int revents);

/* shared by all watchers */
#define EV_WATCHER(type)      \
  int active; /* private */   \
  int pending; /* private */  \
  int priority; /* private */ \
  int dummy;                  \
  void *data; /* rw */        \
  EV_CB_DECLARE(type) /* private */

#define EV_WATCHER_LIST(type) \
  EV_WATCHER(type)            \
  struct ev_watcher_list *next; /* private */

#define EV_WATCHER_TIME(type) \
  EV_WATCHER(type)            \
  ev_tstamp at; /* private */


/* base class, nothing to see here unless you subclass */
typedef struct ev_watcher
{
    EV_WATCHER(ev_watcher)
} ev_watcher;

/* base class, nothing to see here unless you subclass */
typedef struct ev_watcher_list
{
    EV_WATCHER_LIST(ev_watcher_list)
} ev_watcher_list;

/* base class, nothing to see here unless you subclass */
typedef struct ev_watcher_time
{
    EV_WATCHER_TIME(ev_watcher_time)
} ev_watcher_time;


typedef struct ev_async
{
    EV_WATCHER(ev_async)
    EV_ATOMIC_T sent; /* private */
} ev_async;

struct ev_loop* ev_loop_new(unsigned int flags);

void ev_loop_destroy(struct ev_loop* loop);
int ev_run(struct ev_loop* loop, int flags);
void ev_break(struct ev_loop* loop, int how);





#ifdef __cplusplus
}
#endif

#endif