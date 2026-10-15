//
// Grocer.cpp
//

// System includes.
#include <stdlib.h>    // for rand()

// Engine includes.
#include "EventStep.h"
#include "GameManager.h"
#include "LogManager.h"
#include "WorldManager.h"

// Game includes.
#include "Fruit.h"
#include "GameOver.h"
#include "Grocer.h"

Grocer::Grocer() {
    setType(GROCER_STRING);
    setSolidness(df::SPECTRAL);
    setVisible(false);
    registerInterest(df::STEP_EVENT);
    m_wave = 1;
    m_wave_end = WAVE_LEN;
    m_wave_speed = WAVE_SPEED; // Starting speed (spaces/tick).
    m_wave_spawn = WAVE_SPAWN; // Starting spawn rate (ticks).
    m_spawn = m_wave_spawn;
}

// Handle event.
// Return 0 if ignored, else 1.
int Grocer::eventHandler(const df::Event* p_e) {

    // Step event.
    if (p_e->getType() == df::STEP_EVENT)
        return step((df::EventStep*)p_e);

    // If get here, have ignored this event.
    return 0;
}

// Handle step event.
int Grocer::step(const df::EventStep* p_e) {

    // Fruit grocer.
    m_spawn -= 1;
    if (m_spawn < 0) {

        int mod = m_wave + 1 > NUM_FRUITS ? NUM_FRUITS : m_wave + 1;
        Fruit* p_f = new Fruit(FRUIT[rand() % mod]);
        if (!p_f) {
            LM.writeLog("Grocer::step(): Error! Unable to allocate Fruit.");
            return 0;
        }

        p_f->start(m_wave_speed);

        m_spawn = m_wave_spawn;
    }

    // Advance wave.
    m_wave_end -= 1;
    if (m_wave_end < 0) {

        m_wave_end = WAVE_LEN;
        m_wave_spawn += SPAWN_INC; // Increase spawn rate.
        m_wave_speed += SPEED_INC; // Increase Fruit speed.
        m_wave += 1;
        if (m_wave == NUM_WAVES + 1)
            this->gameOver();
    }

    return 1;
}

// Do game over actions.
void Grocer::gameOver() {
    // Create G-A-M-E O-V-E-R object.
    new GameOver();

    // Destroy all remaining Fruit (no points).
    df::ObjectList ol = WM.solidObjects();
    for (int i = 0; i < ol.getCount(); i++)
        if (dynamic_cast <Fruit*>(ol[i]))
            WM.markForDelete(ol[i]);

    WM.markForDelete(this);
}
