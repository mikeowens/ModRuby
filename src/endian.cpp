#include "endian.h"

namespace modruby {

void* swap_endian(void* addr, const int size)
{
    static char buf[16];

    switch (size) {

        case 2:    buf[0]=*((char*)addr+1);
                buf[1]=*((char*)addr  );
                break;

        case 4:    buf[0]=*((char*)addr+3);
                buf[1]=*((char*)addr+2);
                buf[2]=*((char*)addr+1);
                buf[3]=*((char*)addr  );
                break;

        case 8:    buf[0]=*((char*)addr+7);
                buf[1]=*((char*)addr+6);
                buf[2]=*((char*)addr+5);
                buf[3]=*((char*)addr+4);
                buf[4]=*((char*)addr+3);
                buf[5]=*((char*)addr+2);
                buf[6]=*((char*)addr+1);
                buf[7]=*((char*)addr  );
                break;

        case 16:buf[0]=*((char*)addr+15);
                buf[1]=*((char*)addr+14);
                buf[2]=*((char*)addr+13);
                buf[3]=*((char*)addr+12);
                buf[4]=*((char*)addr+11);
                buf[5]=*((char*)addr+10);
                buf[6]=*((char*)addr+9);
                buf[7]=*((char*)addr+8);
                buf[8]=*((char*)addr+7);
                buf[9]=*((char*)addr+6);
                buf[10]=*((char*)addr+5);
                buf[11]=*((char*)addr+4);
                buf[12]=*((char*)addr+3);
                buf[13]=*((char*)addr+2);
                buf[14]=*((char*)addr+1);
                buf[15]=*((char*)addr  );
                break;
    }

    return (void*)buf;
}

namespace endian { const int one = 1; }

} // end namespace modruby
