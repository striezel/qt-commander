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

    static QFont defaultTextViewerFont();

    static const bool defaultAutoStartVideos;

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

    QFont getTextViewerFont() const;
    void setTextViewerFont(QFont font);

    bool getAutoStartVideos() const;
    void setAutoStartVideos(const bool autoStart);
private:
    /// current filters for shown files/directories - applies to both views
    QDir::Filters filters;

    /// current sort flags for shown files/directories - applies to both views
    QDir::SortFlags sortFlags;

    /// the font which is used by the text viewer
    QFont textViewerFont;

    /// whether videos in movie viewer start automatically
    bool autoStartVideos;
};

#endif // SETTINGS_H
