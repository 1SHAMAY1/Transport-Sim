#pragma once

namespace TransportSim {

class Profiling {
public:
    Profiling();
    void render(); // Draw the profiling window
    void setVisible(bool v);
    bool isVisible() const;
private:
    bool m_visible;
    // Dummy stats
    float m_cpuUsage;
    float m_gpuUsage;
    float m_memUsage;
};

} // namespace TransportSim
