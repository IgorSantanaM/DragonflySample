#ifndef STATE_H
#define STATE_H
#include "Object.h"
#include "EventKeyboard.h"
const std::string STATE_STRING = "state";
class State : public df::Object {
    int keyboard(const df::EventKeyboard* event);
public:
    State();
    int eventHandler(const df::Event* event) override;
    void save();
    void load();
};
#endif
