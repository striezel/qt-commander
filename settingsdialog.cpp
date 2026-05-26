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

#include "settingsdialog.h"
#include "ui_settingsdialog.h"
#include <QFontDialog>

SettingsDialog::SettingsDialog(const Settings& currentSettings, QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::SettingsDialog)
    , initialSettings(currentSettings)
    , selectedTextViewerFont(currentSettings.getTextViewerFont())
{
    ui->setupUi(this);

    connect(ui->cbSortDirectoriesFirst, &QAbstractButton::toggled, this, &SettingsDialog::cbSortSomethingFirstToggled);
    connect(ui->cbSortFilesFirst, &QAbstractButton::toggled, this, &SettingsDialog::cbSortSomethingFirstToggled);

    connect(ui->btnChangeTextViewerFont, &QPushButton::clicked, this, &SettingsDialog::btnChangeTextViewerFontClicked);

    connect(ui->sliderAudioVolume, &QSlider::valueChanged, this, &SettingsDialog::sliderAudioVolumeValueChanged);
    connect(ui->sliderVideoVolume, &QSlider::valueChanged, this, &SettingsDialog::sliderVideoVolumeValueChanged);

    putSettingsIntoGui(currentSettings);
}

SettingsDialog::~SettingsDialog()
{
    delete ui;
}

Settings SettingsDialog::selectedSettings() const
{
    Settings settings;

    QDir::Filters filters = QDir::Dirs | QDir::Drives | QDir::NoDot;
    if (ui->cbShowHiddenFiles->isChecked())
    {
        filters.setFlag(QDir::Filter::Hidden);
    }
    if (ui->cbShowSystemFiles->isChecked())
    {
        filters.setFlag(QDir::Filter::System);
    }
    if (!ui->cbHideFiles->isChecked())
    {
        filters.setFlag(QDir::Filter::Files);
    }
    settings.setFilters(filters);

    QDir::SortFlags sortFlags = QDir::SortFlag::Name;
    if (ui->rbSortByDate->isChecked())
    {
        sortFlags.setFlag(QDir::SortFlag::Time);
    }
    if (ui->rbSortBySize->isChecked())
    {
        sortFlags.setFlag(QDir::SortFlag::Size);
    }
    if (ui->rbSortByType->isChecked())
    {
        sortFlags.setFlag(QDir::SortFlag::Type);
    }
    if (ui->cbReverseSort->isChecked())
    {
        sortFlags.setFlag(QDir::SortFlag::Reversed);
    }
    if (ui->cbSortIgnoreCase->isChecked())
    {
        sortFlags.setFlag(QDir::SortFlag::IgnoreCase);
    }
    if (ui->cbSortDirectoriesFirst->isChecked())
    {
        sortFlags.setFlag(QDir::SortFlag::DirsFirst);
    }
    if (ui->cbSortFilesFirst->isChecked())
    {
        sortFlags.setFlag(QDir::SortFlag::DirsLast);
    }
    settings.setSortFlags(sortFlags);

    settings.setUseProvidedFileIcons(ui->cbUseProvidedIcons->isChecked());
    settings.setShowFormattedSize(ui->cbShowFormattedSize->isChecked());

    settings.setTextViewerFont(selectedTextViewerFont);
    settings.setTextViewerAutoSelectLanguage(ui->cbTextViewerAutoSelectLanguage->isChecked());
    const ThemeId hl_theme =  ui->rbDefaultDarkStyle->isChecked() ? ThemeId::DefaultDark : ThemeId::DefaultLight;
    settings.setTextViewerHighlightingTheme(hl_theme);

    settings.setAutoPlayVideo(ui->cbAutoPlayVideo->isChecked());
    settings.setLoopVideoForever(ui->cbLoopVideoForever->isChecked());
    settings.setVideoVolume(ui->sliderVideoVolume->value());

    settings.setAutoPlayAudio(ui->cbAutoPlayAudio->isChecked());
    settings.setLoopAudioForever(ui->cbLoopAudioForever->isChecked());
    settings.setAudioVolume(ui->sliderAudioVolume->value());

    settings.setSelectedHashAlgorithm(initialSettings.getSelectedHashAlgorithm());

    return settings;
}

