#pragma once

#include <Basic/HeapObj.h>
//#include <Engine/Primitive/MassSpring.h>
#include <UGM/UGM>
#include<Eigen/Sparse>
#include<Eigen/Dense>

namespace Ubpa {
	typedef struct adj_edge {
		adj_edge(int a, int b) {
			index = a;
			type = b;
		}
		int type;//0代表斜边，1代表直边
		int index;
	}adj_edge;
	class Simulate : public HeapObj {
	public:
		Simulate(const std::vector<pointf3>& plist,
			const std::vector<unsigned>& elist) {
			edgelist = elist;
			this->positions.resize(plist.size());
			for (int i = 0; i < plist.size(); i++)
			{
				for (int j = 0; j < 3; j++)
				{
					this->positions[i][j] = plist[i][j];
				}
			}
		};
	public:
		static const Ptr<Simulate> New(const std::vector<pointf3>& plist,
			const std::vector<unsigned>& elist) {
			return Ubpa::New<Simulate>(plist, elist);
		}
	public:
		// clear cache data
		void Clear();

		// init cache data (eg. half-edge structure) for Run()
		bool Init();
		//bool Init();

		// call it after Init()
		bool Run();

		const std::vector<pointf3>& GetPositions() const { return positions; };

		const float GetStiff() { return stiff; };
		void SetStiff(float k) { stiff = k; Init(); };
		const float GetTimeStep() { return h; };
		void SetTimeStep(float k) { h = k; Init(); };
		std::vector<unsigned>& GetFix() { return this->fixed_id; };
		void SetFix(const std::vector<unsigned>& f) { this->fixed_id = f; Init(); };
		const std::vector<pointf3>& GetVelocity() { return velocity; };
		//void SetVelocity(const std::vector<pointf3>& v) { velocity = v; };

		void SetLeftFix();


	private:
		// kernel part of the algorithm
		void SimulateOnce();

	private:
		float h = 0.02f;  //步长
		float stiff;
		float source_len0;//原长斜边
		float source_len1;//原长直边
		std::vector<unsigned> fixed_id;  //fixed point id


		//mesh data
		std::vector<unsigned> edgelist;


		//simulation data
		std::vector<pointf3> positions;
		std::vector<pointf3> velocity;


		std::vector<std::vector<adj_edge> > edges;

		std::vector<pointf3> sum_force;


		float mult_f3_1(pointf3 v1, pointf3 v2);
		pointf3 mult_f3(pointf3, float);
		pointf3 dev_f3(pointf3, float);
		pointf3 sum_f3(pointf3, pointf3);
		pointf3 sub_f3(pointf3, pointf3);
		float get_dist(pointf3, pointf3);
		void update_force();
	};
}
