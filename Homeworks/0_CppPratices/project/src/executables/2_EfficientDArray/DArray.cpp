// implementation of class DArray
#include "DArray.h"
#include<iostream>
using namespace std;
// default constructor
DArray::DArray() {
	Init();
}

// set an array with default values
DArray::DArray(int nSize, double dValue)
	:m_pData(new double[nSize]), m_nMax(nSize),m_nSize(nSize)
{
	for (int i = 0; i < nSize; i++)
		m_pData[i] = dValue;
}

DArray::DArray(const DArray& arr)
	:m_nMax(arr.m_nMax), m_nSize(arr.m_nSize), m_pData(new double[arr.m_nMax])
{
	for (int i = 0; i < m_nSize; i++)
		m_pData[i] = arr[i];
}

// deconstructor
DArray::~DArray() {
	Free();
}

// display the elements of the array
void DArray::Print() const {
	if (m_nSize == 0)
		cout << "no element";
	for (int i = 0; i < m_nSize; i++)
		cout << m_pData[i] << " ";
	cout << endl;
}

// initilize the array
void DArray::Init() {
	m_nMax = m_nSize = 0;
	m_pData = nullptr;
}

// free the array
void DArray::Free() {
	m_nMax = m_nSize = 0;
	delete[] m_pData;
	m_pData = nullptr;
}

// get the size of the array
int DArray::GetSize() const {
	return m_nMax;
}

// set the size of the array
void DArray::SetSize(int nSize) {
	if (nSize == m_nMax)
		return;
	double* pdata = new double[nSize];
	for (int i = 0; i < nSize && i < m_nSize; i++)
		pdata[i] = m_pData[i];
	if (nSize > m_nSize)
		m_nMax = nSize;
	else
		m_nMax = m_nSize;
	delete[] m_pData;
	m_pData = pdata;
}

// get an element at an index
const double& DArray::GetAt(int nIndex) const {
	return m_pData[nIndex];
}

// set the value of an element 
void DArray::SetAt(int nIndex, double dValue) {
	if (nIndex < m_nMax)
		m_pData[nIndex] = dValue;
	else {
		SetSize(m_nMax * 2);
		m_pData[nIndex] = dValue;
	}
}

// overload operator '[]'
double& DArray::operator[](int nIndex) {
	return m_pData[nIndex];
}

// overload operator '[]'
const double& DArray::operator[](int nIndex) const {
	return m_pData[nIndex];
}

// add a new element at the end of the array
void DArray::PushBack(double dValue) {
	if (m_nMax > m_nSize)
		m_pData[m_nSize++] = dValue;
	else {
		SetSize((m_nSize+1) * 2);
		m_nMax = (m_nSize+1) * 2;
		m_pData[m_nSize++] = dValue;
	}
}

// delete an element at some index
void DArray::DeleteAt(int nIndex) {
	for (int i = nIndex; i < m_nSize - 1; i++)
		m_pData[i] = m_pData[i + 1];
	m_nSize--;
}

// insert a new element at some index
void DArray::InsertAt(int nIndex, double dValue) {
	if (m_nMax == m_nSize)
		SetSize((m_nSize + 1)*2);
	for (int i = m_nSize; i > nIndex; i--)
		m_pData[i] = m_pData[i - 1];
	m_pData[nIndex] = dValue;
	m_nSize++;
}

// overload operator '='
DArray& DArray::operator = (const DArray& arr) {
	m_nMax = arr.GetSize();
	m_pData = new double[m_nMax];
	for (int i = 0; i < arr.m_nSize; i++)
		m_pData[i] = arr[i];
	m_nSize = arr.m_nSize;
	return *this;
}
