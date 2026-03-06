#include "settings.h"
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

Settings::Settings()
    : filters(defaultFilters)
    , sortFlags(defaultSortFlags)
    , textViewerFont(defaultTextViewerFont())
{}

QFont Settings::defaultTextViewerFont()
{
    return QFontDatabase::systemFont(QFontDatabase::SystemFont::FixedFont);
}

void Settings::resetToDefaults()
{
    filters = defaultFilters;
    sortFlags = defaultSortFlags;
    textViewerFont = defaultTextViewerFont();
}

void Settings::save()
{
    QSettings settings(QSettings::IniFormat, QSettings::UserScope,
                       QCoreApplication::organizationName(),
                       QCoreApplication::applicationName());

    settings.setValue("filters", filters.toInt());
    settings.setValue("sort-flags", sortFlags.toInt());
    settings.setValue("text-viewer-font", textViewerFont);
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

    QFont font = settings.value("text-viewer-font", defaultTextViewerFont()).value<QFont>();
    setTextViewerFont(font);
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
        qDebug() << "Falling back to default font for text viewer.";
        font = defaultTextViewerFont();
    }
    textViewerFont = font;
}
