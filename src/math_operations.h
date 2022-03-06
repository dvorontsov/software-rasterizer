#ifndef MATH_OPERATIONS_H
#define MATH_OPERATIONS_H

#include <stdint.h>
#include <math.h>

typedef uint32_t u32;

typedef struct vector2i_t
{
	int x;
	int y;
} vec2i;

static vec2i Vec2i_0()
{
	vec2i v = {.x=0, .y=0};
	return v;
}

static vec2i Vec2i(int x, int y)
{
	vec2i v = { .x = x, .y = y };
	return v;
}

typedef struct vector2f_t
{
	float x;
	float y;
} vec2;

static vec2 Vec2_0()
{
	vec2 v = {.x = 0.0f, .y = 0.0f};
	return v;
}

static vec2 Vec2(float x, float y)
{
	vec2 v = { .x = x, .y = y };
	return v;
}

static vec2 Vec2_in(vec2i in) {
	vec2 v = { .x = in.x, .y = in.y };
	return v;
}

typedef struct vector3f_t
{
	float x;
	float y;
	float z;
} vec3;

static vec3 Vec3_0()
{
	vec3 v = { .x = 0.0f, .y = 0.0f, .z = 0.0f };
	return v;
}

static vec3 Vec3(float x, float y, float z)
{
	vec3 v = { .x = x, .y = y, .z = z };
	return v;
}

static vec3 Vec3_v2_in(vec2 in)
{
	vec3 v = { .x = in.x, .y = in.y, .z = 0.0f };
	return v;
};

typedef struct vector4f_t
{
	float x;
	float y;
	float z;
	float w;
} vec4;

static vec4 Vec4_0()
{
	vec4 v = { .x = 0.0f, .y = 0.0f, .z = 0.0f, .w = 0.0f };
	return v;
}

static vec4 Vec4(float x, float y, float z, float w)
{
	vec4 v = { .x = x, .y = y, .z = z, .w = w };
	return v;
}

static vec4 Vec4_v3_in(vec3 in, float w)
{
	vec4 v = { .x = in.x, .y = in.y, .z = in.z, .w = w };
	return v;
}

static vec3 xyz(vec4 v)
{
	vec3 result = Vec3(v.x, v.y, v.z);

	return result;
}

typedef struct matrix2x2_t
{
	float m[4];
} mat2;

typedef struct matrix3x3_t
{
	float m[9];
} mat3;

typedef struct matrix4x4_t
{
	float m[16];
} mat4;

static float dot_vec2(vec2 a, vec2 b)
{
	float result = a.x * b.x + a.y + b.y;

	return result;
}

static float dot_vec3(vec3 a, vec3 b)
{
	float result = a.x * b.x + a.y * b.y + a.z * b.z;

	return result;
}

static float dot_vec4(vec4 a, vec4 b)
{
	float result = a.x * b.x + a.y * b.y + a.z * b.z + a.w * b.w;

	return result;
}

static vec3 flip_vec3(vec3 v)
{
	vec3 result = 
	{
		-1.0f * v.x,
		-1.0f * v.y,
		-1.0f * v.z,
	};

	return result;
}

static vec4 divide_by_w(vec4 v)
{
	vec4 result = Vec4_0();

	if(v.w != 0.0f)
	{
		result.x = v.x / v.w;
		result.y = v.y / v.w;
		result.z = v.z / v.w;
		result.w = 1.0f;
	}
	
	return result;
}

static mat3 get_identity_mat3()
{
	mat3 result =
	{
		1.f, 0.f, 0.f,
		0.f, 1.f, 0.f,
		0.f, 0.f, 1.f
	};
	return result;
}

static mat4 get_identity_mat4()
{
	mat4 result =
	{
		1.f, 0.f, 0.f, 0.f,
		0.f, 1.f, 0.f, 0.f,
		0.f, 0.f, 1.f, 0.f,
		0.f, 0.f, 0.f, 1.f
	};
	return result;
}

