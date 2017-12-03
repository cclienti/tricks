#include <cstddef>
#include <cstring>
#include <iostream>


template<typename T>
class Vector
{
public:
	Vector():
		m_capacity(1),
		m_size(0),
		m_array(nullptr)
	{
		m_array = new char [m_capacity*sizeof(T)];
	}

	Vector(const Vector<T> &v):
		m_capacity(v.capacity()),
		m_size(v.size()),
		m_array(nullptr)
	{
		m_array = new char [m_capacity*sizeof(T)];

		for(std::size_t i=0; i<m_size; ++i) {
			new (m_array + i*sizeof(T)) T(v[i]);
		}
	}

	~Vector()
	{
		if(m_array) {
			T* dst = reinterpret_cast<T*>(m_array);
			for(std::size_t i=0; i<m_size; ++i) {
				dst[i].~T();
			}
			delete [] m_array;
		}
	}

	void push_back(const T &value)
	{
		if(m_size == m_capacity) {
			reserve(capacity()*2);
		}

		T* dst = reinterpret_cast<T*>(m_array);
		new (dst + m_size++) T(value);
	}

	void reserve(std::size_t capacity)
	{
		if(capacity > m_capacity) {
			m_capacity = capacity;

			char * tmp = m_array;
			T* tmp_v = reinterpret_cast<T*>(tmp);

			m_array = new char [m_capacity*sizeof(T)];

			for(std::size_t i=0, k=0; i<m_size; ++i, k+=sizeof(T)) {
				new (m_array + k) T(tmp_v[i]);
				tmp_v[i].~T();
			}

			delete [] tmp;
		}
	}

	T & operator[](std::size_t index) const
	{
		T* dst = reinterpret_cast<T*>(m_array);
		return dst[index];
	}

	std::size_t size() const
	{
		return m_size;
	}

	std::size_t capacity() const
	{
		return m_capacity;
	}

private:
	std::size_t m_capacity;
	std::size_t m_size;
	char *m_array;
};


int main(void) {
	//---------------------------------------
	std::cout << "Vector 1 Test" << std::endl;
	Vector<int> v1;
	v1.push_back(0);
	v1.push_back(1);
	v1.push_back(2);
	v1.push_back(3);

	for(std::size_t s=0; s<v1.size(); s++) {
		std::cout << v1[s] << " ";
	}
	std::cout << std::endl;

	//---------------------------------------
	std::cout << "Vector 2 Test" << std::endl;
	Vector<int> v2;
	v2.push_back(4);
	v2.push_back(5);
	v2.push_back(6);
	v2.push_back(7);
	v2.push_back(8);

	for(std::size_t s=0; s<v2.size(); s++) {
		std::cout << v2[s] << " ";
	}
	std::cout << std::endl;

	//---------------------------------------
	std::cout << "Vector 3 Test" << std::endl;
	Vector<int> v3(v2);
	v2.push_back(9);
	for(std::size_t s=0; s<v3.size(); s++) {
		std::cout << v3[s] << " ";
	}
	std::cout << std::endl;

	//---------------------------------------
	std::cout << "Vector 2x2 Test" << std::endl;
	Vector< Vector<int> > v2x2;

	v2x2.push_back(v1);
	v2x2.push_back(v2);

	for(std::size_t s=0; s<v2x2.size(); s++) {
		for(std::size_t t=0; t<v2x2[s].size(); t++) {
			std::cout << v2x2[s][t] << " ";
		}
		std::cout << std::endl;
	}

	return 0;
}
