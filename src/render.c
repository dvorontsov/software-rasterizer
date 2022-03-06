#include "render.h"

void init_z_buffer(float* z_buffer, int width, int height)
{
	int length = width * height;
	for (int i = 0; i < length; i++)
	{
		//z_buffer[i] = 0.0f;
		z_buffer[i] = -1.0f * FLT_MAX;
	}
}

void free_z_buffer(float* z_buffer)
{
	if (z_buffer)
	{
		free(z_buffer);
	}
}

void init_frame_buffer(
	FrameBuffer* buffer,
	unsigned int width,
	unsigned int height,
	unsigned int bytes_per_pixel)
{
	if (!buffer) return;

	buffer->width = width;
	buffer->height = height;
	buffer->bytes_per_pixel = bytes_per_pixel;

	int bytes = height * width * bytes_per_pixel;

	buffer->memory = calloc(bytes, sizeof(char));
	buffer->z_buffer = (float*)malloc(width * height * sizeof(float));
	init_z_buffer(buffer->z_buffer, width, height);
}

void free_frame_buffer(FrameBuffer* buffer)
{
	if (buffer)
	{
		if (buffer->memory)
		{
			free(buffer->memory);
		}

		buffer->width = 0;
		buffer->height = 0;
		buffer->bytes_per_pixel = 0;
	}
}

unsigned char* index_into_buffer(
	unsigned char* buffer,
	int x,
	int y,
	int width,  // in bytes
	int height  // in bytes
)
{
	assert(x < width && y < height);

	unsigned char* buf = buffer;
	buf += width * y;  // offset in y
	buf += x;  // offset in x

	unsigned char* result = buf;

	return result;
}

/*
	x, y - values in pixel space.

	x in range [0, max_value - 1]
	y in range [0, max_value - 1]
*/
void draw_pixel(
	const FrameBuffer* buffer,
	unsigned int x,
	unsigned int y,
	uint32_t color
)
{
#if 0
	fprintf(logfile, "draw_pixel: x = %d, y = %d, color = %x\n", x, y, color);
#endif

	uint8_t* row = (uint8_t*)buffer->memory; // first row
	row += buffer->bytes_per_pixel * buffer->width * y; // offset in y

	/*
		TODO - currently, the fucntion assumes bytes_per_pixel = 4,
		modify later
	*/
	uint32_t* pixel = (uint32_t*)row;
	pixel += x; // offset in x

	*pixel = color;
}

/*
	x, y - screen coordinates
	z - depth
*/
void draw_pixel_3d(
	const FrameBuffer* buffer,
	float** z_buffer,
	unsigned int x,
	unsigned int y,
	float z,
	uint32_t ARGB_color)
{
	// if outside screen boundaries, then discard
	if (x > buffer->width || y > buffer->height) return;

	if (buffer->z_buffer)
	{
		int index = x + y * buffer->width;
		if (z > buffer->z_buffer[index])
		{
			buffer->z_buffer[index] = z;
			draw_pixel(buffer, x, y, ARGB_color);
		}
	}
	else
	{
		draw_pixel(buffer, x, y, ARGB_color);
	}
}

void update_z_buffer(
	float* z_buffer,
	unsigned int width,
	unsigned int x,
	unsigned int y,
	float z)
{
	int index = x + y * width;
	if (z > z_buffer[index])
	{
		z_buffer[index] = z;
	}
}

/*
	@returns: 1 if true. 0, otherwise.
*/
int is_within_screen_boundaries(
	unsigned int x,
	unsigned int y,
	unsigned int screenWidth,
	unsigned int screenHeight
)
{
	int result = (x >= 0 && x < screenWidth && y >= 0 && y < screenHeight) ? 1 : 0;
	return result;
}

