#pragma once
#include <functional>

namespace TransportSim {

class UI {
public:
    UI();
    void renderTopBar();
    // Callbacks for menu actions
    void setProfilingCallback(const std::function<void()>& cb);
    void setMemoryUsageCallback(const std::function<void()>& cb);
    void setChangePathCallback(const std::function<void()>& cb);
    void setSetDestinationCallback(const std::function<void()>& cb);
    void setExportCallback(const std::function<void()>& cb);
    void setImportCallback(const std::function<void()>& cb);
private:
    std::function<void()> m_onProfiling;
    std::function<void()> m_onMemoryUsage;
    std::function<void()> m_onChangePath;
    std::function<void()> m_onSetDestination;
    std::function<void()> m_onExport;
    std::function<void()> m_onImport;
};

} // namespace TransportSim
