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

#include <Presentation.hpp>
#include <vendor/RapidXML/rapidxml.hpp>

#define BUF_LENGTH 64

bool DoesFileExist(const char* file_name){
     if (FILE *file = fopen(file_name, "r")) {
        fclose(file);
        return true;
    } else {
        return false;
    }   
}

static char* GetValue(const char* name, rapidxml::xml_node<> *root_node){
    if(!root_node)
        return new char[1]{0};
    rapidxml::xml_node<> *node = root_node->first_node(name, 0UL, false);
    if(!node)
        return new char[1]{0};
    return node->value();
}

static int GetIntValue(const char* name, rapidxml::xml_node<> *root_node){
    char* str = GetValue(name, root_node);
    if(strlen(str) > 2){
        if(str[0] == '0' && (str[1] == 'x' || str[1] == 'X')){
            str += 2;
            try{
                return std::stoi(str, nullptr, 16);
            }
            catch(std::exception){
                return 0;
            }
        }
    }
    try{
        return std::stoi(str, nullptr, 10);
    }
    catch(std::exception){
        return 0;
    }
    return 0;
}

static bool GetBooleanValue(const char* name, rapidxml::xml_node<> *root_node){
    char* str = GetValue(name, root_node);
    int i = 0;
    while(str[i]){
        str[i] = tolower(str[i]);
        i++;
    }
    if(!strcmp(str, "true"))
        return true;
    return false;
}

Presentation::Presentation(QString FilePath){
    m_TmpDir.setAutoRemove(false);
    this->Slides = std::vector<PresentationSlide>();
    this->m_spres_archive = 0;
    struct zip_file *zf = 0;
    struct zip_stat zs;
    int z_err;
    char buf[BUF_LENGTH];
    if((this->m_spres_archive = zip_open(FilePath.toStdString().c_str(), 0, &z_err)) == NULL){
        zip_error_to_str(buf, BUF_LENGTH, z_err, errno);
        throw new PresentationException(strcat((char*)"Failed to open spres archive. Error: ", buf));
    }
    z_err = zip_stat(this->m_spres_archive, "main.xml", ZIP_FL_NOCASE, &zs);
    if(z_err){
        zip_error_to_str(buf, BUF_LENGTH, z_err, errno);
        throw new PresentationException(strcat((char*)"Failed to decompress spres archive. Error: ", buf));
    }
    zf = zip_fopen(this->m_spres_archive, "main.xml", ZIP_FL_NOCASE);
    if(!zf){
        throw new PresentationException((char*)"Failed to open main.xml file inside the spres archive.");
    }

    char XMLstr[zs.size + 1];
    XMLstr[zs.size] = 0;
    int sum = 0;
    while (sum != zs.size) {
        uint8_t len = zip_fread(zf, buf, BUF_LENGTH);
        if (len < 0) {
            throw new PresentationException("Failed to read main.xml file inside spres archive.");
        }
        for(uint8_t i = 0; i < len; i++){
            XMLstr[sum + i] = buf[i];
        }
        sum += len;
    }
    zip_fclose(zf);

    //PARSING

    rapidxml::xml_document<> xml_doc;
    rapidxml::xml_node<> *root_node = NULL;
    rapidxml::xml_node<> *slide_node = NULL;
    rapidxml::xml_node<> *image_node = NULL;
    rapidxml::xml_node<> *text_node = NULL;
    try{
        xml_doc.parse<0>(XMLstr);
    }
    catch(rapidxml::parse_error e){
        throw new PresentationException(strcat((char*)"Failed to parse main.xml file inside the spres archive. Error: ", e.what()));
    }

    root_node = xml_doc.first_node("Presentation", 0UL, false);
    if(root_node == nullptr){
        throw new PresentationException("Failed to find XML root element (Presentation) in main.xml file inside the spres archive.");
    }

    this->Title = GetValue("Title", root_node);
    
    slide_node = root_node->first_node("Slide", 0UL, false);
    int slide_count = 0;
    while (slide_node)
    {
        PresentationSlide slide;
        slide.SlideTitle = GetValue("SlideTitle ", slide_node);
        slide.SlideBackgroundFileName = GetValue("SlideBackgroundFilename", slide_node);

        unsigned int imageCount = 0;
        image_node = slide_node->first_node("Image", 0UL, false);
        while(image_node){
            PresentationImage image;
            image.Alt = GetValue("Alt", image_node);
            image.FileName = GetValue("Filename", image_node);
            image.Position = QPoint(GetIntValue("XPosition", image_node),
                                    GetIntValue("YPosition", image_node));
            image.Size = QSize(GetIntValue("Width", image_node),
                                    GetIntValue("Height", image_node));
            slide.Images.push_back(image);
            imageCount++;
            image_node = image_node->next_sibling();
        }
        slide.ImageCount = imageCount;

        unsigned int textCount = 0;
        text_node = slide_node->first_node("Text", 0UL, false);
        while(text_node){
            PresentationText text;
            text.Text = GetValue("String", text_node);
            text.Alignment = GetIntValue("Alignment", text_node);
            text.isBold = GetBooleanValue("isBold", text_node);
            text.isItalic = GetBooleanValue("isItalic", text_node);
            text.isUnderlined = GetBooleanValue("isUnderlined", text_node);
            text.isStrikedThrough = GetBooleanValue("isStrikedThrough", text_node);
            text.FontSize = (GetIntValue("FontSize", text_node) < 1) ? GetIntValue("FontSize", text_node) : 1;
            text.Size = QSize(GetIntValue("Width", text_node),
                              GetIntValue("Height", text_node));
            text.Position = QPoint(GetIntValue("XPosition", text_node),
                                   GetIntValue("YPosition", text_node));
            union RGBA
            {
                uint32_t RGBA;
                uint8_t bytes[4];
            };
            RGBA rgba;
            rgba.bytes[3] = (uint8_t)GetIntValue("FontColorR", text_node);
            rgba.bytes[2] = (uint8_t)GetIntValue("FontColorG", text_node);
            rgba.bytes[1] = (uint8_t)GetIntValue("FontColorB", text_node);
            rgba.bytes[0] = 255;
            text.FontColor = rgba.RGBA;
            slide.Texts.push_back(text);
            textCount++;
            text_node = text_node->next_sibling();
        }
        slide.TextCounts = textCount;

        this->Slides.push_back(slide);
        image_node = NULL;
        text_node = NULL;
        slide_count++;
        slide_node = slide_node->next_sibling();
    }
    //END PARSING
}

