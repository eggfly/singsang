#ifndef SINGSANG_BATTERY_WIDGET_HPP
#define SINGSANG_BATTERY_WIDGET_HPP

#include "base_widget.hpp"

namespace singsang
{
class CBatteryWidget : public CBaseWidget
{
  public:
    CBatteryWidget() : CBaseWidget(270, 40, 40, 40) {}

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

        if (m_batteryVoltage > 3.4)
        {
          newIconPath = "/media/icon-battery-1.png";
        }
        else if (m_batteryVoltage > 3.6)
        {
          newIconPath = "/media/icon-battery-2.png";
        }
        else if (m_batteryVoltage > 3.8)
        {
          newIconPath = "/media/icon-battery-3.png";
        }
        else if (m_batteryVoltage > 4.0)
        {
          newIconPath = "/media/icon-battery-4.png";
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
      M5.Lcd.drawString(buf, m_positionX + 3, m_positionY - 10);
    }

  private:
    String m_currentIconPath{"/media/icon-battery.png"};
    float m_batteryVoltage;
};

}  // namespace singsang

#endif  // SINGSANG_BATTERY_WIDGET_HPP
