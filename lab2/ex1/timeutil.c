#include "timeutil.h"
#include <sys/times.h>
#include <unistd.h>

//variables used to measure duration of operations
static clock_t st_time;
static clock_t en_time;
static struct tms st_cpu;
static struct tms en_cpu;


void start_clock()
{
    st_time = times(&st_cpu);
}

void end_clock(double *results)
{
    en_time = times(&en_cpu);

    double clk = sysconf(_SC_CLK_TCK);

    results[0] = (en_time - st_time) / clk;
    results[1] = (en_cpu.tms_utime - st_cpu.tms_utime) / clk;
    results[2] = (en_cpu.tms_stime - st_cpu.tms_stime) / clk;
}