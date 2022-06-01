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

#include <QtCore/QtCore>
#include <vector>
#include <zip.h>
#include <QtGui/QtGui>
#include <exception>

bool DoesFileExist(const char* file_name);

class PresentationException: public std::exception
{
public:
    PresentationException(const char *what) : m_what(what) { }
    virtual const char *what() const throw()
    {
        return m_what;
    }
private:  
    const char *m_what;
};

struct PresentationText
{
public:
    QString Text;
    bool isBold;
    bool isItalic;
    bool isUnderlined;
    bool isStrikedThrough;
    unsigned int Alignment;
    unsigned int FontSize;
    QPoint Position;
    QSize Size;
    uint32_t FontColor;
};

struct PresentationImage
{
public:
    QString FileName;
    QPoint Position;
    QSize Size;
    QString Alt; 
};

struct PresentationSlide
{
public:
    QString SlideBackgroundFileName;
    QString SlideTitle;
    std::vector<PresentationText> Texts;
    unsigned int TextCounts;
    std::vector<PresentationImage> Images;
    unsigned int ImageCount;
};

struct Presentation
{
public:
    Presentation(QString FilePath);
    QPixmap GetImage(QString ImageFileName);
    ~Presentation();
    inline QString GetTitle() const { return m_Title; }
    inline PresentationSlide GetSlide(unsigned int slideIndex) const { return m_Slides.at(slideIndex); }
private:
    unsigned int m_SlideCount;
    QString m_Title;
    struct zip *m_spres_archive;
    std::vector<PresentationSlide> m_Slides;
    QTemporaryDir m_TmpDir = QTemporaryDir();
};