void SettingsDialog::cbSortSomethingFirstToggled(bool checked)
{
    if (!checked)
    {
        return;
    }

    // Disable the other checkbox, because they are mutually exclusive.
    // (Button group does not work, because it does not allow scenarios where
    //  it is OK when none of the alternatives is checked.)
    if (sender() == ui->cbSortDirectoriesFirst)
    {
        ui->cbSortFilesFirst->setChecked(false);
    }
    else
    {
        ui->cbSortDirectoriesFirst->setChecked(false);
    }
}

void SettingsDialog::btnChangeTextViewerFontClicked()
{
    bool ok = false;
    const QFont new_font = QFontDialog::getFont(&ok, selectedTextViewerFont, this);
    if (!ok)
    {
        return;
    }

    selectedTextViewerFont = new_font;
    ui->lblTextViewerFontValue->setText(fontToString(new_font));
}

void SettingsDialog::sliderAudioVolumeValueChanged(int value)
{
    ui->lblAudioVolumeValue->setText(QString::number(value) + " %");
}

void SettingsDialog::sliderVideoVolumeValueChanged(int value)
{
    ui->lblVideoVolumeValue->setText(QString::number(value) + " %");
}

void SettingsDialog::putSettingsIntoGui(const Settings &settings)
{
    // directory view
    const QDir::Filters filters = settings.getFilters();
    ui->cbShowHiddenFiles->setChecked(filters.testFlag(QDir::Filter::Hidden));
    ui->cbShowSystemFiles->setChecked(filters.testFlag(QDir::Filter::System));
    ui->cbHideFiles->setChecked(!filters.testFlag(QDir::Filter::Files));

    const QDir::SortFlags sort = settings.getSortFlags();
    ui->rbSortByName->setChecked(true);
    ui->rbSortByDate->setChecked(sort.testFlag(QDir::SortFlag::Time));
    ui->rbSortBySize->setChecked(sort.testFlag(QDir::SortFlag::Size));
    ui->rbSortByType->setChecked(sort.testFlag(QDir::SortFlag::Type));

    ui->cbReverseSort->setChecked(sort.testFlag(QDir::SortFlag::Reversed));
    ui->cbSortIgnoreCase->setChecked(sort.testFlag(QDir::SortFlag::IgnoreCase));

    ui->cbSortDirectoriesFirst->setChecked(sort.testFlag(QDir::SortFlag::DirsFirst));
    ui->cbSortFilesFirst->setChecked(sort.testFlag(QDir::SortFlag::DirsLast));

    ui->cbUseProvidedIcons->setChecked(settings.getUseProvidedFileIcons());
    ui->cbShowFormattedSize->setChecked(settings.getShowFormattedSize());

    // text viewer
    ui->lblTextViewerFontValue->setText(fontToString(settings.getTextViewerFont()));
    ui->cbTextViewerAutoSelectLanguage->setChecked(settings.getTextViewerAutoSelectLanguage());
    const ThemeId hl_theme = settings.getTextViewerHightlightingTheme();
    if (hl_theme == ThemeId::DefaultDark)
    {
        ui->rbDefaultDarkStyle->setChecked(true);
    }
    else
    {
        ui->rbDefaultLightStyle->setChecked(true);
    }

    // audio player
    ui->cbAutoPlayAudio->setChecked(settings.getAutoPlayAudio());
    ui->cbLoopAudioForever->setCheckable(settings.getLoopAudioForever());
    ui->sliderAudioVolume->setValue(settings.getAudioVolume());

    // video player
    ui->cbAutoPlayVideo->setChecked(settings.getAutoPlayVideo());
    ui->cbLoopVideoForever->setChecked(settings.getLoopVideoForever());
    ui->sliderVideoVolume->setValue(settings.getVideoVolume());
}

QString SettingsDialog::fontToString(const QFont &font)
{
    QString description = font.family();
    if (font.pointSize() != -1)
    {
        description.append(", " + QString::number(font.pointSize()) + " pt");
    }
    if (font.pixelSize() != -1)
    {
        description.append(", " + QString::number(font.pixelSize()) + " px");
    }
    if (font.bold())
    {
        description.append(", fett");
    }
    if (font.italic())
    {
        description.append(", kursiv");
    }
    if (font.underline())
    {
        description.append(", unterstrichen");
    }
    return description;
}
