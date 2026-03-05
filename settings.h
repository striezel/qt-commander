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

    void resetToDefaults();

    void save();
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
