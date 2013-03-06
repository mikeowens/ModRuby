#ifndef MODRUBY_SWAP_ENDIAN
#define MODRUBY_SWAP_ENDIAN

// from http://www.gdargaud.net/Hack/SourceCode.html

namespace modruby {

namespace endian {
extern const int one;
}

// Swap the byte order of a structure
extern void* swap_endian(void* addr, const int size);

} // end namespace modruby

#define IS_BIGENDIAN    (*(char *)(&modruby::endian::one)==0)
#define IS_LITTLEENDIAN (*(char *)(&modruby::endian::one)==1)

#endif
