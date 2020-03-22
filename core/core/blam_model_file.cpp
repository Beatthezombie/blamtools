#include "blam_model_file.hpp"

void read_bmf(fstream& stream, blam_model_file* bmf)
{
	stream.read((char*)&bmf->header, sizeof(bmf_header));
	stream.seekg(bmf->header.offset);

	read_string(stream, (char*)&bmf->name, BMF_STRING_LENGTH);
	stream.read((char*)&bmf->regions, sizeof(tag_block));
	stream.read((char*)&bmf->materials, sizeof(tag_block));
	stream.read((char*)&bmf->nodes, sizeof(tag_block));
	stream.read((char*)&bmf->markers, sizeof(tag_block));
	stream.read((char*)&bmf->meshes, sizeof(tag_block));
	stream.read((char*)&bmf->sh_red, 0x10 * sizeof(float));
	stream.read((char*)&bmf->sh_green, 0x10 * sizeof(float));
	stream.read((char*)&bmf->sh_blue, 0x10 * sizeof(float));
	stream.read((char*)&bmf->light_probes, sizeof(tag_block));
	stream.read((char*)&bmf->depth_test_spheres, sizeof(tag_block));
}