static mat3 transpose_mat3(mat3 a)
{
	mat3 result =
	{
		a.m[0], a.m[3], a.m[6],
		a.m[1], a.m[4], a.m[7],
		a.m[2], a.m[5], a.m[8]
	};

	return result;
}

static mat4 transpose_mat4(mat4 a)
{
	mat4 result =
	{
		a.m[0], a.m[4], a.m[8], a.m[12],
		a.m[1], a.m[5], a.m[9], a.m[13],
		a.m[2], a.m[6], a.m[10], a.m[14],
		a.m[3], a.m[7], a.m[11], a.m[15],
	};

	return result;
}

static float get_elem_index(int dimension, int row, int col)
{
	assert(row >= 0 && row <= (dimension - 1));
	assert(col >= 0 && col <= (dimension - 1));

	int result = dimension * row + col;

	return result;
}

static float get_elem_mat3(mat3 mat, int row, int col)
{
	int index = get_elem_index(3, row, col);
	float result = mat.m[index];

	return result;
}

static float get_elem_mat4(mat4 mat, int row, int col)
{
	int index = get_elem_index(4, row, col);
	float result = mat.m[index];

	return result;
}

static mat2 get_sub_mat2(mat3 mat, int row, int col)
{
	mat2 result = {0};

	int index = 0;
	for (int i = 0; i < 3; i++)
	{
		for (int j = 0; j < 3; j++)
		{
			if (i != row && j != col)
			{
				float elem = get_elem_mat3(mat, i, j);
				result.m[index++] = elem;
			}
		}
	}

	return result;
}

static mat3 get_sub_mat3(mat4 mat, int row, int col)
{
	mat3 result = {0};

	int index = 0;
	for (int i = 0; i < 4; i++)
	{
		for (int j = 0; j < 4; j++)
		{
			if (i != row && j != col)
			{
				float elem = get_elem_mat4(mat, i, j);
				result.m[index++] = elem;
			}						
		}
	}

	return result;
}

static float det_mat2(mat2 mat)
{
	float result = mat.m[0] * mat.m[3] - mat.m[2] * mat.m[1];
		
	return result;
}

static float det_mat3(mat3 mat)
{
	float result =
		(get_elem_mat3(mat, 0, 0) * get_elem_mat3(mat, 1, 1) * get_elem_mat3(mat, 2, 2) +
			get_elem_mat3(mat, 0, 1) * get_elem_mat3(mat, 1, 2) * get_elem_mat3(mat, 2, 0) +
			get_elem_mat3(mat, 0, 2) * get_elem_mat3(mat, 1, 0) * get_elem_mat3(mat, 2, 1)) -
			(get_elem_mat3(mat, 2, 0) * get_elem_mat3(mat, 1, 1) * get_elem_mat3(mat, 0, 2) +
				get_elem_mat3(mat, 2, 1) * get_elem_mat3(mat, 1, 2) * get_elem_mat3(mat, 0, 0) +
				get_elem_mat3(mat, 2, 2) * get_elem_mat3(mat, 1, 0) * get_elem_mat3(mat, 0, 1));

	return result;
}

static float det_mat4(mat4 mat)
{
	float result = 0.0f;

	for (int col = 0; col < 4; col++)
	{
		float coeff = get_elem_mat4(mat, 0, col);
		mat3 sub_mat = get_sub_mat3(mat, 0, col);
		int sign = (col % 2 == 0 ? 1 : -1);

		result += sign * coeff * det_mat3(sub_mat);
	}

	return result;
}

static float minors_elem_mat3(mat3 mat, int row, int col)
{
	float result = det_mat2(get_sub_mat2(mat, row, col));

	return result;
}

static float minors_elem_mat4(mat4 mat, int row, int col)
{
	float result = det_mat3(get_sub_mat3(mat, row, col));

	return result;
}

