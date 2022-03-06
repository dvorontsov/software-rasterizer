#ifndef TEXTURE_H
#define TEXTURE_H

typedef struct texture_t
{
	void* memory;
	int width;
	int height;
	int bytes_per_pixel;
} Texture;


Texture* load_texture(const char* path);
void free_texture(Texture* texture);

#endif