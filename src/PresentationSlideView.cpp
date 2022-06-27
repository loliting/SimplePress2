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

#include <PresentationSlideView.hpp>
#include <stdio.h>
#include <Application.hpp>

PresentationSlideView::PresentationSlideView(QWidget *parent) : QWidget(parent) {
    int w = 0, h = 0, y = 0, ph = 0;
    if(parent){
        w = parent->width();
        ph = parent->height();
    }
    h = ( w / 16) * 9;
    y = (ph - h) / 2;
    this->setGeometry(0, y, w, h);
    this->setPalette(QPalette(QColor::fromRgb(255, 255, 255, 255)));
    this->setAutoFillBackground(true);
}


void PresentationSlideView::clearSlideView(){
    this->setAutoFillBackground(true);
    this->setPalette(QPalette(QColor::fromRgb(255, 255, 255, 255)));
    if(m_backgroundImage)
        delete m_backgroundImage;
    m_textWidgets.clear();
    m_imagesWidgets.clear();
    if(m_parentWidget)
        delete m_parentWidget;

}

static int getXPosition(const float percentXPosition, const QSize& screenSize, const QSize& widgetSize){
    return (int)(percentXPosition * (float)screenSize.width()) - (int)((float)widgetSize.width() / 2.0f);
}

static int getYPosition(const float percentYPosition, const QSize& screenSize, const QSize& widgetSize){
    return (int)((float)-(percentYPosition - 1) * (float)screenSize.height()) - (int)((float)widgetSize.height() / 2.0f);
}

