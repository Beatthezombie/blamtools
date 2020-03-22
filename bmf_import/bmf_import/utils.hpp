#pragma once

#include "blam_definitions.hpp"
#include "blam_model_file.hpp"
#include <cstdlib>
#include <istdplug.h>
#include <dummy.h>
#include <ilayermanager.h>
#include <ilayer.h>
#include <triobj.h>
#include <mesh.h>
#include <MeshNormalSpec.h>
#include <iskin.h>
#include <simpobj.h>
#include <string>

extern float import_scale;

void bmf_vector_3d_to_point3(real_vector3d* vector, Point3* point);

void bmf_real_quat_to_quat(real_quaternion* real_quat, Quat* quat);

void convert_char_to_wchar(char* str, wchar_t* wstr, int length);

INode* create_dummy_node(Interface* ip, bmf_node* node, INode* parent);

ILayer* create_region(ILayerManager* manager, bmf_region* region);

INode* create_mesh(fstream& stream, Interface* ip, bmf_mesh* mesh, ULONG* node_handles, int node_count);

INode* create_marker(Interface* ip, bmf_marker* marker, INode* parent);

INode* create_depth_sphere(Interface* ip, bmf_bounding_sphere* sphere, INode* parent, int sphere_index);