#pragma once

#include <QString>
#include <QStringList>
#include <QMap>

enum JJ2Event {                             // InXL Parameters
    JJ2_EMPTY                       = 0x00, // Y    -
    JJ2_MODIFIER_ONE_WAY            = 0x01, // Y    -
    JJ2_MODIFIER_HURT               = 0x02, // Y    -
    JJ2_MODIFIER_VINE               = 0x03, // Y    -
    JJ2_MODIFIER_HOOK               = 0x04, // Y    -
    JJ2_MODIFIER_SLIDE              = 0x05, // Y    -
    JJ2_MODIFIER_H_POLE             = 0x06, // Y    -
    JJ2_MODIFIER_V_POLE             = 0x07, // Y    -
    JJ2_AREA_FLY_OFF                = 0x08, // Y    -
    JJ2_MODIFIER_RICOCHET           = 0x09, // Y    -
    JJ2_MODIFIER_BELT_RIGHT         = 0x0A, //      Speed: 8s
    JJ2_MODIFIER_BELT_LEFT          = 0x0B, //      Speed: 8s
    JJ2_MODIFIER_ACC_BELT_RIGHT     = 0x0C, //      Speed: 8s
    JJ2_MODIFIER_ACC_BELT_LEFT      = 0x0D, //      Speed: 8s
    JJ2_AREA_STOP_ENEMY             = 0x0E, // Y    -
    JJ2_MODIFIER_WIND_LEFT          = 0x0F, //      Speed: 8s
    JJ2_MODIFIER_WIND_RIGHT         = 0x10, //      Speed: 8s
    JJ2_AREA_EOL                    = 0x11, // Y    Secret: bool
    JJ2_AREA_EOL_WARP               = 0x12, // Y    -
    JJ2_AREA_REVERT_MORPH           = 0x13, // Y    -
    JJ2_AREA_FLOAT_UP               = 0x14, // Y    -
    JJ2_TRIGGER_ROCK                = 0x15, //      ID: 8u
    JJ2_LIGHT_DIM                   = 0x16, // Y    -
    JJ2_LIGHT_SET                   = 0x17, // Y    Intensity: 7u, Red: 4s, Green: 4s, Blue: 4s, Flicker: bool
    JJ2_AREA_LIMIT_X_SCROLL         = 0x18, //      -
    JJ2_LIGHT_RESET                 = 0x19, // Y    -
    JJ2_AREA_SECRET_WARP            = 0x1A, //      Coins: 10u
    JJ2_MODIFIER_ECHO               = 0x1B, //      Amount: 8u
    JJ2_AREA_ACTIVATE_BOSS          = 0x1C, //      Music 2: bool
    JJ2_JAZZ_LEVEL_START            = 0x1D, // Y    Pos: 4u *CHECK*
    JJ2_SPAZ_LEVEL_START            = 0x1E, // Y    Pos: 4u *CHECK*
    JJ2_MP_LEVEL_START              = 0x1F, // Y    Team: bool
    JJ2_AMMO_FREEZER                = 0x21, // Y    -
    JJ2_AMMO_BOUNCER                = 0x22, // Y    -
    JJ2_AMMO_SEEKER                 = 0x23, // Y    -
    JJ2_AMMO_RF                     = 0x24, // Y    -
    JJ2_AMMO_TOASTER                = 0x25, // Y    -
    JJ2_AMMO_TNT                    = 0x26, // Y    -
    JJ2_AMMO_PEPPER                 = 0x27, // Y    -
    JJ2_AMMO_ELECTRO                = 0x28, // Y    -
    JJ2_TURTLE_SHELL                = 0x29, //      -
    JJ2_SWINGING_VINE               = 0x2A, //      -
    JJ2_SCENERY_BOMB                = 0x2B, //      -
    JJ2_COIN_SILVER                 = 0x2C, // Y    -
    JJ2_COIN_GOLD                   = 0x2D, // Y    -
    JJ2_CRATE_AMMO                  = 0x2E, // Y    Extra Event: 8u, Num. Extra Events: 4u, Random Fly: bool
    JJ2_CRATE_CARROT                = 0x2F, // Y    Extra Event: 8u, Num. Extra Events: 4u, Random Fly: bool
    JJ2_CRATE_ONEUP                 = 0x30, // Y    Extra Event: 8u, Num. Extra Events: 4u, Random Fly: bool
    JJ2_BARREL_GEM                  = 0x31, // Y    Red: 4u, Green: 4u, Blue: 4u, Purple: 4u
    JJ2_BARREL_CARROT               = 0x32, // Y    -
    JJ2_BARREL_ONEUP                = 0x33, // Y    -
    JJ2_CRATE_BOMB                  = 0x34, // Y    Extra Event: 8u, Num. Extra Events: 4u, Random Fly: bool
    JJ2_CRATE_AMMO_FREEZER          = 0x35, // Y    -
    JJ2_CRATE_AMMO_BOUNCER          = 0x36, // Y    -
    JJ2_CRATE_AMMO_SEEKER           = 0x37, // Y    -
    JJ2_CRATE_AMMO_RF               = 0x38, // Y    -
    JJ2_CRATE_AMMO_TOASTER          = 0x39, // Y    -
    JJ2_SCENERY_TNT                 = 0x3A, //      -
    JJ2_AIRBOARD                    = 0x3B, // Y    -
    JJ2_SPRING_GREEN_FROZEN         = 0x3C, // Y    Ceiling: bool, Keep X Speed: bool, Keep Y Speed: bool, Delay: 4u
    JJ2_FAST_FIRE                   = 0x3D, // Y    -
    JJ2_CRATE_SPRING                = 0x3E, // Y    Extra Event: 8u, Num. Extra Events: 4u, Random Fly: bool
    JJ2_GEM_RED                     = 0x3F, // Y    -
    JJ2_GEM_GREEN                   = 0x40, // Y    -
    JJ2_GEM_BLUE                    = 0x41, // Y    -
    JJ2_GEM_PURPLE                  = 0x42, // Y    -
    JJ2_GEM_SUPER                   = 0x43, // Y    -
    JJ2_BIRDY                       = 0x44, //      Chuck: bool
    JJ2_BARREL_AMMO                 = 0x45, // Y    -
    JJ2_CRATE_GEM                   = 0x46, //      Red: 4u, Green: 4u, Blue: 4u, Purple: 4u
    JJ2_POWERUP_SWAP                = 0x47, // Y    -
    JJ2_CARROT                      = 0x48, // Y    -
    JJ2_CARROT_FULL                 = 0x49, // Y    -
    JJ2_SHIELD_FIRE                 = 0x4A, // Y    -
    JJ2_SHIELD_WATER                = 0x4B, // Y    -
    JJ2_SHIELD_LIGHTNING            = 0x4C, // Y    -
    JJ2_MAX_WEAPON                  = 0x4D, //      -
    JJ2_AREA_AUTO_FIRE              = 0x4E, //      -
    JJ2_FAST_FEET                   = 0x4F, // Y    -
    JJ2_ONEUP                       = 0x50, // Y    -
    JJ2_EOL_SIGN                    = 0x51, //      Secret: bool; *CHECK* function
    JJ2_SAVE_POINT                  = 0x53, // Y    -
    JJ2_BONUS_SIGN                  = 0x54, //      -
    JJ2_SPRING_RED                  = 0x55, // Y    Ceiling: bool, Keep X Speed: bool, Keep Y Speed: bool, Delay: 4u
    JJ2_SPRING_GREEN                = 0x56, // Y    Ceiling: bool, Keep X Speed: bool, Keep Y Speed: bool, Delay: 4u
    JJ2_SPRING_BLUE                 = 0x57, // Y    Ceiling: bool, Keep X Speed: bool, Keep Y Speed: bool, Delay: 4u
    JJ2_CARROT_INVINCIBLE           = 0x58, // Y    -
    JJ2_SHIELD_TIME                 = 0x59, // Y    -
    JJ2_FREEZE                      = 0x5A, // Y    -
    JJ2_SPRING_RED_HOR              = 0x5B, // Y    Keep X Speed: bool, Keep Y Speed: bool, Delay: 4u
    JJ2_SPRING_GREEN_HOR            = 0x5C, // Y    Keep X Speed: bool, Keep Y Speed: bool, Delay: 4u
    JJ2_SPRING_BLUE_HOR             = 0x5D, // Y    Keep X Speed: bool, Keep Y Speed: bool, Delay: 4u
    JJ2_POWERUP_BIRD                = 0x5E, // Y    -
    JJ2_TRIGGER_CRATE               = 0x5F, // Y    Trigger ID: 5u
    JJ2_CARROT_FLY                  = 0x60, // Y    -
    JJ2_GEM_RED_RECT                = 0x61, // Y    -
    JJ2_GEM_GREEN_RECT              = 0x62, // Y    -
    JJ2_GEM_BLUE_RECT               = 0x63, // Y    -
    JJ2_ENEMY_TUF_TURT              = 0x64, // Y    -
    JJ2_BOSS_TUF_TURT               = 0x65, //      Text: 4u
    JJ2_ENEMY_LAB_RAT               = 0x66, //      -
    JJ2_ENEMY_DRAGON                = 0x67, //      -
    JJ2_ENEMY_LIZARD                = 0x68, // Y    -
    JJ2_ENEMY_BEE                   = 0x69, //      -
    JJ2_ENEMY_RAPIER                = 0x6A, //      -
    JJ2_ENEMY_SPARKS                = 0x6B, //      -
    JJ2_ENEMY_BAT                   = 0x6C, //      -
    JJ2_ENEMY_SUCKER                = 0x6D, //      -
    JJ2_ENEMY_CATERPILLAR           = 0x6E, //      -
    JJ2_CHESHIRE_HOOK               = 0x6F, //      - *CHECK* verify which one was hook
    JJ2_CHESHIRE_2                  = 0x70, //      Duration: 8u *CHECK*
    JJ2_ENEMY_MADDER_HATTER         = 0x71, //      -
    JJ2_BOSS_BILSY                  = 0x72, //      Text: 4u
    JJ2_ENEMY_SKELETON              = 0x73, //      -
    JJ2_ENEMY_DOGGY_DOGG            = 0x74, //      -
    JJ2_ENEMY_TURTLE_NORMAL         = 0x75, // Y    -
    JJ2_ENEMY_HELMUT                = 0x76, //      -
    JJ2_LEAF                        = 0x77, //      -
    JJ2_ENEMY_DEMON                 = 0x78, //      -
    JJ2_FIRE                        = 0x79, //      -
    JJ2_LAVA                        = 0x7A, //      -
    JJ2_ENEMY_DRAGONFLY             = 0x7B, //      -
    JJ2_ENEMY_MONKEY                = 0x7C, //      -
    JJ2_ENEMY_FAT_CHICK             = 0x7D, //      -
    JJ2_ENEMY_FENCER                = 0x7E, //      -
    JJ2_ENEMY_FISH                  = 0x7F, //      -
    JJ2_MOTH                        = 0x80, //      Type: 3u
    JJ2_STEAM                       = 0x81, //      -
    JJ2_ROTATING_ROCK               = 0x82, //      Rock ID: 8u, X Speed: 4s, Y Speed: 4s
    JJ2_POWERUP_BLASTER             = 0x83, // Y    -
    JJ2_POWERUP_BOUNCER             = 0x84, // Y    -
    JJ2_POWERUP_FREEZER             = 0x85, // Y    -
    JJ2_POWERUP_SEEKER              = 0x86, // Y    -
    JJ2_POWERUP_RF                  = 0x87, // Y    -
    JJ2_POWERUP_TOASTER             = 0x88, // Y    -
    JJ2_PINBALL_PADDLE_L            = 0x89, //      -
    JJ2_PINBALL_PADDLE_R            = 0x8A, //      -
    JJ2_PINBALL_BUMP_500            = 0x8B, //      -
    JJ2_PINBALL_BUMP_CARROT         = 0x8C, //      -
    JJ2_FOOD_APPLE                  = 0x8D, // Y    -
    JJ2_FOOD_BANANA                 = 0x8E, // Y    -
    JJ2_FOOD_CHERRY                 = 0x8F, // Y    -
    JJ2_FOOD_ORANGE                 = 0x90, // Y    -
    JJ2_FOOD_PEAR                   = 0x91, // Y    -
    JJ2_FOOD_PRETZEL                = 0x92, // Y    -
    JJ2_FOOD_STRAWBERRY             = 0x93, // Y    -
    JJ2_LIGHT_STEADY                = 0x94, // Y    -
    JJ2_LIGHT_PULSE                 = 0x95, // Y    Speed: 8u, Sync: 4u
    JJ2_LIGHT_FLICKER               = 0x96, // Y    Sample: 8u
    JJ2_BOSS_QUEEN                  = 0x97, //      Text: 4u
    JJ2_ENEMY_SUCKER_FLOAT          = 0x98, //      -
    JJ2_BRIDGE                      = 0x99, //      Width: 4u, Type: 3u, Toughness: 4u
    JJ2_FOOD_LEMON                  = 0x9A, // Y    -
    JJ2_FOOD_LIME                   = 0x9B, // Y    -
    JJ2_FOOD_THING                  = 0x9C, // Y    - *CHECK* give a better name
    JJ2_FOOD_WATERMELON             = 0x9D, // Y    -
    JJ2_FOOD_PEACH                  = 0x9E, // Y    -
    JJ2_FOOD_GRAPES                 = 0x9F, // Y    -
    JJ2_FOOD_LETTUCE                = 0xA0, // Y    -
    JJ2_FOOD_EGGPLANT               = 0xA1, // Y    -
    JJ2_FOOD_CUCUMBER               = 0xA2, // Y    -
    JJ2_FOOD_PEPSI                  = 0xA3, // Y    - reverse if necessary
    JJ2_FOOD_COKE                   = 0xA4, // Y    -
    JJ2_FOOD_MILK                   = 0xA5, // Y    -
    JJ2_FOOD_PIE                    = 0xA6, // Y    -
    JJ2_FOOD_CAKE                   = 0xA7, // Y    -
    JJ2_FOOD_DONUT                  = 0xA8, // Y    -
    JJ2_FOOD_CUPCAKE                = 0xA9, // Y    -
    JJ2_FOOD_CHIPS                  = 0xAA, // Y    -
    JJ2_FOOD_CANDY                  = 0xAB, // Y    -
    JJ2_FOOD_CHOCOLATE              = 0xAC, // Y    -
    JJ2_FOOD_ICE_CREAM              = 0xAD, // Y    -
    JJ2_FOOD_BURGER                 = 0xAE, // Y    -
    JJ2_FOOD_PIZZA                  = 0xAF, // Y    -
    JJ2_FOOD_FRIES                  = 0xB0, // Y    -
    JJ2_FOOD_CHICKEN_LEG            = 0xB1, // Y    -
    JJ2_FOOD_SANDWICH               = 0xB2, // Y    -
    JJ2_FOOD_TACO                   = 0xB3, // Y    -
    JJ2_FOOD_HOT_DOG                = 0xB4, // Y    -
    JJ2_FOOD_HAM                    = 0xB5, // Y    -
    JJ2_FOOD_CHEESE                 = 0xB6, // Y    -
    JJ2_ENEMY_LIZARD_FLOAT          = 0xB7, // Y    Copter Duration: 8u, Copter Drop: bool
    JJ2_ENEMY_MONKEY_STAND          = 0xB8, //      -
    JJ2_SCENERY_DESTRUCT            = 0xB9, // Y    Empty: 10u, SpeedDestr: 5u, Weapon: 4u
    JJ2_SCENERY_DESTR_BOMB          = 0xBA, // Y    Empty: 10 + 5 + 4u
    JJ2_SCENERY_COLLAPSE            = 0xBB, // Y    Wait: 10u, FPS: 5u, Empty: 4u
    JJ2_SCENERY_BUTTSTOMP           = 0xBC, // Y    Empty: 10 + 5 + 4u
    JJ2_SCENERY_GEMSTOMP            = 0xBD, //      -
    JJ2_ENEMY_RAVEN                 = 0xBE, //      -
    JJ2_ENEMY_TURTLE_TUBE           = 0xBF, // Y    -
    JJ2_GEM_RING                    = 0xC0, // Y    Length: 5u, Speed: 5u
    JJ2_SMALL_TREE                  = 0xC1, //      Adjust Y: 5u, Adjust X: 6s
    JJ2_AMBIENT_SOUND               = 0xC2, //      Sample: 8u, Amplify: 8u, Fade: bool, Sine: bool
    JJ2_BOSS_UTERUS                 = 0xC3, //      Text: 4u
    JJ2_ENEMY_CRAB                  = 0xC4, //      -
    JJ2_ENEMY_WITCH                 = 0xC5, //      -
    JJ2_BOSS_TURTLE_ROCKET          = 0xC6, //      Text: 4u
    JJ2_BOSS_BUBBA                  = 0xC7, //      Text: 4u
    JJ2_BOSS_DEVAN_DEVIL            = 0xC8, //      Text: 4u
    JJ2_BOSS_DEVAN_ROBOT            = 0xC9, //      Intro Text: 4u, Text: 4u
    JJ2_BOSS_ROBOT                  = 0xCA, //      -
    JJ2_POLE_CARROTUS               = 0xCB, //      Adjust Y: 5u, Adjust X: 6s
    JJ2_POLE_PSYCH                  = 0xCC, //      Adjust Y: 5u, Adjust X: 6s
    JJ2_POLE_DIAMONDUS              = 0xCD, //      Adjust Y: 5u, Adjust X: 6s
    JJ2_MODIFIER_TUBE               = 0xCE, //      X Speed: 7s, Y Speed: 7s, Trig Sample (unknown) 3u, Wait Time: 3u
    JJ2_TEXT                        = 0xCF, //      Text: 8u, Vanish: bool
    JJ2_MODIFIER_SET_WATER          = 0xD0, //      Height: 8u, Instant: bool
    JJ2_PLATFORM_FRUIT              = 0xD1, //      Sync: 2u, Speed: 6s, Length: 4u, Swing: bool
    JJ2_PLATFORM_BOLL               = 0xD2, //      Sync: 2u, Speed: 6s, Length: 4u, Swing: bool
    JJ2_PLATFORM_GRASS              = 0xD3, //      Sync: 2u, Speed: 6s, Length: 4u, Swing: bool
    JJ2_PLATFORM_PINK               = 0xD4, //      Sync: 2u, Speed: 6s, Length: 4u, Swing: bool
    JJ2_PLATFORM_SONIC              = 0xD5, //      Sync: 2u, Speed: 6s, Length: 4u, Swing: bool
    JJ2_PLATFORM_SPIKE              = 0xD6, //      Sync: 2u, Speed: 6s, Length: 4u, Swing: bool
    JJ2_BOLL_SPIKE                  = 0xD7, //      Sync: 2u, Speed: 6s, Length: 4u, Swing: bool
    JJ2_MODIFIER_GENERATOR          = 0xD8, //      Event: 8u, Delay: 5u
    JJ2_EVA                         = 0xD9, //      -
    JJ2_SCENERY_BUBBLER             = 0xDA, //      Speed: 4u
    JJ2_POWERUP_TNT                 = 0xDB, // Y    -
    JJ2_POWERUP_PEPPER              = 0xDC, // Y    -
    JJ2_POWERUP_ELECTRO             = 0xDD, // Y    -
    JJ2_AREA_MORPH_FROG             = 0xDE, //      -
    JJ2_BOLL_SPIKE_3D               = 0xDF, //      Sync: 2u, Speed: 6s, Length: 4u, Swing: bool, Shade: bool
    JJ2_SPRINGCORD                  = 0xE0, //      -
    JJ2_ENEMY_BEE_SWARM             = 0xE1, //      Num: 8u
    JJ2_COPTER                      = 0xE2, //      Duration: 8u
    JJ2_SHIELD_LASER                = 0xE3, // Y    -
    JJ2_STOPWATCH                   = 0xE4, //      -
    JJ2_POLE_JUNGLE                 = 0xE5, //      Adjust Y: 5u, Adjust X: 6s
    JJ2_WARP_ORIGIN                 = 0xE6, // Y    ID: 8u, Coins: 8u, Set Lap: bool, Show Anim: bool
    JJ2_PUSHABLE_ROCK               = 0xE7, // Y    -
    JJ2_PUSHABLE_BOX                = 0xE8, // Y    -
    JJ2_WATER_BLOCK                 = 0xE9, //      Adjust Y: 8s
    JJ2_TRIGGER_AREA                = 0xEA, // Y    Trigger ID: 5u
    JJ2_BOSS_BOLLY                  = 0xEB, //      -
    JJ2_ENEMY_BUTTERFLY             = 0xEC, //      -
    JJ2_ENEMY_BEEBOY                = 0xED, //      Swarm: 8u
    JJ2_SNOW                        = 0xEE, //      -
    JJ2_WARP_TARGET                 = 0xF0, // Y    ID: 8u
    JJ2_BOSS_TWEEDLE                = 0xF1, //      -
    JJ2_AREA_ID                     = 0xF2, //      Text: 8u
    JJ2_CTF_BASE                    = 0xF4, //      Team: bool, Direction: bool *CHECK*
    JJ2_AREA_NO_FIRE                = 0xF5, //      -
    JJ2_TRIGGER_ZONE                = 0xF6, //      Trigger ID: 5u, Set On: bool, Switch: bool
                                            //     
    // base game unnamed ones               //     
    JJ2_EMPTY_32                    = 0x20, //     
    JJ2_EMPTY_82                    = 0x52, //     
    JJ2_EMPTY_239                   = 0xEF, //     
    JJ2_EMPTY_243                   = 0xF3, //      SP Airboard = ?
    JJ2_EMPTY_247                   = 0xF7, //      Text: 4u
    JJ2_EMPTY_248                   = 0xF8, // Y    -
    JJ2_EMPTY_249                   = 0xF9, // Y    -
    JJ2_EMPTY_250                   = 0xFA, // Y    Copter Duration: 8u, Copter Drop: bool
    JJ2_EMPTY_251                   = 0xFB, //      Text: 4u
    JJ2_EMPTY_252                   = 0xFC, //      -
    JJ2_EMPTY_253                   = 0xFD, //      -
    JJ2_EMPTY_254                   = 0xFE, //     
    JJ2_EMPTY_255                   = 0xFF, //     

