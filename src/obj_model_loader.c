#include "obj_model_loader.h"

char* get_path(char* out,
	const char* root_dir,
	const char* relative_path)
{
	// TODO - change to safer methods
	strcpy(out, root_dir);
	strcat(out, relative_path);
	return out;
}

void parse_attribute(ObjAttributeType type,
	void** memory,
	size_t item_size,
	int* count,
	char* token,
	char* delim,
	char* next,
	int* elem_idx)
{
	(*count)++;
	*memory = realloc(*memory, item_size * (*count));

	while (token != NULL)
	{
		int attrib_index = (*count) - 1;
		token = strtok_r(NULL, delim, &next);
		if (token != NULL)
		{
			float val = atof(token);
			if (type == VERTEX)
			{
				Vertex* mem = (Vertex*)*memory;
				mem[attrib_index].e[*elem_idx] = val;
			}
			else if (type == NORMAL)
			{
				Normal* mem = (Normal*)*memory;
				mem[attrib_index].e[*elem_idx] = val;
			}
			else if (type == TEXTURE_COORDINATE)
			{
				TextureCoordinate* mem = (TextureCoordinate*)*memory;
				mem[attrib_index].e[*elem_idx] = val;
			}
			(*elem_idx)++;
		}
	}
}

Mesh* load_obj_from_file(const char* path)
{
	FILE* file;

	file = fopen(path, "r");
	if (!file)
	{
		printf("Can't open file\n");
		return NULL;
	}

	char line[255] = { 0 };

	Vertex* verts = NULL;
	int vert_count = 0;

	Face* faces = NULL;
	int face_count = 0;

	Normal* normals = NULL;
	int normal_count = 0;

	TextureCoordinate* tex_coords = NULL;
	int tex_coord_count = 0;

	char* space_delim = " ";
	char* slash_delim = "/";

	while (fgets(line, 255, file) != NULL)  // parse file line by line
	{
		int elem_idx = 0;

		char* next;
		char* token = strtok_r(line, space_delim, &next);

		if (strcmp(token, "v") == 0)  // vertices
		{
			parse_attribute(VERTEX,
				(void**)&verts,
				sizeof(Vertex),
				&vert_count,
				token,
				space_delim,
				next,
				&elem_idx);
		}
		else if (strcmp(token, "vn") == 0)  // normals
		{
			parse_attribute(NORMAL,
				(void**)&normals,
				sizeof(Normal),
				&normal_count,
				token,
				space_delim,
				next,
				&elem_idx);
		}
		else if (strcmp(token, "vt") == 0)  // texture coordinates
		{
			// TODO - consider case of "vt u v" with only 2 values and not 3
			parse_attribute(TEXTURE_COORDINATE,
				(void**)&tex_coords,
				sizeof(TextureCoordinate),
				&tex_coord_count,
				token,
				space_delim,
				next,
				&elem_idx);
		}
		else if (strcmp(token, "f") == 0)  // faces
		{
			face_count++;
			faces = (Face*)realloc(faces, sizeof(Face) * face_count);

			unsigned int vertex_elem_idx = 0;
			unsigned int tex_coord_elem_idx = 0;
			unsigned int normal_idx = 0;

			while (token != NULL)
			{
				int face_idx = face_count - 1;
				token = strtok_r(NULL, space_delim, &next);
				if (token != NULL)
				{
					// split "a/b/c"
					int sub_elem_idx = 0;
					for (char* next2, *sub_token = strtok_r(token, slash_delim, &next2);
						sub_token;
						sub_token = strtok_r(NULL, slash_delim, &next2))
					{				
						if (sub_elem_idx == 0)  // vertex position index
						{
							unsigned int val = atoi(sub_token);
							faces[face_idx].vertexIdx[vertex_elem_idx] = val;
							vertex_elem_idx++;
						}
						else if (sub_elem_idx == 1) // texture coordinates index
						{
							unsigned int val = atoi(sub_token);
							faces[face_idx].textureIdx[tex_coord_elem_idx] = val;
							tex_coord_elem_idx++;
						}
						else if (sub_elem_idx == 2) // vertex normals index
						{
							unsigned int val = atoi(sub_token);
							faces[face_idx].normalIdx[normal_idx] = val;
							normal_idx++;
						}
						sub_elem_idx++;
					}
				}
			}
		}
	}
	fclose(file);

	Mesh* mesh = (Mesh*)malloc(sizeof(Mesh));

	mesh->vertices = verts;
	mesh->vertex_count = vert_count;

	mesh->normals = normals;
	mesh->normal_count = normal_count;

	mesh->tex_coords = tex_coords;
	mesh->text_coords_count = tex_coord_count;

	mesh->faces = faces;
	mesh->face_count = face_count;

	return mesh;
}

void free_mesh(Mesh* mesh)
{
	if (mesh)
	{
		free(mesh->faces);
		free(mesh->normals);
		free(mesh->tex_coords);
		free(mesh->vertices);

		free(mesh);
	}
}