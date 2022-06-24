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

#include <Application.hpp>
#include <MainWindow.hpp>
#include <PresentationWindow.hpp>
#include <Presentation.hpp>

Application::Application(int &argc, char **argv) : QApplication(argc, argv)
{
    for(int i = 0; i < argc; i++){
        if(DoesFileExist(argv[i]) && QString(argv[i]).endsWith(".spres")){  
            Presentation* pres = nullptr;
            try{
                pres = new Presentation(argv[i]);
            }
            catch (PresentationException& e){
                QMessageBox *messageBox = new QMessageBox();
                messageBox->setWindowTitle("Failed to Load Presentation");
                messageBox->setText(QString("Failed to Load Presentation:\n   " + QString(e.what())));
                messageBox->show();
                continue;
            }
            presentationWindow = new PresentationWindow();
            presentationWindow->setPresentation(pres);
        }
    }
}

bool Application::event(QEvent *event){
    if (event->type() == QEvent::FileOpen) {
        QFileOpenEvent *openEvent = static_cast<QFileOpenEvent *>(event);
        Presentation* pres = nullptr;
            try{
                pres = new Presentation(openEvent->file());
            }
            catch (PresentationException& e){
                QMessageBox *messageBox = new QMessageBox();
                messageBox->setWindowTitle("Failed to Load Presentation");
                messageBox->setText(QString("Failed to Load Presentation:\n   " + QString(e.what())));
                messageBox->show();
                return QApplication::event(event);
            }
            if(mainWindow)
                mainWindow->hide();
            if(presentationWindow){
                presentationWindow->setPresentation(pres);
                presentationWindow->showFullScreen();

            }
            else{
                presentationWindow = new PresentationWindow();
                presentationWindow->setPresentation(pres);
                presentationWindow->showFullScreen();
            }
    }
    return QApplication::event(event);
}

int Application::Execute(){
    if(!mainWindow)
        mainWindow = new MainWindow();
    if(this->presentationWindow){
        if(presentationWindow->hasPresentation()){
            presentationWindow->show();
            return exec();
        }
    }
    mainWindow->show();
    return exec();
}