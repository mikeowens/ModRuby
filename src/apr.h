#ifndef MODRUBY_APR_POOL_DECLARE
#define MODRUBY_APR_POOL_DECLARE

#include <string>

#include "../include/apr.h"
#include "common.h"
#include "types.h"

struct apr_pool_t;
struct apr_finfo_t;

/** \namespace modruby::apr

The \c modruby::apr namespace wraps the Apache Portable Runtime API. Most of
the functions are thin wrappers around the actual APR functions.
 
*/

namespace modruby
{

namespace apr
{

// Operational functions

/** This function should always be called in a library or program before any APR
 *  function is called. It initialized the APR library.
 */
void init();

/** This is the APR finalization routine which should be called at program
 * exit. It is called begin your program by calling apr::init(), and end it by
 * calling apr::destroy(). Neither of these is needed to as the underlying
 * functions are already called by Apache upon startup and shutdown.
 */
void destroy();

/** This is no longer needed. Don't use it. It will be removed some day when I
 * am really, really, really bored. 
 */
apr_pool_t* pool();

// Error functions
std::string str_error(apr_status_t rv);
apr_status_t last_error();

/* This class abstracts an APR subpool -- a child of a pool. */
class subpool
{
  public:

    apr_pool_t* handle;

    subpool();
    subpool(apr_pool_t* p);
    ~subpool();
};

// File/dir manipulation functions
apr_file_t* tempfile(const char* name, apr_pool_t* pool, i32 flags=0);
const char* file_name(apr_file_t* file);
int chmod(const char* path, int file_perms);
int cp(const char* source_path, const char* dest_path);
int mv(const char* source_path, const char* dest_path);
int rm(const char* source_path);
int mkdir(const char* path, i32 perms = -1);
int rmdir(const char* path);

std::string cwd();
bool path_is_relative(const char* path);
std::string path_merge(const char* root_path, const char* addpath);
std::string path_root(const char* path);

// Wrapper for APR apr_finfo_t struct used in stat() function
class file_info
{
  public:

    /* APR file types:
    **
    **   APR_NOFILE    : No file type determined
    **   APR_REG,      : A regular file
    **   APR_DIR,      : A directory
    **   APR_CHR,      : A character device
    **   APR_BLK,      : A block device
    **   APR_PIPE,     : A FIFO / pipe
    **   APR_LNK,      : A symbolic link
    **   APR_SOCK,     : A [unix domain] socket
    **   APR_UNKFILE   : A file of some other unknown type
    */

  private:

    bool _valid;
    apr_finfo_t _stat;

    file_info();

  public:

    file_info(const char* filename, apr_pool_t* pool, int want_flags = 0);
    file_info(const apr_finfo_t& s);

    ~file_info();

    bool valid();
    bool valid(int flag);
    apr_filetype_e type() const;
    
    apr_uid_t uid() const;
    apr_gid_t gid() const;

    apr_dev_t device() const;
    apr_ino_t inode() const;

    apr_off_t size() const;
    apr_off_t csize() const;

    apr_int32_t nlink() const;
    apr_int32_t perms() const;

    apr_time_t atime() const;
    apr_time_t ctime() const;
    apr_time_t mtime() const;

    const char* fname() const;

    /** apr_finfo_t.name does not work on Unix. This is because both the .fname
     *  and .name members of this struct are just plain boneheaded. Basically,
     *  you should just always call fname(), as name() will be empty even you
     *  specify APR_FINFO_NAME in the want flags to apr_stat().
     */
    const char* name() const;

    std::string md5();
    std::string sha1();
};

file_info stat( apr_file_t* file, apr_pool_t* pool, 
                          int want_flags = APR_FINFO_MIN );
file_info stat( const char* fname, apr_pool_t* pool, 
                          int want_flags = APR_FINFO_MIN );

} // end namespace apr
} // end namespace modruby

#endif
