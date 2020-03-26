/****************************************************************
 * file hcv_background.cc of https://github.com/bstarynk/helpcovid
 *
 * Description:
 *      Implementation of background processing
 *
 * Author(s):
 *      © Copyright 2020
 *      Basile Starynkevitch <basile@starynkevitch.net>
 *      Abhishek Chakravarti <abhishek@taranjali.org>
 *
 *
 * License:
 *    This HELPCOVID program is free software: you can redistribute it and/or modify
 *    it under the terms of the GNU General Public License as published by
 *    the Free Software Foundation, either version 3 of the License, or
 *    (at your option) any later version.
 *
 *    This program is distributed in the hope that it will be useful,
 *    but WITHOUT ANY WARRANTY; without even the implied warranty of
 *    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *    GNU General Public License for more details.
 *
 *    You should have received a copy of the GNU General Public License
 *    along with this program.  If not, see <http://www.gnu.org/licenses/>.
 ******************************************************************************/


#include "hcv_header.hh"


extern "C" const char hcv_background_gitid[] = HELPCOVID_GITID;
extern "C" const char hcv_background_date[] = __DATE__;


std::thread hcv_bgthread;
std::atomic<bool> hcv_should_stop_bg_thread;
int hcv_bg_event_fd;
int hcv_bg_signal_fd;

void hcv_process_SIGTERM_signal(void);
void hcv_process_SIGXCPU_signal(void);
void hcv_process_SIGHUP_signal(void);
void hcv_bg_do_event(int64_t); // handle one event on hcv_bg_event_fd

#define HCV_BACKGROUND_TICK_TIMEOUT 8192 /*milliseconds*/
void hcv_background_thread_body(void)
{
  char thnambuf[16];
  memset (&thnambuf, 0, sizeof(thnambuf));
  {
    snprintf(thnambuf, sizeof(thnambuf), "hcovibg%ld", (long)getpid());
    pthread_setname_np(pthread_self(), thnambuf);
    HCV_SYSLOGOUT(LOG_INFO, "hcv_background_thread_body starting thread " << thnambuf);
  }
  while (!hcv_should_stop_bg_thread.load())
    {
      struct pollfd polltab[4];
      memset(&polltab, 0, sizeof(polltab));
      polltab[0].fd = hcv_bg_event_fd;
      polltab[0].events = POLL_IN;
      polltab[1].fd = hcv_bg_signal_fd;
      polltab[1].events = POLL_IN;
      int nbfd = poll(polltab, 1, HCV_BACKGROUND_TICK_TIMEOUT);
      if (nbfd==0)   /* timedout */
        {
          HCV_SYSLOGOUT(LOG_INFO, "hcv_background_thread_body timed-out");
        }
      else if (nbfd>0)   /* some file descriptor is readable */
        {
          HCV_DEBUGOUT("hcv_background_thread_body: after poll nbfd:" << nbfd);
          if ((polltab[0].revents & POLL_IN) && polltab[0].fd == hcv_bg_event_fd)
            {
              int64_t evrk=0;
              int byrd = read (hcv_bg_event_fd, &evrk, sizeof(evrk));
              if (byrd==sizeof(evrk))
                {
                  HCV_DEBUGOUT("hcv_background_thread_body: got " << evrk
                               << " from hcv_bg_event_fd");
                  hcv_bg_do_event(evrk);
                }
              else
                HCV_SYSLOGOUT(LOG_WARNING,
                              "hcv_background_thread_body read hcv_bg_event_fd#" <<hcv_bg_event_fd << " failed, byrd=" << byrd);
            };
          if ((polltab[1].revents & POLL_IN) && polltab[1].fd == hcv_bg_signal_fd)
            {
              struct signalfd_siginfo signalinfo;
              memset (&signalinfo, 0, sizeof(signalinfo));
              int byrd = read(hcv_bg_signal_fd, &signalinfo, sizeof(signalinfo));
              if (byrd < 0)
                HCV_FATALOUT("hcv_background_thread_body: failed read of hcv_bg_signal_fd="
                             << hcv_bg_signal_fd);
              else if (byrd != sizeof(signalinfo))
                // should never happen... see signalfd(2)
                HCV_FATALOUT("hcv_background_thread_body: corrupted read of hcv_bg_signal_fd="
                             << hcv_bg_signal_fd << ", byrd=" << byrd);
              if (signalinfo.ssi_signo == SIGTERM)
                {
                  HCV_SYSLOGOUT(LOG_NOTICE, "hcv_background_thread_body got SIGTERM at "
                                << (hcv_monotonic_real_time() - hcv_monotonic_start_time)
                                << " elapsed seconds");
                  hcv_process_SIGTERM_signal();
                  hcv_should_stop_bg_thread.store (true);
                }
              else if  (signalinfo.ssi_signo == SIGHUP)
                {
                  HCV_SYSLOGOUT(LOG_NOTICE, "hcv_background_thread_body got SIGHUP at "
                                << (hcv_monotonic_real_time() - hcv_monotonic_start_time)
                                << " elapsed seconds");
                  hcv_process_SIGHUP_signal();
                }
              else if  (signalinfo.ssi_signo == SIGXCPU)
                {
                  HCV_SYSLOGOUT(LOG_NOTICE, "hcv_background_thread_body got SIGXCPU at "
                                << (hcv_monotonic_real_time() - hcv_monotonic_start_time)
                                << " elapsed seconds");
                  hcv_process_SIGXCPU_signal();
                  hcv_should_stop_bg_thread.store (true);
                }
              else
                HCV_FATALOUT("hcv_background_thread_body: unexpected signal #" << signalinfo.ssi_signo);
            };
        }
      else
        {
          HCV_FATALOUT("hcv_background_thread_body: poll failed");
        }
    }
  HCV_SYSLOGOUT(LOG_INFO, "hcv_background_thread_body ending thread " << thnambuf);
} // end hcv_background_thread_body


