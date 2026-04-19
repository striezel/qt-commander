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

#include "settings.h"
#include <algorithm> // for std::clamp()
#include <QCoreApplication>
#include <QFontDatabase>
#include <QSettings>

const QDir::Filters Settings::defaultFilters{QDir::Filter::AllEntries
                                             | QDir::Filter::NoDot
                                             | QDir::Filter::Hidden
                                             | QDir::Filter::System};

const QDir::SortFlags Settings::defaultSortFlags{QDir::SortFlag::Name
                                                 | QDir::SortFlag::DirsFirst
                                                 | QDir::SortFlag::IgnoreCase};

const bool Settings::defaultUseProvidedFileIcons{true};

const bool Settings::defaultShowFormattedSize{true};

const bool Settings::defaultAutoStartVideos{true};
const bool Settings::defaultAutoPlayVideo{true};
const bool Settings::defaultLoopVideoForever{false};
const int Settings::defaultVideoVolume{75};

const bool Settings::defaultAutoPlayAudio{false};
const bool Settings::defaultLoopAudioForever{false};
const int Settings::defaultAudioVolume{50};

Settings::Settings()
    : filters(defaultFilters)
    , sortFlags(defaultSortFlags)
    , useProvidedFileIcons(defaultUseProvidedFileIcons)
    , showFormattedSize(defaultShowFormattedSize)
    , textViewerFont(defaultTextViewerFont())
    , autoStartVideos(defaultAutoStartVideos)
    , autoPlayVideo(defaultAutoPlayVideo)
    , loopVideoForever(defaultLoopVideoForever)
    , videoVolume(defaultVideoVolume)
    , autoPlayAudio(defaultAutoPlayAudio)
    , loopAudioForever(defaultLoopAudioForever)
    , audioVolume(defaultAudioVolume)
{}

QFont Settings::defaultTextViewerFont()
{
    return QFontDatabase::systemFont(QFontDatabase::SystemFont::FixedFont);
}

void Settings::resetToDefaults()
{
    filters = defaultFilters;
    sortFlags = defaultSortFlags;
    useProvidedFileIcons = defaultUseProvidedFileIcons;
    showFormattedSize = defaultShowFormattedSize;
    textViewerFont = defaultTextViewerFont();
    autoStartVideos = defaultAutoStartVideos;
    autoPlayVideo = defaultAutoPlayVideo;
    loopVideoForever = defaultLoopVideoForever;
    videoVolume = defaultVideoVolume;
    autoPlayAudio = defaultAutoPlayAudio;
    loopAudioForever = defaultLoopAudioForever;
    audioVolume = defaultAudioVolume;
}

void Settings::save()
{
    QSettings settings(QSettings::IniFormat, QSettings::UserScope,
                       QCoreApplication::organizationName(),
                       QCoreApplication::applicationName());

    settings.setValue("filters", filters.toInt());
    settings.setValue("sort-flags", sortFlags.toInt());
    settings.setValue("use-provided-file-icons", useProvidedFileIcons);
    settings.setValue("show-formatted-size", showFormattedSize);
    settings.setValue("text-viewer-font", textViewerFont);
    settings.setValue("movie-viewer-auto-start", autoStartVideos);
    settings.setValue("video-player-auto-play", autoPlayVideo);
    settings.setValue("video-player-loop-forever", loopVideoForever);
    settings.setValue("video-player-volume", videoVolume);
    settings.setValue("audio-player-auto-play", autoPlayAudio);
    settings.setValue("audio-player-loop-forever", loopAudioForever);
    settings.setValue("audio-player-volume", audioVolume);
}

void Settings::load()
{
    QSettings settings(QSettings::IniFormat, QSettings::UserScope,
                       QCoreApplication::organizationName(),
                       QCoreApplication::applicationName());

    const int filters = settings.value("filters", defaultFilters.toInt()).toInt();
    setFilters(QDir::Filters::fromInt(filters));

    const int sort_flags = settings.value("sort-flags", defaultSortFlags.toInt()).toInt();
    setSortFlags(QDir::SortFlags::fromInt(sort_flags));

    useProvidedFileIcons = settings.value("use-provided-file-icons", defaultUseProvidedFileIcons).toBool();
    showFormattedSize = settings.value("show-formatted-size", defaultShowFormattedSize).toBool();

    QFont font = settings.value("text-viewer-font", defaultTextViewerFont()).value<QFont>();
    setTextViewerFont(font);

    autoStartVideos = settings.value("movie-viewer-auto-start", defaultAutoStartVideos).toBool();
    autoPlayVideo = settings.value("video-player-auto-play", defaultAutoPlayVideo).toBool();
    loopVideoForever = settings.value("video-player-loop-forever", defaultLoopVideoForever).toBool();
    const int vid_volume = settings.value("video-player-volume", defaultVideoVolume).toInt();
    setVideoVolume(vid_volume);

    autoPlayAudio = settings.value("audio-player-auto-play", defaultAutoPlayAudio).toBool();
    loopAudioForever = settings.value("audio-player-loop-forever", defaultLoopAudioForever).toBool();
    const int volume = settings.value("audio-player-volume", defaultAudioVolume).toInt();
    setAudioVolume(volume);
}