static mat3 inverse_mat3(mat3 mat)
{
	/*
		MCA Algorithm:

		M -			the minors matrix: a matrix of determinants
		C -			the cofactors matrix: the minors matrix Handamard-multiplied
					by a grid of alternating pluses and minuses
		A^(-1) -	the adjugate matrix: the transpose of the cofactors matrix,
					scalar multiplied by one over det of the original matrix.
	*/

	// Check determinant to see if the matrix is invertible
	float det = det_mat3(mat);
	if (det == 0.0f) {
		// Matrix not invertible
		return get_identity_mat3();
	}

	mat3 result = {0};

	int index = 0;
	for (int i = 0; i < 3; i++)
	{
		for (int j = 0; j < 3; j++)
		{
			int sign = pow((-1), (i + j));
			result.m[index++] = sign * minors_elem_mat3(mat, j, i) * (1 / det);
		}
	}

	return result;
}

static mat4 inverse_mat4(mat4 mat)
{
	/*
		MCA Algorithm:

		M -			the minors matrix: a matrix of determinants
		C -			the cofactors matrix: the minors matrix Handamard-multiplied
					by a grid of alternating pluses and minuses
		A^(-1) -	the adjugate matrix: the transpose of the cofactors matrix,
					scalar multiplied by one over det of the original matrix.
	*/

	// Check determinant to see if the matrix is invertible
	float det = det_mat4(mat);
	if (det == 0.0f) {
		// Matrix not invertible
		return get_identity_mat4();
	}

	mat4 result = {0};
	
	int index = 0;
	for (int i = 0; i < 4; i++)
	{
		for (int j = 0; j < 4; j++)
		{			
			int sign = pow((-1), (i + j));
			result.m[index++] = sign * minors_elem_mat4(mat, j, i) * (1/det);
		}
	}

	return result;
}

static vec3 multiply_mat3_vec3(mat3 mat, vec3 p)
{
	vec3 result = {0};

	result.x = mat.m[0] * p.x + mat.m[1] * p.y + mat.m[2] * p.z;
	result.y = mat.m[3] * p.x + mat.m[4] * p.y + mat.m[5] * p.z;
	result.z = mat.m[6] * p.x + mat.m[7] * p.y + mat.m[8] * p.z;

	return result;
}

static vec4 multiply_mat4_vec4(mat4 mat, vec4 p)
{
	vec4 result = {0};

	result.x = mat.m[0] * p.x + mat.m[1] * p.y + mat.m[2] * p.z + mat.m[3] * p.w;
	result.y = mat.m[4] * p.x + mat.m[5] * p.y + mat.m[6] * p.z + mat.m[7] * p.w;
	result.z = mat.m[8] * p.x + mat.m[9] * p.y + mat.m[10] * p.z + mat.m[11] * p.w;
	result.w = mat.m[12] * p.x + mat.m[13] * p.y + mat.m[14] * p.z + mat.m[15] * p.w;

	return result;
}

static mat3 multiply_mat3(mat3 a, mat3 b)
{
	vec4 a_row_0 = Vec4(a.m[0], a.m[1], a.m[2], a.m[3]);
	vec4 a_row_1 = Vec4(a.m[4], a.m[5], a.m[6], a.m[7]);
	vec4 a_row_2 = Vec4(a.m[8], a.m[9], a.m[10], a.m[11]);

	vec4 b_col_0 = Vec4(b.m[0], b.m[4], b.m[8], b.m[12]);
	vec4 b_col_1 = Vec4(b.m[1], b.m[5], b.m[9], b.m[13]);
	vec4 b_col_2 = Vec4(b.m[2], b.m[6], b.m[10], b.m[14]);

	mat3 result =
	{
		dot_vec4(a_row_0, b_col_0), dot_vec4(a_row_0, b_col_1), dot_vec4(a_row_0, b_col_2),
		dot_vec4(a_row_1, b_col_0), dot_vec4(a_row_1, b_col_1), dot_vec4(a_row_1, b_col_2),
		dot_vec4(a_row_2, b_col_0), dot_vec4(a_row_2, b_col_1), dot_vec4(a_row_2, b_col_2),
	};

	return result;
}

