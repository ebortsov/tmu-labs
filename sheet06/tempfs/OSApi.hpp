#pragma once

namespace osapi {
/// Create a directory at a given path.
/// @param path C-string of the path.
void createDir(const char *path);

/// Remove an empty directory at the given path.
/// @param path C-string of the path.
void removeDir(const char *path);

/// Remove a file with the given name from a directory given by the file descriptor.
/// @param parentDirFd File descriptor of the parent directory.
/// @param name Name of the file.
void removeFile(int parentDirFd, const char *name);

/// Open a directory at a given path.
/// @param path C-string of the path.
/// @return The file descriptor of the directory.
int openDir(const char *path);

/// Create and open a file with the given name within the directory given by the file descriptor.
/// @param dirFd File descriptor of the directory.
/// @param name Name of the file.
/// @return The file descriptor of the file.
int openFile(int dirFd, const char *name);

/// Close a file descriptor of a file or a directory
/// @param fd The file descriptor.
void closeFd(int fd);
} // namespace osapi
