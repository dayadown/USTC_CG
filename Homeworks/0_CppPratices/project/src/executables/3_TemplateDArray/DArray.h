#pragma once
#include<iostream>
using namespace std;
// interfaces of Dynamic Array class DArray
template <class DataType>
class DArray {
public:
	DArray(); // default constructor
	DArray(int nSize, DataType dValue = 0); // set an array with default values
	DArray(const DArray<DataType>& arr); // copy constructor
	~DArray(); // deconstructor

	void Print() const; // print the elements of the array

	int GetSize() const; // get the size of the array
	void SetSize(int nSize); // set the size of the array

	const DataType& GetAt(int nIndex) const; // get an element at an index
	void SetAt(int nIndex, DataType dValue); // set the value of an element

	DataType& operator[](int nIndex); // overload operator '[]'
	const DataType& operator[](int nIndex) const; // overload operator '[]'

	void PushBack(DataType dValue); // add a new element at the end of the array
	void DeleteAt(int nIndex); // delete an element at some index
	void InsertAt(int nIndex, DataType dValue); // insert a new element at some index

	DArray<DataType>& operator = (const DArray<DataType>& arr); //overload operator '='

private:
	DataType* m_pData; // the pointer to the array memory
	int m_nSize; // the size of the array
	int m_nMax;

private:
	void Init(); // initilize the array
	void Free(); // free the array
	void Reserve(int nSize); // allocate enough memory
};

// default constructor
template <class DataType>
DArray<DataType>::DArray() {
	Init();
}

// set an array with default values
template <class DataType>
DArray<DataType>::DArray(int nSize, DataType dValue)
	:m_pData(new DataType[nSize]), m_nMax(nSize), m_nSize(nSize)
{
	for (int i = 0; i < nSize; i++)
		m_pData[i] = dValue;
}

template <class DataType>
DArray<DataType>::DArray(const DArray& arr)
	:m_nMax(arr.m_nMax), m_nSize(arr.m_nSize), m_pData(new DataType[arr.m_nMax])
{
	for (int i = 0; i < m_nSize; i++)
		m_pData[i] = arr[i];
}

// deconstructor
template <class DataType>
DArray<DataType>::~DArray() {
	Free();
}

// display the elements of the array
template <class DataType>
void DArray<DataType>::Print() const {
	if (m_nSize == 0)
		cout << "no element";
	for (int i = 0; i < m_nSize; i++)
		cout << m_pData[i] << " ";
	cout << endl;
}

// initilize the array
template <class DataType>
void DArray<DataType>::Init() {
	m_nMax = m_nSize = 0;
	m_pData = nullptr;
}

// free the array
template <class DataType>
void DArray<DataType>::Free() {
	m_nMax = m_nSize = 0;
	delete[] m_pData;
	m_pData = nullptr;
}

// get the size of the array
template <class DataType>
int DArray<DataType>::GetSize() const {
	return m_nMax;
}

// set the size of the array
template <class DataType>
void DArray<DataType>::SetSize(int nSize) {
	if (nSize == m_nMax)
		return;
	DataType* pdata = new DataType[nSize];
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
template <class DataType>
const DataType& DArray<DataType>::GetAt(int nIndex) const {
	return m_pData[nIndex];
}

// set the value of an element 
template <class DataType>
void DArray<DataType>::SetAt(int nIndex, DataType dValue) {
	if (nIndex < m_nMax)
		m_pData[nIndex] = dValue;
	else {
		SetSize(m_nMax * 2);
		m_pData[nIndex] = dValue;
	}
}

// overload operator '[]'
template <class DataType>
DataType& DArray<DataType>::operator[](int nIndex) {
	return m_pData[nIndex];
}

// overload operator '[]'
template <class DataType>
const DataType& DArray<DataType>::operator[](int nIndex) const {
	return m_pData[nIndex];
}

// add a new element at the end of the array
template <class DataType>
void DArray<DataType>::PushBack(DataType dValue) {
	if (m_nMax > m_nSize)
		m_pData[m_nSize++] = dValue;
	else {
		SetSize((m_nSize + 1) * 2);
		m_nMax = (m_nSize + 1) * 2;
		m_pData[m_nSize++] = dValue;
	}
}

// delete an element at some index
template <class DataType>
void DArray<DataType>::DeleteAt(int nIndex) {
	for (int i = nIndex; i < m_nSize - 1; i++)
		m_pData[i] = m_pData[i + 1];
	m_nSize--;
}

// insert a new element at some index
template <class DataType>
void DArray<DataType>::InsertAt(int nIndex, DataType dValue) {
	if (m_nMax == m_nSize)
		SetSize((m_nSize + 1) * 2);
	for (int i = m_nSize; i > nIndex; i--)
		m_pData[i] = m_pData[i - 1];
	m_pData[nIndex] = dValue;
	m_nSize++;
}

// overload operator '='
template <class DataType>
DArray<DataType>& DArray<DataType>::operator = (const DArray<DataType>& arr) {
	m_nMax = arr.GetSize();
	m_pData = new DataType[m_nMax];
	for (int i = 0; i < arr.m_nSize; i++)
		m_pData[i] = arr[i];
	m_nSize = arr.m_nSize;
	return *this;
}