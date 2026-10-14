#include "GameManager.h"
#include "LogManager.h"
#include "ResourceManager.h"
#include "WorldManager.h"

#include "fruit.h"
#include "game.h"
#include "sword.h"
#include "util.h"

int main(int argc, char* argv[]) {

    if (GM.startUp()) {
        LM.writeLog("Error starting game manager!");
        GM.shutDown();
        return 0;
    }

    LM.setFlush(true);
    LM.setLogLevel(1);
    LM.writeLog("Fruit Ninja (v%.1f)", VERSION);

    df::splash();
    loadResources();

    Fruit* p_f = new Fruit(FRUIT[rand() % NUM_FRUITS]);
    p_f->start(0.25f);

    GM.run();

    GM.shutDown();

    return 0;
}
