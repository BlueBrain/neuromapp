#include <cerrno>
#include <system_error>

extern "C" {
#include <time.h>
#include <signal.h>
}

#define THROW_SYSERR throw std::system_error(errno,std::system_category())

namespace llc {

static void clear_wait_flag(int sig,siginfo_t *si,void *context) {
    auto flagp=(volatile sig_atomic_t *)si->si_value.sival_ptr;
    *flagp=0;
}


/** Helper class for busy_wait()
 *
 * Encapsulates timer and signal state for RAII and safe
 * restoration in the event of an error.
 */

struct busy_wait_impl {
    struct sigaction old_action, *oa_ptr=0;
    int notify_signal;

    bool timer_id_set=false;
    timer_t timer_id;

    volatile sig_atomic_t flag;

    explicit busy_wait_impl(int signum): notify_signal(signum) {}

    void run(const timespec &until) {
        int rc=0;
        struct sigaction sa;

        sa.sa_sigaction=clear_wait_flag;
        sa.sa_flags=SA_SIGINFO;
        sigfillset(&sa.sa_mask);
        rc=sigaction(notify_signal,&sa,&old_action);
        if (rc<0) THROW_SYSERR;

        oa_ptr=&old_action;

        sigevent ev;
        ev.sigev_notify=SIGEV_SIGNAL;
        ev.sigev_signo=notify_signal;
        ev.sigev_value.sival_ptr=(void *)&flag;

        rc=timer_create(CLOCK_MONOTONIC,&ev,&timer_id);
        if (rc<0) THROW_SYSERR;
        timer_id_set=true;
    
        flag=1;
        itimerspec nv={{0,0},until};
        rc=timer_settime(timer_id,TIMER_ABSTIME,&nv,0);
        if (rc<0) THROW_SYSERR;

        while (flag) ;
        restore();
    }
        
    void restore() {
        if (timer_id_set) {
            timer_delete(timer_id);
            timer_id_set=false;
        }

        if (oa_ptr) {
            sigaction(notify_signal,oa_ptr,0);
            oa_ptr=0;
        }
    }

    ~busy_wait_impl() { restore(); }
};
    

double busy_wait(double dt,int notify_signal) {
    busy_wait_impl G(notify_signal);

    timespec t0,t1;
    int rc=0;

    rc=clock_gettime(CLOCK_MONOTONIC,&t0);
    if (rc<0) THROW_SYSERR;

    long long t1_nsec=t0.tv_sec*1000000000+t0.tv_nsec+(long long)(1.e9*dt);
    t1.tv_sec=t1_nsec/1000000000;
    t1.tv_nsec=t1_nsec%1000000000;

    G.run(t1);
    
    rc=clock_gettime(CLOCK_MONOTONIC,&t1);
    if (rc<0) THROW_SYSERR;
    
    return t1.tv_sec-t0.tv_sec+1.e-9*(t1.tv_nsec-t0.tv_nsec);
}

double busy_wait(double dt) { return busy_wait(dt,SIGRTMIN); }

} // namespace llc
