#include "settings.h"
#include <QSettings>

const QDir::Filters Settings::defaultFilters{QDir::Filter::AllEntries
                                             | QDir::Filter::NoDot
                                             | QDir::Filter::Hidden
                                             | QDir::Filter::System};

const QDir::SortFlags Settings::defaultSortFlags{QDir::SortFlag::Name
                                                 | QDir::SortFlag::DirsFirst
                                                 | QDir::SortFlag::IgnoreCase};

Settings::Settings()
    : filters(defaultFilters)
    , sortFlags(defaultSortFlags)
{}

void Settings::resetToDefaults()
{
    filters = defaultFilters;
    sortFlags = defaultSortFlags;
}

void Settings::save()
{
    QSettings settings(QSettings::IniFormat, QSettings::UserScope, "Example Organization", "Qt Commander");

    settings.setValue("filters", filters.toInt());
    settings.setValue("sort-flags", sortFlags.toInt());
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
