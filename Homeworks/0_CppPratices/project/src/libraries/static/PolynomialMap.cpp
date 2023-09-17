#include "PolynomialMap.h"

#include <iostream>
#include <fstream>
#include <cassert>
#include <cmath>

using namespace std;

PolynomialMap::PolynomialMap(const PolynomialMap& other) {
	for (auto it = other.m_Polynomial.begin(); it != other.m_Polynomial.end(); it++) {
		m_Polynomial.emplace(*it);
	}
}

PolynomialMap::PolynomialMap(const string& file) {
    ReadFromFile(file);
}

PolynomialMap::PolynomialMap(const double* cof, const int* deg, int n) {
	// TODO
}

PolynomialMap::PolynomialMap(const vector<int>& deg, const vector<double>& cof) {
	assert(deg.size() == cof.size());
	int nSzie = deg.size();
	for (int i = 0; i < nSzie; i++) {
		auto it = m_Polynomial.find(deg[i]);
		if (it != m_Polynomial.end())
			it->second += cof[i];
		else
			m_Polynomial.emplace(pair<int, double>(deg[i], cof[i]));
	}
}

double PolynomialMap::coff(int i) const {
	auto target = m_Polynomial.find(i);
	if (target == m_Polynomial.end())
		return 0.;

	return target->second;
}

double& PolynomialMap::coff(int i) {
	return m_Polynomial[i];
}

void PolynomialMap::compress() {
	// TODO
}

PolynomialMap PolynomialMap::operator+(const PolynomialMap& right) const {
	PolynomialMap poly(*this);
	for (pair<int, double> term : right.m_Polynomial) {
		int deg = term.first;
		int cof = term.second;
		auto it = poly.m_Polynomial.find(deg);
		if (it != poly.m_Polynomial.end())
			it->second += cof;
		else
			poly.m_Polynomial.emplace(pair<int, double>(deg, cof));
	}
	return poly;
}

PolynomialMap PolynomialMap::operator-(const PolynomialMap& right) const {
	PolynomialMap poly(*this);
	for (pair<int, double> term : right.m_Polynomial) {
		int deg = term.first;
		int cof = term.second;
		auto it = poly.m_Polynomial.find(deg);
		if (it != poly.m_Polynomial.end())
			it->second -= cof;
		else
			poly.m_Polynomial.emplace(pair<int, double>(deg, -cof));
	}
	return poly;
}

PolynomialMap PolynomialMap::operator*(const PolynomialMap& right) const {
	PolynomialMap poly;
	for (auto term1 : m_Polynomial) {
		for (auto term2 : right.m_Polynomial) {
			int deg = term1.first * term2.first;
			double cof = term1.second * term2.second;
			auto it = poly.m_Polynomial.find(deg);
			if (it != poly.m_Polynomial.end())
				it->second += cof;
			else
				poly.m_Polynomial.emplace(pair<int, double>(deg, cof));
		}
	}
	return poly;
}

PolynomialMap& PolynomialMap::operator=(const PolynomialMap& right) {
	for (auto term : right.m_Polynomial) {
		m_Polynomial.emplace(term);
	}
	return *this;
}

void PolynomialMap::Print() const {
	auto it = m_Polynomial.begin();
	while (it->second == 0 && it != m_Polynomial.end())
		it++;
	if (it == m_Polynomial.end())
		return;
	else {
		if (it->first == 0)
			cout << it->second;
		else
			cout << it->second << "x^" << it->first;
	}
	while (it != m_Polynomial.end()) {
		if (it->second > 0)
			cout << "+";
		if(it->first!=0)
			cout << it->second << "x^" << it->first;
		it++;
	}
	cout << endl;

}

bool PolynomialMap::ReadFromFile(const string& file) {
    m_Polynomial.clear();
	fstream fs;
	fs.open(file, ios::in);
	if (!fs.is_open()) {
		cout << "file open error!" << endl;
		return false;
	}
	int nSize = 0;
	char temp;
	fs >> temp;
	fs >> nSize;
	int deg = 0;
	double cof = 0.;
	for (int i = 0; i < nSize; i++) {
		fs >> deg;
		fs >> cof;
		m_Polynomial.emplace(pair<int, double>(deg, cof));
	}
	return true;
}

