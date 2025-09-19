#ifndef GETCH_H
#define GETCH_H

static void reset_terminal(void);
void init_terminal(void);

char getThatCh(void);
int moreInp();


typedef enum {
    REGULAR_KEY,
    ESCAPE_KEY,
    ARROW_KEY,
    NOTHING,  // There was a problem so returned NOTHING
} keyReturnType;

typedef struct {
    char key;
    keyReturnType typ;
} keyReturn;

keyReturn* getKey();

#endif
