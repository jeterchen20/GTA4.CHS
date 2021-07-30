#pragma once

typedef unsigned int uint;
typedef unsigned char uchar;

struct pgPtr {
    int offset : 28;
    int type : 4;
    //if (type == 5) {   
    //    local uint doffset = FTell();
    //    FSeek(offset);
    //    byte DataStart;
    //    FSeek(doffset);
    //}
};
