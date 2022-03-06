#include "texture.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

Texture* load_texture(const char* path)
{
	Texture* texture = (Texture*)malloc(sizeof(Texture));

	stbi_set_flip_vertically_on_load(1);

	int num_channels;
	int tex_width;
	int tex_height;

	unsigned char* data = stbi_load(
		path,
		&tex_width,
		&tex_height,
		&num_channels,
		0
	);

	texture->width = tex_width;
	texture->height = tex_height;
	texture->memory = data;
	texture->bytes_per_pixel = num_channels;

	return texture;
}

void free_texture(Texture* texture)
{
	if (texture)
	{
		STBI_FREE(texture->memory);
	}
	free(texture);
}