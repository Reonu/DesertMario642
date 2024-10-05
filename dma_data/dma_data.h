#include "types.h"
#include "macros.h"

struct DMAVideoProps {
    const Texture *address;
    u32 compressedSize;
};

extern ALIGNED16 const struct DMAVideoProps course1_video_data[291];
