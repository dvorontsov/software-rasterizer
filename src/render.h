#ifndef RENDER_H
#define RENDER_H

//#include <iostream>
#include <assert.h>
#include <malloc.h>
#include <limits.h>
#include <float.h>

#include "texture.h"
#include "model.h"
#include "tga_image_loader.h"
#include "math_operations.h"


extern FILE* logfile;

typedef uint32_t u32;


typedef struct
{
	void* memory;
	int width;
	int height;
	int bytes_per_pixel;
	float* z_buffer;
} FrameBuffer;

typedef struct
{
	vec3 position;	// World camera position
	vec3 target;	// Where the camera is looking at
	vec3 up;		// World UP direction
} Camera;

typedef struct
{
	FrameBuffer* frame_buffer;		// Main color buffer
	FrameBuffer* depth_buffer;		// Z-value buffer
	FrameBuffer* shadow_buffer;		// Light source view z-value buffer
	
	mat4 model_mat;
	mat4 view_mat;
	mat4 projection_mat;

	mat4 model_view_it_mat;			// Model-View-Inverse-Transpose
	mat4 mvp_mat;					// Model-View-Projection
	mat4 shadow_buffer_mvp_mat;		// Shadow Buffer Model-View-Projection

	mat4 viewport_mat;

	Camera camera;
} GraphicsContext;

typedef struct
{
	vec2i min;	// Lower-left corner
	vec2i max;	// Upper-rigth corner
} AABB;



void init_z_buffer(float* z_buffer, int width, int height);
void free_z_buffer(float* z_buffer);
void update_z_buffer(
	float* z_buffer, 
	unsigned int width, 
	unsigned int x, 
	unsigned int y, 
	float z);

void init_frame_buffer(
	FrameBuffer* buffer, 
	unsigned int width, 
	unsigned int height, 
	unsigned int bytes_per_pixel);
void free_frame_buffer(FrameBuffer* buffer);

unsigned char* index_into_buffer(
	unsigned char* buffer,
	int x,
	int y,
	int width,  // in bytes
	int height  // in bytes
);

void draw_pixel(
	const FrameBuffer* buffer,
	unsigned int x,
	unsigned int y,
	uint32_t color
);
void draw_pixel_3d(
	const FrameBuffer* buffer,
	float** z_buffer,
	unsigned int x,
	unsigned int y,
	float z,
	uint32_t ARGB_color);

int is_within_screen_boundaries(
	unsigned int x,
	unsigned int y,
	unsigned int screenWidth,
	unsigned int screenHeight
);

void draw_line(const FrameBuffer* buffer, int x1, int y1, int x2, int y2, uint32_t color);
void draw_line_3d(const FrameBuffer* buffer, float** z_buffer, vec3 p1, vec3 p2, uint32_t color);

AABB find_AABB(vec2i* points, u32 array_count);
void draw_AABB(FrameBuffer* buffer, AABB box, u32 ARGB_color);

vec3 sample_texture(Texture texture, vec2 tex_coords_uv);

vec3 normalize_color(vec3 color);
u32 pack_tga_color_ARGB32(TGA_Color color);
u32 pack_color_ARGB32(vec3 color, float alpha);

float flat_shading(vec3 vertex1, vec3 vertex2, vec3 vertex3, vec3 light_source);
float gouraud_shading(vec3 normal1, vec3 normal2, vec3 normal3, vec3 barycentric, vec3 light_source);

void draw_triangle(
	FrameBuffer* buffer, 
	vec2i pt1, 
	vec2i pt2, 
	vec2i pt3, 
	u32 ARGB_color);

mat4 get_look_at_mat(vec3 eye, vec3 center, vec3 up);
mat4 get_perspective_mat(float near, float far);
mat4 get_viewport_mat4(int x, int y, int width, int height, int near, int far);

mat3 get_tbn_mat(
	vec3 p1, vec3 p2, vec3 p3,
	vec2 uv1, vec2 uv2, vec2 uv3,
	vec3 normal);

void render_coordinate_frame(GraphicsContext* g_ctx);

void render_model(GraphicsContext* g_ctx, Model* model, Light light_source);
void render_scene(GraphicsContext* g_ctx, Scene* scene);

void copy_z_buffer_to_frame_buffer(FrameBuffer* buffer, float* z_buffer);
void render_shadow_buffer(GraphicsContext* g_ctx, Scene* scene, Light light);

void render_buffer_fill(
	FrameBuffer* buffer,
	const u32 width,
	const u32 height,
	const vec3 color);

void init_graphics_context(
	GraphicsContext* g_ctx,
	const unsigned int width,
	const unsigned int height,
	const unsigned int bytes_per_pixel,
	vec3 camera_position,
	vec3 camera_target,
	vec3 camera_up
);

void free_graphics_context(GraphicsContext g_ctx);

#endif // !RENDER_H
