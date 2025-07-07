#pragma once

#include <chrono>

namespace TransportSim {

class IntroScreen {
public:
    IntroScreen();
    void update(); // Advance animation
    void render(); // Draw the intro screen
    bool isDone() const; // Animation and button complete
    bool startClicked() const; // Start button pressed
private:
    std::chrono::steady_clock::time_point m_startTime;
    float m_animProgress; // 0.0 to 1.0
    bool m_showButton;
    bool m_startClicked;
};

} // namespace TransportSim
