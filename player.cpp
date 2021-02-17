#include "player.hpp"

namespace singsang
{
CPlayer::CPlayer() {}

void CPlayer::begin()
{
  initializeHardware();
  initializeGui();
  // startNextSong();
}

void CPlayer::loop()
{
  m_audio.loop();

  handleTouchEvents();

  updateGui();

  autoPlayNextSong();

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
  changeBrightness(m_currentBrightness);

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
  m_songInfoWidget.draw(false);
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

void CPlayer::autoPlayNextSong() {
  if (!m_audio.isRunning() && (millis() - m_lastOperationTime) < m_autoPlayMilliSecUntilSleep) {
    Serial.println("autoPlay: playNextSong()");
    startNextSong();
  }
}

void CPlayer::handleInactivityTimeout() {
  if (m_audio.isRunning()) {
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
      Serial.println("Reached inactive timeout, power off...");
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

  bool handled = false;
  if (m_nextSongWidget.isTouched(touchPoint))
  {
    vibrate();
    startNextSong();
    handled = true;
  }

  if (m_volumeDownWidget.isTouched(touchPoint))
  {
    decreaseVolume();
    vibrate();
    handled = true;
  }

  if (m_volumeUpWidget.isTouched(touchPoint))
  {
    increaseVolume();
    vibrate();
    handled = true;
  }

  if (m_lockScreenWidget.isTouched(touchPoint)) {
    changeBrightness(!m_currentBrightness);
    vibrate();
  }

  if (handled) {
    m_lastOperationTime = millis();
    changeBrightness(true);
  }
}

void CPlayer::startNextSong()
{
  if (m_songFiles.size() == 0)
  {
    return;
  }
  m_played_songs.insert(m_activeSongIdx);
  if (m_played_songs.size() * 2 > m_songFiles.size()) {
    Serial.println("re-shuffle.");
    m_played_songs.clear();
  }
  do {
    m_activeSongIdx = random(m_songFiles.size());
  } while (m_played_songs.find(m_activeSongIdx) != std::end(m_played_songs));

  //  if (m_activeSongIdx >= m_songFiles.size() || m_activeSongIdx < 0)
  //  {
  //    m_activeSongIdx = 0;
  //  }

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
  if (m_audio.isRunning() && m_audio.getAudioFileDuration() > 0) {
    audioProgressPercentage = 100. * m_audio.getAudioCurrentTime() / m_audio.getAudioFileDuration();
  }
  m_progressWidget.update(audioProgressPercentage);

  m_fileSelectionWidget.update(m_songFiles.size(), m_activeSongIdx);

  m_volumeDisplayWidget.update(m_currentVolume);

  if (m_activeSongIdx >= 0) {
    m_songInfoWidget.update(m_songFiles[m_activeSongIdx]);
  }
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

void CPlayer::changeBrightness(bool isHigh) {
  m_currentBrightness = isHigh;
  uint8_t brightness = m_currentBrightness ? 96 : 16;
  setScreenBrightness(brightness);
}

void CPlayer::setScreenBrightness(uint8_t brightness) {
  const uint16_t minVoltage = 2500;
  const uint16_t maxVoltage = 3300;
  uint16_t voltage = minVoltage + brightness * (maxVoltage - minVoltage) / 255;
  Serial.printf("change brightness to %d, voltage=%dV\n", brightness, voltage);
  M5.Axp.SetLcdVoltage(voltage);
}

void CPlayer::vibrate()
{
  M5.Axp.SetLDOEnable(3, true);
  delay(150);
  M5.Axp.SetLDOEnable(3, false);
}

}  // namespace singsang