    // alternate names for TSF+ additions
    JJ2_BILSY_DUMMY                 = 0xF7,
    JJ2_ENEMY_NORMAL_TURTLE_XMAS    = 0xF8,
    JJ2_ENEMY_LIZARD_XMAS           = 0xF9,
    JJ2_ENEMY_LIZARD_FLOAT_XMAS     = 0xFA,
    JJ2_EMPTY_BOSS_BILSY_XMAS       = 0xFB,
    JJ2_EMPTY_TSF_DOG               = 0xFC,
    JJ2_EMPTY_TSF_GHOST             = 0xFD

};

// as of 15.7.13 1:45
enum PCEvent {
    PC_EMPTY                = 0x0000,
    PC_FAST_FIRE            = 0x0001,
    PC_AMMO_BOUNCER         = 0x0002,
    PC_AMMO_FREEZER         = 0x0003,
    PC_AMMO_SEEKER          = 0x0004,
    PC_AMMO_RF              = 0x0005,
    PC_AMMO_TOASTER         = 0x0006,
    PC_AMMO_TNT             = 0x0007,
    PC_AMMO_PEPPER          = 0x0008,
    PC_AMMO_ELECTRO         = 0x0009,
    PC_POWERUP_BLASTER      = 0x0011, // not implemented
    PC_POWERUP_BOUNCER      = 0x0012, // not implemented
    PC_POWERUP_FREEZER      = 0x0013, // not implemented
    PC_POWERUP_SEEKER       = 0x0014, // not implemented
    PC_POWERUP_RF           = 0x0015, // not implemented
    PC_POWERUP_TOASTER      = 0x0016, // not implemented
    PC_POWERUP_TNT          = 0x0017, // not implemented
    PC_POWERUP_PEPPER       = 0x0018, // not implemented
    PC_POWERUP_ELECTRO      = 0x0019, // not implemented
    PC_GEM_RED              = 0x0040,
    PC_GEM_GREEN            = 0x0041,
    PC_GEM_BLUE             = 0x0042,
    PC_GEM_PURPLE           = 0x0043,
    PC_SAVE_POINT           = 0x005A,
    PC_SPRING_RED           = 0x00C0,
    PC_SPRING_GREEN         = 0x00C1,
    PC_SPRING_BLUE          = 0x00C2,
    PC_JAZZ_LEVEL_START     = 0x0100,
    PC_SPAZ_LEVEL_START     = 0x0101, // not implemented
    PC_LORI_LEVEL_START     = 0x0102, // not implemented
    PC_AREA_EOL             = 0x0108,
    PC_WARP_ORIGIN          = 0x010E,
    PC_WARP_TARGET          = 0x010F,
    PC_MODIFIER_VINE        = 0x0110,
    PC_MODIFIER_ONE_WAY     = 0x0111,
    PC_MODIFIER_HOOK        = 0x0112,
    PC_MODIFIER_H_POLE      = 0x0113,
    PC_MODIFIER_V_POLE      = 0x0114,
    PC_SCENERY_DESTRUCT     = 0x0116,
    PC_SCENERY_BUTTSTOMP    = 0x0117,
    PC_MODIFIER_HURT        = 0x0115,
    PC_LIGHT_SET            = 0x0120,
    PC_LIGHT_RESET          = 0x0125,
    PC_ENEMY_TURTLE_NORMAL  = 0x0180,
    PC_ENEMY_LIZARD         = 0x0184,
    PC_PUSHABLE_ROCK        = 0x005C,
    PC_TRIGGER_CRATE        = 0x005B,
    PC_TRIGGER_AREA         = 0x011A,
    PC_MODIFIER_RICOCHET    = 0x0145,
    PC_BRIDGE               = 0x00C3,
    PC_AREA_STOP_ENEMY      = 0x0143,
    PC_COIN_SILVER          = 0x0048,
    PC_COIN_GOLD            = 0x0049,
    PC_MOVING_PLATFORM      = 0x00C4,
    PC_AREA_FLOAT_UP        = 0x0142,
    PC_MODIFIER_TUBE        = 0x0144,
    PC_FOOD_APPLE           = 0x008D,
    PC_FOOD_BANANA          = 0x008E,
    PC_FOOD_CHERRY          = 0x008F,
    PC_FOOD_ORANGE          = 0x0090,
    PC_FOOD_PEAR            = 0x0091,
    PC_FOOD_PRETZEL         = 0x0092,
    PC_FOOD_STRAWBERRY      = 0x0093,
    PC_FOOD_LEMON           = 0x009A,
    PC_FOOD_LIME            = 0x009B,
    PC_FOOD_THING           = 0x009C,
    PC_FOOD_WATERMELON      = 0x009D,
    PC_FOOD_PEACH           = 0x009E,
    PC_FOOD_GRAPES          = 0x009F,
    PC_FOOD_LETTUCE         = 0x00A0,
    PC_FOOD_EGGPLANT        = 0x00A1,
    PC_FOOD_CUCUMBER        = 0x00A2,
    PC_FOOD_PEPSI           = 0x00A3,
    PC_FOOD_COKE            = 0x00A4,
    PC_FOOD_MILK            = 0x00A5,
    PC_FOOD_PIE             = 0x00A6,
    PC_FOOD_CAKE            = 0x00A7,
    PC_FOOD_DONUT           = 0x00A8,
    PC_FOOD_CUPCAKE         = 0x00A9,
    PC_FOOD_CHIPS           = 0x00AA,
    PC_FOOD_CANDY           = 0x00AB,
    PC_FOOD_CHOCOLATE       = 0x00AC,
    PC_FOOD_ICE_CREAM       = 0x00AD,
    PC_FOOD_BURGER          = 0x00AE,
    PC_FOOD_PIZZA           = 0x00AF,
    PC_FOOD_FRIES           = 0x00B0,
    PC_FOOD_CHICKEN_LEG     = 0x00B1,
    PC_FOOD_SANDWICH        = 0x00B2,
    PC_FOOD_TACO            = 0x00B3,
    PC_FOOD_HOT_DOG         = 0x00B4,
    PC_FOOD_HAM             = 0x00B5,
    PC_FOOD_CHEESE          = 0x00B6,
    PC_SCENERY_DESTRUCT_SPD = 0x0118,
    PC_SCENERY_COLLAPSE     = 0x0119
};


class EventIDConverter {
public:
    static PCEvent convert(JJ2Event old);
    static quint16 convert(quint16 old);
    static QString eventName(quint16 num);
private:
    static void init();
    static QMap< JJ2Event, PCEvent > assigned;
    static QStringList familiar_names;
};
