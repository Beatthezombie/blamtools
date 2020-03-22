// test.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include <blam_definitions.hpp>
#include <blam_model_file.hpp>

using namespace std;
int main()
{
	const char* filename = "D:\\3dsmax\\blamtools\\test\\x64\\Debug\\warthog.bmf";

	cout << "Loading file: " << filename << endl;

	fstream fs;

	fs.open(filename, ios::in | ios::binary);
	blam_model_file bmf;
	if (fs.is_open())
		read_bmf(fs, &bmf);
	
	cout << bmf.name << endl;

	bmf_region* bmf_regions = new bmf_region[bmf.regions.count];
	read_tag_block(fs, (char*)bmf_regions, &bmf.regions, sizeof(bmf_region));
	for (int i = 0; i < bmf.regions.count; i++)
		cout << bmf_regions[i].name << endl;
	delete[] bmf_regions;

	bmf_mesh* bmf_meshes = new bmf_mesh[bmf.meshes.count];
	read_tag_block(fs, (char*)bmf_meshes, &bmf.meshes, sizeof(bmf_mesh));
	for (int i = 0; i < bmf.meshes.count; i++)
		cout << bmf_meshes[i].name << endl;
	delete[] bmf_meshes;



	fs.close();
}
