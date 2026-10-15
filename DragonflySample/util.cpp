//
// util.cpp
//

// Engine includes.
#include "Fader.h"
#include "ResourceManager.h"
#include "utility.h"

// Game includes.
#include "game.h"
#include "Grocer.h"
#include "Sword.h"

///////////////////////////////////////////////
// Load resources (sprites, sound effects, music).
void loadResources(void) {

    // Fruit sprites.
    for (int i = 0; i < NUM_FRUITS; i++) {
        std::string name = "sprites/" + FRUIT[i] + ".txt";
        RM.loadSprite(name, FRUIT[i]);
    }

}

///////////////////////////////////////////////
// Populate the world with game objects.
void populateWorld(void) {
    new Grocer();
    new Sword();
}

///////////////////////////////////////////////
// Create trail from p1 to p2.
void create_trail(df::Vector p1, df::Vector p2) {

    const float size = 2;
    const int age = 20;
    const int opacity = 255;
    const df::Color color = df::CYAN;
    unsigned char r, g, b;
    df::colorToRGB(color, r, g, b);

    // Calculate step size for interpolation.
    float dist = df::distance(p1, p2) * 10;
    float dX = (p1.getX() - p2.getX()) / (dist + 1.0f);
    float dY = (p1.getY() - p2.getY()) / (dist + 1.0f);

    // Create Fader particles on line from p1 to p2.
    for (int i = 0; i < dist; i++) {
        float x = p2.getX() + dX * i;
        float y = p2.getY() + dY * i;
        df::Fader* p_f = new df::Fader(size, age, opacity, r, g, b);
        p_f->setPosition(df::Vector(x, y));
    }
}

