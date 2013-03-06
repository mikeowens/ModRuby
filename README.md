# mod_ruby

## About

This is an implementation from the ground up which embeds Ruby into a dedicated
Apache module. It has no relataion to the original mod_ruby
(https://github.com/shugo/mod_ruby). It is written from scratch specifically for
Apache 2.x.

mod_ruby makes it possible for you to run Ruby natively in Apache. You run
straight up Ruby in CGI as well as create custom Apache handlers in which to
plug in your own framework.

mod_ruby also includes a fast, embedded RHTML parser implemented in C which
works exactly like eRuby.

## Building

This was developed on Ubuntu Linux, FreeBSD, and Mac OSX. It has not been ported
to Windows. To build, you need to the following packages:

  * Ruby 1.9 or 2.0
  * Apache source code
  * CMake

On Ubuntu, you can build as follows:

    bash $ /usr/lib/pbuilder/pbuilder-satisfydepends
    bash $ fakeroot debian/rules clean
    bash $ dpkg-buildpackage
  
On other systems you can build generally as follows:

    bash $ cmake .
    bash $ make
    bash $ make install

## Additional Information

Redistribution and use in source and binary forms, with or without modification,
are permitted under the terms of the Apache License Version 2.0. Copyright
information is located in the COPYING file. The software license is located in
the LICENSE file.

## Credits

Jesus Christ, the Son of God, my Lord and Savior:

    The true light, which enlightens everyone, was coming into the world. He was
    in the world, and the world was made through him, yet the world did not know
    him. He came to his own, and his own people did not receive him. But to all
    who did receive him, who believed in his name, he gave the right to become
    children of God, who were born, not of blood nor of the will of the flesh
    nor of the will of man, but of God.

                                                            -- John 1:9-13 (ESV)
