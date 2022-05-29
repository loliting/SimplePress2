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

#pragma once

#include <QtWidgets>

class MainWindow : public QMainWindow
{
    Q_OBJECT
public:
    explicit MainWindow(QWidget *parent = nullptr);
    void RetranslateUI();
private:
    void HandleExitAction();
    void HandleOpenFile();
    void HandleOpenAboutAction();
    void HandleLicenseAction();
private:
    QWidget* m_Window;
    QPushButton* m_OpenPresentationPushButton;
    
    QMenuBar* m_MenuBar;
    QMenu* m_FileMenu;
    QMenu* m_HelpMenu;

    QAction* m_OpenFileAction;
    QAction* m_ExitAction;
    QAction* m_AboutAction;
    QAction* m_LicenseAction;
};