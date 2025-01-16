// Collaborator: Seth Mackovjak
// Collaborator: Robin Yi

#define _POSIX_C_SOURCE 200809L
#include <assert.h>
#include <ctype.h>
#include <errno.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "util/gprintf.h"
#include "vars.h"

struct var {
  struct var *next;
  bool export : 1;
  char *value;
  char name[];
};

static struct var *var_list = 0;

/** Checks if a variable name is a valid XBD name 
 *
 * @returns 1 if yes, 0 if not
 *
 * This is an internal function that does not validate its argument. It is
 * faster for internal use, but unsafe to expose as part of the API. Notice
 * we wrap it with vars_is_valid_varname() below.
 */
static int
is_valid_varname(char const *name)
{
  assert(name);
  /* TODO: Implement me.
   * Refer to:
   *  3.230 Name. Base Definitions. POSIX.1-2008
   *  regex to match: [A-Za-z_][A-Za-z0-9_]*
   *
   * You'll most definitely want to use functions from: ctype.h(0P)
   */

  /* BEGIN IMPLEMENTATION BY wisecavc */

    // records string length in a variable
    int var_len = strlen(name);
    // tests remaining letters [A-Za-z0-9_]*
    for (int i = 1; i < var_len; i++) {
      // detected inconsistencies cause a return value of 0
      if (!(isalnum(name[i]) || name[i] == '_')) {
        return 0;
      }
    }
    // return 1 if no inconsistencies found
    return 1;

  /*
   * Information regarding how to implement ctype.h data strcutures
   * was retrieved from the OpenGroup site, found here:
   * https://pubs.opengroup.org/onlinepubs/9699919799/
   */

  /* END IMPLEMENTATION BY wisecavc */

  errno = ENOSYS; /* Not implemented */
  return -1;
}

/** Checks if a variable name is a valid XBD name 
 *
 * @returns 1 if yes, 0 if not
 *
 * This is an external function that validates its argument before calling the
 * internal unsafe version 
 */
int
vars_is_valid_varname(char const *name)
{
  /* TODO: Implement argument validation before tail-calling internal
   * is_valid_varname() function. */

  /* BEGIN IMPLEMENTATION BY wisecavc */

  // tests to see if name is null or an empty string before it's tail-called
  if (name == NULL || *name == '\0') {
    return 0;
  }
  // tests first letter [A-Za-z_]
  else if (!(isalpha(name[0]) || name[0] == '_')) {
    // first character inconsistent with regex
    return 0;
  }

  bool var_name_pro = is_valid_varname(name);

  // if name isn't null or 0, is_valid_varname is called and returns the validity check val
  return var_name_pro;

  /* END IMPLEMENTATION BY wisecavc */

  errno = ENOSYS;
  return -1;
}

/** returns nullptr if not found 
 *
 * XXX DO NOT MODIFY XXX 
 */
static struct var *
find_var(char const *name)
{
  assert(name);
  assert(is_valid_varname(name));

  /* Search local varlist */
  struct var *v = var_list;
  for (; v; v = v->next) {
    if (strcmp(v->name, name) == 0) return v;
  }

  return 0;
}

/** Creates a new var with name and inserts into var list 
 *
 * XXX DO NOT MODIFY XXX 
 */
static struct var *
new_var(char const *name)
{
  assert(is_valid_varname(name));
  assert(!find_var(name));
  struct var *v = malloc(sizeof *v + strlen(name) + 1);
  if (!v) return 0;
  strcpy(v->name, name);

  char *val = getenv(name);
  if (val) {
    v->export = 1;
  } else {
    v->export = 0;
  }
  v->value = 0;
  v->next = var_list;
  var_list = v;
  return v;
}

/** Remove a var from varlist and return it 
 *
 * XXX DO NOT MODIFY XXX 
 */
static void
remove_var(char const *name)
{
  assert(is_valid_varname(name));
  struct var **link = &var_list;
  for (; *link; link = &((*link)->next)) {
    if (strcmp((*link)->name, name) == 0) {
      void *tmp = (*link)->next;
      free((*link)->value);
      free(*link);
      *link = tmp;
      break;
    }
  }
}

/** Return existing var, or make a new var
 *
 * XXX DO NOT MODIFY XXX 
 */
static struct var *
ensure_var(char const *name)
{
  assert(is_valid_varname(name));
  struct var *v = find_var(name);
  return v ? v : new_var(name);
}

/* XXX DO NOT MODIFY XXX */
int
vars_set(char const *name, char const *value)
{
  if (!name || !value || !is_valid_varname(name)) {
    errno = EINVAL;
    return -1;
  }
  gprintf("vars_set(%s, %s)", name, value);

  struct var *v = ensure_var(name);
  if (!v) return -1;

  if (v->export) {
    gprintf("%s=%s is exported, updating env", name, value);
    return setenv(name, value, 1);
  }

  char *dupval = strdup(value);
  if (!dupval) return -1;
  v->value = dupval;
  return 0;
}

/* XXX DO NOT MODIFY XXX */
char const *
vars_get(char const *name)
{
  if (!name || !is_valid_varname(name)) {
    errno = EINVAL;
    return 0;
  }

  gprintf("searching for %s in local var list", name);
  /* Look through our local var list */
  struct var *v = find_var(name);
  if (v && !v->export) {
    gprintf("found local var %s with value %s", name, v->value);
    return v->value;
  }

  gprintf("searching for %s in environment", name);
  /* Fallback to searching environment */
  char const *value = getenv(name);
#ifndef NDEBUG
  if (value) {
    gprintf("found env var %s with value %s", name, value);
  } else {
    gprintf("did not find var %s", name);
  }
#endif
  return value;
}

/* XXX DO NOT MODIFY XXX */
int
vars_unset(char const *name)
{
  if (!name || !is_valid_varname(name)) {
    errno = EINVAL;
    return -1;
  }
  gprintf("unsetting var %s", name);
  remove_var(name);
  return unsetenv(name);
}

/* XXX DO NOT MODIFY XXX */
int
vars_export(char const *name)
{
  if (!name || !is_valid_varname(name)) {
    errno = EINVAL;
    return -1;
  }
  gprintf("marking %s for export", name);
  struct var *v = ensure_var(name);
  if (!v) return -1;

  /* Mark exported */
  v->export = 1;

  /* Only actually export to env if already set */
  if (v->value) {
    gprintf("exporting value %s for var %s", v->value, name);
    if (setenv(v->name, v->value, 1) < 0) {
      return -1;
    }
  }
  return 0;
}

/* XXX DO NOT MODIFY XXX */
void
vars_cleanup(void)
{
  while (var_list) {
    struct var *v = var_list;
    var_list = v->next;
    free(v->value);
    free(v);
  }
}
