//
// Timer.h
// 

#ifndef TIMER_H
#define TIMER_H

// Engine includes.
#include "Event.h"
#include "ViewObject.h"

const std::string TIMER_STRING = "Time";

class Timer : public df::ViewObject {

private:
	int m_ticks = 0;
	// Handle step events.
	int step(const df::EventStep* p_e);

public:
	int serialize(std::stringstream* stream, unsigned int attr = 0) override;
	int deserialize(std::stringstream* stream, unsigned int* attr = nullptr) override;

	// Constructor.
	Timer();

	// Handle events.
	int eventHandler(const df::Event* p_e) override;
};

#endif // TIMER_H
