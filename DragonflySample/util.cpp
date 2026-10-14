//
// util.cpp
//

// Engine includes.
#include "ResourceManager.h"

#include "game.h"
#include "Sword.h"

void loadResources(void) {

    // Fruit sprites.
    for (int i = 0; i < NUM_FRUITS; i++) {
        std::string name = "sprites/" + FRUIT[i] + ".txt";
        RM.loadSprite(name, FRUIT[i]);
    }

}

