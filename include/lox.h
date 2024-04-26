#ifndef LOX_H
#define LOX_H

/*
 * rc for lox, mimics the sysexits.h style
 * https://man.freebsd.org/cgi/man.cgi?query=sysexits&apropos=0&sektion=0&manpath=FreeBSD+4.3-RELEASE&format=html
 */
#define LOX_EX_OK 0

#define LOX_EX_USAGE 64
#define LOX_EX_DATAERR 65

int lox_run_file (const char *filename);
void lox_run_prompt (void);
void lox_run (const char *source);

void lox_error (int line, const char *message, ...);

#endif /* LOX_H */
