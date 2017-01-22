#include "EventConverter.h"
#include <QString>
#include <QStringList>
#include <QMap>

ConversionResult EventConverter::convert(JJ2Event old, quint32 params) {
    if (assigned.size() == 0) {
        init();
    }

    return assigned.value(old, getTrivialConverter(PC_EMPTY))(params);
}

ConversionResult EventConverter::convert(quint16 old, quint32 params) {
    return convert(static_cast<JJ2Event>(old), params);
}

QString EventConverter::getJazz2EventName(quint16 num) {
    if (assigned.size() == 0) {
        init();
    }
    if (num > JJ2EventNames.size()) {
        return "(unknown, out of range)";
    } else {
        return JJ2EventNames[num];
    }
}

QString EventConverter::getPCEventName(PCEvent ev) {
    return PCEventNames.value(ev, "(unknown, not defined)");
}

void EventConverter::init() {
    // initialize convert table
    assigned.insert(JJ2_EMPTY, getTrivialConverter(PC_EMPTY));
    assigned.insert(JJ2_GEM_RED, getTrivialConverter(PC_GEM_RED));
    assigned.insert(JJ2_GEM_GREEN, getTrivialConverter(PC_GEM_GREEN));
    assigned.insert(JJ2_GEM_BLUE, getTrivialConverter(PC_GEM_BLUE));
    assigned.insert(JJ2_ENEMY_TURTLE_NORMAL, getTrivialConverter(PC_ENEMY_TURTLE_NORMAL));
    assigned.insert(JJ2_JAZZ_LEVEL_START, getTrivialConverter(PC_JAZZ_LEVEL_START));
    assigned.insert(JJ2_SPAZ_LEVEL_START, getTrivialConverter(PC_SPAZ_LEVEL_START));
    assigned.insert(JJ2_AMMO_BOUNCER, getTrivialConverter(PC_AMMO_BOUNCER));
    assigned.insert(JJ2_AMMO_FREEZER, getTrivialConverter(PC_AMMO_FREEZER));
    assigned.insert(JJ2_AMMO_SEEKER, getTrivialConverter(PC_AMMO_SEEKER));
    assigned.insert(JJ2_AMMO_RF, getTrivialConverter(PC_AMMO_RF));
    assigned.insert(JJ2_AMMO_TOASTER, getTrivialConverter(PC_AMMO_TOASTER));
    assigned.insert(JJ2_AMMO_TNT, getTrivialConverter(PC_AMMO_TNT));
    assigned.insert(JJ2_AMMO_PEPPER, getTrivialConverter(PC_AMMO_PEPPER));
    assigned.insert(JJ2_AMMO_ELECTRO, getTrivialConverter(PC_AMMO_ELECTRO));
    assigned.insert(JJ2_MODIFIER_HOOK, getTrivialConverter(PC_MODIFIER_HOOK));
    assigned.insert(JJ2_MODIFIER_ONE_WAY, getTrivialConverter(PC_MODIFIER_ONE_WAY));
    assigned.insert(JJ2_MODIFIER_VINE, getTrivialConverter(PC_MODIFIER_VINE));
    assigned.insert(JJ2_MODIFIER_HURT, getTrivialConverter(PC_MODIFIER_HURT));
    assigned.insert(JJ2_SCENERY_DESTRUCT, [](quint32 paramInt) -> ConversionResult {
        auto params = convertParamInt(paramInt, {{
            { UINT, 10 }, // Empty
            { UINT, 5  }, // Speed
            { UINT, 4  }  // Weapon
        }});

        if (params[1] > 0) {
            return { PC_SCENERY_DESTRUCT_SPD, { params[1], 0, 0, 0, 0, 0, 0, 0 } };
        } else {
            return { PC_SCENERY_DESTRUCT, { params[2], 0, 0, 0, 0, 0, 0, 0 } };
        }
    });
    assigned.insert(JJ2_SCENERY_DESTR_BOMB, getConstantParameterListConverter(PC_SCENERY_DESTRUCT, { 7, 0, 0, 0, 0, 0, 0, 0 }));
    assigned.insert(JJ2_SCENERY_BUTTSTOMP, getTrivialConverter(PC_SCENERY_BUTTSTOMP));
    assigned.insert(JJ2_SAVE_POINT, getTrivialConverter(PC_SAVE_POINT));
    assigned.insert(JJ2_MODIFIER_H_POLE, getTrivialConverter(PC_MODIFIER_H_POLE));
    assigned.insert(JJ2_MODIFIER_V_POLE, getTrivialConverter(PC_MODIFIER_V_POLE));
    assigned.insert(JJ2_FAST_FIRE, getTrivialConverter(PC_FAST_FIRE));
    assigned.insert(JJ2_WARP_ORIGIN, [](quint32 paramInt) -> ConversionResult {
        auto params = convertParamInt(paramInt, {{
            { UINT, 8 }, // Warp ID
            { UINT, 8 }, // Coins
            { BOOL, 1 }, // Set Lap
            { BOOL, 1 }  // Show Anim
        }});

        if (params[1] > 0) {
            return { PC_WARP_COIN_BONUS, { params[0], params[2], params[3], params[1], 0, 0, 0, 0 } };
        } else {
            return { PC_WARP_ORIGIN, { params[0], params[2], params[3], 0, 0, 0, 0, 0 } };
        }
    });
    assigned.insert(JJ2_WARP_TARGET, getTrivialParameterListConverter(PC_WARP_TARGET, {{
        { UINT, 8 } // Warp ID
    }}));
    assigned.insert(JJ2_ENEMY_LIZARD, getTrivialConverter(PC_ENEMY_LIZARD));
    assigned.insert(JJ2_LIGHT_SET, getTrivialParameterListConverter(PC_LIGHT_SET, {{
        { UINT, 7 }, // Intensity
        { UINT, 4 }, // Red
        { UINT, 4 }, // Green
        { UINT, 4 }, // Blue
        { BOOL, 1 }  // Flicker
    }}));
    assigned.insert(JJ2_LIGHT_RESET, getTrivialConverter(PC_LIGHT_RESET));
    assigned.insert(JJ2_AREA_EOL, getTrivialParameterListConverter(PC_AREA_EOL, {{
        { BOOL, 1 }  // Secret
    }}));
    assigned.insert(JJ2_PUSHABLE_ROCK, getConstantParameterListConverter(PC_PUSHABLE_ROCK, { 0, 0, 0, 0, 0, 0, 0, 0 }));
    assigned.insert(JJ2_PUSHABLE_BOX, getConstantParameterListConverter(PC_PUSHABLE_ROCK, { 1, 0, 0, 0, 0, 0, 0, 0 }));
    assigned.insert(JJ2_TRIGGER_CRATE, getTrivialParameterListConverter(PC_TRIGGER_CRATE, {{
        { UINT, 5 } // Trigger ID
    }}));
    assigned.insert(JJ2_TRIGGER_AREA, getTrivialParameterListConverter(PC_TRIGGER_AREA, {{
        { UINT, 5 } // Trigger ID
    }}));
    assigned.insert(JJ2_MODIFIER_RICOCHET, getTrivialConverter(PC_MODIFIER_RICOCHET));
    assigned.insert(JJ2_BRIDGE, [](quint32 paramInt) -> ConversionResult {
        auto params = convertParamInt(paramInt, {{
            { UINT, 4 }, // Width
            { UINT, 3 }, // Type
            { UINT, 4 }  // Toughness
        }});

        return { PC_BRIDGE, { quint16(params[0] * 2), params[1], params[2], 0, 0, 0, 0, 0 } };
    });
    assigned.insert(JJ2_AREA_STOP_ENEMY, getTrivialConverter(PC_AREA_STOP_ENEMY));
    assigned.insert(JJ2_COIN_SILVER, getTrivialConverter(PC_COIN_SILVER));
    assigned.insert(JJ2_COIN_GOLD, getTrivialConverter(PC_COIN_GOLD));
    assigned.insert(JJ2_SPRING_RED, getSpringConverter(PC_SPRING_RED, false));
    assigned.insert(JJ2_SPRING_GREEN, getSpringConverter(PC_SPRING_GREEN, false));
    assigned.insert(JJ2_SPRING_BLUE, getSpringConverter(PC_SPRING_BLUE, false));
    assigned.insert(JJ2_SPRING_RED_HOR, getSpringConverter(PC_SPRING_RED, true));
    assigned.insert(JJ2_SPRING_GREEN_HOR, getSpringConverter(PC_SPRING_GREEN, true));
    assigned.insert(JJ2_SPRING_BLUE_HOR, getSpringConverter(PC_SPRING_BLUE, true));
    assigned.insert(JJ2_PLATFORM_FRUIT, getPlatformConverter(1));
    assigned.insert(JJ2_PLATFORM_BOLL, getPlatformConverter(2));
    assigned.insert(JJ2_PLATFORM_GRASS, getPlatformConverter(3));
    assigned.insert(JJ2_PLATFORM_PINK, getPlatformConverter(4));
    assigned.insert(JJ2_PLATFORM_SONIC, getPlatformConverter(5));
    assigned.insert(JJ2_PLATFORM_SPIKE, getPlatformConverter(6));
    assigned.insert(JJ2_MODIFIER_HOOK, getTrivialConverter(PC_MODIFIER_HOOK));
    assigned.insert(JJ2_AREA_FLOAT_UP, getTrivialConverter(PC_AREA_FLOAT_UP));
    assigned.insert(JJ2_MODIFIER_TUBE, [](quint32 paramInt) -> ConversionResult {
        auto params = convertParamInt(paramInt, {{
            { INT,  7 }, // X Speed
            { INT,  7 }, // Y Speed
            { UINT, 3 }, // Trig Sample
            { UINT, 4 }  // Wait Time
        }});

        return { PC_MODIFIER_TUBE, { params[0], params[1], params[3], 0, 0, 0, 0, params[2] } };
    });
    assigned.insert(JJ2_GEM_PURPLE, getTrivialConverter(PC_GEM_PURPLE));
    assigned.insert(JJ2_FOOD_APPLE, getTrivialConverter(PC_FOOD_APPLE));
    assigned.insert(JJ2_FOOD_BANANA, getTrivialConverter(PC_FOOD_BANANA));
    assigned.insert(JJ2_FOOD_CHERRY, getTrivialConverter(PC_FOOD_CHERRY));
    assigned.insert(JJ2_FOOD_ORANGE, getTrivialConverter(PC_FOOD_ORANGE));
    assigned.insert(JJ2_FOOD_PEAR, getTrivialConverter(PC_FOOD_PEAR));
    assigned.insert(JJ2_FOOD_PRETZEL, getTrivialConverter(PC_FOOD_PRETZEL));
    assigned.insert(JJ2_FOOD_STRAWBERRY, getTrivialConverter(PC_FOOD_STRAWBERRY));
    assigned.insert(JJ2_FOOD_LEMON, getTrivialConverter(PC_FOOD_LEMON));
    assigned.insert(JJ2_FOOD_LIME, getTrivialConverter(PC_FOOD_LIME));
    assigned.insert(JJ2_FOOD_THING, getTrivialConverter(PC_FOOD_THING));
    assigned.insert(JJ2_FOOD_WATERMELON, getTrivialConverter(PC_FOOD_WATERMELON));
    assigned.insert(JJ2_FOOD_PEACH, getTrivialConverter(PC_FOOD_PEACH));
    assigned.insert(JJ2_FOOD_GRAPES, getTrivialConverter(PC_FOOD_GRAPES));
    assigned.insert(JJ2_FOOD_LETTUCE, getTrivialConverter(PC_FOOD_LETTUCE));
    assigned.insert(JJ2_FOOD_EGGPLANT, getTrivialConverter(PC_FOOD_EGGPLANT));
    assigned.insert(JJ2_FOOD_CUCUMBER, getTrivialConverter(PC_FOOD_CUCUMBER));
    assigned.insert(JJ2_FOOD_PEPSI, getTrivialConverter(PC_FOOD_PEPSI));
    assigned.insert(JJ2_FOOD_COKE, getTrivialConverter(PC_FOOD_COKE));
    assigned.insert(JJ2_FOOD_MILK, getTrivialConverter(PC_FOOD_MILK));
    assigned.insert(JJ2_FOOD_PIE, getTrivialConverter(PC_FOOD_PIE));
    assigned.insert(JJ2_FOOD_CAKE, getTrivialConverter(PC_FOOD_CAKE));
    assigned.insert(JJ2_FOOD_DONUT, getTrivialConverter(PC_FOOD_DONUT));
    assigned.insert(JJ2_FOOD_CUPCAKE, getTrivialConverter(PC_FOOD_CUPCAKE));
    assigned.insert(JJ2_FOOD_CHIPS, getTrivialConverter(PC_FOOD_CHIPS));
    assigned.insert(JJ2_FOOD_CANDY, getTrivialConverter(PC_FOOD_CANDY));
    assigned.insert(JJ2_FOOD_CHOCOLATE, getTrivialConverter(PC_FOOD_CHOCOLATE));
    assigned.insert(JJ2_FOOD_ICE_CREAM, getTrivialConverter(PC_FOOD_ICE_CREAM));
    assigned.insert(JJ2_FOOD_BURGER, getTrivialConverter(PC_FOOD_BURGER));
    assigned.insert(JJ2_FOOD_PIZZA, getTrivialConverter(PC_FOOD_PIZZA));
    assigned.insert(JJ2_FOOD_FRIES, getTrivialConverter(PC_FOOD_FRIES));
    assigned.insert(JJ2_FOOD_CHICKEN_LEG, getTrivialConverter(PC_FOOD_CHICKEN_LEG));
    assigned.insert(JJ2_FOOD_SANDWICH, getTrivialConverter(PC_FOOD_SANDWICH));
    assigned.insert(JJ2_FOOD_TACO, getTrivialConverter(PC_FOOD_TACO));
    assigned.insert(JJ2_FOOD_HOT_DOG, getTrivialConverter(PC_FOOD_HOT_DOG));
    assigned.insert(JJ2_FOOD_HAM, getTrivialConverter(PC_FOOD_HAM));
    assigned.insert(JJ2_FOOD_CHEESE, getTrivialConverter(PC_FOOD_CHEESE));
    assigned.insert(JJ2_SCENERY_COLLAPSE, getTrivialParameterListConverter(PC_SCENERY_COLLAPSE, {{
        { UINT, 10 }, // Wait Time
        { UINT, 5  }  // FPS
    }}));
    assigned.insert(JJ2_CARROT, getTrivialConverter(PC_CARROT));
    assigned.insert(JJ2_TURTLE_SHELL, getTrivialConverter(PC_TURTLE_SHELL));
    assigned.insert(JJ2_CARROT_FULL, getTrivialConverter(PC_CARROT_FULL));
    assigned.insert(JJ2_ENEMY_SUCKER_FLOAT, getTrivialConverter(PC_ENEMY_SUCKER_FLOAT));
    assigned.insert(JJ2_ENEMY_SUCKER, getTrivialConverter(PC_ENEMY_SUCKER));
    assigned.insert(JJ2_POWERUP_BLASTER, getTrivialConverter(PC_POWERUP_BLASTER));
    assigned.insert(JJ2_POWERUP_BOUNCER, getTrivialConverter(PC_POWERUP_BOUNCER));
    assigned.insert(JJ2_POWERUP_FREEZER, getTrivialConverter(PC_POWERUP_FREEZER));
    assigned.insert(JJ2_POWERUP_SEEKER, getTrivialConverter(PC_POWERUP_SEEKER));
    assigned.insert(JJ2_POWERUP_RF, getTrivialConverter(PC_POWERUP_RF));
    assigned.insert(JJ2_POWERUP_TOASTER, getTrivialConverter(PC_POWERUP_TOASTER));
    assigned.insert(JJ2_POWERUP_PEPPER, getTrivialConverter(PC_POWERUP_PEPPER));
    assigned.insert(JJ2_POWERUP_ELECTRO, getTrivialConverter(PC_POWERUP_ELECTRO));
    assigned.insert(JJ2_ENEMY_LAB_RAT, getTrivialConverter(PC_ENEMY_LAB_RAT));
    assigned.insert(JJ2_LIGHT_DIM, getConstantParameterListConverter(PC_LIGHT_STEADY, { 127, 0, 0, 0, 0, 0, 0, 0 }));
    assigned.insert(JJ2_LIGHT_STEADY, getConstantParameterListConverter(PC_LIGHT_STEADY, { 0, 0, 0, 0, 0, 0, 0, 0 }));
    assigned.insert(JJ2_LIGHT_PULSE, [](quint32 paramInt) -> ConversionResult {
        auto params = convertParamInt(paramInt, {{
            { UINT, 8 }, // Speed
            { UINT, 4 }  // Sync
        }});

        return { PC_LIGHT_PULSE, { 0, params[0], params[1], 0, 0, 0, 0, 0 } };
    });
    assigned.insert(JJ2_AREA_TEXT, getTrivialParameterListConverter(PC_AREA_TEXT, {{
        { UINT, 8 }, // Text
        { BOOL, 1 }  // Vanish
    }}));
    assigned.insert(JJ2_AREA_EOL_WARP, getConstantParameterListConverter(PC_AREA_EOL, { 0, 1 }));
    assigned.insert(JJ2_CRATE_AMMO, getAmmoCrateConverter(0));
    assigned.insert(JJ2_CRATE_AMMO_BOUNCER, getAmmoCrateConverter(1));
    assigned.insert(JJ2_CRATE_AMMO_FREEZER, getAmmoCrateConverter(2));
    assigned.insert(JJ2_CRATE_AMMO_SEEKER, getAmmoCrateConverter(3));
    assigned.insert(JJ2_CRATE_AMMO_RF, getAmmoCrateConverter(4));
    assigned.insert(JJ2_CRATE_AMMO_TOASTER, getAmmoCrateConverter(5));
    assigned.insert(JJ2_CRATE_CARROT, getConstantParameterListConverter(PC_CRATE_GENERAL, { (quint16)PC_CARROT, 1 }));
    assigned.insert(JJ2_CRATE_SPRING, getConstantParameterListConverter(PC_CRATE_GENERAL, { (quint16)PC_SPRING_GREEN, 1 }));
    assigned.insert(JJ2_CRATE_ONEUP, getConstantParameterListConverter(PC_CRATE_GENERAL, { (quint16)PC_ONEUP, 1 }));
    assigned.insert(JJ2_CRATE_BOMB, getConstantParameterListConverter(PC_CRATE_GENERAL, { (quint16)PC_SCENERY_BOMB, 1 }));
    assigned.insert(JJ2_BARREL_AMMO, getConstantParameterListConverter(PC_BARREL_AMMO, { 0 }));
    assigned.insert(JJ2_BARREL_CARROT, getConstantParameterListConverter(PC_BARREL_GENERAL, { (quint16)PC_CARROT, 1 }));
    assigned.insert(JJ2_BARREL_ONEUP, getConstantParameterListConverter(PC_BARREL_GENERAL, { (quint16)PC_ONEUP, 1 }));
    assigned.insert(JJ2_CRATE_GEM, getTrivialParameterListConverter(PC_CRATE_GEM, {{
        { UINT, 4 }, // Red
        { UINT, 4 }, // Green
        { UINT, 4 }, // Blue
        { UINT, 4 }  // Purple
    }}));
    assigned.insert(JJ2_BARREL_GEM, getTrivialParameterListConverter(PC_BARREL_GEM, {{
        { UINT, 4 }, // Red
        { UINT, 4 }, // Green
        { UINT, 4 }, // Blue
        { UINT, 4 }  // Purple
    }}));
    assigned.insert(JJ2_ONEUP, getTrivialConverter(PC_ONEUP));
}

