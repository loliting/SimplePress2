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

bool DoesFileExist(const char* file_name);

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
    std::vector<PresentationSlide> Slides;
    QString Title;
    unsigned int SlideCount;
    QPixmap GetImage(QString ImageFileName);
    static Presentation LoadPresentationFromFile(QString FileName);
    ~Presentation();
private:
    struct zip *m_spres_archive;
};