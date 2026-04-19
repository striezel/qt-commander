/*
 -------------------------------------------------------------------------------
    This file is part of the Qt Commander file manager.
    Copyright (C) 2026  Dirk Stolle

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
 -------------------------------------------------------------------------------
*/

#ifndef SETTINGS_H
#define SETTINGS_H

#include <QDir>
#include <QFont>

class Settings
{
public:
    Settings();

    static const QDir::Filters defaultFilters;

    static const QDir::SortFlags defaultSortFlags;

    static const bool defaultUseProvidedFileIcons;

    static const bool defaultShowFormattedSize;

    static QFont defaultTextViewerFont();

    static const bool defaultAutoStartVideos;
    static const int defaultVideoVolume;

    static const bool defaultAutoPlayAudio;
    static const bool defaultLoopAudioForever;
    static const int defaultAudioVolume;

    /// Resets all values to their defaults.
    void resetToDefaults();

    /// Saves the settings.
    ///
    /// Currently, the save mechanism uses a .ini file, but that may or may not
    /// change in future versions.
    void save();

    /// Loads previously saved settings.
    ///
    /// If settings cannot be loaded, it falls back to default values.
    void load();

    QDir::Filters getFilters() const;
    void setFilters(const QDir::Filters filters);

    QDir::SortFlags getSortFlags() const;
    void setSortFlags(const QDir::SortFlags flags);

    bool getUseProvidedFileIcons() const;
    void setUseProvidedFileIcons(const bool useProvided);

    bool getShowFormattedSize() const;
    void setShowFormattedSize(const bool formatted);

    QFont getTextViewerFont() const;
    void setTextViewerFont(QFont font);

    bool getAutoStartVideos() const;
    void setAutoStartVideos(const bool autoStart);

    int getVideoVolume() const;
    void setVideoVolume(const int volume);

    bool getAutoPlayAudio() const;
    void setAutoPlayAudio(const bool autoPlay);

    bool getLoopAudioForever() const;
    void setLoopAudioForever(const bool loopForever);

    int getAudioVolume() const;
    void setAudioVolume(const int volume);
private:
    /// current filters for shown files/directories - applies to both views
    QDir::Filters filters;

    /// current sort flags for shown files/directories - applies to both views
    QDir::SortFlags sortFlags;

    /// whether to use file icons provided by QFileIconProvider
    bool useProvidedFileIcons;

    /// whether to show file size as human-readable, localized string
    bool showFormattedSize;

    /// the font which is used by the text viewer
    QFont textViewerFont;

    /// whether videos in movie viewer start automatically
    bool autoStartVideos;

    /// volume of video files in range [0;100]
    int videoVolume;

    /// whether files in audio player start playing automatically
    bool autoPlayAudio;

    /// whether files in audio player should be repeated when they reach the end
    bool loopAudioForever;

    /// volume of audio files in range [0;100]
    int audioVolume;
};

#endif // SETTINGS_H