ConversionFunction EventConverter::getTrivialConverter(PCEvent ev) {
    return [ev](quint32 jj2Params) -> ConversionResult {
        return { ev, { 0, 0, 0, 0, 0, 0, 0, 0 } };
    };
}

ConversionFunction EventConverter::getConstantParameterListConverter(PCEvent ev, ParamArray params) {
    return [ev, params](quint32 jj2Params) -> ConversionResult {
        return { ev, params };
    };
}

ConversionFunction EventConverter::getTrivialParameterListConverter(PCEvent ev, JJ2EventParamListDefinition paramDefs) {
    return [ev, paramDefs](quint32 paramInt) -> ConversionResult {
        auto params = convertParamInt(paramInt, paramDefs);

        return { ev, { params[0], params[1], params[2], params[3], params[4], 0, 0, 0 } };
    };
}

ConversionFunction EventConverter::getSpringConverter(PCEvent ev, bool horizontal) {
    return [ev, horizontal](quint32 paramInt) -> ConversionResult {
        auto params = convertParamInt(paramInt, {{
            { BOOL, 1 }, // Orientation
            { BOOL, 1 }, // Keep X Speed
            { BOOL, 1 }, // Keep Y Speed
            { UINT, 4 }  // Delay
        }});

        return { ev, { quint16(horizontal ? 5 : params[0] * 2), params[1], params[2], params[3], 0, 0, 0, 0 }};
    };
}

