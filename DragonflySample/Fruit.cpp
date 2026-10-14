
#include "EventCollision.h"
#include "EventOut.h"
#include "GameManager.h"
#include "LogManager.h"
#include "WorldManager.h"

#include "game.h"
#include "Fruit.h"
#include "Sword.h"

Fruit::Fruit(std::string name) {

    setType(name);
    if (setSprite(name) != 0)
        LM.writeLog("Fruit::Fruit(): Error! Unable to find sprite: %s",
            name.c_str());
    m_first_out = true; 
    setSolidness(df::SOFT);
}

int Fruit::eventHandler(const df::Event* p_e) {

    if (p_e->getType() == df::OUT_EVENT)
        return out((df::EventOut*)p_e);

    if (p_e->getType() == df::COLLISION_EVENT)
        return collide((df::EventCollision*)p_e);

    return 0;
}

int Fruit::out(const df::EventOut* p_e) {

    if (m_first_out) { 
        m_first_out = false;
        return 1;
    }

    WM.markForDelete(this);

    return 1;
}

int Fruit::collide(const df::EventCollision* p_e) {

    if (p_e->getObject1()->getType() == SWORD_STRING) {

        WM.markForDelete(this);
    }

    return 1;
}

Fruit::~Fruit() {

    if (df::boxContainsPosition(WM.getBoundary(), getPosition()) &&
        GM.getGameOver() == false) {
        df::explode(getAnimation().getSprite(), getAnimation().getIndex(), getPosition(),
            EXPLOSION_AGE, EXPLOSION_SPEED, EXPLOSION_ROTATE);
    }
}

void Fruit::start(float speed) {

    df::Vector begin, end;

    int world_x = (int)WM.getBoundary().getHorizontal();
    int world_y = (int)WM.getBoundary().getVertical();
    df::Vector world_center(world_x / 2.0f, world_y / 2.0f);

    switch (rand() % 4) {

    case 0: 
        begin.setX((float)(rand() % world_x));
        begin.setY(0 - 3.0f);
        end.setX((float)(rand() % world_x));
        end.setY(world_y + 3.0f);
        break;

    case 1: 
        begin.setX(world_x + 3.0f);
        begin.setY((float)(rand() % world_y));
        end.setX(0 - 3.0f);
        end.setY((float)(rand() % world_y));
        break;

    case 2: 
        begin.setX((float)(rand() % world_x));
        begin.setY(world_y + 3.0f);
        end.setX((float)(rand() % world_x));
        end.setY(0 - 3.0f);
        break;

    case 3: 
        begin.setX(0 - 3.0f);
        begin.setY((float)(rand() % world_y));
        end.setX(world_x + 3.0f);
        end.setY((float)(rand() % world_y));
        break;

    default:
        break;
    }

    WM.moveObject(this, begin);

    df::Vector velocity = end - begin;
    velocity.normalize();
    setDirection(velocity);
    setSpeed(speed);
}
