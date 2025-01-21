/*
 * The wait module involves the direction of process groups and
 * jobs to the foreground, where the main process waits for them
 * to be completed before being allowed to proceed. Functions
 * found in wait that relate to directing process groups and jobs
 * to the foreground are utilized in the runner and builtins modules,
 * respectively. Furthermore, the wait module has a function that
 * examines jobs running in the background, and checks if any have
 * changed states or not every time a new command is passed to the
 * shell. This function involving backgrounds exists in the main
 * loop in the bigshell module.
 */

// Collaborator: Seth Mackovjak
// Collaborator: Robin Yi

#define _POSIX_C_SOURCE 200809L
#include <assert.h>
#include <err.h>
#include <errno.h>
#include <stdint.h>
#include <stdio.h>
#include <sys/wait.h>
#include <unistd.h>

#include "jobs.h"
#include "params.h"
#include "parser.h"
#include "wait.h"

int
wait_on_fg_pgid(pid_t const pgid)
{
  if (pgid < 0) return -1;
  jid_t const jid = jobs_get_jid(pgid);
  if (jid < 0) return -1;
  /* Make sure the foreground group is running */
  /* TODO send the "continue" signal to the process group 'pgid'
   * XXX review kill(2)
   */
  
  /* BEGIN IMPLEMENTATION BY wisecavc*/

  // SIGCONT is sent to every process in the process group pgid
  if (kill(-pgid, SIGCONT) < 0) {
    // raises error if kill returns error
    return -1;
  }

  /* END IMPLEMENTATION BY wisecavc*/

  if (is_interactive) {
    /* TODO make 'pgid' the foreground process group
     * XXX review tcsetpgrp(3) */

    /* BEGIN IMPLEMENTATION BY wisecavc*/

    // makes the process group defined in pgid the foreground process group
    if (tcsetpgrp(0, pgid) < 0) {
      // raises error if needed
      return -1;
    }

    /* END IMPLEMENTATION BY wisecavc*/

  }

  /* XXX From this point on, all exit paths must account for setting bigshell
   * back to the foreground process group--no naked return statements */
  int retval = 0;

  /* XXX Notice here we loop until ECHILD and we use the status of
   * the last child process that terminated (in the previous iteration).
   * Consider a pipeline,
   *        cmd1 | cmd2 | cmd3
   *
   * We will loop exactly 4 times, once for each child process, and a
   * fourth time to see ECHILD.
   */
  for (;;) {
    /* Wait on ALL processes in the process group 'pgid' */
    int status;

    /* BEGIN IMPLEMENTATION BY wisecavc*/
    
    /*
     * waits for any process whose pgid is equal to abs(pgid)
     * res = pid of child process whose state was changed
     */
    pid_t res = waitpid(-pgid, &status, 0);

    /* END IMPLEMENTATION BY wisecavc*/
    
    if (res < 0) {
      /* Error occurred (some errors are ok, see below)
       *
       * XXX status may have a garbage value, use last_status from the
       * previous loop iteration */
      if (errno == ECHILD) {
        /* No unwaited-for children. The job is done! */
        errno = 0;
        if (jobs_get_status(jid, &status) < 0) goto err;
        if (WIFEXITED(status)) {
          /* TODO set params.status to the correct value */

          /* BEGIN IMPLEMENTATION BY wisecavc*/

          // sets status of params.status to exit status
          params.status = WEXITSTATUS(status);

          /* END IMPLEMENTATION BY wisecavc*/

        } else if (WIFSIGNALED(status)) {
          /* TODO set params.status to the correct value */

          /* BEGIN IMPLEMENTATION BY wisecavc*/

          // sets status of params.status to terminating signal
          params.status = WTERMSIG(status);

          /* END IMPLEMENTATION BY wisecavc*/

        }

        /* TODO remove the job for this group from the job list
         *  see jobs.h
         */

        /* BEGIN IMPLEMENTATION BY wisecavc*/

        if (jobs_remove_pgid(pgid) < 0) {
          return -1;
        }

        /* END IMPLEMENTATION BY wisecavc*/

        goto out;
      }
      goto err; /* An actual error occurred */
    }
    assert(res > 0);
    /* status is valid */

    /* Record status for reporting later when we see ECHILD */
    if (jobs_set_status(jid, status) < 0) goto err;

    /* TODO handle case where a child process is stopped
     *  The entire process group is placed in the background
     */

    /* BEGIN IMPLEMENTATION BY wisecavc*/

    // res greater than 0, implying a child process has stopped
    if (res > 0) {

    /* END IMPLEMENTATION BY wisecavc*/

      fprintf(stderr, "[%jd] Stopped\n", (intmax_t)jid);
      goto out;
    }

    /* A child exited, but others remain. Loop! */
  }

out:
  if (0) {
  err:
    retval = -1;
  }

  if (is_interactive) {
    /* TODO make bigshell the foreground process group again
     * XXX review tcsetpgrp(3)
     *
     * Note: this will cause bigshell to receive a SIGTTOU signal.
     *       You need to also finish signal.c to have full functionality here.
     *       Otherwise you bigshell will get stopped.
     */

    /* BEGIN IMPLEMENTATION BY wisecavc*/

    // makes the pid of calling process the foreground process group
    if (tcsetpgrp(0, getpid()) < 0) {
      // raises error if needed
      return -1;
    }

    /* END IMPLEMENTATION BY wisecavc*/

  }

  return retval;
}

/* XXX DO NOT MODIFY XXX */
int
wait_on_fg_job(jid_t jid)
{
  pid_t pgid = jobs_get_pgid(jid);
  if (pgid < 0) return -1;
  return wait_on_fg_pgid(pgid);
}

int
wait_on_bg_jobs()
{
  size_t job_count = jobs_get_joblist_size();
  struct job const *jobs = jobs_get_joblist();
  for (size_t i = 0; i < job_count; ++i) {
    pid_t pgid = jobs[i].pgid;
    jid_t jid = jobs[i].jid;
    for (;;) {
      /* TODO: Modify the following line to wait for process group
       * XXX make sure to do a nonblocking wait!
       */

      int status;

      /* BEGIN IMPLEMENTATION BY wisecavc*/

      /*
       * waits for child processes that have process group id's euqal to abs. pgid
       */

      pid_t pid = waitpid(pgid, &status, WNOHANG);

      /* END IMPLEMENTATION BY wisecavc*/

      if (pid == 0) {
        /* Unwaited children that haven't exited */
        break;
      } else if (pid < 0) {
        /* Error -- some errors are ok though! */
        if (errno == ECHILD) {
          /* No children -- print saved exit status */
          errno = 0;
          if (jobs_get_status(jid, &status) < 0) return -1;
          if (WIFEXITED(status)) {
            fprintf(stderr, "[%jd] Done\n", (intmax_t)jid);
          } else if (WIFSIGNALED(status)) {
            fprintf(stderr, "[%jd] Terminated\n", (intmax_t)jid);
          }
          jobs_remove_pgid(pgid);
          job_count = jobs_get_joblist_size();
          jobs = jobs_get_joblist();
          break;
        }
        return -1; /* Other errors are not ok */
      }

      /* Record status for reporting later when we see ECHILD */
      if (jobs_set_status(jid, status) < 0) return -1;

      /* Handle case where a process in the group is stopped */
      if (WIFSTOPPED(status)) {
        fprintf(stderr, "[%jd] Stopped\n", (intmax_t)jid);
        break;
      }
    }
  }
  return 0;
}