Presentation::~Presentation(){
    zip_close(m_spres_archive);
    m_TmpDir.remove();
}

QPixmap Presentation::GetImage(QString ImageFileName){
    if(m_TmpDir.isValid()){
        QString filePath = m_TmpDir.path();
        if(!filePath.endsWith("/"))
            filePath += QString("/");
        filePath += ImageFileName;
        if(DoesFileExist(filePath.toStdString().c_str())){
            return QPixmap(filePath);
        }
    }
    else
        throw new PresentationException(strcat((char*)"Failed to load an image. Could not create temporary directory. Error: ", m_TmpDir.errorString().toStdString().c_str()));

    if(!m_spres_archive)
        throw new PresentationException("Could not open spres archive to read image data.");
    QString filePath = m_TmpDir.path();
    if(!filePath.endsWith("/"))
        filePath += QString("/");
    filePath += ImageFileName;
    char err_buf[BUF_LENGTH];
    int z_err = 0; 
    struct zip_file *zf;
    struct zip_stat zs;

    z_err = zip_stat(m_spres_archive, ImageFileName.toStdString().c_str(), ZIP_FL_NOCASE, &zs);
    if(z_err){
        zip_error_to_str(err_buf, BUF_LENGTH, z_err, errno);
        printf("Failed to open Image: %s. Error: %s. Displaying 404 image instead.\n", ImageFileName.toStdString().c_str(), err_buf);
        return QPixmap(); // TODO: return 404 image.
    }
    zf = zip_fopen(m_spres_archive, ImageFileName.toStdString().c_str(), ZIP_FL_NOCASE);
    if(!zf){
        printf("Failed to open Image: %s. Displaying 404 image instead.\n", ImageFileName.toStdString().c_str());
        return QPixmap(); // TODO: return 404 image.
    }
    QFile file(filePath);
    if(!file.open(QIODevice::OpenModeFlag::Append))
    {
        printf("Failed to open file. %s\n", filePath.toStdString().c_str());
    }
    else{
        int sum = 0, len = 0;
        char buf[BUF_LENGTH];
        while (sum != zs.size) {
            len = zip_fread(zf, buf, BUF_LENGTH);
            if (len < 0) {
                throw new PresentationException("Failed to read image data.");
            }
            file.write(buf, len);
            sum += len;
        }
        file.close();
    }
    return QPixmap(filePath);
}