QDir::Filters Settings::getFilters() const
{
    return filters;
}

void Settings::setFilters(QDir::Filters new_filters)
{
    // Only some filters are allowed / useful in the context of the application,
    // so we whitelist them and remove all other flags;
    const QDir::Filters whitelist = QDir::Dirs | QDir::Files| QDir::Drives
                                    | QDir::NoDot | QDir::Hidden | QDir::System
                                    | QDir::CaseSensitive;
    new_filters &= whitelist;

    // Some filters need to be set always to ensure proper working of the
    // application, so we set them here.
    new_filters |= QDir::Dirs | QDir::Drives | QDir::NoDot;

    this->filters = new_filters;
}

QDir::SortFlags Settings::getSortFlags() const
{
    return sortFlags;
}

void Settings::setSortFlags(QDir::SortFlags flags)
{
    // avoid conflicting sort flags for dirs first / dirs last
    if (flags.testFlag(QDir::SortFlag::DirsFirst) && flags.testFlag(QDir::SortFlag::DirsLast))
    {
        flags ^= QDir::SortFlag::DirsLast;
    }
    // avoid conflicting sort flags for sort criterion
    if (flags.testFlag(QDir::SortFlag::Size) + flags.testFlag(QDir::SortFlag::Time) + flags.testFlag(QDir::SortFlag::Type) > 1)
    {
        flags &= ~(QDir::SortFlag::Size | QDir::SortFlag::Time | QDir::SortFlag::Type);
        flags |= QDir::SortFlag::Name;
    }

    sortFlags = flags;
}

bool Settings::getUseProvidedFileIcons() const
{
    return useProvidedFileIcons;
}

void Settings::setUseProvidedFileIcons(const bool useProvided)
{
    useProvidedFileIcons = useProvided;
}

bool Settings::getShowFormattedSize() const
{
    return showFormattedSize;
}

void Settings::setShowFormattedSize(const bool formatted)
{
    showFormattedSize = formatted;
}

QFont Settings::getTextViewerFont() const
{
    return textViewerFont;
}

void Settings::setTextViewerFont(QFont font)
{
    // If font family is not set or the family does not exist in the font
    // database, then we got something weird. Fall back to default font in that
    // case.
    if (font.family().isEmpty()
        || !QFontDatabase::families().contains(font.family()))
    {
        font = defaultTextViewerFont();
    }
    textViewerFont = font;
}

bool Settings::getAutoStartVideos() const
{
    return autoStartVideos;
}

void Settings::setAutoStartVideos(const bool autoStart)
{
    autoStartVideos = autoStart;
}

bool Settings::getAutoPlayVideo() const
{
    return autoPlayVideo;
}

void Settings::setAutoPlayVideo(const bool autoPlay)
{
    autoPlayVideo = autoPlay;
}

bool Settings::getLoopVideoForever() const
{
    return loopVideoForever;
}

void Settings::setLoopVideoForever(const bool loopForever)
{
    loopVideoForever = loopForever;
}

int Settings::getVideoVolume() const
{
    return videoVolume;
}

void Settings::setVideoVolume(const int volume)
{
    videoVolume = std::clamp(volume, 0, 100);
}

bool Settings::getAutoPlayAudio() const
{
    return autoPlayAudio;
}

void Settings::setAutoPlayAudio(const bool autoPlay)
{
    autoPlayAudio = autoPlay;
}

bool Settings::getLoopAudioForever() const
{
    return loopAudioForever;
}

void Settings::setLoopAudioForever(const bool loopForever)
{
    loopAudioForever = loopForever;
}

int Settings::getAudioVolume() const
{
    return audioVolume;
}

void Settings::setAudioVolume(const int volume)
{
    audioVolume = std::clamp(volume, 0, 100);
}
