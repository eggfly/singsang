#ifndef SINGSANG_BATTERY_WIDGET_HPP
#define SINGSANG_BATTERY_WIDGET_HPP

#include "base_widget.hpp"

namespace singsang
{
class CBatteryWidget : public CBaseWidget
{
  public:
    CBatteryWidget() : CBaseWidget(140, 15, 40, 40) {}

    void update()
    {
      String newIconPath{m_currentIconPath};

      const bool isCharging = (M5.Axp.GetVinVoltage() > 3.F);
      if (isCharging)
      {
        newIconPath = "/media/icon-battery-charging.png";
      }
      else
      {
        const auto batteryPower = M5.Axp.GetBatPower();
        m_batteryVoltage = M5.Axp.GetBatVoltage();

        if (m_batteryVoltage > 3.98) {
          // 100% - 80%
          newIconPath = "/media/icon-battery-4.png";
        } else if (m_batteryVoltage > 3.87) {
          // 80% - 60%
          newIconPath = "/media/icon-battery-3.png";
        } else if (m_batteryVoltage > 3.79) {
          // 60% - 40%
          newIconPath = "/media/icon-battery-2.png";
        } else if (m_batteryVoltage > 3.68) {
          // 40% - 20%
          newIconPath = "/media/icon-battery-1.png";
        } else {
          // 20% - 0%
          newIconPath = "/media/icon-battery-0.png";
        }
      }

      if (!m_currentIconPath.equals(newIconPath))
      {
        m_currentIconPath = newIconPath;
        draw(true);
      }
    }

    void draw(const bool updateOnly)
    {
      M5.Lcd.drawPngFile(SD, m_currentIconPath.c_str(), m_positionX,
                         m_positionY, m_sizeX, m_sizeY);
      char buf[128];
      snprintf(buf, 128, "%.2fV", m_batteryVoltage);
      M5.Lcd.setTextColor(0xFF808080);
      M5.Lcd.drawString(buf, m_positionX + 42, m_positionY + 11);
    }

  private:
    String m_currentIconPath{"/media/icon-battery.png"};
    float m_batteryVoltage;
};

}  // namespace singsang

#endif  // SINGSANG_BATTERY_WIDGET_HPP