/*
	p1 - point with x and y in screen coordinates, and z = depth
	p2 - point with x and y in screen coordinates, and z = depth
*/
void draw_line_3d(const FrameBuffer* buffer, float** z_buffer, vec3 p1, vec3 p2, uint32_t color)
{
	int x1 = (int)p1.x;
	int y1 = (int)p1.y;

	int x2 = (int)p2.x;
	int y2 = (int)p2.y;

	float slope = ((float)y2 - (float)y1) / ((float)x2 - (float)x1);

	int x = x1;
	int y = y1;

	int dx = abs(x2 - x1);
	int dy = abs(y2 - y1);

	float z = 0.0f;

	if (dx > dy)  // x-dominant line
	{
		for (int offset = 0; offset < dx; offset++)
		{
			z = lerp(p1.z, p2.z, (float)offset / (float)dx);
			if (x2 - x1 > 0)
			{
				y = slope * ((x + offset) - x1) + y1;
				if (is_within_screen_boundaries((x + offset), y, buffer->width, buffer->height))
				{
					draw_pixel_3d(buffer, z_buffer, (x + offset), y, z, color);
				}
			}
			else if (x2 - x1 < 0)
			{
				y = slope * ((x - offset) - x1) + y1;
				if (is_within_screen_boundaries((x - offset), y, buffer->width, buffer->height))
				{
					draw_pixel_3d(buffer, z_buffer, (x - offset), y, z, color);
				}
			}
		}
	}
	else if (dy > dx)  // y-dominant line
	{
		for (int offset = 0; offset < dy; offset++)
		{
			z = lerp(p1.z, p2.z, (float)offset / (float)dy);
			if (y2 - y1 > 0)
			{
				x = ((y + offset) - y1) / slope + x1;
				if (is_within_screen_boundaries(x, y + offset, buffer->width, buffer->height))
				{
					draw_pixel_3d(buffer, z_buffer, x, (y + offset), z, color);
				}
			}
			else if (y2 - y1 < 0)
			{
				x = ((y - offset) - y1) / slope + x1;
				if (is_within_screen_boundaries(x, y - offset, buffer->width, buffer->height))
				{
					draw_pixel_3d(buffer, z_buffer, x, (y - offset), z, color);
				}
			}
		}
	}
	else if (dx == dy)  // 45 degree angle line
	{
		for (int offset = 0; offset < dx; offset++)
		{
			z = lerp(p1.z, p2.z, (float)offset / (float)dx);
			if (x2 - x1 > 0 && y2 - y1 > 0)
			{
				if (is_within_screen_boundaries(x + offset, y + offset, buffer->width, buffer->height))
				{
					draw_pixel_3d(buffer, z_buffer, x + offset, y + offset, z, color);  // Q1
				}
			}
			else if (x2 - x1 < 0 && y2 - y1 > 0)
			{
				if (is_within_screen_boundaries(x - offset, y + offset, buffer->width, buffer->height))
				{
					draw_pixel_3d(buffer, z_buffer, x - offset, y + offset, z, color);  // Q2
				}
			}
			else if (y2 - y1 < 0 && x2 - x1 < 0)
			{
				if (is_within_screen_boundaries(x - offset, y - offset, buffer->width, buffer->height))
				{
					draw_pixel_3d(buffer, z_buffer, x - offset, y - offset, z, color);  // Q3
				}
			}
			else if (y2 - y1 < 0 && x2 - x1 > 0)
			{
				if (is_within_screen_boundaries(x + offset, y - offset, buffer->width, buffer->height))
				{
					draw_pixel_3d(buffer, z_buffer, x + offset, y - offset, z, color);  // Q4
				}
			}
		}
	}
}

void draw_line(const FrameBuffer* buffer, int x1, int y1, int x2, int y2, uint32_t color)
{
	float slope = ((float)y2 - (float)y1) / ((float)x2 - (float)x1);

	int x = x1;
	int y = y1;

	int dx = abs(x2 - x1);
	int dy = abs(y2 - y1);

	if (dx > dy)  // x-dominant line
	{
		for (int offset = 0; offset < dx; offset++)
		{
			if (x2 - x1 > 0)
			{
				y = slope * ((x + offset) - x1) + y1;
				if (is_within_screen_boundaries((x + offset), y, buffer->width, buffer->height))
				{
					draw_pixel(buffer, (x + offset), y, color);
				}
			}
			else if (x2 - x1 < 0)
			{
				y = slope * ((x - offset) - x1) + y1;
				if (is_within_screen_boundaries((x - offset), y, buffer->width, buffer->height))
				{
					draw_pixel(buffer, (x - offset), y, color);
				}
			}
		}
	}
	else if (dy > dx)  // y-dominant line
	{
		for (int offset = 0; offset < dy; offset++)
		{
			if (y2 - y1 > 0)
			{
				x = ((y + offset) - y1) / slope + x1;
				if (is_within_screen_boundaries(x, y + offset, buffer->width, buffer->height))
				{
					draw_pixel(buffer, x, (y + offset), color);
				}
			}
			else if (y2 - y1 < 0)
			{
				x = ((y - offset) - y1) / slope + x1;
				if (is_within_screen_boundaries(x, y - offset, buffer->width, buffer->height))
				{
					draw_pixel(buffer, x, (y - offset), color);
				}
			}
		}
	}
	else if (dx == dy)  // 45 degree angle line
	{
		for (int offset = 0; offset < dx; offset++)
		{
			if (x2 - x1 > 0 && y2 - y1 > 0)
			{
				if (is_within_screen_boundaries(x + offset, y + offset, buffer->width, buffer->height))
				{
					draw_pixel(buffer, x + offset, y + offset, color);  // Q1
				}
			}
			else if (x2 - x1 < 0 && y2 - y1 > 0)
			{
				if (is_within_screen_boundaries(x - offset, y + offset, buffer->width, buffer->height))
				{
					draw_pixel(buffer, x - offset, y + offset, color);  // Q2
				}
			}
			else if (y2 - y1 < 0 && x2 - x1 < 0)
			{
				if (is_within_screen_boundaries(x - offset, y - offset, buffer->width, buffer->height))
				{
					draw_pixel(buffer, x - offset, y - offset, color);  // Q3
				}
			}
			else if (y2 - y1 < 0 && x2 - x1 > 0)
			{
				if (is_within_screen_boundaries(x + offset, y - offset, buffer->width, buffer->height))
				{
					draw_pixel(buffer, x + offset, y - offset, color);  // Q4
				}
			}
		}
	}
}

