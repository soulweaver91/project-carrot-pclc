#include "EventIDConverter.h"
#include <QString>
#include <QStringList>
#include <QMap>

PCEvent EventIDConverter::convert(JJ2Event old) {
    if (assigned.size() == 0) {
        init();
    }

    return assigned.value(old, PC_EMPTY);
}

quint16 EventIDConverter::convert(quint16 old) {
    return static_cast< quint16 >(convert(static_cast< JJ2Event >(old)));
}

QString EventIDConverter::eventName(quint16 num) {
    if (assigned.size() == 0) {
        init();
    }
    if (num > familiar_names.size()) {
        return "(unknown, out of range)";
    } else {
        return familiar_names[num];
    }
}

void EventIDConverter::init() {
    // initialize JJ2 event name table
    familiar_names << "Empty"
                   << "One Way"
                   << "Hurt"
                   << "Vine"
                   << "Hook"
                   << "Slide"
                   << "Hor. Pole"
                   << "Ver. Pole"
                   << "Area Fly Off"
                   << "Ricochet"
                   << "Belt Right"
                   << "Belt Left"
                   << "Accel. Belt Right"
                   << "Accel. Belt Left"
                   << "Stop Enemy"
                   << "Wind Left"
                   << "Wind Right"
                   << "Area EOL"
                   << "Area EOL Warp"
                   << "Revert Morph"
                   << "Float"
                   << "Trigger Rock"
                   << "Dim Light"
                   << "Set Light"
                   << "Limit X Scroll"
                   << "Reset Light"
                   << "Area Secret Warp"
                   << "Echo"
                   << "Activate Boss"
                   << "Jazz Level Start"
                   << "Spaz Level Start"
                   << "Multipl. Level Start"
                   << "UNUSED"
                   << "Freezer Ammo"
                   << "Bouncer Ammo"
                   << "Seeker Ammo"
                   << "RF Ammo"
                   << "Toaster Ammo"
                   << "TNT Ammo"
                   << "Pepper Ammo"
                   << "Electro Ammo"
                   << "Turtle Shell"
                   << "Swinging Vine"
                   << "Bomb"
                   << "Silver Coin"
                   << "Gold Coin"
                   << "Ammo Crate"
                   << "Carrot Crate"
                   << "1UP Crate"
                   << "Gem Barrel"
                   << "Carrot Barrel"
                   << "1UP Barrel"
                   << "Bomb Crate"
                   << "Freezer Ammo Crate"
                   << "Bouncer Ammo Crate"
                   << "Seeker Ammo Crate"
                   << "RF Ammo Crate"
                   << "Toaster Ammo Crate"
                   << "Scenery TNT"
                   << "Airboard"
                   << "Frozen Green Spring"
                   << "Fast Fire"
                   << "Spring Crate"
                   << "Red Gem"
                   << "Green Gem"
                   << "Blue Gem"
                   << "Purple Gem"
                   << "Super Red Gem"
                   << "Birdy"
                   << "Ammo Barrel"
                   << "Gem Crate"
                   << "Character Swap"
                   << "Carrot"
                   << "Carrot (Full HP)"
                   << "Fire Shield"
                   << "Water Shield"
                   << "Lightning Shield"
                   << "Max Weapons"
                   << "Auto Fire Area"
                   << "Fast Feet"
                   << "1UP"
                   << "EOL Sign"
                   << "UNUSED"
                   << "Save Point"
                   << "Bonus Sign"
                   << "Red Spring"
                   << "Green Spring"
                   << "Blue Spring"
                   << "Invincibility"
                   << "Shield Extra Time"
                   << "Freeze"
                   << "Hor. Red Spring"
                   << "Hor. Green Spring"
                   << "Hor. Blue Spring"
                   << "Bird Morph"
                   << "Trigger Crate"
                   << "Copter Carrot"
                   << "Rect. Red Gem"
                   << "Rect. Green Gem"
                   << "Rect. Blue Gem"
                   << "Tough Turtle"
                   << "Tough Turtle (Boss)"
                   << "Lab Rat"
                   << "Dragon"
                   << "Lizard"
                   << "Bee"
                   << "Rapier"
                   << "Sparks"
                   << "Bat"
                   << "Sucker"
                   << "Caterpillar"
                   << "Cheshire Cat 1"
                   << "Cheshire Cat 2"
                   << "Madder Hatter"
                   << "Bilsy Boss"
                   << "Skeleton"
                   << "Doggy Dogg"
                   << "Normal Turtle"
                   << "Helmut"
                   << "Leaf"
                   << "Demon"
                   << "Fire"
                   << "Lava"
                   << "Dragonfly"
                   << "Monkey"
                   << "Fat Chick"
                   << "Fencer"
                   << "Fish"
                   << "Moth"
                   << "Steam"
                   << "Rotating Rock"
                   << "Blaster Power-Up"
                   << "Bouncer Power-Up"
                   << "Freezer Power-Up"
                   << "Seeker Power-Up"
                   << "RF Power-Up"
                   << "Toaster Power-Up"
                   << "Pinball Left Paddle"
                   << "Pinball Right Paddle"
                   << "Pinball 500 Points Bumper"
                   << "Pinball Carrot Bumper"
                   << "Apple"
                   << "Banana"
                   << "Cherry"
                   << "Orange"
                   << "Pear"
                   << "Pretzel"
                   << "Strawberry"
                   << "Steady Light"
                   << "Pulsing Light"
                   << "Flickering Light"
                   << "Queen Boss"
                   << "Sucker Float"
                   << "Bridge"
                   << "Lemon"
                   << "Lime"
                   << "Thing"
                   << "Watermelon"
                   << "Peach"
                   << "Grapes"
                   << "Lettuce"
                   << "Eggplant"
                   << "Cucumber"
                   << "Pepsi"
                   << "Coke"
                   << "Milk"
                   << "Pie"
                   << "Cake"
                   << "Donut"
                   << "Cupcake"
                   << "Chips"
                   << "Candy"
                   << "Chocolate"
                   << "Ice Cream"
                   << "Burger"
                   << "Pizza"
                   << "Fries"
                   << "Chicken Leg"
                   << "Sandwich"
                   << "Taco"
                   << "Hot Dog"
                   << "Ham"
                   << "Cheese"
                   << "Float Lizard"
                   << "Stand Monkey"
                   << "Destructible Scenery"
                   << "Destructible Scenery Bomb"
                   << "Collapsing Scenery"
                   << "Buttstomp Scenery"
                   << "Gem Stomp"
                   << "Raven"
                   << "Tube Turtle"
                   << "Gem Ring"
                   << "Small Tree"
                   << "Ambient Sound"
                   << "Uterus Boss"
                   << "Crab"
                   << "Witch"
                   << "Rocket Turtle"
                   << "Bubba Boss"
                   << "Devil Devan Boss"
                   << "Devan (Robot)"
                   << "Robot Boss"
                   << "Carrotus Pole"
                   << "Psych Pole"
                   << "Diamondus Pole"
                   << "Tube"
                   << "Text"
                   << "Set Water"
                   << "Fruit Platform"
                   << "Boll Platform"
                   << "Grass Platform"
                   << "Pink Platform"
                   << "Sonic Platform"
                   << "Spike Platform"
                   << "Spike Boll"
                   << "Generator"
                   << "Eva Earlong"
                   << "Bubbler"
                   << "TNT Power-Up"
                   << "Pepper Power-Up"
                   << "Electro Power-Up"
                   << "Morph Frog"
                   << "3D Spike Boll"
                   << "Springcord"
                   << "Bee Swarm"
                   << "Copter"
                   << "Laser Shield"
                   << "Stopwatch"
                   << "Jungle Pole"
                   << "Warp (source)"
                   << "Pushable Rock"
                   << "Pushable Box"
                   << "Water Block"
                   << "Trigger Area"
                   << "Bolly Boss"
                   << "Butterfly"
                   << "Bee Boy"
                   << "Snow"
                   << "UNUSED"
                   << "Warp (target)"
                   << "Tweedle Boss"
                   << "Area ID"
                   << "UNUSED"
                   << "CTF Base"
                   << "No Fire Area"
                   << "Trigger Zone"
                   << "Bilsy Dummy"
                   << "Christmas Turtle"
                   << "Christmas Lizard"
                   << "Christmas Copter Lizard"
                   << "Christmas Bilsy Boss"
                   << "Cat Enemy"
                   << "Ghost"
                   << "UNUSED"
                   << "UNUSED";

    // initialize convert table
    assigned.insert(JJ2_EMPTY, PC_EMPTY);
    assigned.insert(JJ2_GEM_RED, PC_GEM_RED);
    assigned.insert(JJ2_GEM_GREEN, PC_GEM_GREEN);
    assigned.insert(JJ2_GEM_BLUE, PC_GEM_BLUE);
    assigned.insert(JJ2_ENEMY_TURTLE_NORMAL, PC_ENEMY_TURTLE_NORMAL);
    assigned.insert(JJ2_JAZZ_LEVEL_START, PC_JAZZ_LEVEL_START);
    assigned.insert(JJ2_SPAZ_LEVEL_START, PC_SPAZ_LEVEL_START);
    assigned.insert(JJ2_AMMO_BOUNCER, PC_AMMO_BOUNCER);
    assigned.insert(JJ2_AMMO_FREEZER, PC_AMMO_FREEZER);
    assigned.insert(JJ2_AMMO_SEEKER, PC_AMMO_SEEKER);
    assigned.insert(JJ2_AMMO_RF, PC_AMMO_RF);
    assigned.insert(JJ2_AMMO_TOASTER, PC_AMMO_TOASTER);
    assigned.insert(JJ2_AMMO_TNT, PC_AMMO_TNT);
    assigned.insert(JJ2_AMMO_PEPPER, PC_AMMO_PEPPER);
    assigned.insert(JJ2_AMMO_ELECTRO, PC_AMMO_ELECTRO);
    assigned.insert(JJ2_MODIFIER_HOOK, PC_MODIFIER_HOOK);
    assigned.insert(JJ2_MODIFIER_ONE_WAY, PC_MODIFIER_ONE_WAY);
    assigned.insert(JJ2_MODIFIER_VINE, PC_MODIFIER_VINE);
    assigned.insert(JJ2_MODIFIER_HURT, PC_MODIFIER_HURT);
    assigned.insert(JJ2_SCENERY_DESTRUCT, PC_SCENERY_DESTRUCT);
    assigned.insert(JJ2_SCENERY_DESTR_BOMB, PC_SCENERY_DESTRUCT);
    assigned.insert(JJ2_SCENERY_BUTTSTOMP, PC_SCENERY_BUTTSTOMP);
    assigned.insert(JJ2_SAVE_POINT, PC_SAVE_POINT);
    assigned.insert(JJ2_MODIFIER_H_POLE, PC_MODIFIER_H_POLE);
    assigned.insert(JJ2_MODIFIER_V_POLE, PC_MODIFIER_V_POLE);
    assigned.insert(JJ2_FAST_FIRE, PC_FAST_FIRE);
    assigned.insert(JJ2_WARP_ORIGIN, PC_WARP_ORIGIN);
    assigned.insert(JJ2_WARP_TARGET, PC_WARP_TARGET);
    assigned.insert(JJ2_ENEMY_LIZARD, PC_ENEMY_LIZARD);
    assigned.insert(JJ2_LIGHT_SET, PC_LIGHT_SET);
    assigned.insert(JJ2_LIGHT_RESET, PC_LIGHT_RESET);
    assigned.insert(JJ2_AREA_EOL, PC_AREA_EOL);
    assigned.insert(JJ2_PUSHABLE_ROCK, PC_PUSHABLE_ROCK);
    assigned.insert(JJ2_PUSHABLE_BOX, PC_PUSHABLE_ROCK);
    assigned.insert(JJ2_TRIGGER_CRATE, PC_TRIGGER_CRATE);
    assigned.insert(JJ2_TRIGGER_AREA, PC_TRIGGER_AREA);
    assigned.insert(JJ2_MODIFIER_RICOCHET, PC_MODIFIER_RICOCHET);
    assigned.insert(JJ2_BRIDGE, PC_BRIDGE);
    assigned.insert(JJ2_AREA_STOP_ENEMY, PC_AREA_STOP_ENEMY);
    assigned.insert(JJ2_COIN_SILVER, PC_COIN_SILVER);
    assigned.insert(JJ2_COIN_GOLD, PC_COIN_GOLD);
    assigned.insert(JJ2_SPRING_RED, PC_SPRING_RED);
    assigned.insert(JJ2_SPRING_GREEN, PC_SPRING_GREEN);
    assigned.insert(JJ2_SPRING_BLUE, PC_SPRING_BLUE);
    assigned.insert(JJ2_SPRING_RED_HOR, PC_SPRING_RED);
    assigned.insert(JJ2_SPRING_GREEN_HOR, PC_SPRING_GREEN);
    assigned.insert(JJ2_SPRING_BLUE_HOR, PC_SPRING_BLUE);
    assigned.insert(JJ2_PLATFORM_BOLL, PC_MOVING_PLATFORM);
    assigned.insert(JJ2_PLATFORM_FRUIT, PC_MOVING_PLATFORM);
    assigned.insert(JJ2_PLATFORM_GRASS, PC_MOVING_PLATFORM);
    assigned.insert(JJ2_PLATFORM_PINK, PC_MOVING_PLATFORM);
    assigned.insert(JJ2_PLATFORM_SONIC, PC_MOVING_PLATFORM);
    assigned.insert(JJ2_PLATFORM_SPIKE, PC_MOVING_PLATFORM);
    assigned.insert(JJ2_MODIFIER_HOOK, PC_MODIFIER_HOOK);
    assigned.insert(JJ2_AREA_FLOAT_UP, PC_AREA_FLOAT_UP);
    assigned.insert(JJ2_MODIFIER_TUBE, PC_MODIFIER_TUBE);
    assigned.insert(JJ2_GEM_PURPLE, PC_GEM_PURPLE);
    assigned.insert(JJ2_FOOD_APPLE, PC_FOOD_APPLE);
    assigned.insert(JJ2_FOOD_BANANA, PC_FOOD_BANANA);
    assigned.insert(JJ2_FOOD_CHERRY, PC_FOOD_CHERRY);
    assigned.insert(JJ2_FOOD_ORANGE, PC_FOOD_ORANGE);
    assigned.insert(JJ2_FOOD_PEAR, PC_FOOD_PEAR);
    assigned.insert(JJ2_FOOD_PRETZEL, PC_FOOD_PRETZEL);
    assigned.insert(JJ2_FOOD_STRAWBERRY, PC_FOOD_STRAWBERRY);
    assigned.insert(JJ2_FOOD_LEMON, PC_FOOD_LEMON);
    assigned.insert(JJ2_FOOD_LIME, PC_FOOD_LIME);
    assigned.insert(JJ2_FOOD_THING, PC_FOOD_THING);
    assigned.insert(JJ2_FOOD_WATERMELON, PC_FOOD_WATERMELON);
    assigned.insert(JJ2_FOOD_PEACH, PC_FOOD_PEACH);
    assigned.insert(JJ2_FOOD_GRAPES, PC_FOOD_GRAPES);
    assigned.insert(JJ2_FOOD_LETTUCE, PC_FOOD_LETTUCE);
    assigned.insert(JJ2_FOOD_EGGPLANT, PC_FOOD_EGGPLANT);
    assigned.insert(JJ2_FOOD_CUCUMBER, PC_FOOD_CUCUMBER);
    assigned.insert(JJ2_FOOD_PEPSI, PC_FOOD_PEPSI);
    assigned.insert(JJ2_FOOD_COKE, PC_FOOD_COKE);
    assigned.insert(JJ2_FOOD_MILK, PC_FOOD_MILK);
    assigned.insert(JJ2_FOOD_PIE, PC_FOOD_PIE);
    assigned.insert(JJ2_FOOD_CAKE, PC_FOOD_CAKE);
    assigned.insert(JJ2_FOOD_DONUT, PC_FOOD_DONUT);
    assigned.insert(JJ2_FOOD_CUPCAKE, PC_FOOD_CUPCAKE);
    assigned.insert(JJ2_FOOD_CHIPS, PC_FOOD_CHIPS);
    assigned.insert(JJ2_FOOD_CANDY, PC_FOOD_CANDY);
    assigned.insert(JJ2_FOOD_CHOCOLATE, PC_FOOD_CHOCOLATE);
    assigned.insert(JJ2_FOOD_ICE_CREAM, PC_FOOD_ICE_CREAM);
    assigned.insert(JJ2_FOOD_BURGER, PC_FOOD_BURGER);
    assigned.insert(JJ2_FOOD_PIZZA, PC_FOOD_PIZZA);
    assigned.insert(JJ2_FOOD_FRIES, PC_FOOD_FRIES);
    assigned.insert(JJ2_FOOD_CHICKEN_LEG, PC_FOOD_CHICKEN_LEG);
    assigned.insert(JJ2_FOOD_SANDWICH, PC_FOOD_SANDWICH);
    assigned.insert(JJ2_FOOD_TACO, PC_FOOD_TACO);
    assigned.insert(JJ2_FOOD_HOT_DOG, PC_FOOD_HOT_DOG);
    assigned.insert(JJ2_FOOD_HAM, PC_FOOD_HAM);
    assigned.insert(JJ2_FOOD_CHEESE, PC_FOOD_CHEESE);
    assigned.insert(JJ2_SCENERY_COLLAPSE, PC_SCENERY_COLLAPSE);
    assigned.insert(JJ2_CARROT, PC_CARROT);
    assigned.insert(JJ2_TURTLE_SHELL, PC_TURTLE_SHELL);
    assigned.insert(JJ2_CARROT_FULL, PC_CARROT_FULL);
    assigned.insert(JJ2_ENEMY_SUCKER_FLOAT, PC_ENEMY_SUCKER_FLOAT);
    assigned.insert(JJ2_ENEMY_SUCKER, PC_ENEMY_SUCKER);
    assigned.insert(JJ2_POWERUP_BLASTER, PC_POWERUP_BLASTER);
    assigned.insert(JJ2_POWERUP_BOUNCER, PC_POWERUP_BOUNCER);
    assigned.insert(JJ2_POWERUP_FREEZER, PC_POWERUP_FREEZER);
    assigned.insert(JJ2_POWERUP_SEEKER, PC_POWERUP_SEEKER);
    assigned.insert(JJ2_POWERUP_RF, PC_POWERUP_RF);
    assigned.insert(JJ2_POWERUP_TOASTER, PC_POWERUP_TOASTER);
    assigned.insert(JJ2_POWERUP_PEPPER, PC_POWERUP_PEPPER);
    assigned.insert(JJ2_POWERUP_ELECTRO, PC_POWERUP_ELECTRO);
    assigned.insert(JJ2_ENEMY_LAB_RAT, PC_ENEMY_LAB_RAT);
    assigned.insert(JJ2_LIGHT_DIM, PC_LIGHT_STEADY);
    assigned.insert(JJ2_LIGHT_STEADY, PC_LIGHT_STEADY);
    assigned.insert(JJ2_LIGHT_PULSE, PC_LIGHT_PULSE);
}

QMap< JJ2Event, PCEvent > EventIDConverter::assigned = QMap< JJ2Event, PCEvent >();
QStringList EventIDConverter::familiar_names = QStringList();
