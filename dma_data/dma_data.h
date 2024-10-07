#include "types.h"
#include "macros.h"

struct DMAImageProps {
    const Texture *address;
    u32 compressedSize;
};

enum ImageDMAIDs {
    BB_IMAGE_AMOGUS,
    BB_IMAGE_ASS_IMPACT,
    BB_IMAGE_BART,
    BB_IMAGE_BETTERCALLSHY,
    BB_IMAGE_APPY,
    BB_IMAGE_COMIT_ANTIS,
    BB_IMAGE_COMIT_STANS,
    BB_IMAGE_CROSS,
    BB_IMAGE_FARM,
    BB_IMAGE_OATMEAL,
    BB_IMAGE_YOUTUBE,
    BB_IMAGE_HACKERN64,
    BB_IMAGE_IDIOT,
    BB_IMAGE_JOEL,
    BB_IMAGE_MAKE_CLEAN,
    BB_IMAGE_MALTINGIUS,
    BB_IMAGE_MOTH,
    BB_IMAGE_MOTIVATE,
    BB_IMAGE_COMIT,
    BB_IMAGE_MVC,
    BB_IMAGE_MVH,
    BB_IMAGE_NINTENDO_EMPLOYEES,
    BB_IMAGE_PATCHMAKER,
    BB_IMAGE_PREDATOR,
    BB_IMAGE_MUSHROOM,
    BB_IMAGE_ROVERT,
    BB_IMAGE_SIMPLEFLIPS,
    BB_IMAGE_SPK,
    BB_IMAGE_SPOON,
    BB_IMAGE_CHEEZEPIN,
    BB_IMAGE_SYNERGY,
    BB_IMAGE_TCS,
    BB_IMAGE_COZIES,
    BB_IMAGE_XENOBLADE,
    BB_IMAGE_XENOGEARS,
    BB_IMAGE_YUGAMINEENA,
    BB_IMAGE_GAMER_TYPE,
    BB_IMAGE_ROSALU,
    BB_IMAGE_BLOCKINGTON,
    BB_IMAGE_DESERT_MARIO,

    BB_IMAGE_COUNT,
};

enum VideoDMAIDs {
    BB_VIDEO_IMAGES, // Used as the 'video data' for individual images
    BB_VIDEO_FREE_PSP,
    BB_VIDEO_GBJ_PAINTING,
    BB_VIDEO_LEON,
    BB_VIDEO_FELIZ_JEUVES,

    BB_VIDEO_COUNT,
};

extern const struct DMAImageProps dma_image_data[BB_IMAGE_COUNT];

extern const struct DMAImageProps free_psp_video_data[13];
extern const struct DMAImageProps gbj_painting_video_data[77];
extern const struct DMAImageProps leon_video_data[122];
extern const struct DMAImageProps feliz_jeuves_video_data[33];
