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

static char* GetAttributeValue(const char* name, rapidxml::xml_node<> *node){
    if(!node)
        return new char[1]{0};
    rapidxml::xml_attribute<char> *attrib = node->first_attribute(name, 0UL, false);
    if(!attrib)
        return new char[1]{0}; 
    char* attrib_val = attrib->value();
    return attrib_val;
}

static char* GetValue(const char* name, rapidxml::xml_node<> *parent_node){
    if(!parent_node)
        return new char[1]{0};
    rapidxml::xml_node<> *node = parent_node->first_node(name, 0UL, false);
    if(!node)
        return new char[1]{0};
    return node->value();
}

static int GetIntValue(const char* name, rapidxml::xml_node<> *parent_node){
    char* str = GetValue(name, parent_node);
    if(strlen(str) > 2){
        if(str[0] == '0' && (str[1] == 'x' || str[1] == 'X')){
            str += 2;
            try{
                return std::stoi(str, nullptr, 16);
            }
            catch(std::exception&){
                return 0;
            }
        }
    }
    try{
        return std::stoi(str, nullptr, 10);
    }
    catch(std::exception&){
        return 0;
    }
    return 0;
}

static bool GetBooleanValue(const char* name, rapidxml::xml_node<> *parent_node){
    char* str = GetValue(name, parent_node);
    int i = 0;
    while(str[i]){
        str[i] = tolower(str[i]);
        i++;
    }
    if(!strcmp(str, "true"))
        return true;
    return false;
}

static void GetIntValue(const char* name, rapidxml::xml_node<> *parent_node, int* value, SizeType *type){
    QString qstr = GetValue(name, parent_node);
    if(type){
        if(qstr.endsWith("px")){
            *type = SizeType::pixels;
            qstr.remove(qstr.length() - 2, 2);
        }
        else if(qstr.endsWith("%")){
            *type = SizeType::percent;
            qstr.remove(qstr.length() - 1, 1);
        }
        else if(qstr.endsWith("pt")){
            *type = SizeType::points;
            qstr.remove(qstr.length() - 2, 2);
        }
        else{
            *type = SizeType::none;
        }
    }
    if(qstr.length() > 2){
        qstr = qstr.toLower();
        if(qstr.at(0) == '0' && qstr.at(1) == 'x'){
            qstr.remove(0, 2);
            
            try{
                *value = std::stoi(qstr.toStdString()  , nullptr, 16);
            }
            catch(std::exception&){
                if(type)
                    *type = SizeType::none;
                *value = 0;
            }
            return;
        }
    }
    try{
        *value = std::stoi(qstr.toStdString(), nullptr, 10);
    }
    catch(std::exception&){
        if(type)
            *type = SizeType::none;
        *value = 0;
    }
    return;
}

