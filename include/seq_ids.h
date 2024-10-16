#ifndef SEQ_IDS_H
#define SEQ_IDS_H

// Sometimes a sequence id is represented as one of the below ids (the base id),
// optionally OR'd with SEQ_VARIATION.
#define SEQ_BASE_ID 0x7f
#define SEQ_VARIATION 0x80
#define SEQUENCE_NONE 0xFF

#define SEQ_MENU_GAME_OVER (SEQ_MENU_TITLE_SCREEN | SEQ_VARIATION)

enum SeqId {
    SEQ_SOUND_PLAYER,                  // 0x00
    SEQ_EVENT_CUTSCENE_COLLECT_STAR,   // 0x01
    SEQ_MENU_TITLE_SCREEN,             // 0x02
    SEQ_LEVEL_GRASS,                   // 0x03
    SEQ_LEVEL_INSIDE_CASTLE,           // 0x04
    SEQ_LEVEL_WATER,                   // 0x05
    SEQ_LEVEL_HOT,                     // 0x06
    SEQ_LEVEL_BOSS_KOOPA,              // 0x07
    SEQ_LEVEL_SNOW,                    // 0x08
    SEQ_LEVEL_SLIDE,                   // 0x09
    SEQ_LEVEL_SPOOKY,                  // 0x0A
    SEQ_EVENT_PIRANHA_PLANT,           // 0x0B
    SEQ_LEVEL_UNDERGROUND,             // 0x0C
    SEQ_MENU_STAR_SELECT,              // 0x0D
    SEQ_EVENT_POWERUP,                 // 0x0E
    SEQ_EVENT_METAL_CAP,               // 0x0F
    SEQ_EVENT_KOOPA_MESSAGE,           // 0x10
    SEQ_LEVEL_KOOPA_ROAD,              // 0x11
    SEQ_EVENT_HIGH_SCORE,              // 0x12
    SEQ_EVENT_MERRY_GO_ROUND,          // 0x13
    SEQ_EVENT_RACE,                    // 0x14
    SEQ_EVENT_CUTSCENE_STAR_SPAWN,     // 0x15
    SEQ_EVENT_BOSS,                    // 0x16
    SEQ_EVENT_CUTSCENE_COLLECT_KEY,    // 0x17
    SEQ_EVENT_ENDLESS_STAIRS,          // 0x18
    SEQ_LEVEL_BOSS_KOOPA_FINAL,        // 0x19
    SEQ_EVENT_CUTSCENE_CREDITS,        // 0x1A
    SEQ_EVENT_SOLVE_PUZZLE,            // 0x1B
    SEQ_EVENT_TOAD_MESSAGE,            // 0x1C
    SEQ_EVENT_PEACH_MESSAGE,           // 0x1D
    SEQ_EVENT_CUTSCENE_INTRO,          // 0x1E
    SEQ_EVENT_CUTSCENE_VICTORY,        // 0x1F
    SEQ_EVENT_CUTSCENE_ENDING,         // 0x20
    SEQ_MENU_FILE_SELECT,              // 0x21
    SEQ_EVENT_CUTSCENE_LAKITU,         // 0x22 (not in JP)
    SEQ_KALIMARI_DESERT,               // 0x23
    SEQ_CROSSING_THOSE_HILLS,          // 0x24
    SEQ_ROUTE_203,                     // 0x25
    SEQ_SMO_SAND_KINGDOM,              // 0x26
    SEQ_REDC_MANSION_BASEMENT,         // 0x27
    SEQ_CASCADE_CAPERS_WATERFALL,      // 0x28   
    SEQ_SKY_HIGH_GRAND_NUAGE,          // 0x29
    SEQ_BEAT_THE_HEAT,                 // 0x2A
    SEQ_LIVING_WATERWAY,               // 0x2B
    SEQ_COSTA_DEL_SOL,                 // 0x2C
    SEQ_CONFRONTING_MYSELF,            // 0x2D
    SEQ_DRACULAS_TEARS,                // 0x2E
    SEQ_EGYPTIAN_DUEL,                 // 0x2F
    SEQ_FIRE_ISLAND_VOLCANO,           // 0x30
    SEQ_FIELD,                         // 0x31
    SEQ_RAY_OF_HOPE,                   // 0x32  
    SEQ_SAVANNAH_DESERT,               // 0x33
    SEQ_ARID_SANDS,                    // 0x34
    SEQ_RAIN_FORMERLY_KNOWN_AS_PURPLE, // 0x35
    SEQ_PIZZA_TIME,                    // 0x36
    SEQ_SEA_ME_NOW,                    // 0x37
    SEQ_PEPPERMAN_STRIKES,             // 0x38
    SEQ_JELLYFISH_FIELDS,              // 0x39
    SEQ_CHINATOWN,                     // 0x3A
    SEQ_FROM_RUSSIA_WITH_LOVE,         // 0x3B
    SEQ_NSANITY_ISLAND,                // 0x3C
    SEQ_SONIC_DROWNING,                // 0x3D
    SEQ_SM64DS_CASINO,                 // 0x3E
    SEQ_GRASS_N_SEAS,                  // 0x3F
    SEQ_POP_N_BUBBLE_INC,              // 0x40
    SEQ_SELECT_YOUR_LEVEL,             // 0x41
    SEQ_SHOP_THEME,                    // 0x42
    SEQ_THE_MEAT_GRINDER,              // 0x43
    SEQ_XBC_MECHONIS_FIELD,            // 0x44
    SEQ_PKMN_RSE_MT_CHIMNEY,           // 0x45
    SEQ_MGTT_FOREST_STAR,              // 0x46

    SEQ_COUNT,
};

#endif // SEQ_IDS_H
