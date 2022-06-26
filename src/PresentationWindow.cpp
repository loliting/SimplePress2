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
#include <Application.hpp>

PresentationWindow::PresentationWindow(QWidget *parent) : QMainWindow(parent) {    
    this->setWindowTitle("Simple Press 2");
    setWindowFlags(windowFlags() & ~Qt::WindowFullscreenButtonHint);
    this->setFixedSize(QGuiApplication::primaryScreen()->size());
    this->showFullScreen();
    this->setPalette(QPalette(QColor::fromRgb(0, 0, 0, 255)));
    m_closeWindowAction = new QAction(this);
    m_nextSlideAction = new QAction(this);
    m_previousSlideAction = new QAction(this);
    this->setAttribute(Qt::WA_DeleteOnClose, true);
    QList<QKeySequence> keySequenceList;
#if __APPLE__
    keySequenceList << Qt::Key_Escape << QKeySequence(Qt::CTRL | Qt::Key_W);
    m_closeWindowAction->setShortcuts(keySequenceList);
#else
    m_closeWindowAction->setShortcut(Qt::Key_Escape);
#endif
    keySequenceList = QList<QKeySequence>();
    keySequenceList << Qt::Key_Right << Qt::Key_L << Qt::Key_D;
    m_nextSlideAction->setShortcuts(keySequenceList);

    keySequenceList = QList<QKeySequence>();
    keySequenceList << Qt::Key_Left << Qt::Key_H << Qt::Key_A;
    m_previousSlideAction->setShortcuts(keySequenceList);

    connect(m_closeWindowAction, &QAction::triggered, this, &PresentationWindow::handleCloseWindowAction);
    connect(m_nextSlideAction, &QAction::triggered, this, &PresentationWindow::handleNextSlideAction);
    connect(m_previousSlideAction, &QAction::triggered, this, &PresentationWindow::handlePreviousSlideSlideAction);
    this->addAction(m_closeWindowAction);
    this->addAction(m_nextSlideAction);
    this->addAction(m_previousSlideAction);
}

void PresentationWindow::setPresentation(Presentation *Pres){
    m_presentation = Pres;
    if(!m_presentation->Title.isEmpty() && !m_presentation->Title.isNull())
        this->setWindowTitle("Simple Press 2 - " + m_presentation->Title);
    
    m_slideView = new PresentationSlideView(this);
    m_currentSlide = 0;
    if(m_presentation->Slides.size() > 0){
        m_slideView->setSlide(m_presentation, m_currentSlide);
        m_slideView->show();
    }
}

void PresentationWindow::handleCloseWindowAction(){
    Application* app = static_cast<Application*>(QApplication::instance());
    close();
    app->presentationWindow = 0;
    if(app->mainWindow){
        app->mainWindow->showNormal();
    }
    else{
        app->mainWindow = new MainWindow();
        app->mainWindow->showNormal();
    }
}

void PresentationWindow::handleNextSlideAction(){
    if(m_currentSlide + 1 != m_presentation->Slides.size() && m_presentation->Slides.size() > 1){
        m_currentSlide++;
        PresentationSlideView* newSlideView = new PresentationSlideView(this);
        newSlideView->setSlide(m_presentation, m_currentSlide);
        newSlideView->show();
        m_slideView->hide();
        m_slideView->deleteLater();
        m_slideView = newSlideView;
    }
}

void PresentationWindow::handlePreviousSlideSlideAction(){
    if(m_currentSlide){
        m_currentSlide--;
        PresentationSlideView* newSlideView = new PresentationSlideView(this);
        newSlideView->setSlide(m_presentation, m_currentSlide);
        newSlideView->show();
        m_slideView->hide();
        m_slideView->deleteLater();
        m_slideView = newSlideView;
    }
}