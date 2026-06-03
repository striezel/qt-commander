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

#include "mainwindow.h"

#include <QApplication>
#include <QCommandLineParser>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    QCoreApplication::setOrganizationName("Dirk Stolle");
    QCoreApplication::setApplicationName("Qt Commander");
    QCoreApplication::setApplicationVersion("0.1.9");

    QCommandLineParser parser;
    parser.setApplicationDescription("A file manager with two directory views placed side by side.");
    parser.addVersionOption();
    parser.addHelpOption();
    parser.addPositionalArgument(
        "left_dir",
        QCoreApplication::translate("main", "The directory to open on the left side of the application."),
        "[left_dir]");
    parser.addPositionalArgument(
        "right_dir",
        QCoreApplication::translate("main", "The directory to open on the right side of the application."),
        "[right_dir]");
    parser.process(a);
    const QStringList pos_args = parser.positionalArguments();

    MainWindow w(nullptr, pos_args);
    w.show();
    return a.exec();
}
