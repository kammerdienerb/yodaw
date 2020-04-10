#ifndef __INTERFACE_H__
#define __INTERFACE_H__

struct yodaw_state_t;

#define YODAW_NORMAL (0x1)
#define YODAW_QUIT   (0x2)
#define YODAW_RELOAD (0x3)

#define WINDOW_WIDTH  (640)
#define WINDOW_HEIGHT (480)

typedef struct yodaw_lib {
    void                  *handle;
    struct yodaw_state_t* (*_init)(struct yodaw_lib *, int, char**);
    void                  (*_fini)(struct yodaw_state_t*);
    int                   (*_pump)(void);
    struct yodaw_state_t* (*_get_state)(void);
    void                  (*_set_state)(struct yodaw_state_t*);
} yodaw_lib_t;


struct yodaw_state_t* yodaw_init(yodaw_lib_t *, int argc, char **argv);
void                  yodaw_fini(struct yodaw_state_t *state);
int                   yodaw_pump(void);
void                  yodaw_set_state(struct yodaw_state_t *state);
struct yodaw_state_t* yodaw_get_state(void);

#endif
