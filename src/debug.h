#ifndef __DEBUG_H_
#define __DEBUG_H_

#define ANSI_COLOR_RED     "\x1b[31m"
#define ANSI_COLOR_GREEN   "\x1b[32m"
#define ANSI_COLOR_YELLOW  "\x1b[33m"
#define ANSI_COLOR_BLUE    "\x1b[34m"
#define ANSI_COLOR_MAGENTA "\x1b[35m"
#define ANSI_COLOR_CYAN    "\x1b[36m"
#define ANSI_COLOR_RESET   "\x1b[0m"

enum {RED, GREEN, YELLOW, BLUE, MAGENTA, CYAN, DEFAULT};

/* Print only if DEBUG is defined, and in the specified color */
void debugprint(char* string, int color);
/* Print in specified color, no matter if DEBUG is defined */
void colorprint(char* string, int color);

#endif