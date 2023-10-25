#pragma once
#include <UHEMesh/HEMesh.h>
#include <UGM/UGM>
#include <Basic/HeapObj.h>
#include<Eigen/Sparse>
#include<Eigen/Dense>


//ASAP和ARAP的前置操作类，即加载3D网格数据结构
namespace Ubpa {
	class TriMesh;

	class pre_Paramaterize
	{
	public:
		pre_Paramaterize(Ptr<TriMesh> triMesh);
		virtual ~pre_Paramaterize();

	public:
		void Clear();
		bool Init(Ptr<TriMesh> triMesh);
		bool Run();
		virtual bool Paramater()=0;
	public:
		enum Method
		{
			ASAP,
			ARAP
		};

	protected:
		class V;
		class E;
		class P;
		class V : public TVertex<V, E, P> {
		public:
			vecf3 pos;
		};
		class E : public TEdge<V, E, P> { };
		class P :public TPolygon<V, E, P> { };

	public:
		Ptr<TriMesh> triMesh;
		Ptr<HEMesh<V>> heMesh;

		double get_distance(V*, V*);
		std::vector<pointf2> cl;
	};
}