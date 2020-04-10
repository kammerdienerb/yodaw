#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <dlfcn.h>
#include <unistd.h>

#include "interface.h"

static yodaw_lib_t yodaw_lib;
static struct yodaw_state_t *state;

int load_yodaw_lib(void);
void call_yodaw_fini(void);


int main(int argc, char **argv) {
    int status;

    if (load_yodaw_lib() != 0) {
        return 1;
    }

    state = yodaw_lib._init(&yodaw_lib, argc, argv);

    if (!state)    { return 1; }

    atexit(call_yodaw_fini);

    while (1) {
        status = yodaw_lib._pump();

        if (status == YODAW_QUIT) {
            break;
        } else if (status == YODAW_RELOAD) {
            if (load_yodaw_lib() != 0) {
                return 1;
            }
        }
    }
}


void call_yodaw_fini(void)    { yodaw_lib._fini(state); }

/*
 * If some doofus decides to take load libyodaw.so from
 * a plugin for some crazy reason, they will remove our
 * ability to do any dynamic reloading because they've
 * grabbed another reference to the library.
 * As long as that other reference exists, the system
 * won't unload our mapped image...
 *
 * So, this function will clear out essentially acquire
 * _all_ of the references to the library once by one
 * and remove them so that the ref count eventually goes
 * to zero and the library is unloaded.
 *
 * This works, but it will probably screw up whoever took
 * the additional reference. Tough.
 */
void force_yodaw_unload(void *handle) {
    void *try_handle;

    while ((try_handle = dlopen("libyodaw.so", RTLD_NOW | RTLD_NOLOAD))) {
        dlclose(try_handle);
        dlclose(handle);
    }
}

int load_yodaw_lib(void) {

#define LOAD_YODAW_FN(fn) do {                                    \
    yodaw_lib._##fn = dlsym(yodaw_lib.handle, "yodaw_" #fn);          \
    if (yodaw_lib._##fn == NULL) {                                \
        printf("[yodaw]! could not load symbol 'yodaw_%s'\n", #fn); \
    }                                                           \
} while (0)

    if (yodaw_lib.handle) {
        state = yodaw_lib._get_state();
        force_yodaw_unload(yodaw_lib.handle);
    }

    yodaw_lib.handle = dlopen("libyodaw.so", RTLD_NOW | RTLD_LOCAL);

    if (yodaw_lib.handle == NULL) {
        printf("[yodaw]! could not load 'libyodaw.so'\n%s\n", dlerror());
        return 1;
    }

    LOAD_YODAW_FN(init);
    LOAD_YODAW_FN(fini);
    LOAD_YODAW_FN(pump);
    LOAD_YODAW_FN(get_state);
    LOAD_YODAW_FN(set_state);

    yodaw_lib._set_state(state);

    return 0;
}
