#pragma once
#include <Engine/MeshEdit/pre_Paramaterize.h>
namespace Ubpa {
	class TriMesh;
	class method_ASAP :
		public pre_Paramaterize
	{
	public:
		method_ASAP(Ptr<TriMesh> triMesh);
		~method_ASAP();
		bool Paramater();
	};
}