ConversionFunction EventConverter::getPlatformConverter(quint8 type) {
    return [type](quint32 paramInt) -> ConversionResult {
        auto params = convertParamInt(paramInt, {{
            { UINT, 2 }, // Sync
            { INT,  6 }, // Speed
            { UINT, 4 }, // Length
            { BOOL, 1 }  // Swing
        }});

        return { PC_MOVING_PLATFORM, { type, params[0], params[1], params[2], params[3], 0, 0, 0 }};
    };
}

ConversionFunction EventConverter::getAmmoCrateConverter(quint8 type) {
    return [type](quint32 paramInt) -> ConversionResult {
        return{ PC_CRATE_AMMO, { type, 0, 0, 0, 0, 0, 0, 0 } };
    };
}

ParamArray EventConverter::convertParamInt(quint32 paramInt, JJ2EventParamListDefinition paramTypes) {
    ParamArray params;
    params.fill(0);

    quint8 i = 0;
    for (auto t : paramTypes) {
        if (t.first == NONE || t.second == 0) {
            continue;
        }

        switch (t.first) {
            case BOOL:
                params[i] = paramInt % 2;
                paramInt = paramInt >> 1;
                break;
            case UINT:
                params[i] = (paramInt % (1 << t.second));
                paramInt = paramInt >> t.second;
                break;
            case INT:
                {
                    quint32 val = (paramInt % (1 << t.second));

                    // Complement of two, with variable bit length
                    qint16 highestBitValue = (1 << (t.second - 1));
                    if (val >= highestBitValue) {
                        val = -highestBitValue + (val - highestBitValue);
                    }

                    params[i] = val;
                    paramInt = paramInt >> t.second;
                }
                break;
            default:
                break;
        }

        i++;
    }

    return params;
}

