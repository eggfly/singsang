#ifndef SINGSANG_PLAYER_HPP
#define SINGSANG_PLAYER_HPP

#include <Audio.h>
#include <M5Core2.h>
#include <SD.h>
#include <WiFi.h>

#include <memory>
#include <vector>

#include "battery_widget.hpp"
#include "file_selection_widget.hpp"
#include "lock_screen_widget.hpp"
#include "next_song_widget.hpp"
#include "progress_widget.hpp"
#include "volume_display_widget.hpp"
#include "volume_down_widget.hpp"
#include "volume_up_widget.hpp"

namespace singsang
{
class CPlayer
{
  public:
    CPlayer();

    ~CPlayer() = default;

    void begin();

    void loop();

    void startNextSong();

    void updateVolume(int f_deltaVolume);

    void increaseVolume();

    void decreaseVolume();
    void changeBrightnessOrLockScreen();


  private:
    void handleInactivityTimeout();
    void handleTouchEvents();
    void initializeGui();
    void initializeHardware();
    void populateMusicFileList();
    void updateGui();
    void vibrate();
    void setScreenBrightness(uint8_t brightness);

    Audio m_audio{};
    const static uint8_t minBrightness = 32;
    const static uint8_t maxBrightness = 255;
    int                 m_currentVolume{minBrightness};
    uint8_t             m_currentBrightness{64};
    int                 m_activeSongIdx{ -1};
    unsigned int        m_turnOffAfterInactiveForMilliSec{5 * 60 * 1000};
    unsigned int        m_lastActivityTimestamp{0};
    std::vector<String> m_songFiles{};

    CBatteryWidget       m_batteryWidget;
    CFileSelectionWidget m_fileSelectionWidget;
    CLockScreenWidget    m_lockScreenWidget;
    CNextSongWidget      m_nextSongWidget;
    CProgressWidget      m_progressWidget;
    CVolumeDisplayWidget m_volumeDisplayWidget;
    CVolumeDownWidget    m_volumeDownWidget;
    CVolumeUpWidget      m_volumeUpWidget;
};

}  // namespace singsang

#endif  // SINGSANG_PLAYER_HPP
