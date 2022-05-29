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

#include <MainWindow.hpp>

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent) {
    this->setObjectName(QString::fromUtf8("MainWindow"));
    this->setFixedSize(575, 400);

    m_Window = new QWidget(this);
    this->setCentralWidget(m_Window);

    m_OpenPresentationPushButton = new QPushButton("", m_Window);
    m_OpenPresentationPushButton->setFixedSize(225, 75);
    m_OpenPresentationPushButton->setGeometry((this->width() / 2) - m_OpenPresentationPushButton->width() / 2,
                                              (this->height() / 2) + 25,
                                              m_OpenPresentationPushButton->width(), m_OpenPresentationPushButton->height());

    m_OpenFileAction = new QAction(this);
    m_AboutAction = new QAction(this);
    m_ExitAction = new QAction(this);
    m_LicenseAction = new QAction(this);

    m_MenuBar = new QMenuBar(this);
    m_FileMenu = new QMenu(m_MenuBar);
    m_HelpMenu = new QMenu(m_MenuBar);

    m_MenuBar->addAction(m_FileMenu->menuAction());
    m_MenuBar->addAction(m_HelpMenu->menuAction());
    m_MenuBar->setGeometry(0, 0, this->width(), 40);
    m_FileMenu->addAction(m_OpenFileAction);
    m_FileMenu->addAction(m_ExitAction);
    m_HelpMenu->addAction(m_AboutAction);
    m_HelpMenu->addAction(m_LicenseAction);

    m_ExitShortcut = new QShortcut(this);
    m_ExitShortcut->setKey(Qt::Key_Escape);
    connect(m_ExitShortcut, &QShortcut::activated, this, &MainWindow::HandleExitAction);

    m_OpenShortcut = new QShortcut(this);
    m_OpenShortcut->setKey(Qt::CTRL | Qt::Key_O);
    connect(m_OpenShortcut, &QShortcut::activated, this, &MainWindow::HandleOpenFile);
    connect(m_OpenPresentationPushButton, &QPushButton::clicked, this, &MainWindow::HandleOpenFile);
    connect(m_OpenFileAction, &QAction::triggered, this, &MainWindow::HandleOpenFile);
    connect(m_ExitAction, &QAction::triggered, this, &MainWindow::HandleExitAction);
    connect(m_AboutAction, &QAction::triggered, this, &MainWindow::HandleOpenAboutAction);
    connect(m_LicenseAction, &QAction::triggered, this, &MainWindow::HandleLicenseAction);
    
    this->RetranslateUI();
    show();
}

void MainWindow::RetranslateUI() {
    this->setWindowTitle("Simple Press 2");
    m_OpenPresentationPushButton->setText("Open Presentation");
    m_OpenFileAction->setText("Open File (Ctrl + O)");
    m_AboutAction->setText("About program");
    m_LicenseAction->setText("License");
    m_ExitAction->setText("Exit");
    m_FileMenu->setTitle("File");
    m_HelpMenu->setTitle("Help");
}

void MainWindow::HandleExitAction(){
    QApplication::quit();
}

void MainWindow::HandleOpenAboutAction(){
    QMessageBox::about(this, "Simple Press 2", "Simple Press 2 - Simple Presentation Program.");
}

void MainWindow::HandleOpenFile(){
    printf("[ERROR] File Opening is not implemented.\n");
}

void MainWindow::HandleLicenseAction(){
    QDesktopServices::openUrl(QUrl("https://www.gnu.org/licenses/old-licenses/gpl-2.0-standalone.html"));
}