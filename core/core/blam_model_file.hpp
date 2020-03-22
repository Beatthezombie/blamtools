#pragma once

#pragma pack(1)
#include "blam_definitions.hpp"

constexpr auto BMF_STRING_LENGTH = 0x40;

struct bmf_header
{
	tag signature;
	int32_t version;
	int32_t flags;
	int32_t content_flags;
	int32_t offset;
};
static_assert(sizeof(bmf_header) == 0x14, "");

struct bmf_region
{
	char name[BMF_STRING_LENGTH];
};
static_assert(sizeof(bmf_region) == BMF_STRING_LENGTH, "");

struct bmf_material
{
	char name[BMF_STRING_LENGTH];
	int32_t draw_type;
};
static_assert(sizeof(bmf_material) == BMF_STRING_LENGTH + 0x4, "");

struct bmf_node
{
	char name[BMF_STRING_LENGTH];
	int32_t parent_index;
	int32_t first_child_index;
	int32_t next_sibling_index;
	real_vector3d position;
	real_quaternion rotation;
	float scale;
};
static_assert(sizeof(bmf_node) == BMF_STRING_LENGTH + 0x2C, "");

struct bmf_marker
{
	char name[BMF_STRING_LENGTH];
	int32_t node_index;
	real_vector3d position;
	real_quaternion rotation;
	float scale;
};
static_assert(sizeof(bmf_marker) == BMF_STRING_LENGTH + 0x24, "");

struct bmf_light_probe
{
	real_vector3d position;
	float coefficients[81];
};
static_assert(sizeof(bmf_light_probe) == 0x150, "");

struct bmf_bounding_sphere
{
	real_vector3d plane_normal;
	float plane_distance;
	real_vector3d position;
	float radius;
	char node_indices[4];
	float node_weights[3];
};
static_assert(sizeof(bmf_bounding_sphere) == 0x30, "");

struct bmf_face
{
	short material_index;
	short vertex_indices[3];
};
static_assert(sizeof(bmf_face) == 0x8, "");

struct bmf_vertex_weight
{
	int32_t node_index;
	float weight;
};
static_assert(sizeof(bmf_vertex_weight) == 0x8, "");

struct bmf_vertex
{
	real_vector3d position;
	real_vector3d texcoord;
	real_vector3d normal;
	real_vector3d tangent;
	real_vector3d binormal;
	bmf_vertex_weight weights[4];
};
static_assert(sizeof(bmf_vertex) == 0x5C, "");

struct bmf_lighting_vertex
{
	int32_t sh_order;
	float coefficients[9];
};
static_assert(sizeof(bmf_lighting_vertex) == 0x28, "");

struct bmf_mesh
{
	char name[BMF_STRING_LENGTH];
	int32_t region_index;
	tag_block mesh_vertices;
	tag_block mesh_per_vertex_lighting;
	tag_block mesh_faces;
};
static_assert(sizeof(bmf_mesh) == BMF_STRING_LENGTH + 0x28, "");

struct blam_model_file
{
	bmf_header header;
	char name[BMF_STRING_LENGTH];
	tag_block regions;
	tag_block materials;
	tag_block nodes;
	tag_block markers;
	tag_block meshes;
	float sh_red[16];
	float sh_green[16];
	float sh_blue[16];
	tag_block light_probes;
	tag_block depth_test_spheres;
};
static_assert(sizeof(blam_model_file) == BMF_STRING_LENGTH + 0x128, "");

#pragma pack()

void read_bmf(fstream& stream, blam_model_file* bmf);