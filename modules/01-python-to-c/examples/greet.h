/* greet.h - the PUBLIC INTERFACE of the greet module.
 *
 * A header holds declarations only (no function bodies). Any .c file that wants
 * to call greet() includes this header so the compiler knows greet's shape.
 */
#ifndef GREET_H          /* include guard: prevents double-inclusion */
#define GREET_H

/* Print a greeting for the given name. */
void greet(const char *name);

#endif /* GREET_H */