void PresentationSlideView::setSlide(Presentation* presentation, unsigned int index){
    clearSlideView();
    m_presentation = nullptr;
    m_slide = nullptr;
    if(presentation){
        m_presentation = presentation;
        if(m_presentation->Slides.at(index)){
            m_parentWidget = new QWidget(this);
            m_parentWidget->setFixedSize(this->size());
            m_slide = m_presentation->Slides.at(index);
            if(!m_slide->SlideBackgroundFileName.isEmpty()){
                m_backgroundImage = new QLabel(m_parentWidget);
                m_backgroundImage->setFixedSize(this->size());
                m_backgroundImage->setScaledContents(true);
                try
                {
                    QPixmap pixmap = m_presentation->GetImage(m_slide->SlideBackgroundFileName);
                    
                    m_backgroundImage->setPixmap(pixmap);
                    this->setAutoFillBackground(false);
                }
                catch(const PresentationException& e)
                {
                    printf("[WARNING] Failed to display image: %s. Error: %s.\n", m_slide->SlideBackgroundFileName.toStdString().c_str(), e.what());
                }
            }
            for(unsigned int i = 0; i < m_slide->Images.size(); i++){
                QLabel* image = new QLabel(m_parentWidget);
                try
                {
                    QPixmap pixmap = m_presentation->GetImage(m_slide->Images.at(i).FileName);
                    image->setPixmap(pixmap);
                }
                catch(const PresentationException& e)
                {
                    printf("[WARNING] Failed to display image: %s. Error: %s.\n", m_slide->Images.at(i).FileName.toStdString().c_str(), e.what());
                    image->setText(m_slide->Images.at(i).Alt);
                    image->setAlignment(Qt::AlignCenter);
                }
                image->setScaledContents(true);

                if(m_slide->Images.at(i).Size_type[0] == SizeType::pixels){
                    image->setFixedWidth(m_slide->Images.at(i).Size.width());
                }
                else{
                    image->setFixedWidth(m_slide->Images.at(i).Size.width() / 100.0f * m_parentWidget->width());
                }
                if(m_slide->Images.at(i).Size_type[1] == SizeType::pixels){
                    image->setFixedHeight(m_slide->Images.at(i).Size.height());
                }
                else{
                    image->setFixedHeight(m_slide->Images.at(i).Size.height() / 100.0f * m_parentWidget->height());
                }
                if(m_slide->Images.at(i).Position_type[0] == SizeType::pixels){
                    image->move(m_slide->Images.at(i).Position.x(), 0);
                }
                else{
                    image->move(
                            getXPosition((float)m_slide->Images.at(i).Position.x() / 100.0f,
                            m_parentWidget->size(), image->size()), 0);
                }
                if(m_slide->Images.at(i).Position_type[1] == SizeType::pixels){
                    image->move(image->x(), m_slide->Images.at(i).Position.y());
                }
                else{
                    image->move(image->x(),
                            getYPosition((float)m_slide->Images.at(i).Position.y() / 100.0f,
                            m_parentWidget->size(), image->size()));
                }
            }
            for(unsigned int i = 0; i < m_slide->Texts.size(); i++){
                QLabel *text = new QLabel(m_parentWidget);
                text->setText(m_slide->Texts.at(i).Text);
                text->setTextFormat(Qt::TextFormat::PlainText);
                QFont font = QFont();
                font.setBold(m_slide->Texts.at(i).isBold);
                font.setItalic(m_slide->Texts.at(i).isItalic);
                font.setStrikeOut(m_slide->Texts.at(i).isStrikedThrough);
                font.setUnderline(m_slide->Texts.at(i).isUnderlined);
                switch (m_slide->Texts.at(i).fontSizeType)
                {
                    case SizeType::points:
                        font.setPixelSize((int)((float)m_parentWidget->height() / 400.0f * (float)m_slide->Texts.at(i).fontSize));
                        break;
                    case SizeType::pixels:
                        font.setPixelSize(m_slide->Texts.at(i).fontSize);
                        break;
                    default:
                        if(m_slide->Texts.at(i).fontSize < 1)
                            m_slide->Texts.at(i).fontSize = 1;
                        font.setPixelSize((int)((float)m_parentWidget->height() / 400.0f * (float)m_slide->Texts.at(i).fontSize));
                        break;
                }
                text->setFont(font);
                QPalette palette = QPalette();
                palette.setColor(text->foregroundRole(), QColor::fromRgba(m_slide->Texts.at(i).FontColor));
                text->setPalette(palette);
                if(m_slide->Texts.at(i).Size_type[0] == SizeType::pixels){
                    text->setFixedWidth(m_slide->Texts.at(i).Size.width());
                }
                else{
                    text->setFixedWidth(m_slide->Texts.at(i).Size.width() / 100.0f * m_parentWidget->width());
                }
                if(m_slide->Texts.at(i).Size_type[1] == SizeType::pixels){
                    text->setFixedHeight(m_slide->Texts.at(i).Size.height());
                }
                else{
                    text->setFixedHeight(m_slide->Texts.at(i).Size.height() / 100.0f * m_parentWidget->height());
                }
                if(m_slide->Texts.at(i).Position_type[0] == SizeType::pixels){
                    text->move(m_slide->Texts.at(i).Position.x(), 0);
                }
                else{
                    text->move(
                            getXPosition((float)m_slide->Texts.at(i).Position.x() / 100.0f,
                            m_parentWidget->size(), text->size()), 0);
                }
                if(m_slide->Texts.at(i).Position_type[1] == SizeType::pixels){
                    text->move(text->x(), m_slide->Texts.at(i).Position.y());
                }
                else{
                    text->move(text->x(),
                            getYPosition((float)m_slide->Texts.at(i).Position.y() / 100.0f,
                            m_parentWidget->size(), text->size()));
                }
                text->setAlignment(Qt::AlignCenter);
                text->setScaledContents(true);
                text->setWordWrap(true);
                
            }
        }
    }
}

PresentationSlideView::~PresentationSlideView(){
    if(m_backgroundImage)
        delete m_backgroundImage;
    if(m_parentWidget)
        delete m_parentWidget;
    m_textWidgets.clear();
    m_imagesWidgets.clear();
}