AABB find_AABB(vec2i* points, u32 array_count)
{
	AABB result = { 0 };

	u32 min_x = INT_MAX;
	u32 min_y = INT_MAX;
	u32 max_x = 0;
	u32 max_y = 0;

	for (int i = 0; i < array_count; i++)
	{
		vec2i p = points[i];

		if (p.x < min_x)
		{
			min_x = p.x;
		}
		if (p.y < min_y)
		{
			min_y = p.y;
		}

		if (p.x > max_x)
		{
			max_x = p.x;
		}
		if (p.y > max_y)
		{
			max_y = p.y;
		}
	}

	result.min = Vec2i(min_x, min_y);
	result.max = Vec2i(max_x, max_y);

	return result;
}

void draw_AABB(FrameBuffer* buffer, AABB box, u32 ARGB_color)
{
	// bottom hirozintal
	draw_line(buffer, box.min.x, box.min.y, box.max.x, box.min.y, ARGB_color);

	// right vertical
	draw_line(buffer, box.max.x, box.min.y, box.max.x, box.max.y, ARGB_color);

	// top horizontal
	draw_line(buffer, box.max.x, box.max.y, box.min.x, box.max.y, ARGB_color);

	// left vertical
	draw_line(buffer, box.min.x, box.max.y, box.min.x, box.min.y, ARGB_color);
}

mat4 get_viewport_mat4(int x, int y, int width, int height, int near, int far)
{
	mat4 result =
	{
		0.5f * width, 0, 0, x + 0.5f * width,
		0, 0.5f * height, 0, y + 0.5f * height,
		0, 0, 1, 0,
		0, 0, 0, 1
	};

	return result;
}

vec3 sample_texture(Texture texture, vec2 tex_coords_uv)
{
	vec3 result;

	int width = texture.width;
	int height = texture.height;

	float u = tex_coords_uv.x;
	float v = tex_coords_uv.y;

	int x = roundf(lerp(0, width - 1, u));
	int y = roundf(lerp(0, height - 1, v));

	int offset = y * width * texture.bytes_per_pixel + x * texture.bytes_per_pixel;

	unsigned char* memory = (unsigned char*)texture.memory;

	// TODO - impl alpha, if bytes_per_pixel = 4

	// TODO - below impl is for bytes_per_pixel = 3 only!
	result.x = memory[offset + 0];
	result.y = memory[offset + 1];
	result.z = memory[offset + 2];

	return result;
}

vec3 normalize_color(vec3 color)
{
	vec3 result = { 0 };

	result.x += color.x / 255;
	result.y += color.y / 255;
	result.z += color.z / 255;

	return result;
}

/*
	Color values should be normalized to [0, 1] range.
*/
u32 pack_tga_color_ARGB32(TGA_Color color)
{
	uint8_t A = (uint8_t)(color.a * 255);
	uint8_t R = (uint8_t)(color.r * 255);
	uint8_t G = (uint8_t)(color.g * 255);
	uint8_t B = (uint8_t)(color.b * 255);

	uint32_t result = A << 24 | R << 16 | G << 8 | B;

	return result;
}

/*
	Color values should be normalized to [0, 1] range.
*/
u32 pack_color_ARGB32(vec3 color, float alpha)
{
	uint8_t A = (uint8_t)(alpha * 255);
	uint8_t R = (uint8_t)(color.x * 255);
	uint8_t G = (uint8_t)(color.y * 255);
	uint8_t B = (uint8_t)(color.z * 255);

	uint32_t result = A << 24 | R << 16 | G << 8 | B;

	return result;
}

/*
	Flat shading - assumes that all vertices face in the same direction,
	making the surface look "flat"
*/
float flat_shading(vec3 vertex1, vec3 vertex2, vec3 vertex3, vec3 light_source)
{
	float result;

	light_source = multiply_scalar_vec3(-1.f, light_source);
	vec3 v1 = subtract_vec3(vertex3, vertex1);
	vec3 v2 = subtract_vec3(vertex2, vertex1);
	vec3 surface_normal = cross(v1, v2);
	float light_intensity_flat = dot_vec3(normalize_vec3(surface_normal), light_source);

	if (light_intensity_flat < 0.0f)	// If the surface is facing away from the light
	{
		result = 0.0f;	// No light shines on the surface
	}
	else
	{
		result = light_intensity_flat;
	}

	return result;
}

