#include "blam_definitions.hpp"

void read_string(fstream& stream, char* str, int32_t length)
{
	stream.read(str, length);	
}

void write_string(fstream& stream, char* str, int32_t length)
{
	stream.write(str, length);
}

//Assumes char* is preinitialized with the proper amount of structs in tag_block
void read_tag_block(fstream& stream, char* result, tag_block* block, int32_t size)
{
	auto pos = stream.tellg();
	stream.seekg(block->offset);
	for (int i = 0; i < block->count; i++)
		stream.read((char*)(result + i * size), size);
	stream.seekg(pos);
}