static mat4 multiply_mat4(mat4 a, mat4 b)
{
	vec4 a_row_0 = Vec4(a.m[0], a.m[1], a.m[2], a.m[3]);
	vec4 a_row_1 = Vec4(a.m[4], a.m[5], a.m[6], a.m[7]);
	vec4 a_row_2 = Vec4(a.m[8], a.m[9], a.m[10], a.m[11]);
	vec4 a_row_3 = Vec4(a.m[12], a.m[13], a.m[14], a.m[15]);

	vec4 b_col_0 = Vec4(b.m[0], b.m[4], b.m[8], b.m[12]);
	vec4 b_col_1 = Vec4(b.m[1], b.m[5], b.m[9], b.m[13]);
	vec4 b_col_2 = Vec4(b.m[2], b.m[6], b.m[10], b.m[14]);
	vec4 b_col_3 = Vec4(b.m[3], b.m[7], b.m[11], b.m[15]);

	mat4 result =
	{
		dot_vec4(a_row_0, b_col_0), dot_vec4(a_row_0, b_col_1), dot_vec4(a_row_0, b_col_2), dot_vec4(a_row_0, b_col_3),
		dot_vec4(a_row_1, b_col_0), dot_vec4(a_row_1, b_col_1), dot_vec4(a_row_1, b_col_2), dot_vec4(a_row_1, b_col_3),
		dot_vec4(a_row_2, b_col_0), dot_vec4(a_row_2, b_col_1), dot_vec4(a_row_2, b_col_2), dot_vec4(a_row_2, b_col_3),
		dot_vec4(a_row_3, b_col_0), dot_vec4(a_row_3, b_col_1), dot_vec4(a_row_3, b_col_2), dot_vec4(a_row_3, b_col_3)
	};

	return result;
}

static void print_vec2(FILE* file, vec2 vertex, char* header)
{
	fprintf(file, "%s: %f, %f\n", header, vertex.x, vertex.y);
}

static void print_vec3(FILE* file, vec3 vertex, char* header)
{
	fprintf(file, "%s: %f, %f, %f\n", header, vertex.x, vertex.y, vertex.z);
}

static void print_mat3(FILE* file, mat3 a, char* header)
{
	fprintf(file, "%s\n", header);
	fprintf(file, "%f %f %f \n", a.m[0], a.m[1], a.m[2]);
	fprintf(file, "%f %f %f \n", a.m[3], a.m[4], a.m[5]);
	fprintf(file, "%f %f %f \n", a.m[6], a.m[7], a.m[8]);
	fprintf(file, "\n");
}

static void print_mat4(FILE* file, mat4 a, char* header)
{
	fprintf(file, "%s\n", header);
	fprintf(file, "%f %f %f %f\n", a.m[0], a.m[1], a.m[2], a.m[3]);
	fprintf(file, "%f %f %f %f\n", a.m[4], a.m[5], a.m[6], a.m[7]);
	fprintf(file, "%f %f %f %f\n", a.m[8], a.m[9], a.m[10], a.m[11]);
	fprintf(file, "%f %f %f %f\n", a.m[12], a.m[13], a.m[14], a.m[15]);
	fprintf(file, "\n");
}

static vec2 multiply_scalar_vec2(float scalar, vec2 vector)
{
	vec2 result = Vec2(
		scalar * vector.x, 
		scalar * vector.y
	);

	return result;
}

static vec3 multiply_scalar_vec3(float scalar, vec3 vector)
{
	vec3 result = Vec3(
		scalar * vector.x, 
		scalar * vector.y, 
		scalar * vector.z
	);

	return result;
}

static vec2 add_vec2(vec2 a, vec2 b)
{
	vec2 result = Vec2(
		a.x + b.x,
		a.y + b.y
	);

	return result;
}

static vec3 add_vec3(vec3 a, vec3 b)
{
	vec3 result = Vec3(
		a.x + b.x, 
		a.y + b.y, 
		a.z + b.z
	);

	return result;
}

static vec2 subtract_vec2(vec2 a, vec2 b)
{
	vec2 result = Vec2(
		a.x - b.x, 
		a.y - b.y
	);

	return result;
}

