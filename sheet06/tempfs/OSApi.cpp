#include "OSApi.hpp"
#include <cstdio>
#include <fcntl.h>
#include <sys/stat.h>
#include <unistd.h>

namespace osapi {
void createDir(const char *path) {
    if (mkdir(path, S_IRWXU) < 0) { perror("createDir failed"); }
}

void removeDir(const char *path) {
    if (rmdir(path) < 0) { perror("removeDir failed"); }
}

void removeFile(int parentDirFd, const char *name) {
    if (unlinkat(parentDirFd, name, 0) < 0) { perror("removeFile failed"); }
}

int openDir(const char *path) {
    int fd = open(path, O_DIRECTORY);
    if (fd < 0) { perror("openDir failed"); }
    return fd;
}

int openFile(int dirFd, const char *name) {
    int fd = openat(dirFd, name, O_CREAT, S_IRUSR | S_IWUSR);
    if (fd < 0) { perror("openFile failed"); }
    return fd;
}

void closeFd(int fd) {
    if (close(fd) < 0) { perror("closeFd failed"); }
}
} // namespace osapi
