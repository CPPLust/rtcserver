
#include "ev.h"

#define MIN_INTERVAL 0.0001220703125  // 1/2**13, good till 4000
#define MIN_TIMEJUMP 1. /* minimum timejump that gets detected (if monotonic clock available) */
#define MAX_BLOCKTIME 59.743 /* never wait longer than this time (to detect time jumps) */
#define ev_floor(v) floor(v)
#define NUMPRI (EV_MAXPRI - EV_MINPRI + 1)



static void* ev_realloc_emul(void* ptr, size_t size)
{
    /* some systems, notably openbsd and darwin, fail to properly
     * implement realloc (x, 0) (as required by both ansi c-89 and
     * the single unix specification, so work around them here.
     * recently, also (at least) fedora and debian started breaking it,
     * despite documenting it otherwise.
     */
    if (size) {
        return realloc(ptr, size);
    }
    else {
        free(ptr);
        return 0;
    }
}

static void* ev_realloc(void* ptr, int size)
{
    ptr = ev_realloc_emul(ptr, (size_t)size);
    if (!ptr && size) {
        ev_printerr("(libev) memory allocation failed, aborting.\n");
        abort();
    }
    return ptr;
}

#define MALLOC_ROUND 4096
#define PTR_SIZE ((int) (sizeof(void *)))

static int array_nextsize(int elem, int cur, int cnt)
{
    int ncur = cur + 1;
    do {
        ncur <<= 1;
    } while (cnt > ncur);
    /* if size is large, round to MALLOC_ROUND - 4 * longs to accommodate malloc overhead */
    if (elem * ncur > MALLOC_ROUND - PTR_SIZE * 4) {
        ncur *= elem;
        ncur = (ncur + elem + (MALLOC_ROUND - 1) + PTR_SIZE * 4) & ~(MALLOC_ROUND - 1);
        ncur = ncur - PTR_SIZE * 4;
        ncur /= elem;
    }
    return ncur;
}

static void* array_realloc(int elem, void* base, int* cur, int cnt)
{
    *cur = array_nextsize(elem, *cur, cnt);
    return ev_realloc(base, elem * (*cur));
}

#define ev_malloc(size) ev_realloc(0, (size))
#define ev_free(ptr) ev_realloc((ptr), 0)
#define array_init_zero(base, count) memset((void *) (base), 0, sizeof(*(base)) * ((size_t) count))
#define array_needsize(type, base, cur, cnt, init)                                \
  if (expect_false((cnt) > (cur))) {                                              \
    int ocur_ = (cur);                                                            \
    (base) = (type *) array_realloc(((int) sizeof(type)), (base), &(cur), (cnt)); \
    init((base) + (ocur_), (cur) -ocur_);                                         \
  }
