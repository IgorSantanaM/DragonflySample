#include "state.h"
#include "fruit.h"
#include "grocer.h"
#include "sword.h"
#include "timer.h"
#include "points.h"
#include "LogManager.h"
#include "WorldManager.h"
#include <climits>
#include <cstdint>
#include <fstream>
#include <sstream>
#include <vector>

namespace {
const char* SAVE_FILE = "savegame.df";
const std::uint32_t MAGIC = 0x44465331; 
template<class T> void write(std::stringstream& s, const T& value) {
    s.write(reinterpret_cast<const char*>(&value), sizeof(value));
}
template<class T> bool read(std::stringstream& s, T& value) {
    return static_cast<bool>(s.read(reinterpret_cast<char*>(&value), sizeof(value)));
}
bool persistent(df::Object* object) {
    return dynamic_cast<Fruit*>(object) || dynamic_cast<Grocer*>(object) ||
        dynamic_cast<Sword*>(object) || dynamic_cast<Timer*>(object) ||
        dynamic_cast<Points*>(object);
}
df::Object* create(const std::string& type) {
    if (type == GROCER_STRING) return new Grocer();
    if (type == SWORD_STRING) return new Sword();
    if (type == TIMER_STRING) return new Timer();
    if (type == POINTS_STRING) return new Points();
    for (int i = 0; i < NUM_FRUITS; ++i)
        if (type == FRUIT[i]) return new Fruit(type);
    return nullptr;
}
// Avoid Fruit's explosion/sound destructor, without emitting score events.
void silenceFruit(df::Object* object) {
    if (dynamic_cast<Fruit*>(object))
        object->setPosition(df::Vector(-10000, -10000));
}
}

State::State() {
    setType(STATE_STRING);
    setSolidness(df::SPECTRAL);
    setVisible(false);
    registerInterest(df::KEYBOARD_EVENT);
}
int State::eventHandler(const df::Event* event) {
    if (event->getType() == df::KEYBOARD_EVENT)
        return keyboard(static_cast<const df::EventKeyboard*>(event));
    return 0;
}
int State::keyboard(const df::EventKeyboard* event) {
    if (event->getKeyboardAction() != df::KEY_PRESSED) return 0;
    if (event->getKey() == df::Keyboard::S) { save(); return 1; }
    if (event->getKey() == df::Keyboard::L) { load(); return 1; }
    return 0;
}
void State::save() {
    std::stringstream stream;
    df::ObjectList objects = WM.getAllObjects();
    int count = 0;
    for (int i = 0; i < objects.getCount(); ++i)
        if (persistent(objects[i])) ++count;
    write(stream, MAGIC);
    write(stream, count);
    for (int i = 0; i < objects.getCount(); ++i) {
        df::Object* object = objects[i];
        if (!persistent(object)) continue;
        const std::string type = object->getType();
        std::stringstream payload;
        if (object->serialize(&payload, UINT_MAX) < 0 || !payload) {
            LM.writeLog("State::save(): serialization failed.");
            return;
        }
        const std::string data = payload.str();
        const std::uint32_t nameSize = static_cast<std::uint32_t>(type.size());
        const std::uint32_t dataSize = static_cast<std::uint32_t>(data.size());
        write(stream, nameSize);
        stream.write(type.data(), nameSize);
        write(stream, dataSize);
        stream.write(data.data(), dataSize);
    }
    std::ofstream file(SAVE_FILE, std::ios::binary | std::ios::trunc);
    const std::string data = stream.str();
    file.write(data.data(), data.size());
    file.close();
    LM.writeLog(file ? "State::save(): saved %d objects." :
        "State::save(): could not write save file.", count);
}
void State::load() {
    std::ifstream file(SAVE_FILE, std::ios::binary | std::ios::ate);
    if (!file || file.tellg() < 0 || file.tellg() > 16 * 1024 * 1024) {
        LM.writeLog("State::load(): missing or oversized save file.");
        return;
    }
    file.seekg(0);
    std::stringstream stream;
    stream << file.rdbuf();
    std::uint32_t magic = 0;
    int count = 0;
    if (!read(stream, magic) || magic != MAGIC || !read(stream, count) ||
        count < 0 || count > 10000) {
        LM.writeLog("State::load(): invalid save header.");
        return;
    }
    df::ObjectList previous = WM.getAllObjects();
    std::vector<df::Object*> restored;
    std::vector<bool> active;
    bool valid = true;
    for (int i = 0; i < count && valid; ++i) {
        std::uint32_t nameSize = 0, dataSize = 0;
        if (!read(stream, nameSize) || nameSize == 0 || nameSize > 128) {
            valid = false; break;
        }
        std::string type(nameSize, '\0');
        stream.read(type.data(), nameSize);
        if (!read(stream, dataSize) || dataSize > 1024 * 1024) {
            valid = false; break;
        }
        std::string data(dataSize, '\0');
        if (!stream.read(data.data(), dataSize)) { valid = false; break; }
        df::Object* object = create(type);
        if (!object) { valid = false; break; }
        restored.push_back(object);
        const int id = object->getId();
        std::stringstream payload(data);
        valid = object->deserialize(&payload) >= 0 && payload &&
            payload.peek() == std::char_traits<char>::eof() && object->getType() == type;
        // Keep the engine's fresh ID while old objects await deferred deletion.
        object->setId(id);
        active.push_back(object->isActive());
        object->setActive(false);
    }
    valid = valid && stream.peek() == std::char_traits<char>::eof();
    if (!valid) {
        for (df::Object* object : restored) {
            silenceFruit(object);
            delete object;
        }
        LM.writeLog("State::load(): invalid save; current game retained.");
        return;
    }
    for (int i = 0; i < previous.getCount(); ++i) {
        df::Object* object = previous[i];
        if (object == this) continue;
        silenceFruit(object);
        object->setActive(false);
        WM.markForDelete(object);
    }
    for (size_t i = 0; i < restored.size(); ++i)
        restored[i]->setActive(active[i]);
    LM.writeLog("State::load(): restored %d objects.", count);
}

