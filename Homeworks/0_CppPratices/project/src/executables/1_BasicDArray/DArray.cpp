// implementation of class DArray
#include "DArray.h"
#include<iostream>
using namespace std;
// default constructor
DArray::DArray() {
	Init();
}

// set an array with default values
DArray::DArray(int nSize, double dValue) {
	m_pData = new double[nSize];
	m_nSize = nSize;
	for (int i = 0; i < nSize; i++)
		m_pData[i] = dValue;
}

DArray::DArray(const DArray& arr) {
	m_nSize = arr.GetSize();
	m_pData = new double[m_nSize];
	for (int i = 0; i < m_nSize; i++)
		m_pData[i] = arr[i];
}

// deconstructor
DArray::~DArray() {
	Free();
}

// display the elements of the array
void DArray::Print() const {
	if (m_nSize == 0) {
		cout << "元素个数为0！" << endl;
		return;
	}
	for (int i = 0; i < m_nSize; i++)
		cout << m_pData[i] << " ";
	cout << endl;
}

// initilize the array
void DArray::Init() {
	m_pData = nullptr;
	m_nSize = 0;
}

// free the array
void DArray::Free() {
	delete[] m_pData;
	m_pData = nullptr;
	m_nSize = 0;
}

// get the size of the array
int DArray::GetSize() const {
	return m_nSize;
}

// set the size of the array
void DArray::SetSize(int nSize) {
	if (nSize <= m_nSize)
		m_nSize = nSize;
	else {
		double* arr = new double[nSize];
		for (int i = 0; i < m_nSize; i++)
			arr[i] = m_pData[i];
		for (int i = m_nSize; i < nSize; i++)
			arr[i] = 0.;
		delete[] m_pData;
		m_pData = arr;
		m_nSize = nSize;
	}
}

// get an element at an index
const double& DArray::GetAt(int nIndex) const {
	return m_pData[nIndex];
}

// set the value of an element 
void DArray::SetAt(int nIndex, double dValue) {
	m_pData[nIndex] = dValue;
}

// overload operator '[]'
const double& DArray::operator[](int nIndex) const {
	return m_pData[nIndex];
}

// add a new element at the end of the array
void DArray::PushBack(double dValue) {
	InsertAt(m_nSize, dValue);
}

// delete an element at some index
void DArray::DeleteAt(int nIndex) {
	for (int i = nIndex; i < m_nSize - 1; i++)
		m_pData[i] = m_pData[i + 1];
	m_nSize--;
}

// insert a new element at some index
void DArray::InsertAt(int nIndex, double dValue) {
	SetSize(m_nSize + 1);
	for (int i = m_nSize - 1; i > nIndex; i--)
		m_pData[i] = m_pData[i - 1];
	m_pData[nIndex] = dValue;
}

// overload operator '='
DArray& DArray::operator = (const DArray& arr) {
	m_nSize = arr.GetSize();
	m_pData = new double[m_nSize];
	for (int i = 0; i < m_nSize; i++)
		m_pData[i] = arr[i];
	return *this;
}
