#include "SDFS.h"
#include "../FS/FS.h"
#include <string.h>
#if !defined(NO_GLOBAL_INSTANCES) && !defined(NO_GLOBAL_SDFS)
FS SDFS = FS(FSImplPtr(new sdfs::SDFSImpl()));
#endif

time_t (*__sdfs_timeCallback)(void) = nullptr;
#define DEBUGV(...) do { } while(0)

FileImplPtr SDFSImpl::open(const char* path, OpenMode openMode, AccessMode accessMode) {
    if (!_mounted) {
        DEBUGV("SDFSImpl::open() called on unmounted FS\n");
        return FileImplPtr();
    }
    if (!path || !path[0]) {
        DEBUGV("SDFSImpl::open() called with invalid filename\n");
        return FileImplPtr();
    }
    int flags = _getFlags(openMode, accessMode);
    if ((openMode && OM_CREATE) && strchr(path, '/')) {
        // For file creation, silently make subdirs as needed.  If any fail,
        // it will be caught by the real file open later on
        char *pathStr = strdup(path);
        if (pathStr) {
            // Make dirs up to the final fnamepart
            char *ptr = strrchr(pathStr, '/');
            if (ptr && ptr != pathStr) { // Don't try to make root dir!
                *ptr = 0;
                _fs.mkdir(pathStr, true);
            }
        }
        free(pathStr);
    }
    File32 fd = _fs.open(path, flags);
    if (!fd) {
        DEBUGV("SDFSImpl::openFile: fd=%p path=`%s` openMode=%d accessMode=%d",
               &fd, path, openMode, accessMode);
        return FileImplPtr();
    }
    auto sharedFd = std::make_shared<File32>(fd);
    return std::make_shared<SDFSFileImpl>(this, sharedFd, path);
}
