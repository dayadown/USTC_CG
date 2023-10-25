#include <Engine/MeshEdit/pre_Paramaterize.h>
#include <Engine/Primitive/TriMesh.h>
#include <Engine/MeshEdit/ASAP.h>
using namespace Ubpa;
using namespace std;

pre_Paramaterize::pre_Paramaterize(Ptr<TriMesh> triMesh)
{
	heMesh = make_shared<HEMesh<V>>();
	Init(triMesh);
}

pre_Paramaterize::~pre_Paramaterize()
{
}

void pre_Paramaterize::Clear() {
	heMesh->Clear();
	triMesh = nullptr;
}

bool pre_Paramaterize::Init(Ptr<TriMesh> triMesh)
{
	Clear();

	if (triMesh == nullptr)
		return true;

	if (triMesh->GetType() == TriMesh::INVALID) {
		printf("ERROR::Paramaterize::Init:\n"
			"\t""trimesh is invalid\n");
		return false;
	}

	// init half-edge structure
	size_t nV = triMesh->GetPositions().size();
	vector<vector<size_t>> triangles;
	triangles.reserve(triMesh->GetTriangles().size());
	for (auto triangle : triMesh->GetTriangles())
		triangles.push_back({ triangle->idx[0], triangle->idx[1], triangle->idx[2] });
	heMesh->Reserve(nV);
	heMesh->Init(triangles);

	if (!heMesh->IsTriMesh() || !heMesh->HaveBoundary()) {
		printf("ERROR::Paramaterize::Init:\n"
			"\t""trimesh is not a triangle mesh or hasn't a boundaries\n");
		heMesh->Clear();
		return false;
	}

	// triangle mesh's positions ->  half-edge structure's positions
	for (int i = 0; i < nV; i++) {
		auto v = heMesh->Vertices().at(i);
		v->pos = triMesh->GetPositions()[i].cast_to<vecf3>();
	}

	this->triMesh = triMesh;
	return true;
}



bool pre_Paramaterize::Run() {
	return true;
}


double pre_Paramaterize::get_distance(V* p, V* q) {
	double dis = pow(p->pos.at(0) - q->pos.at(0), 2)
		+ pow(p->pos.at(1) - q->pos.at(1), 2)
		+ pow(p->pos.at(2) - q->pos.at(2), 2);
	return sqrt(dis);
}