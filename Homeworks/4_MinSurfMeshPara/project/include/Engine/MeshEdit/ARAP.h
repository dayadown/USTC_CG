#pragma once
#include <Engine/MeshEdit/pre_Paramaterize.h>
namespace Ubpa {
	class TriMesh;
	class method_ARAP :
		public pre_Paramaterize
	{
	public:
		method_ARAP(Ptr<TriMesh> triMesh);
		~method_ARAP();
		bool Paramater();
	};
}