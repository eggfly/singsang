#include "player.hpp"

namespace singsang
{
CPlayer::CPlayer() {}

void CPlayer::begin()
{
  initializeHardware();
  initializeGui();
}

void CPlayer::loop()
{
  m_audio.loop();

  handleTouchEvents();

  updateGui();

  handleInactivityTimeout();
}

void CPlayer::initializeHardware()
{
  M5.begin();

  M5.Axp.SetLed(false);
  M5.Axp.SetLcdVoltage(1800);  // dimmed, nominal value is 2800
  M5.Axp.SetSpkEnable(true);

  WiFi.mode(WIFI_OFF);
  delay(100);

  m_audio.setPinout(12, 0, 2);
  m_audio.setVolume(m_currentVolume);
  setScreenBrightness(m_currentBrightness);

  populateMusicFileList();

  vibrate();
}

void CPlayer::initializeGui()
{
  M5.Lcd.fillScreen(TFT_BLACK);
  M5.Lcd.setTextFont(2);

  M5.Lcd.drawJpgFile(SD, "/media/logo.jpg", 60, 28, 200, 200);

  m_batteryWidget.draw(false);
  m_fileSelectionWidget.draw(false);
  m_lockScreenWidget.draw(false);
  m_nextSongWidget.draw(false);
  m_progressWidget.draw(false);
  m_volumeDisplayWidget.draw(false);
  m_volumeDownWidget.draw(false);
  m_volumeUpWidget.draw(false);
}

void CPlayer::populateMusicFileList()
{
  File musicDir = SD.open("/music");

  bool nextFileFound;
  do
  {
    nextFileFound = false;

    File entry = musicDir.openNextFile();

    if (entry)
    {
      nextFileFound = true;

      const bool entryIsFile = (entry.size() > 0);
      if (entryIsFile)
      {
        m_songFiles.push_back(entry.name());
      }

      entry.close();
    }
  } while (nextFileFound);

  Serial.print("MusicFileList length: ");
  Serial.println(m_songFiles.size());
}

void CPlayer::handleInactivityTimeout()
{
  if (m_audio.isRunning())
  {
    m_lastActivityTimestamp = millis();
  }
  else
  {
    const auto currentTimestamp = millis();
    const bool isTimeoutReached =
      (currentTimestamp >
       (m_lastActivityTimestamp + m_turnOffAfterInactiveForMilliSec));

    if (isTimeoutReached)
    {
      m_audio.stopSong();
      // M5.Axp.DeepSleep(0U);  // power off
      M5.Axp.PowerOff();  // power off
    }
  }
}

void CPlayer::handleTouchEvents()
{
  const auto touchPoint = M5.Touch.getPressPoint();

  const bool isButtonPressed = (touchPoint.x > -1 && touchPoint.y > -1);
  if (!isButtonPressed)
  {
    return;
  }

  if (m_nextSongWidget.isTouched(touchPoint))
  {
    vibrate();
    startNextSong();
  }

  if (m_volumeDownWidget.isTouched(touchPoint))
  {
    decreaseVolume();
    vibrate();
  }

  if (m_volumeUpWidget.isTouched(touchPoint))
  {
    increaseVolume();
    vibrate();
  }

  if (m_lockScreenWidget.isTouched(touchPoint)) {
    changeBrightnessOrLockScreen();
    vibrate();
  }
}

void CPlayer::startNextSong()
{
  if (m_songFiles.size() == 0)
  {
    return;
  }

  m_activeSongIdx++;
  if (m_activeSongIdx >= m_songFiles.size() || m_activeSongIdx < 0)
  {
    m_activeSongIdx = 0;
  }

  if (m_audio.isRunning())
  {
    m_audio.stopSong();
  }

  m_audio.connecttoSD(m_songFiles[m_activeSongIdx]);
}

void CPlayer::updateGui()
{
  m_batteryWidget.update();

  int audioProgressPercentage = 0;
  if (m_audio.isRunning() && m_audio.getAudioFileDuration() > 0)
  {
    audioProgressPercentage = 100. * m_audio.getAudioCurrentTime() /
                              m_audio.getAudioFileDuration();
  }
  m_progressWidget.update(audioProgressPercentage);

  m_fileSelectionWidget.update(m_songFiles.size(), m_activeSongIdx);

  m_volumeDisplayWidget.update(m_currentVolume);
}

void CPlayer::updateVolume(int f_deltaVolume)
{
  constexpr int minVolume = 1;
  constexpr int maxVolume = 16;

  int newVolume = m_currentVolume + f_deltaVolume;

  if (newVolume < minVolume)
  {
    newVolume = minVolume;
  }
  if (newVolume > maxVolume)
  {
    newVolume = maxVolume;
  }

  m_currentVolume = newVolume;
  m_audio.setVolume(m_currentVolume);
}

void CPlayer::increaseVolume()
{
  updateVolume(+4);
}

void CPlayer::decreaseVolume() {
  updateVolume(-4);
}

void CPlayer::changeBrightnessOrLockScreen() {
  // M5.Axp.PowerOff();
  // M5.Lcd.sleep();
  m_currentBrightness += 32;
  if (m_currentBrightness > maxBrightness) {
    m_currentBrightness = minBrightness;
  }
  setScreenBrightness(m_currentBrightness);
}

void CPlayer::setScreenBrightness(uint8_t brightness) {
  const uint16_t minVoltage = 2500;
  const uint16_t maxVoltage = 3300;
  uint16_t voltage = minVoltage + brightness * (maxVoltage - minVoltage) / 255;
  Serial.printf("change brightness to %d\n, voltage=%dV", brightness, voltage);
  M5.Axp.SetLcdVoltage(voltage);
}

void CPlayer::vibrate()
{
  M5.Axp.SetLDOEnable(3, true);
  delay(150);
  M5.Axp.SetLDOEnable(3, false);
}

}  // namespace singsang