float gouraud_shading(vec3 normal1, vec3 normal2, vec3 normal3, vec3 barycentric, vec3 light_source)
{
	float result;

	light_source = normalize_vec3(light_source);

	float light_intensity_vert1 = dot_vec3(normalize_vec3(normal1), light_source);
	float light_intensity_vert2 = dot_vec3(normalize_vec3(normal2), light_source);
	float light_intensity_vert3 = dot_vec3(normalize_vec3(normal3), light_source);

	float light_intensity_gouraud =
		light_intensity_vert1 * barycentric.x +
		light_intensity_vert2 * barycentric.y +
		light_intensity_vert3 * barycentric.z;

	if (light_intensity_gouraud < 0.0f)	// If the surface is facing away from the light
	{
		result = 0.0f;	// No light shines on the surface
	}
	else
	{
		result = light_intensity_gouraud;
	}

	return result;
}

void draw_triangle(FrameBuffer* buffer, vec2i pt1, vec2i pt2, vec2i pt3, u32 ARGB_color)
{
	draw_line(buffer, pt1.x, pt1.y, pt2.x, pt2.y, ARGB_color);
	draw_line(buffer, pt2.x, pt2.y, pt3.x, pt3.y, ARGB_color);
	draw_line(buffer, pt3.x, pt3.y, pt1.x, pt1.y, ARGB_color);
}

mat4 get_perspective_mat(float near, float far)
{
	mat4 result =
	{
		1, 0, 0, 0,
		0, 1, 0, 0,
		0, 0, 0.5f * (far - near), 0.5f * (near + far),
		0, 0, -1.f, 0
	};

	return result;
}

mat4 get_look_at_mat(vec3 eye, vec3 center, vec3 up)
{
	// Build coordinate frame
	vec3 z_axis = normalize_vec3(subtract_vec3(eye, center)); // Looking down negative z-axis
	vec3 x_axis = normalize_vec3(cross(up, z_axis));
	vec3 y_axis = cross(normalize_vec3(z_axis), normalize_vec3(x_axis));

	mat4 translation_inv_mat =
	{
		1, 0, 0, -eye.x,
		0, 1, 0, -eye.y,
		0, 0, 1, -eye.z,
		0, 0, 0, 1,
	};

	mat4 rotation_inv_mat =
	{
		x_axis.x, x_axis.y, x_axis.z, 0,
		y_axis.x, y_axis.y, y_axis.z, 0,
		z_axis.x, z_axis.y, z_axis.z, 0,
		0, 0, 0, 1
	};

	mat4 result = multiply_mat4(rotation_inv_mat, translation_inv_mat);

	return result;
}

void render_coordinate_frame(GraphicsContext* g_ctx)
{
	FrameBuffer* buffer = g_ctx->frame_buffer;
	Camera camera = g_ctx->camera;
	mat4 mvp_mat = g_ctx->mvp_mat;
	mat4 viewport_mat = g_ctx->viewport_mat;

	vec4 center = Vec4(0, 0, 0, 1);
	vec4 x_axis = Vec4(1, 0, 0, 1);
	vec4 y_axis = Vec4(0, 1, 0, 1);
	vec4 z_axis = Vec4(0, 0, 1, 1);

	// Model-View-Projection transform
	center = multiply_mat4_vec4(mvp_mat, center);
	x_axis = multiply_mat4_vec4(mvp_mat, x_axis);
	y_axis = multiply_mat4_vec4(mvp_mat, y_axis);
	z_axis = multiply_mat4_vec4(mvp_mat, z_axis);

	// Perspective division
	center = divide_by_w(center);
	x_axis = divide_by_w(x_axis);
	y_axis = divide_by_w(y_axis);
	z_axis = divide_by_w(z_axis);

	// Viewport transform
	center = multiply_mat4_vec4(viewport_mat, center);
	x_axis = multiply_mat4_vec4(viewport_mat, x_axis);
	y_axis = multiply_mat4_vec4(viewport_mat, y_axis);
	z_axis = multiply_mat4_vec4(viewport_mat, z_axis);

	draw_line(buffer, center.x, center.y, x_axis.x, x_axis.y, pack_color_ARGB32(Vec3(1, 0, 0), 1));	// x-axis
	draw_line(buffer, center.x, center.y, y_axis.x, y_axis.y, pack_color_ARGB32(Vec3(0, 1, 0), 1));	// y-axis
	draw_line(buffer, center.x, center.y, z_axis.x, z_axis.y, pack_color_ARGB32(Vec3(0, 0, 1), 1));	// z-axis
}

