#ifndef SINGSANG_LOCK_SCREEN_WIDGET_HPP
#define SINGSANG_LOCK_SCREEN_WIDGET_HPP

#include "base_widget.hpp"

namespace singsang
{
class CLockScreenWidget : public CBaseWidget
{
  public:
    CLockScreenWidget() : CBaseWidget(251, 30, 64, 64) {}

    void update() {}

    void draw(const bool f_updateOnly)
    {
      M5.Lcd.drawPngFile(SD, "/media/icon-lock-screen.png", m_positionX,
                         m_positionY, m_sizeX, m_sizeY);
    }
};

}  // namespace singsang

#endif  // SINGSANG_LOCK_SCREEN_WIDGET_HPP
