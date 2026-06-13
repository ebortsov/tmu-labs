#pragma once
#include "TempDirectory.hpp"
#include <memory>
#include <string>

namespace tempfs {
/// A temporary file.
class TempFile {
public:
    /*
        PUT COPY/MOVE CONSTRUCTORS/ASSIGNMENT HERE
    */

    /// Delete this file.
    ~TempFile();

    /// Get the name of this file.
    /// @return Name of this file.
    std::string getName() const;
    /// Get the full path of this file.
    /// @return Full path of this file.
    std::string getPath() const;

private:
};
} // namespace tempfs