Presentation::Presentation(QString FilePath){
    m_TmpDir.setAutoRemove(false);
    this->Slides = std::vector<PresentationSlide*>();
    this->m_spres_archive = 0;
    struct zip_file *zf = 0;
    struct zip_stat zs;
    int z_err;
    char buf[BUF_LENGTH];
    if((this->m_spres_archive = zip_open(FilePath.toStdString().c_str(), 0, &z_err)) == NULL){
        zip_error_to_str(buf, BUF_LENGTH, z_err, errno);
        char* err_str = new char[128];
        strcpy(err_str, "Failed to open spres archive.\n\nError: ");
        throw PresentationException(strcat(err_str, buf));
    }
    z_err = zip_stat(this->m_spres_archive, "main.xml", ZIP_FL_NOCASE, &zs);
    if(z_err){
        zip_error_to_str(buf, BUF_LENGTH, z_err, errno);
        char* err_str = new char[128];
        strcpy(err_str, "Failed to open main.xml file inside the spres archive.\n\nError: ");
        throw PresentationException(strcat(err_str, buf));
    }
    zf = zip_fopen(this->m_spres_archive, "main.xml", ZIP_FL_NOCASE);
    if(!zf){
        throw PresentationException("Failed to open main.xml file inside the spres archive.");
    }

    char XMLstr[zs.size + 1];
    XMLstr[zs.size] = 0;
    int sum = 0;
    while (sum != zs.size) {
        uint8_t len = zip_fread(zf, buf, BUF_LENGTH);
        if (len < 0) {
            throw PresentationException("Failed to read main.xml file inside spres archive.");
        }
        for(uint8_t i = 0; i < len; i++){
            XMLstr[sum + i] = buf[i];
        }
        sum += len;
    }
    zip_fclose(zf);

#pragma region PARSING

    rapidxml::xml_document<> xml_doc;
    rapidxml::xml_node<> *root_node = NULL;
    rapidxml::xml_node<> *slide_node = NULL;
    rapidxml::xml_node<> *image_node = NULL;
    rapidxml::xml_node<> *text_node = NULL;
    rapidxml::xml_node<> *temp_node = NULL;
    
    try{
        xml_doc.parse<0>(XMLstr);
    }
    catch(rapidxml::parse_error& e){
        char* err_str = new char[128];
        strcpy(err_str, "Failed to parse main.xml file inside the spres archive.\n\nError: ");
        throw PresentationException(strcat(err_str, e.what()));
    }

    root_node = xml_doc.first_node("Presentation", 0UL, false);
    if(root_node == nullptr){
        throw PresentationException("Failed to find XML root element (Presentation) in main.xml file inside the spres archive.");
    }

    this->Title = GetAttributeValue("Title", root_node);
    slide_node = root_node->first_node("Slide", 0UL, false);
    int slide_count = 0;
    while (slide_node)
    {
        PresentationSlide* slide = new PresentationSlide;
        slide->SlideTitle = GetAttributeValue("Title", slide_node);
        temp_node = slide_node->first_node("SlideBackground");
        slide->SlideBackgroundFileName = GetAttributeValue("Filename", temp_node);
        if(slide->SlideBackgroundFileName.isEmpty()){
            if(temp_node){
                temp_node = temp_node->first_node("Color", 0UL, false);
                union {
                    uint32_t RGBA;
                    uint8_t bytes[4];
                } RGBA;
                RGBA.bytes[0] = (uint8_t)GetIntValue("r", temp_node);
                RGBA.bytes[1] = (uint8_t)GetIntValue("g", temp_node);
                RGBA.bytes[2] = (uint8_t)GetIntValue("b", temp_node);
                RGBA.bytes[3] = 255;
                slide->SlideBackgroundColor = RGBA.RGBA;
                slide->hasBackgroundColor = true;
            }
        }

        unsigned int imageCount = 0;
        image_node = slide_node->first_node("Image", 0UL, false);
        while(image_node){
            PresentationImage image;
            image.Alt = GetValue("Alt", image_node);
            image.FileName = GetAttributeValue("Filename", image_node);
            int x, y;
            SizeType x_type, y_type;
            temp_node = image_node->first_node("size", 0UL, false);
            GetIntValue("width", temp_node, &x, &x_type);
            GetIntValue("height", temp_node, &y, &y_type);
            image.Size_type[0] = x_type;
            image.Size_type[1] = y_type;
            image.Size = QSize(x, y);
            temp_node = image_node->first_node("position", 0UL, false);
            GetIntValue("x", temp_node, &x, &x_type);
            GetIntValue("y", temp_node, &y, &y_type);
            image.Position_type[0] = x_type;
            image.Position_type[1] = y_type;
            image.Position = QPoint(x, y);
            slide->Images.push_back(image);
            imageCount++;
            image_node = image_node->next_sibling(image_node->name(), image_node->name_size(), false);
        }

        unsigned int textCount = 0;
        text_node = slide_node->first_node("Text", 0UL, false);
        while(text_node){
            PresentationText text;
            text.Text = GetValue("String", text_node);
            text.Text.replace("\n", "\n");

            /*text.Alignment = GetIntValue("Alignment", text_node);
            text.isBold = GetBooleanValue("isBold", text_node);
            text.isItalic = GetBooleanValue("isItalic", text_node);
            text.isUnderlined = GetBooleanValue("isUnderlined", text_node);
            text.isStrikedThrough = GetBooleanValue("isStrikedThrough", text_node);*/ // these features are not implemeneted yet in new syntax.

            temp_node = text_node->first_node("Font", 0UL, false);
            GetIntValue("Size", temp_node, &text.fontSize, &text.fontSizeType);
            if(temp_node)
                temp_node = temp_node->first_node("Color", 0UL, false);
            union {
                uint32_t RGBA;
                uint8_t bytes[4];
            } RGBA;
            RGBA.bytes[0] = (uint8_t)GetIntValue("r", temp_node);
            RGBA.bytes[1] = (uint8_t)GetIntValue("g", temp_node);
            RGBA.bytes[2] = (uint8_t)GetIntValue("b", temp_node);
            RGBA.bytes[3] = 255;
            text.FontColor = RGBA.RGBA;
            int x, y;
            SizeType x_type, y_type;
            temp_node = text_node->first_node("size", 0UL, false);
            GetIntValue("width", temp_node, &x, &x_type);
            GetIntValue("height", temp_node, &y, &y_type);
            text.Size_type[0] = x_type;
            text.Size_type[1] = y_type;
            text.Size = QSize(x, y);
            temp_node = text_node->first_node("position", 0UL, false);
            GetIntValue("x", temp_node, &x, &x_type);
            GetIntValue("y", temp_node, &y, &y_type);
            text.Position_type[0] = x_type;
            text.Position_type[1] = y_type;
            text.Position = QPoint(x, y);

            slide->Texts.push_back(text);
            textCount++;
            text_node = text_node->next_sibling(text_node->name(), text_node->name_size(), false);
        }

        this->Slides.push_back(slide);
        image_node = NULL;
        text_node = NULL;
        slide_count++;
        slide_node = slide_node->next_sibling();
    }
#pragma endregion PARSING
}

