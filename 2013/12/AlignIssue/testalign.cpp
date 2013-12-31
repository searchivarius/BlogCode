#include <signal.h>
#include <stdio.h>
#include <memory.h>
#include <unistd.h>

#include <iostream>
#include <algorithm>
#include <cmath>

#define CATCH_SIGBUS

using namespace std;

double dist(const double*x,const double*y,size_t qty){
  double res=0;
  for (size_t i = 0; i < qty; i++) 
    res += x[i] - y[i];
  return res;
}

// Uncomment this line to see which signal handler is activated.
//#ifdef CATCH_SIGBUS
#ifdef CATCH_SIGBUS
//Based http://stackoverflow.com/questions/13834643/catch-sigbus-in-c-and-c
void
seg_handler(int sig, siginfo_t *si, void *vuctx)
{
#if 1
        const char *msg = "SEG FAULT!\n";
        
        ssize_t res = write(STDERR_FILENO, msg, strlen(msg));
        res = res * res; // to shut up the warning
#else
        /*                                                                                                                           
         * This is a trick I sometimes use for debugging , this will                                                                 
         * be visible in strace while not messing with external state too                                                            
         * much except breaking errno.                                                                                                
         */
        write(-1, NULL, si->si_code);
#endif
        _exit(1);
}

void
bus_handler(int sig, siginfo_t *si, void *vuctx)
{
        const char *msg = "BUS FAULT!\n";
        
        ssize_t res = write(STDERR_FILENO, msg, strlen(msg));
        res = res * res; // to shut up the warning

        _exit(1);
}

#endif


int main(int,char**) {
#ifdef CATCH_SIGBUS
  {
    struct sigaction sa;

    memset(&sa, 0, sizeof(sa));
    sa.sa_sigaction = seg_handler;
    sa.sa_flags = SA_SIGINFO;
    sigfillset(&sa.sa_mask);
    sigaction(SIGSEGV, &sa, NULL);
  }

  {
   struct sigaction sa;

    memset(&sa, 0, sizeof(sa));
    sa.sa_sigaction = bus_handler;
    sa.sa_flags = SA_SIGINFO;
    sigfillset(&sa.sa_mask);
 
    sigaction(SIGBUS, &sa, NULL);
  }
#endif


  int N = 128;
  char *arr1 = new char[sizeof(double)*N*2 + 4 + 1024*N /* a lot to be safe */];
  char *arr2 = new char[sizeof(double)*N *2+ 4 + 1024*N /* a lot to be safe */];
  cout << "Ignore1: " << dist((double*)(arr1),(double*)(arr2), N)<<endl;;
  cout << "Ignore2: " << dist((double*)(arr1 + 4),(double*)(arr2+4), N)<<endl;;
  return 0;
};
