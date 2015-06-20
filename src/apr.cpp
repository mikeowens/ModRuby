#include <stdlib.h>

#include <string>
#include <sstream>

#include "apr.h"
#include "md5.h"
#include "sha1.h"

using std::string;
using std::stringstream;

namespace modruby
{
namespace apr
{

static apr_pool_t* apr_pool    = NULL;
static apr_status_t apr_status = APR_SUCCESS;
static unsigned int refcount   = 0;

static string error;

void init()
{
    if (refcount++ == 0)
    {
        // Initialize Apache Portable Runtime
        apr_initialize();
        atexit(apr_terminate);

        // Create a global pool
        apr_pool_create(&apr_pool, NULL);
    }
}

void destroy()
{
    refcount--;

    if (refcount == 0)
    {
        apr_pool_destroy(apr_pool);
    }
}

apr_pool_t* pool()
{
    return apr_pool;
}

subpool::subpool()
{
    apr_pool_create(&handle, pool());
}

subpool::subpool(apr_pool_t* p)
{
    apr_pool_create(&handle, p);
}

subpool::~subpool()
{
    apr_pool_destroy(handle);
}

string str_error(apr_status_t rv)
{
    char buf[256];

    return apr_strerror(rv, buf, 256);
}

apr_file_t* tempfile(const char* name, apr_pool_t* pool, i32 flags)
{
    string filename = (string)name + "XXXXXX";
    apr_file_t* fp;
    apr_file_mktemp(&fp, (char*)filename.c_str(), flags, pool);

    return fp;
}

const char* file_name(apr_file_t* file)
{
    const char* path;
    apr_file_name_get(&path, (apr_file_t*)file);
    return path;
}

int chmod(const char* path, int file_perms)
{
    return apr_file_perms_set(path, file_perms);
}

int cp(const char* source_path, const char* dest_path)
{
    apr_status = apr_file_copy( source_path,
                                dest_path,
                                APR_FILE_SOURCE_PERMS,
                                pool() );

    if (apr_status == APR_SUCCESS)
    {
        return true;
    }

    return false;
}

int mv(const char* source_path, const char* dest_path)
{
    return apr_file_rename(source_path, dest_path, pool());
}

int rm(const char* source_path)
{
    return apr_file_remove(source_path, pool());
}

int mkdir(const char* path, i32 perms)
{
    if (perms == -1)
    {
        perms = APR_UREAD | APR_UWRITE | APR_UEXECUTE;
    }

    apr_status = apr_dir_make_recursive(path, perms, pool());

    if ((apr_status == APR_SUCCESS) || (apr_status == EEXIST))
    {
        return APR_SUCCESS;
    }

    return apr_status;
}

// Shamlessly taken from subversion svn_io_remove_dir()
int rmdir(const char* path)
{
    apr_dir_t*   this_dir;
    apr_finfo_t  this_entry;
    apr_pool_t*  subpool;
    apr_int32_t  flags = APR_FINFO_TYPE | APR_FINFO_NAME;

    /* APR doesn't like "" directories */
    if (path[0] == '\0')
    {
        path = ".";
    }

    apr_status = apr_dir_open(&this_dir, path, pool());

    if (apr_status)
    {
        string err = (string)"Can't open directory " + path + ".";

        // TODO
        //log(err);

        return apr_status;
    }

    apr_pool_create(&subpool, pool());

    for ( apr_status = apr_dir_read (&this_entry, flags, this_dir);
            apr_status == APR_SUCCESS;
            apr_status = apr_dir_read (&this_entry, flags, this_dir) )
    {
        apr_pool_clear(subpool);

        if ( (this_entry.filetype == APR_DIR)
                && ((this_entry.name[0] == '.')
                    && ((this_entry.name[1] == '\0')
                        || ((this_entry.name[1] == '.')
                            && (this_entry.name[2] == '\0')))))
        {
            continue;
        }
        else
        {
            /* something other than "." or "..", so proceed */
            const char* fullpath, *entry;

            entry = this_entry.name;

            stringstream strm;

            if (entry == NULL)
            {
                fullpath = path;
            }
            else if (*entry == '/')
            {
                fullpath = entry;
            }
            else if (path == NULL)
            {
                fullpath = entry;
            }
            else
            {
                strm << path << "/" << entry;
                fullpath = strm.str().c_str();
            }

            if (this_entry.filetype == APR_DIR)
            {
                if (apr::rmdir(fullpath) == false)
                {
                    return apr_status;
                }

                //MACOSX_REWINDDIR_HACK (this_dir, path);
            }
            else
            {
                apr_status = apr_file_remove(fullpath, subpool);

                if (apr_status)
                {
                    string err = (string)"Can't remove  " + fullpath + ".";

                    // TODO
                    //log(err);

                    apr_pool_destroy(subpool);

                    return apr_status;
                }

                // TODO:
                //MACOSX_REWINDDIR_HACK (this_dir, path);
            }
        }
    }

    apr_pool_destroy(subpool);

    if (!APR_STATUS_IS_ENOENT(apr_status))
    {
        string err = (string)"Can't read directory  " + path + ".";
        // TODO
        //log(err);

        return apr_status;
    }

    apr_status = apr_dir_close(this_dir);

    if (apr_status)
    {
        string err = (string)"Error closing directory  " + path + ".";
        // log(err);

        return apr_status;
    }

    apr_status = apr_dir_remove(path, pool());

    // TODO:
    //WIN32_RETRY_LOOP (status, apr_dir_remove (path_apr, pool));

    if (apr_status)
    {
        string err = (string)"Can't remove  " + path + ".";
        // log(err);

        return apr_status;
    }

    return APR_SUCCESS;
}

string cwd()
{
    char* path;
    subpool s;

    apr_filepath_get(&path, 0, s.handle);

    return path;
}

bool path_is_relative(const char* path)
{
    const char* rootpath = "";
    subpool s;

    apr_status_t rc = apr_filepath_root( &rootpath, &path,
                                         APR_FILEPATH_TRUENAME,
                                         s.handle );

    return rc == APR_ERELATIVE;
}

string path_merge(const char* root_path, const char* add_path)
{
    char* new_path;
    subpool s;

    // APR will fail if add path is absolute.
    if (*add_path == '/')
    {
        add_path++;
    }

    apr_status_t rc = apr_filepath_merge( &new_path, root_path, add_path,
                                          APR_FILEPATH_NATIVE, s.handle );

    return new_path;
}

string path_root(const char* path)
{
    const char* rootpath = "";
    subpool s;

    apr_status_t rc = apr_filepath_root( &rootpath, &path,
                                         APR_FILEPATH_NATIVE,
                                         s.handle );

    return rootpath;
}

file_info stat(apr_file_t* file, apr_pool_t* pool, int want_flags)
{
    return modruby::apr::stat(file_name(file), pool, want_flags);
}

file_info stat(const char* filename, apr_pool_t* pool, int want_flags)
{
    return file_info(filename, pool, want_flags);
}

file_info::file_info(const char* filename, apr_pool_t* pool, int want_flags)
{
    memset(&_stat, 0, sizeof(apr_finfo_t));

    int ret = apr_stat( &_stat, filename,
                        want_flags | APR_FINFO_MIN | APR_FINFO_LINK | APR_FINFO_IDENT,
                        pool);

    if ((ret != APR_SUCCESS) && (ret != APR_INCOMPLETE))
    {
        _valid = false;
    }
    else
    {
        _valid = true;
    }
}

file_info::file_info(const apr_finfo_t& s) : _stat(s), _valid(true)
{

}

file_info::~file_info()
{

}

bool file_info::valid()
{
    return _valid;
}

bool file_info::valid(int flag)
{
    if (_stat.valid & flag)
    {
        return true;
    }

    return false;
}

apr_filetype_e file_info::type() const
{
    return _stat.filetype;
}

apr_uid_t file_info::uid() const
{
    return _stat.user;
}

apr_gid_t file_info::gid() const
{
    return _stat.group;
}

apr_dev_t file_info::device() const
{
    return _stat.device;
}

apr_ino_t file_info::inode() const
{
    return _stat.inode;
}

apr_int32_t file_info::nlink() const
{
    return _stat.nlink;
}

apr_off_t file_info::size() const
{
    return _stat.size;
}

apr_int32_t file_info::perms() const
{
    return _stat.protection;
}

apr_off_t file_info::csize() const
{
    return _stat.csize;
}

apr_off_t file_info::atime() const
{
    return _stat.atime;
}

apr_off_t file_info::ctime() const
{
    return _stat.ctime;
}

apr_off_t file_info::mtime() const
{
    return _stat.mtime;
}

const char* file_info::name() const
{
    if (_stat.valid & APR_FINFO_NAME)
    {
        return _stat.name;
    }

    return NULL;
}

const char* file_info::fname() const
{
    return _stat.fname;
}

string file_info::md5()
{
    modruby::md5 md5h(_stat.fname);

    return md5h.result();
}

string file_info::sha1()
{
    modruby::sha1 sha;
    sha.hashfile(_stat.fname);

    return sha.result();
}

} // end namespace apr
} // end namespace modruby