QMap<JJ2Event, ConversionFunction> EventConverter::assigned = QMap<JJ2Event, ConversionFunction>();

// initialize JJ2 event name table
const QStringList EventConverter::JJ2EventNames = {
    "Empty",
    "One Way",
    "Hurt",
    "Vine",
    "Hook",
    "Slide",
    "Hor. Pole",
    "Ver. Pole",
    "Area Fly Off",
    "Ricochet",
    "Belt Right",
    "Belt Left",
    "Accel. Belt Right",
    "Accel. Belt Left",
    "Stop Enemy",
    "Wind Left",
    "Wind Right",
    "Area EOL",
    "Area EOL Warp",
    "Revert Morph",
    "Float",
    "Trigger Rock",
    "Dim Light",
    "Set Light",
    "Limit X Scroll",
    "Reset Light",
    "Area Secret Warp",
    "Echo",
    "Activate Boss",
    "Jazz Level Start",
    "Spaz Level Start",
    "Multipl. Level Start",
    "UNUSED",
    "Freezer Ammo",
    "Bouncer Ammo",
    "Seeker Ammo",
    "RF Ammo",
    "Toaster Ammo",
    "TNT Ammo",
    "Pepper Ammo",
    "Electro Ammo",
    "Turtle Shell",
    "Swinging Vine",
    "Bomb",
    "Silver Coin",
    "Gold Coin",
    "Ammo Crate",
    "Carrot Crate",
    "1UP Crate",
    "Gem Barrel",
    "Carrot Barrel",
    "1UP Barrel",
    "Bomb Crate",
    "Freezer Ammo Crate",
    "Bouncer Ammo Crate",
    "Seeker Ammo Crate",
    "RF Ammo Crate",
    "Toaster Ammo Crate",
    "Scenery TNT",
    "Airboard",
    "Frozen Green Spring",
    "Fast Fire",
    "Spring Crate",
    "Red Gem",
    "Green Gem",
    "Blue Gem",
    "Purple Gem",
    "Super Red Gem",
    "Birdy",
    "Ammo Barrel",
    "Gem Crate",
    "Character Swap",
    "Carrot",
    "Carrot (Full HP)",
    "Fire Shield",
    "Water Shield",
    "Lightning Shield",
    "Max Weapons",
    "Auto Fire Area",
    "Fast Feet",
    "1UP",
    "EOL Sign",
    "UNUSED",
    "Save Point",
    "Bonus Sign",
    "Red Spring",
    "Green Spring",
    "Blue Spring",
    "Invincibility",
    "Shield Extra Time",
    "Freeze",
    "Hor. Red Spring",
    "Hor. Green Spring",
    "Hor. Blue Spring",
    "Bird Morph",
    "Trigger Crate",
    "Copter Carrot",
    "Rect. Red Gem",
    "Rect. Green Gem",
    "Rect. Blue Gem",
    "Tough Turtle",
    "Tough Turtle (Boss)",
    "Lab Rat",
    "Dragon",
    "Lizard",
    "Bee",
    "Rapier",
    "Sparks",
    "Bat",
    "Sucker",
    "Caterpillar",
    "Cheshire Cat 1",
    "Cheshire Cat 2",
    "Madder Hatter",
    "Bilsy Boss",
    "Skeleton",
    "Doggy Dogg",
    "Normal Turtle",
    "Helmut",
    "Leaf",
    "Demon",
    "Fire",
    "Lava",
    "Dragonfly",
    "Monkey",
    "Fat Chick",
    "Fencer",
    "Fish",
    "Moth",
    "Steam",
    "Rotating Rock",
    "Blaster Power-Up",
    "Bouncer Power-Up",
    "Freezer Power-Up",
    "Seeker Power-Up",
    "RF Power-Up",
    "Toaster Power-Up",
    "Pinball Left Paddle",
    "Pinball Right Paddle",
    "Pinball 500 Points Bumper",
    "Pinball Carrot Bumper",
    "Apple",
    "Banana",
    "Cherry",
    "Orange",
    "Pear",
    "Pretzel",
    "Strawberry",
    "Steady Light",
    "Pulsing Light",
    "Flickering Light",
    "Queen Boss",
    "Sucker Float",
    "Bridge",
    "Lemon",
    "Lime",
    "Thing",
    "Watermelon",
    "Peach",
    "Grapes",
    "Lettuce",
    "Eggplant",
    "Cucumber",
    "Pepsi",
    "Coke",
    "Milk",
    "Pie",
    "Cake",
    "Donut",
    "Cupcake",
    "Chips",
    "Candy",
    "Chocolate",
    "Ice Cream",
    "Burger",
    "Pizza",
    "Fries",
    "Chicken Leg",
    "Sandwich",
    "Taco",
    "Hot Dog",
    "Ham",
    "Cheese",
    "Float Lizard",
    "Stand Monkey",
    "Destructible Scenery",
    "Destructible Scenery Bomb",
    "Collapsing Scenery",
    "Buttstomp Scenery",
    "Gem Stomp",
    "Raven",
    "Tube Turtle",
    "Gem Ring",
    "Small Tree",
    "Ambient Sound",
    "Uterus Boss",
    "Crab",
    "Witch",
    "Rocket Turtle",
    "Bubba Boss",
    "Devil Devan Boss",
    "Devan (Robot)",
    "Robot Boss",
    "Carrotus Pole",
    "Psych Pole",
    "Diamondus Pole",
    "Tube",
    "Text",
    "Set Water",
    "Fruit Platform",
    "Boll Platform",
    "Grass Platform",
    "Pink Platform",
    "Sonic Platform",
    "Spike Platform",
    "Spike Boll",
    "Generator",
    "Eva Earlong",
    "Bubbler",
    "TNT Power-Up",
    "Pepper Power-Up",
    "Electro Power-Up",
    "Morph Frog",
    "3D Spike Boll",
    "Springcord",
    "Bee Swarm",
    "Copter",
    "Laser Shield",
    "Stopwatch",
    "Jungle Pole",
    "Warp (source)",
    "Pushable Rock",
    "Pushable Box",
    "Water Block",
    "Trigger Area",
    "Bolly Boss",
    "Butterfly",
    "Bee Boy",
    "Snow",
    "UNUSED",
    "Warp (target)",
    "Tweedle Boss",
    "Area ID",
    "UNUSED",
    "CTF Base",
    "No Fire Area",
    "Trigger Zone",
    "Bilsy Dummy",
    "Christmas Turtle",
    "Christmas Lizard",
    "Christmas Copter Lizard",
    "Christmas Bilsy Boss",
    "Cat Enemy",
    "Ghost",
    "UNUSED",
    "UNUSED"
};

