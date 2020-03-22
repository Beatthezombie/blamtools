//**************************************************************************/
// Copyright (c) 1998-2018 Autodesk, Inc.
// All rights reserved.
// 
// Use of this software is subject to the terms of the Autodesk license 
// agreement provided at the time of installation or download, or which 
// otherwise accompanies this software in either electronic or hard copy form.
//**************************************************************************/
// DESCRIPTION: Appwizard generated plugin
// AUTHOR: Beatthezombie
//***************************************************************************/

#include "bmf_import.h"

#include "blam_definitions.hpp"
#include "blam_model_file.hpp"

#include <ilayer.h>
#include <ilayermanager.h>
#include <stdmat.h>

#define bmf_import_CLASS_ID	Class_ID(0x7656939e, 0x6c3507ef)

class bmf_import : public SceneImport
{
public:
	//Constructor/Destructor
	bmf_import();
	virtual ~bmf_import();

	virtual int				ExtCount();					// Number of extensions supported
	virtual const TCHAR *	Ext(int n);					// Extension #n (i.e. "3DS")
	virtual const TCHAR *	LongDesc();					// Long ASCII description (i.e. "Autodesk 3D Studio File")
	virtual const TCHAR *	ShortDesc();				// Short ASCII description (i.e. "3D Studio")
	virtual const TCHAR *	AuthorName();				// ASCII Author name
	virtual const TCHAR *	CopyrightMessage();			// ASCII Copyright message
	virtual const TCHAR *	OtherMessage1();			// Other message #1
	virtual const TCHAR *	OtherMessage2();			// Other message #2
	virtual unsigned int	Version();					// Version number * 100 (i.e. v3.01 = 301)
	virtual void			ShowAbout(HWND hWnd);		// Show DLL's "About..." box
	virtual int				DoImport(const TCHAR *name,ImpInterface *i,Interface *gi, BOOL suppressPrompts=FALSE);	// Import file
};



class bmf_importClassDesc : public ClassDesc2 
{
public:
	virtual int           IsPublic() override                       { return TRUE; }
	virtual void*         Create(BOOL /*loading = FALSE*/) override { return new bmf_import(); }
	virtual const TCHAR * ClassName() override                      { return GetString(IDS_CLASS_NAME); }
	virtual SClass_ID     SuperClassID() override                   { return SCENE_IMPORT_CLASS_ID; }
	virtual Class_ID      ClassID() override                        { return bmf_import_CLASS_ID; }
	virtual const TCHAR*  Category() override                       { return GetString(IDS_CATEGORY); }

	virtual const TCHAR*  InternalName() override                   { return _T("bmf_import"); } // Returns fixed parsable name (scripter-visible name)
	virtual HINSTANCE     HInstance() override                      { return hInstance; } // Returns owning module handle


};

ClassDesc2* Getbmf_importDesc()
{
	static bmf_importClassDesc bmf_importDesc;
	return &bmf_importDesc; 
}




INT_PTR CALLBACK bmf_importOptionsDlgProc(HWND hWnd,UINT message,WPARAM ,LPARAM lParam) {
	static bmf_import* imp = nullptr;

	switch(message) {
		case WM_INITDIALOG:
			imp = (bmf_import *)lParam;
			CenterWindow(hWnd,GetParent(hWnd));
			return TRUE;

		case WM_CLOSE:
			EndDialog(hWnd, 0);
			return 1;
	}
	return 0;
}


//--- bmf_import -------------------------------------------------------
bmf_import::bmf_import()
{

}

bmf_import::~bmf_import() 
{

}

int bmf_import::ExtCount()
{
	return 1;
}

const TCHAR *bmf_import::Ext(int /*n*/)
{		
	return _T("bmf");
}

const TCHAR *bmf_import::LongDesc()
{
	return _T("Blam Model File");
}
	
const TCHAR *bmf_import::ShortDesc() 
{			
	return _T("BMF");
}

const TCHAR *bmf_import::AuthorName()
{			
	return _T("Beat the Zombie");
}

const TCHAR *bmf_import::CopyrightMessage() 
{	
	return _T("Copyright: None yet");
}

const TCHAR *bmf_import::OtherMessage1() 
{		
	return _T("");
}

const TCHAR *bmf_import::OtherMessage2() 
{		
	return _T("");
}

unsigned int bmf_import::Version()
{				
	return 10;
}

void bmf_import::ShowAbout(HWND /*hWnd*/)
{			
	// Optional
}

