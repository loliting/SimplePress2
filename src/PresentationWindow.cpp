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
#include <QtCore/QtCore>
#include <QtGui/QtGui>
#include <PresentationWindow.hpp>

PresentationWindow::PresentationWindow(QWidget *parent) : QMainWindow(parent) {    
    #ifdef __APPLE__
    Qt::WindowFlags windowFlags = 
        Qt::WindowFullscreenButtonHint |
        Qt::WindowSystemMenuHint |
        Qt::FramelessWindowHint;
    setWindowFlags(windowFlags);
    this->setFixedSize(QGuiApplication::primaryScreen()->size());
    this->showFullScreen();
    this->setPalette(QPalette(QColor::fromRgb(255,255,255,255)));
    #elif
    //TODO
    #endif
}

void PresentationWindow::setPresentation(Presentation *Pres){
    m_presentation = Pres;
}