mat3 get_tbn_mat(
	vec3 p1, vec3 p2, vec3 p3,
	vec2 uv1, vec2 uv2, vec2 uv3,
	vec3 normal)
{
	normal = normalize_vec3(normal);

	/*
		Edge1 = delta_u1 * T + delta_v1 * B
		Edge2 = delta_u2 * T + delta_v2 * B
	*/

	vec3 edge1 = subtract_vec3(p2, p1);
	vec3 edge2 = subtract_vec3(p3, p1);

	vec2 delta_uv1 = subtract_vec2(uv2, uv1);
	vec2 delta_uv2 = subtract_vec2(uv3, uv1);

	/*
		|Ex1, Ey1, Ez1| = |delta_u1, delta_v1| * |Tx, Ty, Tz|
		|Ex2, Ey2, Ez2|   |delta_u2, delta_v2|   |Bx, By, Bz|

		|delta_u1, delta_v1|^(-1)  * |Ex1, Ey1, Ez1| = |Tx, Ty, Tz|
		|delta_u2, delta_v2|         |Ex2, Ey2, Ez2|   |Bx, By, Bz|
	*/

	float det = delta_uv1.x * delta_uv2.y - delta_uv2.x * delta_uv1.y;

	float tangent_x = 1.0f / det * (delta_uv2.y * edge1.x - delta_uv1.y * edge2.x);
	float tangent_y = 1.0f / det * (delta_uv2.y * edge1.y - delta_uv1.y * edge2.y);
	float tangent_z = 1.0f / det * (delta_uv2.y * edge1.z - delta_uv1.y * edge2.z);

	float bitangent_x = 1.0f / det * (-1.0f * delta_uv2.x * edge1.x + delta_uv1.x * edge2.x);
	float bitangent_y = 1.0f / det * (-1.0f * delta_uv2.x * edge1.y + delta_uv1.x * edge2.y);
	float bitangent_z = 1.0f / det * (-1.0f * delta_uv2.x * edge1.z + delta_uv1.x * edge2.z);

	vec3 tangent = { tangent_x, tangent_y, tangent_z };
	vec3 bitanget = { bitangent_x, bitangent_y, bitangent_z };

	mat3 result =
	{
		tangent_x, bitangent_x, normal.x,
		tangent_y, bitangent_y, normal.y,
		tangent_z, bitangent_z, normal.z,
	};

	return result;
}

