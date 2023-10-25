#include <Engine/Primitive/TriMesh.h>
#include <Engine/MeshEdit/ARAP.h>
using namespace Ubpa;
using namespace std;
method_ARAP::method_ARAP(Ptr<TriMesh> triMesh)
	:pre_Paramaterize::pre_Paramaterize(triMesh) {

}
method_ARAP::~method_ARAP() {
	cl.clear();
}

bool method_ARAP::Paramater() {
	cout << "arap run" << endl;
	return true;
}