const QMap<PCEvent, QString> EventConverter::PCEventNames = {
    { PC_EMPTY                   , "Empty" },
    { PC_FAST_FIRE               , "Fast Fire" },
    { PC_AMMO_BOUNCER            , "Bouncer Ammo" },
    { PC_AMMO_FREEZER            , "Freezer Ammo" },
    { PC_AMMO_SEEKER             , "Seeker Ammo" },
    { PC_AMMO_RF                 , "RF Ammo" },
    { PC_AMMO_TOASTER            , "Toaster Ammo" },
    { PC_AMMO_TNT                , "TNT Ammo" },
    { PC_AMMO_PEPPER             , "Pepper Ammo" },
    { PC_AMMO_ELECTRO            , "Electro Ammo" },
    { PC_POWERUP_BLASTER         , "Blaster Power-Up" },
    { PC_POWERUP_BOUNCER         , "Bouncer Power-Up" },
    { PC_POWERUP_FREEZER         , "Freezer Power-Up" },
    { PC_POWERUP_SEEKER          , "Seeker Power-Up" },
    { PC_POWERUP_RF              , "RF Power-Up" },
    { PC_POWERUP_TOASTER         , "Toaster Power-Up" },
    { PC_POWERUP_TNT             , "TNT Power-Up" },
    { PC_POWERUP_PEPPER          , "Pepper Power-Up" },
    { PC_POWERUP_ELECTRO         , "Electro Power-Up" },
    { PC_GEM_RED                 , "Red Gem" },
    { PC_GEM_GREEN               , "Green Gem" },
    { PC_GEM_BLUE                , "Blue Gem" },
    { PC_GEM_PURPLE              , "Purple Gem" },
    { PC_SAVE_POINT              , "Save Point" },
    { PC_SPRING_RED              , "Red Spring" },
    { PC_SPRING_GREEN            , "Green Spring" },
    { PC_SPRING_BLUE             , "Blue Spring" },
    { PC_JAZZ_LEVEL_START        , "Jazz Level Start" },
    { PC_SPAZ_LEVEL_START        , "Spaz Level Start" },
    { PC_LORI_LEVEL_START        , "Lori Level Start" },
    { PC_AREA_EOL                , "Area EOL" },
    { PC_WARP_ORIGIN             , "Warp Origin" },
    { PC_WARP_TARGET             , "Warp Target" },
    { PC_MODIFIER_VINE           , "Vine" },
    { PC_MODIFIER_ONE_WAY        , "One Way" },
    { PC_MODIFIER_HOOK           , "Hook" },
    { PC_MODIFIER_H_POLE         , "Hor. Pole" },
    { PC_MODIFIER_V_POLE         , "Ver. Pole" },
    { PC_SCENERY_DESTRUCT        , "Destructible Scenery" },
    { PC_SCENERY_BUTTSTOMP       , "Buttstomp Scenery" },
    { PC_MODIFIER_HURT           , "Hurt" },
    { PC_LIGHT_SET               , "Set Light" },
    { PC_LIGHT_RESET             , "Reset Light" },
    { PC_ENEMY_TURTLE_NORMAL     , "Normal Turtle" },
    { PC_ENEMY_LIZARD            , "Lizard" },
    { PC_PUSHABLE_ROCK           , "Pushable Object" },
    { PC_TRIGGER_CRATE           , "Trigger Crate" },
    { PC_TRIGGER_AREA            , "Trigger Area" },
    { PC_MODIFIER_RICOCHET       , "Ricochet" },
    { PC_BRIDGE                  , "Bridge" },
    { PC_AREA_STOP_ENEMY         , "Stop Enemy" },
    { PC_COIN_SILVER             , "Silver Coin" },
    { PC_COIN_GOLD               , "Gold Coin" },
    { PC_MOVING_PLATFORM         , "Moving Platform" },
    { PC_AREA_FLOAT_UP           , "Float Up" },
    { PC_MODIFIER_TUBE           , "Tube" },
    { PC_FOOD_APPLE              , "Apple" },
    { PC_FOOD_BANANA             , "Banana" },
    { PC_FOOD_CHERRY             , "Cherry" },
    { PC_FOOD_ORANGE             , "Orange" },
    { PC_FOOD_PEAR               , "Pear" },
    { PC_FOOD_PRETZEL            , "Pretzel" },
    { PC_FOOD_STRAWBERRY         , "Strawberry" },
    { PC_FOOD_LEMON              , "Lemon" },
    { PC_FOOD_LIME               , "Lime" },
    { PC_FOOD_THING              , "Thing" },
    { PC_FOOD_WATERMELON         , "Watermelon" },
    { PC_FOOD_PEACH              , "Peach" },
    { PC_FOOD_GRAPES             , "Grapes" },
    { PC_FOOD_LETTUCE            , "Lettuce" },
    { PC_FOOD_EGGPLANT           , "Eggplant" },
    { PC_FOOD_CUCUMBER           , "Cucumber" },
    { PC_FOOD_PEPSI              , "Pepsi" },
    { PC_FOOD_COKE               , "Coke" },
    { PC_FOOD_MILK               , "Milk" },
    { PC_FOOD_PIE                , "Pie" },
    { PC_FOOD_CAKE               , "Cake" },
    { PC_FOOD_DONUT              , "Donut" },
    { PC_FOOD_CUPCAKE            , "Cupcake" },
    { PC_FOOD_CHIPS              , "Chips" },
    { PC_FOOD_CANDY              , "Candy" },
    { PC_FOOD_CHOCOLATE          , "Chocolate" },
    { PC_FOOD_ICE_CREAM          , "Ice Cream" },
    { PC_FOOD_BURGER             , "Burger" },
    { PC_FOOD_PIZZA              , "Pizza" },
    { PC_FOOD_FRIES              , "Fries" },
    { PC_FOOD_CHICKEN_LEG        , "Chicken Leg" },
    { PC_FOOD_SANDWICH           , "Sandwich" },
    { PC_FOOD_TACO               , "Taco" },
    { PC_FOOD_HOT_DOG            , "Hot Dog" },
    { PC_FOOD_HAM                , "Ham" },
    { PC_FOOD_CHEESE             , "Cheese" },
    { PC_SCENERY_DESTRUCT_SPD    , "Speed Scenery" },
    { PC_SCENERY_COLLAPSE        , "Collapsible Scenery" },
    { PC_CARROT                  , "Carrot" },
    { PC_TURTLE_SHELL            , "Turtle Shell" },
    { PC_CARROT_FULL             , "Full Energy Carrot" },
    { PC_WARP_COIN_BONUS         , "Bonus Warp" },
    { PC_ENEMY_SUCKER            , "Sucker" },
    { PC_ENEMY_SUCKER_FLOAT      , "Sucker Float" },
    { PC_ENEMY_LAB_RAT           , "Lab Rat" },
    { PC_LIGHT_STEADY            , "Steady Light" },
    { PC_LIGHT_PULSE             , "Pulse Light" },
    { PC_AREA_TEXT               , "Text" },
    { PC_CRATE_GENERAL           , "General Crate" },
    { PC_BARREL_GENERAL          , "General Barrel" },
    { PC_CRATE_AMMO              , "Ammo Crate" },
    { PC_BARREL_AMMO             , "Ammo Barrel" },
    { PC_CRATE_GEM               , "Gem Crate" },
    { PC_BARREL_GEM              , "Gem Barrel" },
    { PC_ONEUP                   , "1UP" },
    { PC_SCENERY_BOMB            , "Scenery Bomb" },
};
