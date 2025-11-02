//
// Created by neko on 11/2/25.
//

extern "C" {
int __cxa_atexit(void (*func)(void *), void *arg, void *dso_handle) {
    (void) func;
    (void) arg;
    (void) dso_handle;

    return 0;
}
}
