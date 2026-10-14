#pragma once
#include "GameManager.h"
#include <EventOut.h>
#include <EventCollision.h>
class Fruit : public df::Object {
	private:
		bool m_first_out;

		int out(const df::EventOut *p_e);

		int collide(const df::EventCollision *p_e);

	public:
		Fruit(std::string name);

		~Fruit();

		int eventHandler(const df::Event *p_e) override;

		void start(float speed);
};