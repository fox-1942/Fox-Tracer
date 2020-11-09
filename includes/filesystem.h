/* Copyright:
 *
 * LearnOpenGl.com Joey de Vries
 * https://twitter.com/JoeyDeVriez.
 *
 * Changes were made by Tamás Boros.
 *
 * Attribution-NonCommercial 4.0 International (CC BY-NC 4.0), Creative Commons
*/

#ifndef FILESYSTEM_H
#define FILESYSTEM_H

#include <string>
#include <cstdlib>
#include "root_directory.h"

class File
{
private:
    typedef std::string (*Builder) (const std::string& path);

public:
    static std::string getPath(const std::string& path)
    {
        static std::string(*pathBuilder)(std::string const &) = getPathBuilder();
        return (*pathBuilder)(path);
    }

private:
    static std::string const & getRoot()
    {
        static char const * envRoot = getenv("LOGL_ROOT_PATH");
        static char const * givenRoot = (envRoot != nullptr ? envRoot : logl_root);
        static std::string root = (givenRoot != nullptr ? givenRoot : "");
        return root;
    }

    static Builder getPathBuilder()
    {
        if (getRoot() != "")
            return &File::getRootPath;
        else
            return &File::getRelativePath;
    }

    static std::string getRootPath(const std::string& path)
    {
        return getRoot() + std::string("/") + path;
    }

    static std::string getRelativePath(const std::string& path)
    {
        return "../../../" + path;
    }
};

// FILESYSTEM_H
#endif