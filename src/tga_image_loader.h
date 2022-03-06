#ifndef TGA_IMAGE_LOADER_H
#define TGA_IMAGE_LOADER_H

#include <assert.h>
#include <stdlib.h>
#include <stdio.h>

#pragma pack(push, 1)
typedef struct {
    char  idlength;
    char  colourmaptype;
    char  datatypecode;
    short int colourmaporigin;
    short int colourmaplength;
    char  colourmapdepth;
    short int x_origin;
    short int y_origin;
    short width;
    short height;
    char  bitsperpixel;
    char  imagedescriptor;
} TGA_Header2;
#pragma pack(pop)

typedef struct tga_color_t
{
    float r;
    float g;
    float b;
    float a;
} TGA_Color;

typedef enum tga_image_data_type_t
{
    RGB,
    RGBA
} TGA_ImageDataType;

typedef struct tga_image_t
{
    TGA_Header2 header;
    unsigned char* imageBuffer;
}TGA_Image;

static void write_tga_image(const char *filename,
                            const int width, 
                            const int height, 
                            const TGA_ImageDataType dataType,
                            const unsigned char *buffer)
{
    assert(sizeof(TGA_Header2) == 18);
    assert(buffer != 0);

    // Open in binary mode!  If text mode (w+), Windows will add 0D to every 0A
    char *mode = "wb+";

	FILE *file;    
	file = fopen(filename, mode);  
    assert(file != NULL);
    setbuf(file, NULL);

    TGA_Header2 header = {0};

    header.datatypecode = 2;
    header.width = width;
    header.height = height;

    if (dataType == RGB)
    {
        header.bitsperpixel = 24;
    }
    else if (dataType == RGBA)
    {
        header.bitsperpixel = 32;
    }
    
    int header_elements_written = fwrite(&header, sizeof(TGA_Header2), 1, file);

    int buffer_size_in_bytes = width * height * (header.bitsperpixel / 8);
    int body_elements_written = fwrite(buffer, sizeof(char), buffer_size_in_bytes, file);

	fclose(file);
}

// TODO - load is not implemented completely yet (impl rle)
static TGA_Image load_tga_image(const char *filename)
{
    TGA_Image result = {0};

    char* mode = "rb";
    long file_size = 0;

    FILE* file;

    // Open file
    file = fopen(filename, mode);
    if (file == NULL)
    {        
        return result;
    }
       
    // Get file size
    int seek_result = fseek(file, 0, SEEK_END);
    if (seek_result != 0)
    {
        return result;
    }
    file_size = ftell(file);

    // Reset pointer to beginning of file
    rewind(file);

    // Allocate memory for file buffer
    void* buffer = malloc(file_size);
    if (buffer == NULL)
    {
        return result;
    }

    fread(buffer, sizeof(char), file_size, file);
   
    TGA_Header2 *header = (TGA_Header2*)buffer;

    result.header.height = header->height;
    result.header.width = header->width;
    result.header.bitsperpixel = header->bitsperpixel;
    // TODO - fill out the rest

    result.imageBuffer = (unsigned char*)(++header);
    
    fclose(file);
    return result;
}


#endif // !TGA_IMAGE_LOADER_H