#define array_free(stem, idx)        \
  ev_free(stem##s idx);              \
  stem##cnt idx = stem##max idx = 0; \
  stem##s idx = 0

typedef struct ev_io
{
    EV_WATCHER_LIST(ev_io)
        int fd; /* ro */
    int events; /* ro */
} ev_io;

/* stores the pending event set for a given watcher */
typedef struct
{
    ev_watcher* w;
    int events; /* the pending event set for the given watcher */
    int dummy;
} ANPENDING;

#define EV_ANFD_REIFY 1
/* file descriptor info structure */
typedef struct
{
    ev_watcher_list* head;
    unsigned char events; /* the events watched for */
    unsigned char reify; /* flag set when this ANFD needs reification (EV_ANFD_REIFY, EV__IOFDSET) */
    unsigned char emask; /* the epoll backend stores the actual kernel mask in here */
    unsigned char unused;
#if EV_USE_EPOLL
    unsigned int egen; /* generation counter to counter epoll bugs */
#else
    unsigned int dummy;
#endif
} ANFD;

/* a heap element */
typedef struct
{
    ev_tstamp at;
    ev_watcher_time* w;
} ANHE;

#define ANHE_w(he) (he).w /* access watcher, read-write */
#define ANHE_at(he) (he).at /* access cached at, read-only */
#define ANHE_at_cache(he) (he).at = (he).w->at /* update at from watcher */

struct ev_loop
{
    ev_tstamp ev_rt_now;

    ev_tstamp now_floor; /* last time we refreshed rt_time */
    ev_tstamp mn_now; /* monotonic clock "now" */
    ev_tstamp rtmn_diff; /* difference realtime - monotonic time */

    /* for reverse feeding of events */
    ev_watcher** rfeeds;
    int rfeedmax;
    int rfeedcnt;

    ANPENDING* pendings[NUMPRI];
    int pendingmax[NUMPRI];
    int pendingcnt[NUMPRI];
    int pendingpri; /* highest priority currently pending */
    int dummy;
    ev_watcher pending_w; /* dummy pending watcher */

    ev_tstamp io_blocktime;
    ev_tstamp timeout_blocktime;

    int backend;
    int activecnt; /* total number of active events ("refcount") */
    EV_ATOMIC_T loop_done; /* signal by ev_break */

    int backend_fd;
    ev_tstamp backend_mintime; /* assumed typical timer resolution */
    void (*backend_modify)(struct ev_loop* loop, int fd, int oev, int nev);
    void (*backend_poll)(struct ev_loop* loop, ev_tstamp timeout);

    ANFD* anfds;
    int anfdmax;

    int evpipe[2];
    int postfork; /* true if we need to recreate kernel state after fork */
    ev_io pipe_w;
    EV_ATOMIC_T pipe_write_wanted;
    EV_ATOMIC_T pipe_write_skipped;

#if EV_USE_POLL
    struct pollfd* polls;
    int pollmax;
    int pollcnt;
    int* pollidxs; /* maps fds into structure indices */
    int pollidxmax;
    int dummy3;
#endif

#if EV_USE_EPOLL
    struct epoll_event* epoll_events;
    int epoll_eventmax;
    int dummy4;
    int* epoll_eperms;
    int epoll_epermcnt;
    int epoll_epermmax;
#endif

#if EV_USE_KQUEUE
    pid_t kqueue_fd_pid;
    struct kevent* kqueue_changes;
    int kqueue_changemax;
    int kqueue_changecnt;
    struct kevent* kqueue_events;
    int kqueue_eventmax;
    int dummy5;
#endif

    int* fdchanges;
    int fdchangemax;
    int fdchangecnt;

    ANHE* timers;
    int timermax;
    int timercnt;

    struct ev_prepare** prepares;
    int preparemax;
    int preparecnt;

    struct ev_check** checks;
    int checkmax;
    int checkcnt;

    EV_ATOMIC_T async_pending;
    int dummy6;
    ev_async** asyncs;
    int asyncmax;
    int asynccnt;

    unsigned int loop_count; /* total number of loop iterations/blocks */
    unsigned int loop_depth; /* #ev_run enters - #ev_run leaves */
};

static void pendingcb(struct ev_loop* loop, ev_watcher* w, int revents)
{
    (void)loop;
    (void)w;
    (void)revents;
}

/* called whenever the libev signal pipe */
/* got some events (signal, async) */
static void pipecb(struct ev_loop* loop, ev_io* iow, int revents)
{
    int i;
}

static void loop_init(struct ev_loop* loop, unsigned int flags)
{
    (void)flags;
    if (!loop->backend) {
        loop->ev_rt_now = ev_time();
        loop->mn_now = get_clock();
        loop->now_floor = loop->mn_now;
        loop->rtmn_diff = loop->ev_rt_now - loop->mn_now;

        loop->io_blocktime = 0.;
        loop->timeout_blocktime = 0.;
        loop->backend = 0;
        loop->backend_fd = -1;
        loop->async_pending = 0;
        loop->pipe_write_skipped = 0;
        loop->pipe_write_wanted = 0;
        loop->evpipe[0] = -1;
        loop->evpipe[1] = -1;
#if EV_USE_KQUEUE
        loop->backend = kqueue_init(loop);
#endif
#if EV_USE_EPOLL
        loop->backend = epoll_init(loop);
#endif
#if EV_USE_POLL
        loop->backend = poll_init(loop);
#endif
    }
    ev_init(&loop->pending_w, pendingcb);
    ev_init(&loop->pipe_w, pipecb);
    ev_set_priority(&loop->pipe_w, EV_MAXPRI);
}

static unsigned int ev_backend(struct ev_loop* loop)
{
    return (unsigned int)loop->backend;
}
struct ev_loop* ev_loop_new(unsigned int flags)
{
    struct ev_loop* loop = (struct ev_loop*)ev_malloc(sizeof(struct ev_loop));

    memset(loop, 0, sizeof(struct ev_loop));
    loop_init(loop, flags);

    if (ev_backend(loop)) {
        return loop;
    }

    ev_free(loop);
    return 0;
}




int ev_run(struct ev_loop* loop, int flags)
{
    (void)flags;
    ++loop->loop_depth;

#if QTNG_EV_ASSERT
    assert(("libev: ev_loop recursion during release detected", loop->loop_done != EVBREAK_RECURSE));
#endif

    loop->loop_done = EVBREAK_CANCEL;

    ev_invoke_pending(loop); /* in case we recurse, ensure ordering stays nice and clean */

    do {
#if EV_VERIFY
        ev_verify(loop);
#endif
        /* queue prepare watchers (and execute them) */
        if (loop->preparecnt) {
            queue_events(loop, (ev_watcher**)loop->prepares, loop->preparecnt, EV_PREPARE);
            ev_invoke_pending(loop);
        }
        /* we might have forked, so queue fork handlers */
        if (expect_false(loop->loop_done)) {
            break;
        }
        /* we might have forked, so reify kernel state if necessary */
        if (expect_false(loop->postfork)) {
            loop_fork(loop);
        }
        fd_reify(loop);

        /* calculate blocking time */
        {
            ev_tstamp waittime = 0.;
            ev_tstamp sleeptime = 0.;

            /* remember old timestamp for io_blocktime calculation */
            ev_tstamp prev_mn_now = loop->mn_now;

            /* update time to cancel out callback processing overhead */
            time_update(loop, 1e100);

            /* from now on, we want a pipe-wake-up */
            loop->pipe_write_wanted = 1;

            ECB_MEMORY_FENCE; /* make sure pipe_write_wanted is visible before we check for potential skips */

            if (expect_true(loop->activecnt && !loop->pipe_write_skipped)) {
                waittime = MAX_BLOCKTIME;

                if (loop->timercnt) {
                    ev_tstamp to = ANHE_at(loop->timers[HEAP0]) - loop->mn_now;
                    if (waittime > to) {
                        waittime = to;
                    }
                }

                /* don't let timeouts decrease the waittime below timeout_blocktime */
                if (expect_false(waittime < loop->timeout_blocktime)) {
                    waittime = loop->timeout_blocktime;
                }

                /* at this point, we NEED to wait, so we have to ensure */
                /* to pass a minimum nonzero value to the backend */
                if (expect_false(waittime < loop->backend_mintime)) {
                    waittime = loop->backend_mintime;
                }

                /* extra check because io_blocktime is commonly 0 */
                if (expect_false(loop->io_blocktime != 0.0)) {
                    sleeptime = loop->io_blocktime - (loop->mn_now - prev_mn_now);

                    if (sleeptime > waittime - loop->backend_mintime) {
                        sleeptime = waittime - loop->backend_mintime;
                    }

                    if (expect_true(sleeptime > 0.)) {
                        ev_sleep(sleeptime);
                        waittime -= sleeptime;
                    }
                }
            }

            ++loop->loop_count;
            assert((loop->loop_done = EVBREAK_RECURSE, 1)); /* assert for side effect */
            loop->backend_poll(loop, waittime);
            assert((loop->loop_done = EVBREAK_CANCEL, 1)); /* assert for side effect */

            loop->pipe_write_wanted = 0; /* just an optimisation, no fence needed */

            ECB_MEMORY_FENCE_ACQUIRE;
            if (loop->pipe_write_skipped) {
#if QTNG_EV_ASSERT
                assert(("libev: pipe_w not active, but pipe not written", ev_is_active(&loop->pipe_w)));
#endif
                ev_feed_event(loop, &loop->pipe_w, EV_CUSTOM);
            }

            /* update ev_rt_now, do magic */
            time_update(loop, waittime + sleeptime);
        }

        /* queue pending timers and reschedule them */
        timers_reify(loop); /* relative timers called last */

        /* queue check watchers, to be executed first */
        if (loop->checkcnt) {
            queue_events(loop, (ev_watcher**)loop->checks, loop->checkcnt, EV_CHECK);
        }

        ev_invoke_pending(loop);
    } while (expect_true(loop->activecnt && !loop->loop_done));

    if (loop->loop_done == EVBREAK_ONE) {
        loop->loop_done = EVBREAK_CANCEL;
    }

    --loop->loop_depth;
    return loop->activecnt;
}