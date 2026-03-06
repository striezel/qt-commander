#ifndef SETTINGS_H
#define SETTINGS_H

#include <QDir>
#include <QSettings>

class Settings
{
public:
    Settings();

    static const QDir::Filters defaultFilters;

    static const QDir::SortFlags defaultSortFlags;

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

private:
    /// current filters for shown files/directories - applies to both views
    QDir::Filters filters;

    /// current sort flags for shown files/directories - applies to both views
    QDir::SortFlags sortFlags;
};

#endif // SETTINGS_H