int bmf_import::DoImport(const TCHAR* filename, ImpInterface* importerInt, Interface* ip, BOOL suppressPrompts)
{
	if (!suppressPrompts)
		DialogBoxParam(hInstance,
			MAKEINTRESOURCE(IDD_PANEL),
			GetActiveWindow(),
			bmf_importOptionsDlgProc, (LPARAM)this);

	fstream fs;

	fs.open(filename, ios::in | ios::binary);
	blam_model_file bmf;
	if (fs.is_open())
		read_bmf(fs, &bmf);
	else
		return FALSE;


	bmf_node* bmf_nodes = new bmf_node[bmf.nodes.count];
	bmf_mesh* bmf_meshes = new bmf_mesh[bmf.meshes.count];
	bmf_region* bmf_regions = new bmf_region[bmf.regions.count];
	bmf_marker* bmf_markers = new bmf_marker[bmf.markers.count];
	bmf_material* bmf_materials = new bmf_material[bmf.materials.count];
	bmf_bounding_sphere* bmf_bounding_spheres = new bmf_bounding_sphere[bmf.depth_test_spheres.count];


	read_tag_block(fs, (char*)bmf_nodes, &bmf.nodes, sizeof(bmf_node));
	read_tag_block(fs, (char*)bmf_meshes, &bmf.meshes, sizeof(bmf_mesh));
	read_tag_block(fs, (char*)bmf_regions, &bmf.regions, sizeof(bmf_region));
	read_tag_block(fs, (char*)bmf_markers, &bmf.markers, sizeof(bmf_marker));
	read_tag_block(fs, (char*)bmf_materials, &bmf.materials, sizeof(bmf_material));
	read_tag_block(fs, (char*)bmf_bounding_spheres, &bmf.depth_test_spheres, sizeof(bmf_bounding_sphere));

	// parse regions, access layers using the manager by index + 1 (skip default layer)
	ILayerManager* manager = GetCOREInterface13()->GetLayerManager();
	for (int i = 0; i < bmf.regions.count; i++)
	{
		ILayer* layer = create_region(manager, &bmf_regions[i]);
	}

	// parse nodes
	ULONG node_handles[255];
	for (int i = 0; i < bmf.nodes.count; i++)
	{
		INode* parent = nullptr;
		if (bmf_nodes[i].parent_index != -1)
			parent = ip->GetINodeByHandle(node_handles[bmf_nodes[i].parent_index]);

		INode* cur = create_dummy_node(ip, &bmf_nodes[i], parent);
		node_handles[i] = cur->GetHandle();
	}
	
	// parse materials
	MultiMtl* multi_material = NewDefaultMultiMtl();
	wchar_t name[BMF_STRING_LENGTH];
	convert_char_to_wchar(bmf.name, (wchar_t*)&name, BMF_STRING_LENGTH);
	multi_material->SetName(name);

	for (int i = 0; i < bmf.materials.count; i++)
	{
		StdMat* material = NewDefaultStdMat();
		wchar_t name[BMF_STRING_LENGTH];
		convert_char_to_wchar(bmf_materials[i].name, (wchar_t*)&name, BMF_STRING_LENGTH);
		material->SetName(name);
		multi_material->SetSubMtl(i, material);
	}

	// parse meshes
	for (int i = 0; i < bmf.meshes.count; i++)
	{
		INode* mesh_node = create_mesh(fs, ip, &bmf_meshes[i], node_handles, bmf.nodes.count);
		ILayer* mesh_layer = manager->GetLayer(bmf_meshes[i].region_index + 1);
		mesh_layer->AddToLayer(mesh_node);
		mesh_node->SetMtl(multi_material);
	}

	// parse markers
	ILayer* marker_layer = manager->CreateLayer(_T("markers"));
	marker_layer->Hide(true);

	for (int i = 0; i < bmf.markers.count; i++)
	{
		INode* parent = nullptr;
		if (bmf_markers[i].node_index != -1)
		{
			parent = ip->GetINodeByHandle(node_handles[bmf_markers[i].node_index]);
		}
		INode* marker = create_marker(ip, &bmf_markers[i], parent);
		marker_layer->AddToLayer(marker);
	}

	// parse bounding spheres
	ILayer* depth_sort_layer = manager->CreateLayer(_T("depth_sort_spheres"));
	depth_sort_layer->Hide(true);

	for (int i = 0; i < bmf.depth_test_spheres.count; i++)
	{
		INode* parent = nullptr;
		if (bmf_bounding_spheres[i].node_indices[0] != -1)
		{
			parent = ip->GetINodeByHandle(node_handles[bmf_bounding_spheres[i].node_indices[0]]);
		}
		INode* depth_sphere = create_depth_sphere(ip, &bmf_bounding_spheres[i], parent, i);
		depth_sort_layer->AddToLayer(depth_sphere);
	}


	ip->RedrawViews(ip->GetTime());

	fs.close();
	delete[] bmf_nodes;
	delete[] bmf_meshes;
	delete[] bmf_regions;
	delete[] bmf_markers;
	delete[] bmf_materials;
	delete[] bmf_bounding_spheres;

	return TRUE;
}
	
