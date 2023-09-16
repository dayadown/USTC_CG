#include "PolynomialList.h"
#include<fstream>
#include<iostream>
using namespace std;

PolynomialList::PolynomialList(const PolynomialList& other) {
    // TODO
}

PolynomialList::PolynomialList(const string& file) {
    ReadFromFile(file);
}

PolynomialList::PolynomialList(const double* cof, const int* deg, int n) {
    // TODO
}

PolynomialList::PolynomialList(const vector<int>& deg, const vector<double>& cof) {
    // TODO
}

double PolynomialList::coff(int i) const {
    // TODO
    return 0.; // you should return a correct value
}

double& PolynomialList::coff(int i) {
    // TODO
    static double ERROR; // you should delete this line
    return ERROR; // you should return a correct value
}

void PolynomialList::compress() {
    // TODO
}

PolynomialList PolynomialList::operator+(const PolynomialList& right) const {
    // TODO
    return {}; // you should return a correct value
}

PolynomialList PolynomialList::operator-(const PolynomialList& right) const {
    // TODO
    return {}; // you should return a correct value
}

PolynomialList PolynomialList::operator*(const PolynomialList& right) const {
    // TODO
    return {}; // you should return a correct value
}

PolynomialList& PolynomialList::operator=(const PolynomialList& right) {
    // TODO
    return *this;
}

void PolynomialList::Print() const {
    ///*
    if (m_Polynomial.empty()) {
        cout << "empty list!" << endl;
        return;
    }
    auto it = m_Polynomial.begin();
    cout << it->cof << "x^" << it->deg;
    it++;
    while (it != m_Polynomial.end()) {
        if (it->cof > 0)
            cout << "+";
        cout << it->cof << "x^" << it->deg;
        it++;
    }
    cout << endl;
}

bool PolynomialList::ReadFromFile(const string& file) {
    fstream ifs;
    ifs.open(file, ios::in);
    if (!ifs.is_open()) {
        cout << "file open error!" << endl;
        return false;
    }
    char temp;
    int nSize = 0;
    int deg = 0;
    double cof = 0;
    ifs >> temp;
    ifs >> nSize;
    for (int i = 0; i < nSize; i++) {
        ifs >> deg;
        ifs >> cof;
        AddOneTerm(Term(deg, cof));
    }
    ifs.close();
    return true;
}

void PolynomialList::AddOneTerm(const Term& term) {
    m_Polynomial.push_back(term);
}