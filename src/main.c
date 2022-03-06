#include <stdio.h>

#include "render.h"

FILE* logfile;

int main()
{
	logfile = fopen("log.txt", "w+");

	Model* floor_model = load_model(
		"Floor",
		"assets/floor/floor.obj",
		"assets/floor/floor_diffuse.tga",
		"assets/floor/floor_nm_tangent.tga",
		0
	);

	Model* cube_model = load_model(
		"Cube",
		"assets/cube/cube.obj",
		"assets/cube/cube_diffuse_map.tga",
		"assets/cube/cube_normal_map.tga",
		0
	);

	Model* suzanne_model = load_model(
		"Suzanne",
		"assets/suzanne/suzanne.obj",
		"assets/default.png",
		0,
		0
	);

	int width = 800;
	int height = 800;
	int bytes_per_pixel = 4;

	GraphicsContext g_ctx = { 0 };
	init_graphics_context(
		&g_ctx,
		width,
		height,
		bytes_per_pixel,		
		Vec3(0.5f, 0.5f, 2.0f),		// camera position
		Vec3(0.0f, 0.0f, 0.0f),		// camera target
		Vec3(0.0f, 1.0f, 0.0f)		// world up
	);


	Light light = { Vec3(2, 2, 2) };

	Scene scene = { 0 };
	scene.modelCount = 1;
	scene.models[0] = suzanne_model;
	scene.light = light;

	vec3 light_blue_color = { 0.23f, 0.65f, 0.82f };
	render_buffer_fill(g_ctx.frame_buffer, width, height, light_blue_color);

	//render_shadow_buffer(&g_ctx, &scene, light);

	render_scene(&g_ctx, &scene);
	
	//render_coordinate_frame(&g_ctx);

	write_tga_image(
		"out_image.tga",
		width,
		height,
		RGBA,
		(unsigned char*)g_ctx.frame_buffer->memory
	);
		
	free_model(suzanne_model);
	free_model(cube_model);
	free_graphics_context(g_ctx);
	fclose(logfile);

	printf("End\n");
	return 0;
}