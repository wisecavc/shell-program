/*
 * The exit module is responsible for cleaning and exiting the shell
 * whenever bigshell_exit() is called. It relies on jobs to pass the
 * SIGHUP signal to every active job by using the jobs_cleanup() function
 * so that the job list and objects are freed. Also runs vars_cleanup()
 * from the vars module for the same purpose, and terminates the active
 * shell with a call to exit().
 */

/* XXX DO NOT MODIFY THIS FILE XXX */
#define _POSIX_C_SOURCE 200809L
#include <signal.h>
#include <stdlib.h>

#include "exit.h"
#include "jobs.h"
#include "params.h"
#include "vars.h"

/** cleans up and exits the shell
 */
void
bigshell_exit(void)
{
  /* Send SIGHUP (Hangup) signal to all jobs */
  size_t job_count = jobs_get_joblist_size();
  struct job const *jobs = jobs_get_joblist();
  for (size_t i = 0; i < job_count; ++i) {
    pid_t pgid = jobs[i].pgid;
    kill(-pgid, SIGHUP);
  }

  /* Call associated cleanup routines */
  jobs_cleanup();
  vars_cleanup();
  exit(params.status);
}
