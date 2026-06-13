#pragma once
#include <chrono>
#include <climits>
#include <print>
#include <ftw.h>
#include <gtest/gtest.h>
#include <poll.h>
#include <sys/inotify.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

/// Clean the test directory before and after a test runs
class CleanTestDirectory {
private:
    /// The directory
    std::string directory;

    /// Remove the test directory
    void clean() {
        struct stat buf {};
        if (stat(directory.c_str(), &buf) != 0) {
            if ((errno == ENOENT) || (errno == ENOTDIR)) { return; }

            perror("stat failed");
            std::println(stderr, "error while cleaning {}, please delete the directory manually", directory);
            throw;
        }

        auto callback = [](const char *path, const struct stat * /*stat*/, int info, struct FTW * /*ftw*/) -> int {
            switch (info) {
                case FTW_D: break;
                case FTW_NS:
                case FTW_DNR: return 1;
                case FTW_DP:
                    if (rmdir(path) < 0) {
                        perror("rmdir failed");
                        return 1;
                    }

                    break;
                case FTW_F:
                case FTW_SL:
                case FTW_SLN:
                    if (unlink(path) < 0) {
                        perror("unlink failed");
                        return 1;
                    }

                    break;
                default: break;
            }

            return 0;
        };

        if (nftw(directory.c_str(), callback, 16, FTW_DEPTH | FTW_MOUNT | FTW_PHYS) != 0) {
            std::println(stderr, "error while cleaning {}, please delete the directory manually", directory);
            throw;
        }
    }

public:
    /// Constructor
    explicit CleanTestDirectory(std::string directory) : directory(std::move(directory)) { clean(); }

    /// Destructor
    ~CleanTestDirectory() {
        try {
            clean();
        } catch (std::exception &) { exit(1); }
    }
};
//---------------------------------------------------------------------------
/// An inotify context
struct InotifyContext {
    /// The inotify fd
    int fd;

    /// Constructor
    InotifyContext() {
        fd = inotify_init();
        if (fd < 0) {
            perror("inotify_init failed");
            throw;
        }
    }

    InotifyContext(InotifyContext &other) = delete;
    InotifyContext(InotifyContext &&other) noexcept : fd(other.fd) { other.fd = -1; }
    InotifyContext &operator=(InotifyContext &other) = delete;
    InotifyContext &operator=(InotifyContext &&other) = delete;

    /// Destructor
    ~InotifyContext() {
        if (fd >= 0) { close(fd); }
    }
};
//---------------------------------------------------------------------------
/// Watch a directory for certain events
class DirectoryWatch {
private:
    /// The inotify context
    InotifyContext context;
    /// The directory
    std::string directory;
    /// The watch descriptor
    int wd;

    /// Check if a certain file or directory has been created in the directory
    bool expectCreated(const std::string &filename, bool expectFile) {
        std::array<pollfd, 1> fds{};
        fds[0].fd = context.fd;
        fds[0].events = POLLIN;

        int ret = poll(fds.data(), 1, 500);

        if (ret < 0) {
            perror("poll failed");

            return false;
        } else if (ret == 0) {
            std::println(stderr, "no events occurred in directory {} after 500ms", directory);
            return false;
        }

        std::vector<char> buffer(sizeof(inotify_event) + NAME_MAX + 1);
        if (read(context.fd, &buffer[0], buffer.size()) < 0) {
            perror("read failed");
            return false;
        }

        auto *event = reinterpret_cast<struct inotify_event *>(&buffer[0]);

        if (event->wd != wd) {
            std::println(stderr, "unexpected event");
            return false;
        }

        if (filename != std::string(event->name)) {
            std::println(stderr, "name mismatch (expected: {}, actual: {})", filename, std::string(event->name));
            return false;
        }

        if (!(event->mask & IN_CREATE)) {
            std::println(stderr, "event mismatch (expected: creation)");
            return false;
        }

        std::string fullPath = directory + "/" + filename;
        struct stat buf {};
        if (stat(fullPath.c_str(), &buf) < 0) {
            perror("stat failed");

            return false;
        }

        if (expectFile) {
            if (!S_ISREG(buf.st_mode)) {
                std::println(stderr, "file type mismatch (expected file)");
                return false;
            }
        } else if (!S_ISDIR(buf.st_mode)) {
            std::println(stderr, "file type mismatch (expected directory)");
            return false;
        }

        return true;
    }

public:
    /// Constructor
    explicit DirectoryWatch(std::string directory) : directory(std::move(directory)) {
        wd = inotify_add_watch(context.fd, this->directory.c_str(), IN_CREATE | IN_DELETE);
        if (wd < 0) {
            perror("inotify_add_watch failed");
            throw;
        }
    }

