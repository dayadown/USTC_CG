#include "PolynomialList.h"
#include<fstream>
#include<iostream>
using namespace std;
#define EPSILON 1.0e-10
PolynomialList::PolynomialList(const PolynomialList& other) {
    auto it = other.m_Polynomial.begin();
    for (; it != other.m_Polynomial.end(); it++) {
        AddOneTerm(Term(it->deg, it->cof));
    }
}

PolynomialList::PolynomialList(const string& file) {
    ReadFromFile(file);
}

PolynomialList::PolynomialList(const double* cof, const int* deg, int n) {
    for (int i = 0; i < n; i++) {
        AddOneTerm(Term(*deg, *cof));
    }
}

PolynomialList::PolynomialList(const vector<int>& deg, const vector<double>& cof) {
    for (int i = 0; i < deg.size(); i++) {
        AddOneTerm(Term(deg[i], cof[i]));
    }
}

double PolynomialList::coff(int i) const {
    auto it = m_Polynomial.begin();
    while (it != m_Polynomial.end()) {
        if (it->deg == i) return it->cof;
    }
    return 0.;
}

double& PolynomialList::coff(int i) {
    return AddOneTerm(Term(i, 0)).cof;
}

void PolynomialList::compress() {
    auto itr = m_Polynomial.begin();
    while (itr != m_Polynomial.end()) {
        if (fabs((*itr).cof) < EPSILON)
            itr = m_Polynomial.erase(itr);
        else
            itr++;
    }
}

PolynomialList PolynomialList::operator+(const PolynomialList& right) const {
    PolynomialList poly(*this);
    for (auto term : right.m_Polynomial)
        poly.AddOneTerm(term);
    return poly;

}

PolynomialList PolynomialList::operator-(const PolynomialList& right) const {
    PolynomialList poly(*this);
    for (auto term : right.m_Polynomial)
        poly.AddOneTerm(Term(term.deg, -term.cof));
    return poly;
}

PolynomialList PolynomialList::operator*(const PolynomialList& right) const {
    PolynomialList poly;
    for (auto term1 : m_Polynomial) {
        for (auto term2 : right.m_Polynomial) {
            poly.AddOneTerm(Term(term1.deg * term2.deg, term1.cof * term2.cof));
        }
    }
    return poly;
}

PolynomialList& PolynomialList::operator=(const PolynomialList& right) {
    for (auto term : right.m_Polynomial) {
        AddOneTerm(term);
    }
    return *this;
}

void PolynomialList::Print() const {
    if (m_Polynomial.empty()) {
        cout << "empty list!" << endl;
        return;
    }
    auto it = m_Polynomial.begin();
    while(it->cof == 0&&it!=m_Polynomial.end())
        it++;
    if (it == m_Polynomial.end())
        return;
    else {
        if (it->deg == 0)
            cout << it->cof;
        else
            cout << it->cof << "x^" << it->deg;
        it++;
    }
    while (it != m_Polynomial.end()) {
        if (it->cof != 0) {
            if (it->cof > 0)
                cout << "+";
            if (it->deg == 0)
                cout << it->cof;
            else
                cout << it->cof << "x^" << it->deg;
        }
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

PolynomialList::Term& PolynomialList::AddOneTerm(const Term& term) {
    auto it = m_Polynomial.begin();
    for (; it != m_Polynomial.end(); it++) {
        if (it->deg == term.deg) {
            it->cof += term.cof;
            return *it;
        }
        if (term.deg > it->deg)
            break;
    }
    return *m_Polynomial.insert(it,term);
}