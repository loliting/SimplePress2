// This file is part of Simple Press 2
// Copyright (C) 2022 Karol Maksymowicz
//
// Simple Press 2 is free software; you can redistribute it and/or
// modify it under the terms of the GNU General Public License
// as published by the Free Software Foundation; either version 2
// of the License, or (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program; if not, If not,
// see <https://www.gnu.org/licenses/>.

#include <QtWidgets/QtWidgets>
#include <QtCore/QtDebug>
#include <QtGui/QFileOpenEvent>
#include <MainWindow.hpp>
#include <PresentationWindow.hpp>

class Application : public QApplication
{
    Q_OBJECT
public:
    Application(int &argc, char **argv);

    bool event(QEvent *event) override;
    int Execute();
    MainWindow *mainWindow = nullptr;
    PresentationWindow *presentationWindow = nullptr;
};