void render_model(
	GraphicsContext* g_ctx,
	Model* model,
	Light light_source)
{
	Camera camera = g_ctx->camera;
	mat4 model_view_mat = g_ctx->view_mat;
	mat4 model_view_it_mat = g_ctx->model_view_it_mat;
	mat4 projection_mat = g_ctx->projection_mat;
	mat4 viewport_mat = g_ctx->viewport_mat;
	mat4 shadow_mvp_mat = g_ctx->shadow_buffer_mvp_mat;
	float* z_buffer = g_ctx->frame_buffer->z_buffer;

	Mesh* mesh = model->mesh;
	Texture* diffuse_texture = model->diffuse_map;
	Texture* normal_texture = model->normal_map;
	Texture* specular_texture = model->specular_map;

	for (int i = 0; i < mesh->face_count; i++)
	{
		Face face = mesh->faces[i];

		// By OBJ format spec, index must start with 1.  If 0, then bad format?
		int index_offset = 1;

		// Vertecies
		Vertex vert1 = mesh->vertices[face.vertexIdx[0] - index_offset];
		Vertex vert2 = mesh->vertices[face.vertexIdx[1] - index_offset];
		Vertex vert3 = mesh->vertices[face.vertexIdx[2] - index_offset];

		vec3 vertex1_v3 = Vec3(vert1.x, vert1.y, vert1.z);
		vec3 vertex2_v3 = Vec3(vert2.x, vert2.y, vert2.z);
		vec3 vertex3_v3 = Vec3(vert3.x, vert3.y, vert3.z);

		// Normals
		Normal normal1 = mesh->normals[face.normalIdx[0] - index_offset];
		Normal normal2 = mesh->normals[face.normalIdx[1] - index_offset];
		Normal normal3 = mesh->normals[face.normalIdx[2] - index_offset];

		vec3 normal1_v3 = Vec3(normal1.x, normal1.y, normal1.z);
		vec3 normal2_v3 = Vec3(normal2.x, normal2.y, normal2.z);
		vec3 normal3_v3 = Vec3(normal3.x, normal3.y, normal3.z);

		// Texture Coordinates
		TextureCoordinate tex_coords1 = mesh->tex_coords[face.textureIdx[0] - index_offset];
		TextureCoordinate tex_coords2 = mesh->tex_coords[face.textureIdx[1] - index_offset];
		TextureCoordinate tex_coords3 = mesh->tex_coords[face.textureIdx[2] - index_offset];

		vec2 tex_coords1_v2 = Vec2(tex_coords1.u, tex_coords1.v);
		vec2 tex_coords2_v2 = Vec2(tex_coords2.u, tex_coords2.v);
		vec2 tex_coords3_v2 = Vec2(tex_coords3.u, tex_coords3.v);

		// TODO - if Flat Shading, it can be performed here for optimization (or after ModelView transform?)
		float flat_shaded = flat_shading(vertex1_v3, vertex2_v3, vertex3_v3, Vec3(1, 1, 1));

		// Homogenous coordinates for vertex positions
		vec4 vertex1_v4 = Vec4_v3_in(vertex1_v3, 1.f);
		vec4 vertex2_v4 = Vec4_v3_in(vertex2_v3, 1.f);
		vec4 vertex3_v4 = Vec4_v3_in(vertex3_v3, 1.f);

		// ModelView transformation
		vertex1_v4 = multiply_mat4_vec4(model_view_mat, vertex1_v4);
		vertex2_v4 = multiply_mat4_vec4(model_view_mat, vertex2_v4);
		vertex3_v4 = multiply_mat4_vec4(model_view_mat, vertex3_v4);

		// Projection transformation
		vec4 vertex1_clip_space_v4 = multiply_mat4_vec4(projection_mat, vertex1_v4);
		vec4 vertex2_clip_space_v4 = multiply_mat4_vec4(projection_mat, vertex2_v4);
		vec4 vertex3_clip_space_v4 = multiply_mat4_vec4(projection_mat, vertex3_v4);

		// Division by w
		vertex1_v4 = divide_by_w(vertex1_clip_space_v4);
		vertex2_v4 = divide_by_w(vertex2_clip_space_v4);
		vertex3_v4 = divide_by_w(vertex3_clip_space_v4);

		// Viewport transformation
		vertex1_v4 = multiply_mat4_vec4(viewport_mat, vertex1_v4);
		vertex2_v4 = multiply_mat4_vec4(viewport_mat, vertex2_v4);
		vertex3_v4 = multiply_mat4_vec4(viewport_mat, vertex3_v4);

		float x1 = (int)vertex1_v4.x;
		float y1 = (int)vertex1_v4.y;

		float x2 = (int)vertex2_v4.x;
		float y2 = (int)vertex2_v4.y;

		float x3 = (int)vertex3_v4.x;
		float y3 = (int)vertex3_v4.y;

		vec2i pts[] =
		{
			Vec2i(roundf(x1), roundf(y1)),
			Vec2i(roundf(x2), roundf(y2)),
			Vec2i(roundf(x3), roundf(y3))
		};

		AABB aabb = find_AABB(pts, 3);

		// Line sweep inside the bouding box and check if each point P is inside the triangle
		for (int y = aabb.max.y - 1; y >= aabb.min.y; y--) {
			for (int x = aabb.min.x; x < aabb.max.x; x++) {
				vec3 bary = barycentric(Vec2(x1, y1), Vec2(x2, y2), Vec2(x3, y3), Vec2(x, y));

				// Perspective correct linear interpolation
				float denom = (bary.x / vertex1_clip_space_v4.w + bary.y / vertex2_clip_space_v4.w + bary.z / vertex3_clip_space_v4.w);
				vec3 bary_clip = Vec3(
					(bary.x / vertex1_clip_space_v4.w) / denom,
					(bary.y / vertex2_clip_space_v4.w) / denom,
					(bary.z / vertex3_clip_space_v4.w) / denom
				);

				float error = 0.0001f;

				int is_outside_the_triangle =
					bary.x < 0 ||
					bary.y < 0 ||
					bary.z < 0 ||
					(bary.x + bary.y + bary.z) > (1 + error) ||
					(bary.x + bary.y + bary.z) < (1 - error);

				if (!is_outside_the_triangle)
				{

					float depth_clip_space = bary_clip.x * vertex1_clip_space_v4.z + bary_clip.y * vertex2_clip_space_v4.z + bary_clip.z * vertex3_clip_space_v4.z;
					vec2 P = { x, y }; // x, y - in screen coordinates

					vec2 weighted_uv1 = multiply_scalar_vec2(bary_clip.x, tex_coords1_v2);
					vec2 weighted_uv2 = multiply_scalar_vec2(bary_clip.y, tex_coords2_v2);
					vec2 weighted_uv3 = multiply_scalar_vec2(bary_clip.z, tex_coords3_v2);

					vec2 tex_coord = add_vec2(add_vec2(weighted_uv1, weighted_uv2), weighted_uv3);

					vec3 texel_color = Vec3(127, 127, 127);
					if (diffuse_texture)
					{
						texel_color = sample_texture(*diffuse_texture, tex_coord);
					}

					vec3 texel_normal = Vec3_0();
					if (normal_texture)
					{
						texel_normal = sample_texture(*normal_texture, tex_coord);
					}				
					
					vec3 texel_specular = Vec3_0();
					if(specular_texture)
					{
						texel_specular = sample_texture(*specular_texture, tex_coord);
					}

					float gouraud_shaded = gouraud_shading(normal1_v3, normal2_v3, normal3_v3, bary_clip, light_source.position);

					// Modify color based on computed light intensity					
					texel_color.x *= gouraud_shaded;
					texel_color.y *= gouraud_shaded;
					texel_color.z *= gouraud_shaded;

					texel_color = normalize_color(texel_color);

					u32 ARGB_color = pack_color_ARGB32(texel_color, 1);
					u32 ARGB_debug_color = pack_color_ARGB32(Vec3(1, 0, 0), 1);

					// Write final color to frame buffer
					draw_pixel_3d(
						g_ctx->frame_buffer,
						g_ctx->frame_buffer->z_buffer,
						P.x, P.y, depth_clip_space,
						ARGB_color
					);
				}
			}
		}
	}
}

