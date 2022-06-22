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

#include <QtWidgets/QtWidgets>
#include <Presentation.hpp>
#include <PresentationSlideView.hpp>

class PresentationWindow : public QMainWindow
{
    Q_OBJECT
public:
    explicit PresentationWindow(QWidget *parent = nullptr);
    void RetranslateUI();
    void setPresentation(Presentation* presentation);
    inline bool hasPresentation() const { return !(!m_presentation); };
private:
    void handleNextSlideAction();
    void handlePreviousSlideSlideAction();
    void handleCloseWindowAction();
private:
    QWidget* m_Window;
    Presentation *m_presentation;
    PresentationSlideView *m_slideView;
    unsigned int m_currentSlide;
    QAction *m_nextSlideAction, *m_previousSlideAction, *m_closeWindowAction;
};