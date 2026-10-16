#include "state.h"
#include "fruit.h"
#include "grocer.h"
#include "sword.h"
#include "timer.h"
#include "points.h"
#include "util.h"
#include "GameManager.h"
#include "WorldManager.h"
#include "EventStep.h"
#include <cassert>
#include <climits>
#include <fstream>
#include <map>
#include <sstream>
#include <iostream>

std::multimap<std::string, std::string> snapshot() {
    std::multimap<std::string, std::string> result;
    auto objects = WM.getAllObjects();
    for (int i = 0; i < objects.getCount(); ++i) {
        auto object = objects[i];
        if (!object->isActive() || object->getType() == STATE_STRING) continue;
        std::stringstream stream;
        object->serialize(&stream, UINT_MAX & ~static_cast<unsigned int>(df::ObjectAttribute::ID));
        result.emplace(object->getType(), stream.str());
    }
    return result;
}
int main() {
    assert(GM.startUp() == 0);
    loadResources();
    auto state = new State();
    auto grocer = new Grocer();
    auto points = new Points();
    auto sword = new Sword();
    auto timer = new Timer();
    points->setValue(123);
    sword->setPosition(df::Vector(20, 10));
    df::EventStep step;
    for (int i = 0; i < 7; ++i) {
        grocer->eventHandler(&step);
        timer->eventHandler(&step);
    }
    for (int i = 0; i < NUM_FRUITS; ++i) {
        auto fruit = new Fruit(FRUIT[i]);
        fruit->start(0.3f + i * 0.1f);
    }
    auto expected = snapshot();
    df::EventKeyboard key;
    key.setKeyboardAction(df::KEY_PRESSED);
    key.setKey(df::Keyboard::S);
    assert(state->eventHandler(&key) == 1);
    points->setValue(999);
    sword->setPosition(df::Vector(1, 1));
    key.setKey(df::Keyboard::L);
    assert(state->eventHandler(&key) == 1);
    assert(snapshot() == expected);
    state->save();
    state->load();
    assert(snapshot() == expected);
    std::ifstream saved("savegame.df", std::ios::binary);
    std::string bytes((std::istreambuf_iterator<char>(saved)), {});
    saved.close();
    std::ofstream truncated("savegame.df", std::ios::binary | std::ios::trunc);
    truncated.write(bytes.data(), bytes.size() - 1);
    truncated.close();
    state->load();
    assert(snapshot() == expected);
    std::ofstream("savegame.df", std::ios::binary | std::ios::trunc) << "bad";
    state->load();
    assert(snapshot() == expected);
    std::cout << "PASS: keyboard save/load, all fruit types, repeated snapshots, invalid file retention\n";
    GM.setGameOver(true);
    GM.shutDown();
}