    DirectoryWatch(DirectoryWatch &other) = delete;
    DirectoryWatch(DirectoryWatch &&other) noexcept
        : context(std::move(other.context)), directory(std::move(other.directory)), wd(other.wd) {
        other.wd = -1;
    }

    DirectoryWatch &operator=(DirectoryWatch &other) = delete;
    DirectoryWatch &operator=(DirectoryWatch &&other) = delete;

    /// Destructor
    ~DirectoryWatch() {
        if (wd < 0) { return; }
        if (inotify_rm_watch(context.fd, wd) < 0) {
            // Silencing this because watches of already deleted directories get deleted automatically
            // and this basically always triggers if you poll after the directory gets removed. It doesn't actually
            // break anything, the wd is already closed so we're off the hook.
            // perror("inotify_rm_watch failed");
        } else {
            // consume the IN_IGNORE event generated by inotify_rm_watch
            std::vector<char> buffer(sizeof(inotify_event) + NAME_MAX + 1);
            if (read(context.fd, &buffer[0], buffer.size()) < 0) { perror("read failed"); }
        }
    }

    /// Check if a certain file has been created in the directory
    bool expectFileCreated(const std::string &filename) { return expectCreated(filename, true); }

    /// Check if a certain directory has been created in the directory
    bool expectDirectoryCreated(const std::string &name) { return expectCreated(name, false); }
    /// Check if a certain file or directory has been deleted in the directory
    bool expectDeleted(const std::string &name) {
        std::array<pollfd, 1> fds{};
        fds[0].fd = context.fd;
        fds[0].events = POLLIN;

        int ret = poll(fds.data(), 1, 500);

        if (ret < 0) {
            perror("poll failed");
            return false;
        }
        if (ret == 0) {
            std::println(stderr, "no events occurred in directory {} after 500ms", directory);
            return false;
        }

        std::vector<char> buffer(sizeof(inotify_event) + NAME_MAX + 1);
        if (read(context.fd, &buffer[0], buffer.size()) < 0) {
            perror("read failed");
            return false;
        }

        auto *event = reinterpret_cast<struct inotify_event *>(&buffer[0]);

        if (name != std::string(event->name)) {
            std::println(stderr, "name mismatch (expected: {}, actual: {})", name, std::string(event->name));
            return false;
        }

        if (!(event->mask & IN_DELETE)) {
            std::println(stderr, "event mismatch (expected: deletion)");
            return false;
        }

        return true;
    }

    /// Check if all files from a list have been deleted
    bool expectDeleted(std::vector<std::string> names) {
        std::array<pollfd, 1> fds{};
        fds[0].fd = context.fd;
        fds[0].events = POLLIN;

        while (!names.empty()) {
            int ret = poll(fds.data(), 1, 500);

            if (ret < 0) {
                perror("poll failed");
                return false;
            }
            if (ret == 0) {
                std::println(stderr, "no events occurred in directory {} after 500ms", directory);
                return false;
            }

            std::vector<char> buffer(sizeof(inotify_event) + NAME_MAX + 1);
            size_t bytesRead = read(context.fd, &buffer[0], buffer.size());
            if (bytesRead < 0) {
                perror("read failed");
                return false;
            }

            size_t idx = 0;
            while (idx < bytesRead && !names.empty()) {
                auto *event = reinterpret_cast<struct inotify_event *>(&buffer[idx]);
                if (!(event->mask & IN_DELETE)) {
                    std::println(stderr, "event mismatch (expected: deletion)");
                    return false;
                }

                auto name = std::string(event->name);
                auto loc = std::find(names.begin(), names.end(), name);

                if (loc == names.end()) {
                    std::println(stderr, "name mismatch ({} not expected)", name);
                    return false;
                }
                names.erase(loc);

                idx += sizeof(inotify_event) + event->len;
            }
        }
        return true;
    }
};