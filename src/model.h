#ifndef MODEL_H
#define MODEL_H

#include "texture.h"
#include "tga_image_loader.h"
#include "math_operations.h"
#include "obj_model_loader.h"

#define MAX_MODEL_COUNT_PER_SCENE 10

typedef struct
{
	char* name;
	Mesh* mesh;
	Texture* diffuse_map;
	Texture* normal_map;
	Texture* specular_map;
} Model;

typedef struct
{
	vec3 position;
} Light;

typedef struct
{
	Model* models[MAX_MODEL_COUNT_PER_SCENE];
	int modelCount;
	Light light;
} Scene;

Model* load_model(
	char* name,
	char* obj,
	char* diffuse_map,
	char* normal_map,
	char* specular_map);

void free_model(Model* model);

#endif // !MODEL_H