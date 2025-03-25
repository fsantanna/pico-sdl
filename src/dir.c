#include <libgen.h>

#ifdef _WIN32
#include <Windows.h>
#elif defined(__APPLE__)
#elif defined(__linux__)
#include <unistd.h>
#endif

char* pico_dir_exe_get (void) {
    static char path[1024];
#ifdef _WIN32
    GetModuleFileName(NULL, path, sizeof(path));
#elif defined(__APPLE__)
    int n = sizeof(path);
    _NSGetExecutablePath(path, &n);
#elif defined(__linux__)
    int n = readlink("/proc/self/exe", path, sizeof(path) - 1);
    if (n != -1) {
        path[n] = '\0';
    } else {
        return NULL;
    }
#endif
    return dirname(path);
}