void
hcv_start_background_thread(void)
{
  hcv_bg_event_fd = eventfd(0, EFD_CLOEXEC | EFD_NONBLOCK);
  if (hcv_bg_event_fd < 0)
    HCV_FATALOUT("hcv_start_background_thread: eventfd failed for hcg_bg_event_fd");
  {
    sigset_t  sigmaskbits;
    memset (&sigmaskbits, 0, sizeof(sigmaskbits));
    sigemptyset(&sigmaskbits);
    sigaddset(&sigmaskbits, SIGTERM);
    sigaddset(&sigmaskbits, SIGHUP);
    sigaddset(&sigmaskbits, SIGXCPU);
    hcv_bg_signal_fd = signalfd(-1, &sigmaskbits, SFD_NONBLOCK|SFD_CLOEXEC);
    if (hcv_bg_signal_fd < 0)
      HCV_FATALOUT("hcv_start_background_thread: signalfd failure");
  }
  hcv_bgthread = std::thread([]()
  {
    hcv_background_thread_body();
  });
  HCV_DEBUGOUT("hcv_start_background_thread did start hcv_bgthread of id "
               << hcv_bgthread.get_id());
} // end of hcv_start_background_thread


void
hcv_stop_background_thread(void)
{
  if (hcv_bg_event_fd <= 0)
    HCV_FATALOUT("hcv_stop_background_thread: bad hcv_bg_event_fd=" << hcv_bg_event_fd);
  hcv_should_stop_bg_thread.store(true);
  int64_t one = 1;
  if (write(hcv_bg_event_fd, &one, sizeof(one)) != sizeof(one))
    HCV_FATALOUT("hcv_stop_background_thread: failed to write on hcg_bg_event_fd");
} // end hcv_stop_background_thread



/////////////////////////////// Unix signal processing thru signalfd(2)
////////// see http://man7.org/linux/man-pages/man7/signal.7.html
////////// and http://man7.org/linux/man-pages/man7/signal-safety.7.html
void
hcv_process_SIGTERM_signal(void)
{
  HCV_FATALOUT("hcv_process_SIGTERM_signal unimplemented");
#warning hcv_process_SIGTERM_signal unimplemented
  /* we should properly close the database */
} // end hcv_process_SIGTERM_signal


void
hcv_process_SIGXCPU_signal(void)
{
  HCV_FATALOUT("hcv_process_SIGXCPU_signal unimplemented");
#warning hcv_process_SIGXCPU_signal unimplemented
  /* we should try to close the database */
} // end hcv_process_SIGXCPU_signal


void
hcv_process_SIGHUP_signal(void)
{
  HCV_FATALOUT("hcv_process_SIGHUP_signal unimplemented");
#warning hcv_process_SIGHUP_signal unimplemented
  /* we want to flush the database */
} // end hcv_process_SIGHUP_signal


void
hcv_bg_do_event(int64_t ev)
{
  HCV_SYSLOGOUT(LOG_INFO, "hcv_bg_do_event unimplemented " << ev);
#warning hcv_bg_do_event unimplemented
  /*** we probably want to use some std::condition_variable etc, to
       manage a short TODO list, so that any thread could add closures into it.
  ****/
} // end hcv_bg_do_event



void
hcv_do_postpone_background(double delay,  const std::string&name, void*data,
                           const std::function<void(void*)>& todofun)
{
  if (delay<HCV_POSTPONE_MINIMAL_DELAY)
    delay = HCV_POSTPONE_MINIMAL_DELAY;
  else if (delay > HCV_POSTPONE_MAXIMAL_DELAY)
    delay = HCV_POSTPONE_MAXIMAL_DELAY;
  if (!todofun)
    HCV_FATALOUT("hcv_do_postpone_background missing todo: delay="  << delay
                 << ", name=" << name << ", data=" << data);
  HCV_FATALOUT("hcv_do_postpone_background unimplemented: delay="  << delay
               << ", name=" << name << ", data=" << data);
#warning unimplemented hcv_do_postpone_background
} // end hcv_do_postpone_background


/************************ end of file hcv_background.cc in github.com/bstarynk/helpcovid ***/
