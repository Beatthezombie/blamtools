#include "utils.hpp"

float import_scale = 100.0f;

void bmf_vector_3d_to_point3(real_vector3d* vector, Point3* point)
{
	point->x = vector->x;
	point->y = vector->y;
	point->z = vector->z;
}

void bmf_real_quat_to_quat(real_quaternion* real_quat, Quat* quat)
{
	quat->x = real_quat->i;
	quat->y = real_quat->j;
	quat->z = real_quat->k;
	quat->w = real_quat->w;
}

void bmf_scale_to_point3(float scale, Point3* point_scale)
{
	point_scale->x = scale;
	point_scale->y = scale;
	point_scale->z = scale;
}

void convert_char_to_wchar(char* str, wchar_t* wstr, int length)
{
	std::mbstowcs(wstr, str, length);
}

INode* create_dummy_node(Interface* ip, bmf_node* node, INode* parent)
{
	DummyObject* dummy = (DummyObject*)ip->CreateInstance(HELPER_CLASS_ID, Class_ID(DUMMY_CLASS_ID, 0));
	INode* inode = ip->CreateObjectNode(dummy);
	wchar_t name[BMF_STRING_LENGTH];
	convert_char_to_wchar(node->name, (wchar_t*)&name, BMF_STRING_LENGTH);
	inode->SetName(name);
	if(parent != nullptr)
		parent->AttachChild(inode);
	
	Matrix3 transform;
	
	Point3 position;
	Quat rotation;
	Point3 scale;

	bmf_real_quat_to_quat(&node->rotation, &rotation);
	bmf_vector_3d_to_point3(&node->position, &position);
	bmf_scale_to_point3(node->scale, &scale);

	transform.SetScale(scale);
	transform.SetRotate(rotation.Inverse());
	transform.SetTrans(import_scale * position);

	if (parent != nullptr)
		transform = transform * parent->GetNodeTM(0);

	inode->SetNodeTM(0, transform);
	inode->ShowBone(2);
	inode->SetWireColor(RGB(255, 255, 255));
	inode->SetBoneNodeOnOff(true, 0);
	return inode;
}

INode* create_marker(Interface* ip, bmf_marker* marker, INode* parent)
{
	GenSphere* sphere_object = (GenSphere*)ip->CreateInstance(GEOMOBJECT_CLASS_ID, Class_ID(SPHERE_CLASS_ID, 0));
	sphere_object->SetParams(1.0f + marker->scale, 12, TRUE, FALSE);
	INode* marker_node = ip->CreateObjectNode(sphere_object);
	wchar_t name[BMF_STRING_LENGTH];
	convert_char_to_wchar(marker->name, (wchar_t*)&name, BMF_STRING_LENGTH);
	marker_node->SetName(name);

	if (parent != nullptr)
		parent->AttachChild(marker_node);

	Matrix3 transform;

	Point3 position;
	Quat rotation;

	bmf_real_quat_to_quat(&marker->rotation, &rotation);
	bmf_vector_3d_to_point3(&marker->position, &position);

	transform.SetScale(Point3(1,1,1));
	transform.SetRotate(rotation.Inverse());
	transform.SetTrans(import_scale * position);

	if (parent != nullptr)
		transform = transform * parent->GetNodeTM(0);

	marker_node->SetNodeTM(0, transform);
	marker_node->SetWireColor(RGB(255, 0, 0));
	return marker_node;
}

INode* create_depth_sphere(Interface* ip, bmf_bounding_sphere* sphere, INode* parent, int sphere_index)
{
	GenSphere* sphere_object = (GenSphere*)ip->CreateInstance(GEOMOBJECT_CLASS_ID, Class_ID(SPHERE_CLASS_ID, 0));
	sphere_object->SetParams(sphere->radius * import_scale, 12, TRUE, FALSE);
	INode* sphere_node = ip->CreateObjectNode(sphere_object);
	std::string name("depth_test_sphere");
	wchar_t wname[BMF_STRING_LENGTH];
	convert_char_to_wchar((char*)(name.c_str()), (wchar_t*)&wname, BMF_STRING_LENGTH);
	sphere_node->SetName(wname);

	Matrix3 transform;

	Point3 position;
	bmf_vector_3d_to_point3(&sphere->position, &position);
	transform.SetScale(Point3(1, 1, 1));
	transform.SetTrans(import_scale * position);

	sphere_node->SetNodeTM(0, transform);
	sphere_node->SetWireColor(RGB(0, 0, 255));
	return sphere_node;
}

ILayer* create_region(ILayerManager* manager, bmf_region* region)
{
	wchar_t name[BMF_STRING_LENGTH];
	convert_char_to_wchar(region->name, (wchar_t*)&name, BMF_STRING_LENGTH);
	ILayer* layer = manager->CreateLayer(name);
	return layer;
}

