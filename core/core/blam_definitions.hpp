#pragma once

#include <fstream>

enum e_strings
{
	k_maximum_file_string_length = 0x40
};

typedef uint32_t tag;

using namespace std;

struct tag_block
{
	int32_t count = 0;
	int32_t offset = 0;
	int32_t unused = 0;
};
static_assert(sizeof(tag_block) == 0xC, "");

struct tag_data
{
	int32_t length = 0;
	int32_t unused1 = 0;
	int32_t unused2 = 0;
	int32_t offset = 0;
	int32_t unused3 = 0;
};
static_assert(sizeof(tag_data) == 0x14, "");

struct real_vector3d
{
	float x;
	float y;
	float z;
};
static_assert(sizeof(real_vector3d) == 0xC, "");

struct real_vector4d
{
	float x;
	float y;
	float z;
	float w;
};
static_assert(sizeof(real_vector4d) == 0x10, "");

struct real_quaternion
{
	float i;
	float j;
	float k;
	float w;
};
static_assert(sizeof(struct real_quaternion) == 0x10, "");

void read_string(fstream& stream, char* str, int32_t length);

void write_string(fstream& stream, char* str, int32_t length);

void read_tag_block(fstream& stream, char* result, tag_block* block, int32_t size);