Presentation::Presentation(Presentation& other){
    this->m_spres_archive = other.m_spres_archive;
    this->Slides = other.Slides;
    this->Title = other.Title;
}

Presentation::Presentation(Presentation&& other){
    this->m_spres_archive = other.m_spres_archive;
    this->Slides = other.Slides;
    this->Title = other.Title;
}

Presentation::~Presentation(){
    zip_close(m_spres_archive);
    m_TmpDir.remove();
}

QPixmap Presentation::GetImage(QString ImageFileName){
    if(ImageFileName.contains("..") || ImageFileName.contains("/") || ImageFileName.contains("\\"))
        throw PresentationException("Detected Path Traversal. File access denied.");
    if(m_TmpDir.isValid()){
        QString filePath = m_TmpDir.path();
        if(!filePath.endsWith("/"))
            filePath += QString("/");
        filePath += ImageFileName;
        if(DoesFileExist(filePath.toStdString().c_str())){
            return QPixmap(filePath);
        }
    }
    else{
        char* err_str = new char[128];
        strcpy(err_str, "Failed to load an image. Could not create temporary directory.\n\nError: ");
        throw PresentationException(strcat(err_str, m_TmpDir.errorString().toStdString().c_str()));
    }

    if(!m_spres_archive)
        throw PresentationException("Could not open spres archive to read image data.");
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
        char* err_str = new char[128];
        strcpy(err_str, "Failed to open Image: ");
        strcat(err_str, ImageFileName.toStdString().c_str());
        strcat(err_str, ". Error: ");
        strcat(err_str, err_buf);
        throw PresentationException(err_str);
    }
    zf = zip_fopen(m_spres_archive, ImageFileName.toStdString().c_str(), ZIP_FL_NOCASE);
    if(!zf){
        char* err_str = new char[128];
        strcpy(err_str, "Failed to open Image: ");
        throw PresentationException(strcat(err_str, ImageFileName.toStdString().c_str()));
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
                throw PresentationException("Failed to read image data.");
            }
            file.write(buf, len);
            sum += len;
        }
        file.close();
    }
    return QPixmap(filePath);
}
