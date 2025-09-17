#ifndef GETCH_H
#define GETCH_H

static void reset_terminal(void);
void init_terminal(void);

char getch(void);
int moreInp();

#endif
