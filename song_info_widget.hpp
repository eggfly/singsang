#ifndef SINGSANG_SONG_INFO_WIDGET_HPP
#define SINGSANG_SONG_INFO_WIDGET_HPP

#include "base_widget.hpp"

namespace singsang
{
class CSongInfoWidget : public CBaseWidget
{
  public:
    CSongInfoWidget() : CBaseWidget(72, 22, 320 - 72 - 72, 13) {}

    void update(String songName) {
      if (!m_currentSongName.equals(songName)) {
        Serial.println(songName);
        m_currentSongName = songName;
        draw(true);
      }
    }

    void draw(const bool updateOnly) {
      auto p_str = m_currentSongName.c_str() + strlen("/music/");
      auto width = M5.Lcd.textWidth(p_str);
      // M5.Lcd.color565(0x80, 0x80, 0x80)
      M5.Lcd.fillRect(m_positionX, m_positionY, m_sizeX, m_sizeY, TFT_BLACK);
      M5.Lcd.setTextColor(TFT_YELLOW);
      int pos_x = m_positionX;
      if (width < m_sizeX) {
        pos_x += (m_sizeX - width) / 2;
      }
      // Serial.printf("text=%s, width=%d, m_sizeX=%d\n", p_str, width, m_sizeX);
      M5.Lcd.drawString(p_str, pos_x, m_positionY - 3);
    }

  private:
    String m_currentSongName{""};
};

}  // namespace singsang

#endif  // SINGSANG_SONG_INFO_WIDGET_HPP
