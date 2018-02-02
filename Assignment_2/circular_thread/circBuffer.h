typedef void (*fooptr)(void);

typedef struct {
    fooptr * const buffer;
    int head;
    int tail;
    const int maxLen;
} circBuf_t;

int circBuffPush(circBuf_t *cb, fooptr funPtr);
 
int circBuffPop(circBuf_t *cp, fooptr *funPtr);