#include <Engine/MeshEdit/Paramaterize.h>

#include <Engine/MeshEdit/MinSurf.h>

#include <Engine/Primitive/TriMesh.h>

using namespace Ubpa;

using namespace std;

Paramaterize::Paramaterize(Ptr<TriMesh> triMesh)
	: heMesh(make_shared<HEMesh<V>>())
{
	Init(triMesh);
}

void Paramaterize::Clear() {
	heMesh->Clear();
	triMesh = nullptr;
}

bool Paramaterize::Init(Ptr<TriMesh> triMesh) {
	Clear();

	if (triMesh == nullptr)
		return true;

	if (triMesh->GetType() == TriMesh::INVALID) {
		printf("ERROR::MinSurf::Init:\n"
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
		printf("ERROR::MinSurf::Init:\n"
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

bool Paramaterize::Run() {
	if (heMesh->IsEmpty() || !triMesh) {
		printf("ERROR::MinSurf::Run\n"
			"\t""heMesh->IsEmpty() || !triMesh\n");
		return false;
	}

	Paramater();

	// half-edge structure -> triangle mesh
	size_t nV = heMesh->NumVertices();
	size_t nF = heMesh->NumPolygons();
	vector<pointf3> positions;
	vector<unsigned> indice;
	positions.reserve(nV);
	indice.reserve(3 * nF);
	for (auto v : heMesh->Vertices())
		positions.push_back(v->pos.cast_to<pointf3>());
	for (auto f : heMesh->Polygons()) { // f is triangle
		for (auto v : f->BoundaryVertice()) // vertices of the triangle
			indice.push_back(static_cast<unsigned>(heMesh->Index(v)));
	}

	triMesh->Init(indice, positions);

	return true;
}

void Paramaterize::Paramater(){
	fix_Bound();
	Minimize();
}

//固定边界
void Paramaterize::fix_Bound() {
	size_t n = heMesh->Boundaries()[0].size();
	double sum_Bound = 0;
	vector<int> bound_index;
	for (size_t i = 0; i < n; i++) {
		auto v_now = heMesh->Boundaries()[0][i];
		auto p = v_now->Origin();
		auto q = v_now->End();
		sum_Bound += get_distance(p, q);
	}
	double now_sum = 0;
	for (size_t i = 0; i < n; i++) {
		int k = 1;
		auto v_now = heMesh->Boundaries()[0][i];
		auto p = v_now->Origin();
		auto q = v_now->End();
		int index = heMesh->Index(p);
		double weigt = now_sum / sum_Bound * 4 * k;
		now_sum += get_distance(p, q);
		if (weigt >= 0 && weigt < k) {
			heMesh->Vertices()[index]->pos.at(0) = weigt;
			heMesh->Vertices()[index]->pos.at(1) = k;
			heMesh->Vertices()[index]->pos.at(2) = -2;
		}
		else if (weigt >= k && weigt < 2*k) {
			heMesh->Vertices()[index]->pos.at(0) = k;
			heMesh->Vertices()[index]->pos.at(1) = 2*k-weigt;
			heMesh->Vertices()[index]->pos.at(2) = -2;
		}
		else if (weigt >= 2*k && weigt < 3*k) {
			heMesh->Vertices()[index]->pos.at(0) = 3*k-weigt;
			heMesh->Vertices()[index]->pos.at(1) = 0;
			heMesh->Vertices()[index]->pos.at(2) = -2;
		}
		else {
			heMesh->Vertices()[index]->pos.at(0) = 0;
			heMesh->Vertices()[index]->pos.at(1) = weigt-3*k;
			heMesh->Vertices()[index]->pos.at(2) = -2;
		}
	}
}


void Paramaterize::Minimize() {
	//结点数
	size_t v_num = heMesh->NumVertices();

	//创建稀疏矩阵三元组
	vector<Eigen::Triplet<double>> tr_T;

	//创建待解列向量
	Eigen::VectorXd b_x(v_num);
	Eigen::VectorXd b_y(v_num);
	Eigen::VectorXd b_z(v_num);
	b_x.setZero();
	b_y.setZero();
	b_z.setZero();


	//填充T
	for (size_t i = 0; i < v_num; i++) {
		auto now_v = heMesh->Vertices()[i];
		if (!now_v->IsBoundary()) {
			size_t adj_v_num = now_v->AdjVertices().size();
			tr_T.push_back(Eigen::Triplet<double>(i, i, adj_v_num));
			for (size_t j = 0; j < adj_v_num; j++) {
				auto adj_v = now_v->AdjVertices()[j];
				if (!adj_v->IsBoundary()) {
					tr_T.push_back(Eigen::Triplet<double>(i, heMesh->Index(adj_v), -1));
				}
				else {
					//cout << adj_v->pos.at(2);
					b_x(i) += adj_v->pos.at(0);
					b_y(i) += adj_v->pos.at(1);
					b_z(i) += adj_v->pos.at(2);
				}
			}
		}
		else {
			//cout << now_v->pos.at(2);
			tr_T.push_back(Eigen::Triplet<double>(i, i, 1));
			b_x(i) = now_v->pos.at(0);
			b_y(i) = now_v->pos.at(1);
			b_z(i) = now_v->pos.at(2);
		}
	}

	//LU分解
	Eigen::SparseMatrix<double> T;
	T.resize(v_num, v_num);
	Eigen::SparseLU<Eigen::SparseMatrix<double>> solver;

	//填充T矩阵
	T.setFromTriplets(tr_T.begin(), tr_T.end());

	solver.compute(T);

	Eigen::VectorXd x(v_num);
	Eigen::VectorXd y(v_num);
	Eigen::VectorXd z(v_num);

	x = solver.solve(b_x);
	y = solver.solve(b_y);
	z = solver.solve(b_z);


	for (size_t i = 0; i < v_num; i++)
	{
		heMesh->Vertices()[i]->pos.at(0) = x(i);
		heMesh->Vertices()[i]->pos.at(1) = y(i);
		heMesh->Vertices()[i]->pos.at(2) = z(i);
	}
}


double Paramaterize::get_distance(V* p, V* q) {
	double dis = pow(p->pos.at(0) - q->pos.at(0), 2)
		+ pow(p->pos.at(1) - q->pos.at(1), 2)
		+ pow(p->pos.at(2) - q->pos.at(2), 2);
	return sqrt(dis);
}