void render_scene(GraphicsContext* g_ctx, Scene* scene)
{
	for (int i = 0; i < scene->modelCount; i++)
	{
		render_model(g_ctx, scene->models[i], scene->light);
	}
}

void copy_z_buffer_to_frame_buffer(FrameBuffer* buffer, float* z_buffer)
{
	for (int i = 0; i < buffer->width * buffer->height; i++)
	{
		float z_value = z_buffer[i];
		u32 x = i % buffer->width;
		u32 y = i / buffer->height;

		vec3 z_color = Vec3(z_value, z_value, z_value);
		draw_pixel(buffer, x, y, pack_color_ARGB32(z_color, 1.0f));
	}
}

void render_shadow_buffer(GraphicsContext* g_ctx, Scene* scene, Light light)
{
	// Set camera to light's position
	mat4 light_view_mat = get_look_at_mat(light.position, Vec3(0, 0, 0), Vec3(0, 1, 0));

	// TODO - for directional light, need to use orthographic projection!

	// render from new camera position
	mat4 model_view_mat = light_view_mat;
	mat4 projection_mat = g_ctx->projection_mat;
	mat4 viewport_mat = g_ctx->viewport_mat;

	g_ctx->shadow_buffer_mvp_mat = multiply_mat4(projection_mat, model_view_mat);

	for (int m_idx = 0; m_idx < scene->modelCount; m_idx++)
	{
		Model* model = scene->models[m_idx];
		Mesh* mesh = model->mesh;
		for (int i = 0; i < mesh->face_count; i++)
		{
			Face face = mesh->faces[i];

			// By OBJ format spec, index must start with 1.  If 0, then bad format?
			int index_offset = 1;

			// Vertecies
			Vertex vert1 = mesh->vertices[face.vertexIdx[0] - index_offset];
			Vertex vert2 = mesh->vertices[face.vertexIdx[1] - index_offset];
			Vertex vert3 = mesh->vertices[face.vertexIdx[2] - index_offset];

			vec3 vertex1_v3 = Vec3(vert1.x, vert1.y, vert1.z);
			vec3 vertex2_v3 = Vec3(vert2.x, vert2.y, vert2.z);
			vec3 vertex3_v3 = Vec3(vert3.x, vert3.y, vert3.z);

			// Homogenous coordinates for vertex positions
			vec4 vertex1_v4 = Vec4_v3_in(vertex1_v3, 1.f);
			vec4 vertex2_v4 = Vec4_v3_in(vertex2_v3, 1.f);
			vec4 vertex3_v4 = Vec4_v3_in(vertex3_v3, 1.f);

			// ModelView transformation
			vertex1_v4 = multiply_mat4_vec4(model_view_mat, vertex1_v4);
			vertex2_v4 = multiply_mat4_vec4(model_view_mat, vertex2_v4);
			vertex3_v4 = multiply_mat4_vec4(model_view_mat, vertex3_v4);

			// Projection transformation
			vec4 vertex1_clip_space_v4 = multiply_mat4_vec4(projection_mat, vertex1_v4);
			vec4 vertex2_clip_space_v4 = multiply_mat4_vec4(projection_mat, vertex2_v4);
			vec4 vertex3_clip_space_v4 = multiply_mat4_vec4(projection_mat, vertex3_v4);

			// Division by w
			vertex1_v4 = divide_by_w(vertex1_clip_space_v4);
			vertex2_v4 = divide_by_w(vertex2_clip_space_v4);
			vertex3_v4 = divide_by_w(vertex3_clip_space_v4);

			// Viewport transformation
			vertex1_v4 = multiply_mat4_vec4(viewport_mat, vertex1_v4);
			vertex2_v4 = multiply_mat4_vec4(viewport_mat, vertex2_v4);
			vertex3_v4 = multiply_mat4_vec4(viewport_mat, vertex3_v4);

			float x1 = (int)vertex1_v4.x;
			float y1 = (int)vertex1_v4.y;

			float x2 = (int)vertex2_v4.x;
			float y2 = (int)vertex2_v4.y;

			float x3 = (int)vertex3_v4.x;
			float y3 = (int)vertex3_v4.y;

			vec2i pts[] =
			{
				Vec2i(roundf(x1), roundf(y1)),
				Vec2i(roundf(x2), roundf(y2)),
				Vec2i(roundf(x3), roundf(y3))
			};

			AABB aabb = find_AABB(pts, 3);

			// Line sweep inside the bouding box and check if each point P is inside the triangle
			for (int y = aabb.max.y - 1; y >= aabb.min.y; y--) {
				for (int x = aabb.min.x; x < aabb.max.x; x++) {
					vec3 bary = barycentric(Vec2(x1, y1), Vec2(x2, y2), Vec2(x3, y3), Vec2(x, y));

					// Perspective correct linear interpolation
					float denom = (bary.x / vertex1_clip_space_v4.w + bary.y / vertex2_clip_space_v4.w + bary.z / vertex3_clip_space_v4.w);
					vec3 bary_clip = Vec3(
						(bary.x / vertex1_clip_space_v4.w) / denom,
						(bary.y / vertex2_clip_space_v4.w) / denom,
						(bary.z / vertex3_clip_space_v4.w) / denom
					);

					float error = 0.0001f;

					int is_outside_the_triangle =
						bary.x < 0 ||
						bary.y < 0 ||
						bary.z < 0 ||
						(bary.x + bary.y + bary.z) > (1 + error) ||
						(bary.x + bary.y + bary.z) < (1 - error);

					if (!is_outside_the_triangle)
					{
						// We only care about the depth (z-value) of shadow buffer
						float depth = bary_clip.x * vertex1_clip_space_v4.z + bary_clip.y * vertex2_clip_space_v4.z + bary_clip.z * vertex3_clip_space_v4.z;
						update_z_buffer(g_ctx->shadow_buffer->z_buffer, g_ctx->shadow_buffer->width, x, y, depth);
					}
				}
			}
		}
	}

	copy_z_buffer_to_frame_buffer(g_ctx->shadow_buffer, g_ctx->shadow_buffer->z_buffer);
}

