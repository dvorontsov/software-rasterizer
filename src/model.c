#include "model.h"

Model* load_model(
	char* name,
	char* obj,
	char* diffuse_map,
	char* normal_map,
	char* specular_map)
{
	Model* model = (Model*)calloc(1, sizeof(Model));

	model->name = name;

	char model_path[1024];

	get_path(model_path, ROOT_DIR, obj);
	Mesh* mesh = load_obj_from_file(model_path);
	assert(mesh != NULL);
	model->mesh = mesh;

	char texture_path[1024];

	if (diffuse_map)
	{
		get_path(texture_path, ROOT_DIR, diffuse_map);
		Texture* diffuse_texture = load_texture(texture_path);
		model->diffuse_map = diffuse_texture;
	}

	if (normal_map)
	{
		get_path(texture_path, ROOT_DIR, normal_map);
		Texture* normal_texture = load_texture(texture_path);
		model->normal_map = normal_texture;
	}

	if (specular_map)
	{
		get_path(texture_path, ROOT_DIR, specular_map);
		Texture* specular_texture = load_texture(texture_path);
		model->specular_map = specular_texture;
	}

	return model;
}

void free_model(Model* model)
{
	if (model)
	{
		if (model->diffuse_map) free_texture(model->diffuse_map);
		if (model->normal_map) free_texture(model->normal_map);
		if (model->specular_map) free_texture(model->specular_map);

		free_mesh(model->mesh);

		free(model);
	}	
}
