#include <Engine/Primitive/TriMesh.h>
#include <Engine/MeshEdit/ASAP.h>
using namespace Ubpa;
using namespace std;
method_ASAP::method_ASAP(Ptr<TriMesh> triMesh)
	:pre_Paramaterize::pre_Paramaterize(triMesh) {

}
method_ASAP::~method_ASAP() {
	cl.clear();
}

bool method_ASAP::Paramater() {
	cout << "asap start" << "  ";
	size_t nv = heMesh->NumVertices();
	size_t np = heMesh->NumPolygons();
	
	//稀疏矩阵三元组
	vector<Eigen::Triplet<double>> tr_T;

	size_t num = 0;
	//依次遍历空间三角的每个顶点填充稀疏矩阵
	
	for (size_t i = 0; i < nv; i++) {
		V* vi = heMesh->Vertices()[i];
		size_t adj_num = vi->AdjVertices().size();
		//依次遍历每个角
		for (size_t j = 0; j < adj_num; j++) {
			size_t k = (j + 1) == adj_num ? 0 : j + 1;
			V* vj = vi->AdjVertices()[j];
			V* vk = vi->AdjVertices()[k];

			vecf3 vii = vi->pos;
			vecf3 vjj = vj->pos;
			vecf3 vkk = vk->pos;

			size_t index_i = heMesh->Index(vi);
			size_t index_j = heMesh->Index(vj);
			size_t index_k = heMesh->Index(vk);

			double r = get_distance(vk, vi) / get_distance(vj, vi);
			double cos_jik = vecf3::cos_theta(vjj - vii, vkk - vii);
			double sin_jik = sqrt(1 - cos_jik * cos_jik);
			//填充三元组
			tr_T.push_back(Eigen::Triplet<double>(num, index_i * 2, r * cos_jik - 1));
			tr_T.push_back(Eigen::Triplet<double>(num, index_i * 2 + 1, -r * sin_jik));
			tr_T.push_back(Eigen::Triplet<double>(num, index_j * 2, -r * cos_jik));
			tr_T.push_back(Eigen::Triplet<double>(num, index_j * 2 + 1, r * sin_jik));
			tr_T.push_back(Eigen::Triplet<double>(num, index_k * 2, 1));
			tr_T.push_back(Eigen::Triplet<double>(num, index_k * 2 + 1, 0));

			num++;

			tr_T.push_back(Eigen::Triplet<double>(num, index_i * 2, r * sin_jik));
			tr_T.push_back(Eigen::Triplet<double>(num, index_i * 2 + 1, r * cos_jik - 1));
			tr_T.push_back(Eigen::Triplet<double>(num, index_j * 2, -r * sin_jik));
			tr_T.push_back(Eigen::Triplet<double>(num, index_j * 2 + 1, -r * cos_jik));
			tr_T.push_back(Eigen::Triplet<double>(num, index_k * 2, 0));
			tr_T.push_back(Eigen::Triplet<double>(num, index_k * 2 + 1, 1));

			num++;

		}
	}

	//稀疏矩阵T
	Eigen::SparseMatrix<double> T;
	T.resize(num + 4, 2 * nv);
	T.setZero();

	//列向量
	Eigen::VectorXd b(num + 4);
	b.setZero();

	//固定两个点
	V* fixed_V1 = heMesh->Vertices()[0];
	V* fixed_V2 = heMesh->Vertices()[0]->AdjVertices()[0];
	size_t index_V1 = heMesh->Index(fixed_V1);
	cout << index_V1<<endl;
	size_t index_V2 = heMesh->Index(fixed_V2);
	tr_T.push_back(Eigen::Triplet<double>(num, index_V1 * 2, 1));
	b(num) = fixed_V1->pos.at(0);

	num++;

	tr_T.push_back(Eigen::Triplet<double>(num, index_V1 * 2 + 1, 1));
	b(num) = fixed_V1->pos.at(1);

	num++;

	tr_T.push_back(Eigen::Triplet<double>(num, index_V2 * 2, 1));
	b(num) = fixed_V2->pos.at(0);;

	num++;

	tr_T.push_back(Eigen::Triplet<double>(num, index_V2 * 2 + 1, 1));
	b(num) = fixed_V2->pos.at(1);


	//填充T
	T.setFromTriplets(tr_T.begin(), tr_T.end());

	//最小二乘解
	Eigen::JacobiSVD<Eigen::MatrixXd> svd(T, Eigen::ComputeThinU | Eigen::ComputeThinV);
	Eigen::VectorXd ans = svd.solve(b);
	
	for (size_t i = 0; i < nv; i++)
	{
		heMesh->Vertices()[i]->pos.at(0) = ans(i * 2);
		heMesh->Vertices()[i]->pos.at(1) = ans(i * 2 + 1);
		heMesh->Vertices()[i]->pos.at(2) = 0;
		cl.push_back(pointf2(ans[i * 2], ans[i * 2 + 1]));
	}


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
	//更新3D网格
	triMesh->Init(indice, positions);

	//只更新纹理坐标
	//triMesh->Update(cl);

	return true;
}