void render_buffer_fill(
	FrameBuffer* buffer,
	const u32 width,
	const u32 height,
	const vec3 color)
{
	for (int index = 0; index < width * height; index++)
	{
		u32 x = index % width;
		u32 y = index / height;
		u32 packed_color = pack_color_ARGB32(color, 1.f);

		draw_pixel(buffer, x, y, packed_color);
	}
}

void init_graphics_context(
	GraphicsContext* g_ctx,
	const unsigned int width,
	const unsigned int height,
	const unsigned int bytes_per_pixel,
	vec3 camera_position,
	vec3 camera_target,
	vec3 camera_up
)
{
	if (!g_ctx) return;

	g_ctx->frame_buffer = (FrameBuffer*)malloc(sizeof(FrameBuffer));
	init_frame_buffer(g_ctx->frame_buffer, width, height, bytes_per_pixel);

	g_ctx->depth_buffer = (FrameBuffer*)malloc(sizeof(FrameBuffer));
	init_frame_buffer(g_ctx->depth_buffer, width, height, bytes_per_pixel);

	g_ctx->shadow_buffer = (FrameBuffer*)malloc(sizeof(FrameBuffer));
	init_frame_buffer(g_ctx->shadow_buffer, width, height, bytes_per_pixel);

	g_ctx->camera.position = camera_position;
	g_ctx->camera.target = camera_target;
	g_ctx->camera.up = camera_up;

	g_ctx->model_mat = get_identity_mat4();
	print_mat4(logfile, g_ctx->model_mat, "Model:");

	g_ctx->view_mat = get_look_at_mat(camera_position, camera_target, camera_up);
	print_mat4(logfile, g_ctx->view_mat, "View:");

	// TODO - this needs to be inverse_transpose(View * Model)
	g_ctx->model_view_it_mat = inverse_mat4(transpose_mat4(g_ctx->view_mat));
	print_mat4(logfile, g_ctx->model_view_it_mat, "ModelView inverse transpose:");

	g_ctx->projection_mat = get_perspective_mat(0.f, 1.f);
	print_mat4(logfile, g_ctx->projection_mat, "Projection:");

	mat4 model_view_mat = multiply_mat4(g_ctx->view_mat, g_ctx->model_mat);
	g_ctx->model_view_it_mat = inverse_mat4(transpose_mat4(model_view_mat));

	g_ctx->mvp_mat = multiply_mat4(g_ctx->projection_mat, model_view_mat);

	g_ctx->viewport_mat = get_viewport_mat4(0, 0, width, height, 0, 1);
	print_mat4(logfile, g_ctx->viewport_mat, "Viewport:");
}

void free_graphics_context(GraphicsContext g_ctx)
{
	free_frame_buffer(g_ctx.frame_buffer);
	free_frame_buffer(g_ctx.depth_buffer);
	free_frame_buffer(g_ctx.shadow_buffer);
}