INode* create_mesh(fstream& stream, Interface* ip, bmf_mesh* mesh, ULONG* node_handles, int node_count)
{
	// read bmf vertices, faces
	bmf_vertex* bmf_vertices = new bmf_vertex[mesh->mesh_vertices.count];
	bmf_face* bmf_faces = new bmf_face[mesh->mesh_faces.count];
	read_tag_block(stream, (char*)bmf_vertices, &mesh->mesh_vertices, sizeof(bmf_vertex));
	read_tag_block(stream, (char*)bmf_faces, &mesh->mesh_faces, sizeof(bmf_face));
	
	int vertex_count = mesh->mesh_vertices.count;
	int face_count = mesh->mesh_faces.count;

	TriObject* tri_object = CreateNewTriObject();
	Mesh* max_mesh = &tri_object->GetMesh();

	max_mesh->setNumVerts(vertex_count);

	for (int i = 0; i < vertex_count; i++)
	{
		Point3 position;
		bmf_vector_3d_to_point3(&bmf_vertices[i].position, &position);
		max_mesh->setVert(i, import_scale * position);
	}

	max_mesh->SpecifyNormals();
	MeshNormalSpec* normal_spec = max_mesh->GetSpecifiedNormals();
	normal_spec->ClearNormals();
	normal_spec->SetNumNormals(vertex_count);

	for (int i = 0; i < vertex_count; i++)
	{
		Point3 normal;
		bmf_vector_3d_to_point3(&bmf_vertices[i].normal, &normal);
		normal_spec->Normal(i) = normal.Normalize();
		normal_spec->SetNormalExplicit(i, true);
	}

	max_mesh->setNumMaps(2);
	max_mesh->setMapSupport(1, true);

	MeshMap* map = &max_mesh->Map(1);

	map->setNumVerts(vertex_count);

	for (int i = 0; i < vertex_count; i++)
	{
		UVVert* tex_vertex = &map->tv[i];
		bmf_vector_3d_to_point3(&bmf_vertices[i].texcoord, tex_vertex);
	}


	max_mesh->setNumFaces(face_count);
	normal_spec->SetNumFaces(face_count);
	map->setNumFaces(face_count);

	for (int i = 0; i < face_count; i++)
	{
		int v0 = bmf_faces[i].vertex_indices[0];
		int v1 = bmf_faces[i].vertex_indices[1];
		int v2 = bmf_faces[i].vertex_indices[2];

		Face* face = &max_mesh->faces[i];

		// MATERIAL IDS TODO
		face->setMatID(bmf_faces[i].material_index);
		face->setEdgeVisFlags(1, 1, 1);
		face->setVerts(v0, v1, v2);
		/* this makes it really ugly
		MeshNormalFace *normal_face = &normal_spec->Face(i);
		normal_face->SpecifyAll();
		normal_face->SetNormalID(0, v0);
		normal_face->SetNormalID(1, v1);
		normal_face->SetNormalID(2, v2);
		*/
		TVFace *tex_face = &map->tf[i];
		tex_face->setTVerts(v0, v1, v2);
	}

	max_mesh->InvalidateGeomCache();
	max_mesh->InvalidateTopologyCache();
		


	INode* inode = ip->CreateObjectNode(tri_object);
	wchar_t name[BMF_STRING_LENGTH];
	convert_char_to_wchar(mesh->name, (wchar_t*)&name, BMF_STRING_LENGTH);
	inode->SetName(name);


	Modifier* skinned_mesh_mod = (Modifier*)ip->CreateInstance(OSM_CLASS_ID, SKIN_CLASSID);
	GetCOREInterface17()->AddModifier(*inode, *skinned_mesh_mod);

	ISkinImportData* skin = (ISkinImportData*)skinned_mesh_mod->GetInterface(I_SKINIMPORTDATA);
	BitArray used_bones = BitArray(node_count);

	for (int i = 0; i < vertex_count; i++)
	{
		for (int j = 0; j < 4; j++)
		{
			bmf_vertex_weight cur_weight = bmf_vertices[i].weights[j];
			if (cur_weight.weight > 0.0f)
				used_bones.Set(cur_weight.node_index);
		}
	}


	for (int i = 0; i < node_count; i++)
	{
		if (used_bones[i] == 1)
		{
			INode* cur_node = ip->GetINodeByHandle(node_handles[i]);
			skin->AddBoneEx(cur_node, true);
		}
	}

	for (int i = 0; i < vertex_count; i++)
	{
		Tab<float> weights;
		Tab<INode*> nodes;
		for (int j = 0; j < 4; j++)
		{
			bmf_vertex_weight cur_weight = bmf_vertices[i].weights[j];
			if (cur_weight.weight > 0.0f)
			{
				weights.Append(1, &cur_weight.weight);
				INode* cur_node = ip->GetINodeByHandle(node_handles[cur_weight.node_index]);
				nodes.Append(1, &cur_node);
			}
		}
		skin->AddWeights(inode, i, nodes, weights);
	}

	ISkin* i_skin = (ISkin*)skinned_mesh_mod->GetInterface(I_SKIN);
	ISkinContextData* skin_context = i_skin->GetContextInterface(inode);
	
	
	
	delete[] bmf_vertices;
	delete[] bmf_faces;
	return inode;

}