static vec3 subtract_vec3(vec3 a, vec3 b)
{
	vec3 result = Vec3(
		a.x - b.x, 
		a.y - b.y, 
		a.z - b.z
	);

	return result;
}

static float len_vec2(vec2 a)
{
	float result = sqrt(a.x * a.x + a.y * a.y);

	return result;
}

static float len_vec3(vec3 a)
{
	float result = sqrt(a.x * a.x + a.y * a.y + a.z * a.z);

	return result;
}

static vec3 normalize_vec3(vec3 a)
{
	float length = len_vec3(a);
	if (length == 0.0f)
	{
		return Vec3(0, 0, 0);
	}
	vec3 result = Vec3(
		a.x / length, 
		a.y / length, 
		a.z / length
	);

	return result;
}

static vec3 cross(vec3 a, vec3 b)
{
	vec3 result = Vec3(
		a.y * b.z - a.z * b.y,
		a.z * b.x - a.x * b.z,
		a.x * b.y - a.y * b.x
	);

	return result;
}

static vec2 proj_vec2(vec2 of, vec2 onto)
{
	vec2 result = {0};
	if (len_vec2(onto) != 0.0f)
	{
		result = multiply_scalar_vec2((dot_vec2(of, onto) / dot_vec2(onto, onto)), onto);
	}

	return result;
}

static vec3 proj_vec3(vec3 of, vec3 onto)
{
	vec3 result = {0};
	if (len_vec3(onto) != 0.0f)
	{
		result = multiply_scalar_vec3((dot_vec3(of, onto) / dot_vec3(onto, onto)), onto);
	}
	
	return result;
}

/*
	Reflection of a vector from sufrace around surface normal.  
	Meaning the vector v goes into the surface and gets reflected.

	v		- vector that gets reflected from the surface
	normal	- surface normal
*/
static vec3 reflection_vec3(vec3 v, vec3 normal)
{
	// r = v - 2 * proj(v, n)
	vec3 result = subtract_vec3(v, multiply_scalar_vec3(2, proj_vec3(v, normal)));

	return result;
}

static vec3 barycentric(vec2 A, vec2 B, vec2 C, vec2 P)
{
	// u + v + w = 1
	// P = uA + vB + wC

	// ABC,   ABP, ACP, BCP

	vec2 AC = subtract_vec2(C, A);
	vec2 AB = subtract_vec2(B, A);
	vec2 AP = subtract_vec2(P, A);
	vec2 BC = subtract_vec2(C, B);
	vec2 BP = subtract_vec2(P, B);

	float u = 0.0f;
	float v = 0.0f;
	float w = 0.0f;

	float area_ABC = len_vec3(cross(Vec3_v2_in(AC), Vec3_v2_in(AB))) / 2;
	if (area_ABC != 0.f)
	{
		float area_ABP = len_vec3(cross(Vec3_v2_in(AP), Vec3_v2_in(AB))) / 2;
		float area_ACP = len_vec3(cross(Vec3_v2_in(AP), Vec3_v2_in(AC))) / 2;
		float area_BCP = len_vec3(cross(Vec3_v2_in(BC), Vec3_v2_in(BP))) / 2;

		u = area_BCP / area_ABC;		
		v = area_ACP / area_ABC;
		w = area_ABP / area_ABC;
	}

	vec3 result = Vec3(u, v, w);
	return result;
}

static vec3 barycentric2(vec2 A, vec2 B, vec2 C, vec2 P)
{
	// u + v + w = 1
	// P = uA + vB + wC

	mat3 ABC =
	{
		A.x, B.x, C.x,
		A.y, B.y, C.y,
		1,   1,   1
	};

	vec3 P_v3 = { P.x, P.y, 1 };

	vec3 result = multiply_mat3_vec3(inverse_mat3(ABC), P_v3);
	
	return result;
}

/*
	when t = 0.0f, min.
	when t = 1.0f, max
*/
static float lerp(float min, float max, float t)
{
	float result = (1 - t) * min + t * max;

	return result;
}

#endif // !MATH_OPERATIONS_H
