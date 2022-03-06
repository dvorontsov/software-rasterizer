#ifndef OBJ_IMAGE_LOADER_H
#define OBJ_IMAGE_LOADER_H

#include <stddef.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#include "utils/root_dir.h"

#if defined(_WIN32) || defined(_WIN64)
// Welcome to portability
#define strtok_r strtok_s
#endif


typedef union vetrex_t
{
	struct
	{
		float x;
		float y;
		float z;
	};
	struct
	{
		float u;
		float v;
		float w;
	};
	float e[3];
} Vertex, Normal, TextureCoordinate;

typedef union face_t
{
	struct
	{
		unsigned int vertexIdx[3];
		unsigned int textureIdx[3];
		unsigned int normalIdx[3];
	};
	unsigned int e[9];
} Face;

typedef struct mesh_t
{
	Vertex* vertices;
	unsigned int vertex_count;

	Normal* normals;
	unsigned int normal_count;

	TextureCoordinate* tex_coords;
	unsigned int text_coords_count;

	Face* faces;
	unsigned int face_count;
} Mesh;

typedef enum obj_attribute_type_t
{
	VERTEX,
	NORMAL,
	TEXTURE_COORDINATE
} ObjAttributeType;


void parse_attribute(ObjAttributeType type,
	void** memory,
	size_t item_size,
	int* count,
	char* token,
	char* delim,
	char* next,
	int* elem_idx);

Mesh* load_obj_from_file(const char* path);
void free_mesh(Mesh* mesh);

#endif // !OBJ_IMAGE_LOADER_H
