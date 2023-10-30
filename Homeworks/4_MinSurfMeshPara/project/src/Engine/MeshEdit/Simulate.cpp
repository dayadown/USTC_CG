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
	//position�ڴ���ʱ���Ѿ����
	stiff = 1e3;

	int ne = edgelist.size();
	int nv = positions.size();
	int nvv = sqrt(nv);
	source_len0 = get_dist(positions[0], positions[nvv + 1]);//б�߳�
	source_len1 = get_dist(positions[0], positions[nvv]);//ֱ�߳�

	//����������
	edges.resize(nv);
	for (int i = 1; i < ne; i=i+2) {
		//��ʼ����ֻ������
		sum_force.push_back(pointf3(0, -10, 0));
		int a = edgelist[i];
		int b = edgelist[i - 1];
		int type = -1;
		if (abs(a - b) == 1 ) {
			if (a / nvv == b / nvv) {//ab��ͬһ��
				type = 1;
			}
			else {
				type = 0;
			}
		}
		else if (abs(a - b) == nvv) {
			type = 1;
		}
		else {//Ϊб��
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
	SimulateOnce();//����һ������



	return true;
}

void Ubpa::Simulate::SetLeftFix()
{
	//�̶�����x������С��
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
	//����ÿһ������
	int ne = edgelist.size();
	int nv = positions.size();
	int nvv = sqrt(nv);
	for (int i = 0; i < nv; i++) {
		if (i == nvv-1 || i == nv-1) continue;//���Թ̶���
		pointf3 v_now = positions[i];
		//ŷ������ʽ���ȼ������ٶȣ�����m=1�����ܵ���sum_force[i]��Ϊ���ٶ�
		velocity[i] = sum_f3(velocity[i], mult_f3(sum_force[i], h));
		//cout << i << "���ٶ�" << velocity[i]<<endl;

		//�ٸ����ٶȼ�����λ��
		positions[i] = sum_f3(positions[i], mult_f3(velocity[i], h));
		//cout << i << "��λ��" << positions[i]  << endl;
	}
	//��������i�ĵ��ɵĵ���������
	//���º���
	update_force();

	//cout << "--------------------------------------------------------------------------------" << endl;

}


//��������i�ĵ��ɵĵ���������,�����º���
void Simulate::update_force() {
	int nv = positions.size();
	int nvv = sqrt(nv);
	//�������н��
	for (int k = 0; k < nv; k++) {
		if (k == nvv - 1 || k == nv - 1) continue;//���Թ̶���

		//ÿ�����¼���
		sum_force[k] = pointf3(0, -10, 0);

		//���ᣬ���ٶȷ����෴����
		pointf3 damping = mult_f3(velocity[k], -1);
		damping = mult_f3(damping, 0.5);
		sum_force[k] = sum_f3(sum_force[k], damping);


		int adj_n = edges[k].size();
		pointf3 v_k = positions[k];//��ǰ��
		//���������ڽӵĵ���
		for (int i = 0; i < adj_n; i++) {

			int index_adj = edges[k][i].index;//�ڽӵ������
			pointf3 v_adj = positions[index_adj];//�ڽӵ㱾��

			float dist_k_adj = get_dist(v_k, v_adj);//�������
			float source_len = edges[k][i].type == 0 ? source_len0 : source_len1;//����ԭ��


			pointf3 direct;//�������ķ���
			float a = abs(dist_k_adj - source_len);
			if (a > 0.0001) {
				if (dist_k_adj > source_len) {//���ɱ�����,��������
					direct = dev_f3(sub_f3(v_adj, v_k), dist_k_adj);
				}
				if (dist_k_adj < source_len) {//���ɱ�ѹ��,��������
					direct = dev_f3(sub_f3(v_k, v_adj), dist_k_adj);
				}
			}
			else {
				direct = pointf3(0, 0, 0);
			}
			//�������Ĵ�С
			float force = 0;
			force = stiff * abs(a);
			//cout << k<<" "<<index_adj<<" "<<force << endl;

			//�ڲ�������
			pointf3 damping_direct(dev_f3(sub_f3(v_adj, v_k), dist_k_adj));
			pointf3 vel_dis(sub_f3(velocity[index_adj], velocity[k]));
			vel_dis = dev_f3(vel_dis, source_len);
			float damping_num = mult_f3_1(vel_dis, damping_direct) * 0.1;
			pointf3 damping_1(mult_f3(damping_direct, damping_num));

			sum_force[k] = sum_f3(sum_force[k], mult_f3(direct, force));//���ϵ���
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

float Simulate::mult_f3_1(pointf3 v1, pointf3 v2) {//���
	return v1.at(0) * v2.at(0) + v1.at(1) * v2.at(1) + v1.at(2) * v2.at(2);
}