/*
 * The params module is very small and only contains a definition for a struct
 * called “params” that represents parameters of the shell. The structure holds
 * parameters representing both exit status and the last background process ID.
 * The “Params” struct is referenced and its parameters are updated in a significant
 * number of other modules as the shell continues to process commands [3].
 */

/* XXX DO NOT MODIFY THIS FILE XXX */
#include <errno.h>
#include <unistd.h>

#include "params.h"

/* Definition for a struct holding the two special paramters we're using in our
 * shell: status ($?) and last bg pid ($!).
 */
struct params params = {.status = 0, .bg_pid = 0};

