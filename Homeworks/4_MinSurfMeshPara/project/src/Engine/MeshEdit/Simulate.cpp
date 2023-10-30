#include <Engine/MeshEdit/Simulate.h>


#include <Eigen/Sparse>

using namespace Ubpa;
using namespace std;
using namespace Eigen;


void Simulate::Clear() {
	this->positions.clear();
	this->velocity.clear();
}

bool Simulate::Init() {
	//position在创建时就已经填充
	stiff = 1e3;

	int ne = edgelist.size();
	int nv = positions.size();
	int nvv = sqrt(nv);
	source_len0 = get_dist(positions[0], positions[nvv + 1]);//斜边长
	source_len1 = get_dist(positions[0], positions[nvv]);//直边长

	//创建边索引
	edges.resize(nv);
	for (int i = 1; i < ne; i=i+2) {
		//初始合力只有重力
		sum_force.push_back(pointf3(0, -10, 0));
		int a = edgelist[i];
		int b = edgelist[i - 1];
		int type = -1;
		if (abs(a - b) == 1 ) {
			if (a / nvv == b / nvv) {//ab在同一层
				type = 1;
			}
			else {
				type = 0;
			}
		}
		else if (abs(a - b) == nvv) {
			type = 1;
		}
		else {//为斜边
			type = 0;
		}
		edges[a].push_back(adj_edge(b, type));
		edges[b].push_back(adj_edge(a, type));
	}

	this->velocity.resize(positions.size());
	for (int i = 0; i < positions.size(); i++)
	{
		for (int j = 0; j < 3; j++)
		{
			this->velocity[i][j] = 0;
		}
	}
	return true;
}

bool Simulate::Run() {
	SimulateOnce();//进行一个步长



	return true;
}

void Ubpa::Simulate::SetLeftFix()
{
	//固定网格x坐标最小点
	fixed_id.clear();
	double x = 100000;
	for (int i = 0; i < positions.size(); i++)
	{
		if (positions[i][0] < x)
		{
			x = positions[i][0];
		}
	}

	for (int i = 0; i < positions.size(); i++)
	{
		if (abs(positions[i][0] - x) < 1e-5)
		{
			fixed_id.push_back(i);
		}
	}

	Init();
}

void Simulate::SimulateOnce() {
	// TODO
	//cout << "WARNING::Simulate::SimulateOnce:" << endl;
//		<< "\t" << "not implemented" << endl;
	//遍历每一个顶点
	int ne = edgelist.size();
	int nv = positions.size();
	int nvv = sqrt(nv);
	for (int i = 0; i < nv; i++) {
		if (i == nvv-1 || i == nv-1) continue;//测试固定点
		pointf3 v_now = positions[i];
		//欧拉半隐式，先计算新速度，质量m=1，所受的力sum_force[i]即为加速度
		velocity[i] = sum_f3(velocity[i], mult_f3(sum_force[i], h));
		//cout << i << "的速度" << velocity[i]<<endl;

		//再根据速度计算新位置
		positions[i] = sum_f3(positions[i], mult_f3(velocity[i], h));
		//cout << i << "的位置" << positions[i]  << endl;
	}
	//更新连接i的弹簧的弹力和阻尼
	//更新合力
	update_force();

	//cout << "--------------------------------------------------------------------------------" << endl;

}


//更新连接i的弹簧的弹力和阻尼,并更新合力
void Simulate::update_force() {
	int nv = positions.size();
	int nvv = sqrt(nv);
	//遍历所有结点
	for (int k = 0; k < nv; k++) {
		if (k == nvv - 1 || k == nv - 1) continue;//测试固定点

		//每次重新计算
		sum_force[k] = pointf3(0, -10, 0);

		//阻尼，与速度方向相反的力
		pointf3 damping = mult_f3(velocity[k], -1);
		damping = mult_f3(damping, 0.5);
		sum_force[k] = sum_f3(sum_force[k], damping);


		int adj_n = edges[k].size();
		pointf3 v_k = positions[k];//当前点
		//遍历整个邻接的弹簧
		for (int i = 0; i < adj_n; i++) {

			int index_adj = edges[k][i].index;//邻接点的索引
			pointf3 v_adj = positions[index_adj];//邻接点本点

			float dist_k_adj = get_dist(v_k, v_adj);//两点距离
			float source_len = edges[k][i].type == 0 ? source_len0 : source_len1;//两点原长


			pointf3 direct;//弹簧力的方向
			float a = abs(dist_k_adj - source_len);
			if (a > 0.0001) {
				if (dist_k_adj > source_len) {//弹簧被拉伸,合力向内
					direct = dev_f3(sub_f3(v_adj, v_k), dist_k_adj);
				}
				if (dist_k_adj < source_len) {//弹簧被压缩,合力向外
					direct = dev_f3(sub_f3(v_k, v_adj), dist_k_adj);
				}
			}
			else {
				direct = pointf3(0, 0, 0);
			}
			//弹簧力的大小
			float force = 0;
			force = stiff * abs(a);
			//cout << k<<" "<<index_adj<<" "<<force << endl;

			//内部抑制力
			pointf3 damping_direct(dev_f3(sub_f3(v_adj, v_k), dist_k_adj));
			pointf3 vel_dis(sub_f3(velocity[index_adj], velocity[k]));
			vel_dis = dev_f3(vel_dis, source_len);
			float damping_num = mult_f3_1(vel_dis, damping_direct) * 0.1;
			pointf3 damping_1(mult_f3(damping_direct, damping_num));

			sum_force[k] = sum_f3(sum_force[k], mult_f3(direct, force));//加上弹力
			sum_force[k] = sum_f3(sum_force[k], damping_1);
		}
	}
}

float Simulate::get_dist(pointf3 v1, pointf3 v2) {
	float v1x = v1.at(0);
	float v1y = v1.at(1);
	float v1z = v1.at(2);
	float v2x = v2.at(0);
	float v2y = v2.at(1);
	float v2z = v2.at(2);
	return sqrt((v1x - v2x) * (v1x - v2x) + (v1y - v2y) * (v1y - v2y) + (v1z - v2z) * (v1z - v2z));
}

pointf3 Simulate::dev_f3(pointf3 v, float k) {
	pointf3 new_p;
	new_p.at(0) = v.at(0) / k;
	new_p.at(1) = v.at(1) / k;
	new_p.at(2) = v.at(2) / k;
	return new_p;
}

pointf3 Simulate::mult_f3(pointf3 v, float k) {
	pointf3 new_p;
	new_p.at(0) = v.at(0) * k;
	new_p.at(1) = v.at(1) * k;
	new_p.at(2) = v.at(2) * k;
	return new_p;
}

pointf3 Simulate::sum_f3(pointf3 v1, pointf3 v2) {
	pointf3 new_p;
	new_p.at(0) = v1.at(0) + v2.at(0);
	new_p.at(1) = v1.at(1) + v2.at(1);
	new_p.at(2) = v1.at(2) + v2.at(2);
	return new_p;
}

pointf3 Simulate::sub_f3(pointf3 v1, pointf3 v2) {
	pointf3 new_p;
	new_p.at(0) = v1.at(0) - v2.at(0);
	new_p.at(1) = v1.at(1) - v2.at(1);
	new_p.at(2) = v1.at(2) - v2.at(2);
	return new_p;
}

float Simulate::mult_f3_1(pointf3 v1, pointf3 v2) {//点积
	return v1.at(0) * v2.at(0) + v1.at(1) * v2.at(1) + v1.at(2) * v2.at(2);
}