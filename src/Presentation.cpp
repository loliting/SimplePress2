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
#include "vendor/RapidXML/rapidxml.hpp"

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
        return new char[]{0};
    rapidxml::xml_node<> *node = root_node->first_node(name, 0UL, false);
    if(!node)
        return new char[]{0};
    return node->value();
}

static int GetIntValue(const char* name, rapidxml::xml_node<> *root_node){
    char* str = GetValue(name, root_node);
    if(str[0] == '0' && (str[1] == 'x' || str[1] == 'X')){
        str += 2;
        try{
            return std::stoi(str, nullptr, 16);
        }
        catch(std::exception){
            return 0;
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

static int ParseXML(char* XMLstr, Presentation* Pres){
    rapidxml::xml_document<> doc;
    rapidxml::xml_node<> *root_node = NULL;
    rapidxml::xml_node<> *slide_node = NULL;
    rapidxml::xml_node<> *image_node = NULL;
    rapidxml::xml_node<> *text_node = NULL;
    try{
        doc.parse<0>(XMLstr);
    }
    catch(rapidxml::parse_error e){
        printf("Failed to parse XML, what: %s\n", e.what());
        return -2;
    }

    root_node = doc.first_node("Presentation", 0UL, false);
    if(root_node == nullptr){
        printf("Could not find Presentation XML root element.\n");
        return -1;
    }

    Pres->Title = GetValue("Title", root_node);
    
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

        Pres->Slides.push_back(slide);
        image_node = NULL;
        text_node = NULL;
        slide_count++;
        slide_node = slide_node->next_sibling();
    }
    Pres->SlideCount = slide_count;
    return 0;
}

Presentation Presentation::LoadPresentationFromFile(QString FileName){
    Presentation *retval = new Presentation();

    struct zip_file *zf;
    struct zip_stat xml_stat;
    int z_err;
    char buf[64];
    if((retval->m_spres_archive = zip_open(FileName.toStdString().c_str(), 0, &z_err)) == NULL){
        zip_error_to_str(buf, sizeof(buf), z_err, errno);
        printf("[ERROR] Failed to open archive. Error: %s\n", buf);
        return Presentation();
    }

    z_err = zip_stat(retval->m_spres_archive, "main.xml", ZIP_FL_NOCASE, &xml_stat);
    if(z_err){
        zip_error_to_str(buf, sizeof(buf), z_err, errno);
        printf("[ERROR] Failed to decompress archive. Error: %s\n", buf);
        return Presentation();
    }
    zf = zip_fopen(retval->m_spres_archive, "main.xml", ZIP_FL_NOCASE);
    if(!zf){
        printf("[ERROR]: Failed to open main.xml file inside the archive.\n");
    }
    
    char XMLstr[xml_stat.size + 1];

    int sum = 0;
    while (sum != xml_stat.size) {
        uint8_t len = zip_fread(zf, buf, 64);
        if (len < 0) {
            printf("Failed to read presentation.\n");
            return Presentation();
        }
        for(uint8_t i = 0; i < len; i++){
            XMLstr[sum + i] = buf[i];
        }
        sum += len;
    }
    zip_fclose(zf);
    XMLstr[xml_stat.size] = 0;
    int err = ParseXML(XMLstr, retval);
    if(err < 0){
        printf("Failed to parse XML\n");
        return Presentation();
    }
    return *retval;
}

Presentation::~Presentation(){
    zip_close(m_spres_archive);
}