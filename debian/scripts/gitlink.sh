#!/bin/bash

# This script takes a file of sylinks and creates them in the current
# directory. The symlink file is of the form:
#
#  {target_path} {link_path}
#
# That is, each line defined a single symlink. If the link_path does not exist,
# it will create it relative to the current directory.
#
# The target_path is relative the global source directory (/var/data/dev/source)
# and the project name (second argument of script). From these it expands every
# $target_path to /var/data/dev/source/$project_name/$target_path for each
# entry.
#
# For example, say you have a file with the following contents:
#
#   debian/r4a.conf etc/apache2/mods-available/
#   debian/r4a.load etc/apache2/mods-available/
#   util/4ra usr/bin/
#   apache/librhtml.so usr/lib/
#   apache/mod_r4a.so usr/lib/apache2/modules/
#   debian/r4a-dev/usr/lib/r4a/test usr/lib/r4a/test
#   lib/core/r4a usr/lib/ruby/1.9.1/
#   lib/core/r4a.rb usr/lib/ruby/1.9.1/
#   rsp/src/rsp.rb usr/lib/ruby/1.9.1/
#   rsp/src/rsp usr/lib/ruby/1.9.1/
#
# This script will create the following directory structure in the current directory:
#
# etc
# etc/apache2
# etc/apache2/mods-available
# etc/apache2/mods-available/r4a.conf -> /var/data/dev/source/r4a/debian/r4a.conf
# etc/apache2/mods-available/r4a.load -> /var/data/dev/source/r4a/debian/r4a.load
# usr
# usr/bin
# usr/bin/4ra -> /var/data/dev/source/r4a/util/4ra
# usr/lib
# usr/lib/librhtml.so -> /var/data/dev/source/r4a/apache/librhtml.so
# usr/lib/apache2
# usr/lib/apache2/modules/mod_r4a.so -> /var/data/dev/source/r4a/apache/mod_r4a.so
# usr/lib/r4a
# usr/lib/r4a/test
# usr/lib/r4a/test/test -> /var/data/dev/source/r4a/debian/r4a-dev/usr/lib/r4a/test
# usr/lib/ruby
# usr/lib/ruby/1.9.1
# usr/lib/ruby/1.9.1/r4a -> /var/data/dev/source/r4a/lib/core/r4a
# usr/lib/ruby/1.9.1/r4a.rb -> /var/data/dev/source/r4a/lib/core/r4a.rb
# usr/lib/ruby/1.9.1/rsp -> /var/data/dev/source/r4a/rsp/src/rsp
# usr/lib/ruby/1.9.1/rsp.rb -> /var/data/dev/source/r4a/rsp/src/rsp.rb

ARGS=2        # Number of arguments expected.
E_BADARGS=65  # Exit value if incorrect number of args passed.
msg="Usage: `basename $0` {file_name} #{project_dir}" 

# Make sure args were give
test $# -ne $ARGS && echo $msg && exit $E_BADARGS

project_name=$2
source_dir=/var/data/dev/source/active/$project_name

# Check that the file is a regular file and exists
if [ ! -f $1 ]
then
    echo "Error: file $1 does not exist"
    exit 1
fi

# Read the file
while read curline; do
    set -- $curline
    echo $1 '->' $2

    # Compute the target directory. If $2 does not end in a slash, it is a
    # file. In that case, we went the file's direname.
    if [[ $2 != */ ]] 
    then
        dir=`dirname $2`
    else
        # Else this is a directory
        dir=$2
    fi

    # Now check that taget directory exists
    if [ ! -d $dir ]
    then
        # Doesn't. So make it.
        mkdir -p $dir
    fi

    ls usr/lib/r4a/test/

    # Create symlink
    ln -sf $source_dir/$1 $2
done < "$1"
