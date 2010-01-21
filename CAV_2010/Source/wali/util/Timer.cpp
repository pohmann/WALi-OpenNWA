#include <iomanip>
#include <iostream>
#include "wali/Common.hpp"

#include "wali/util/Timer.hpp"

#ifndef _WIN32
clock_t sum_tms( const struct tms& t ) {
  clock_t c = t.tms_utime 
    + t.tms_stime + t.tms_cutime + t.tms_cstime;
  return c;
}
#endif

namespace wali {

  namespace util {

    bool Timer::measureAndReport = true;
#ifndef _WIN32
    const long Timer::TIMER_CLK_TICK = sysconf(_SC_CLK_TCK);
#endif


    Timer::Timer(const std::string &task, std::ostream& os)
      : start(clock()), task(task), os(os)
    {
#ifndef _WIN32
      times(&st_tms);
#endif
    }

    Timer::~Timer()
    {
      if (measureAndReport)
        print( os );
    }

    double Timer::elapsed() const
    {

#ifndef _WIN32
      struct tms end_tms;
      clock_t now = times(&end_tms);
      if( now == -1 ) {
        *waliErr << "[WARNING] times() returned an error.\n";
        return 0;
      }
      else {
        clock_t t = sum_tms(end_tms) - sum_tms(st_tms);
        return (t * 1.0) / (double)(TIMER_CLK_TICK);
      }
#else
      return 0;
#endif
    }

    std::ostream& Timer::print( std::ostream& out ) const
    {
      if (0 != task.length()) {
        const double difference = elapsed();
        out << task << ": " << difference << " secs" << std::endl;
      }
      return out;
    }

    std::ostream& Timer::printTime(std::ostream& out, clock_t clk ) {
      return out << (clk * 1.0 / (double)(CLOCKS_PER_SEC));
    }

  } // namespace util

} // namespace wali

