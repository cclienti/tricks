#include <cstddef>
#include <cstring>
#include <iostream>
#include <utility>


template<typename T>
class Vector
{
public:
	typedef T* iterator;
	typedef const T* const_iterator;

	/**
	 * Default constructor
	 */
	Vector():
		m_capacity(1),
		m_size(0),
		m_array(nullptr)
	{
		// The array allocated is just a chunk of data without any
		// initialization. Allocating builtin types with the new
		// operator leaves the memory uninitialized. We have just built
		// a kind of memory pool.
		m_array = new char [m_capacity*sizeof(T)];
	}

	/**
	 * Copy constructor
	 */
	Vector(const Vector<T> &v):
		m_capacity(v.capacity()),
		m_size(v.size()),
		m_array(nullptr)
	{
		// Allocating a chunk of data without initialization (See
		// default ctor).
		m_array = new char [m_capacity*sizeof(T)];

		// We cast the memory chunk to the appropriate pointer type.
		T* dst = reinterpret_cast<T*>(m_array);

		// We copy the content
		for(std::size_t i=0; i<m_size; ++i) {
			// We use the placement new to call the copy ctor of "v" in
			// order to initialize the previously allocated memory.
			new (dst+i) T(v[i]);
		}
	}

	/**
	 * Move constructor
	 */
	Vector(Vector<T> &&v):
		m_capacity(v.m_capacity),
		m_size(v.m_size),
		m_array(v.m_array)
	{
		// Resetting all values of v to properly delete the object.
		v.m_capacity = 0;
		v.m_size = 0;
		v.m_array = nullptr;
		std::cerr << "move ctor" << std::endl;
	}

	/**
	 * Destructor
	 */
	virtual ~Vector()
	{
		// Before freeing the memory, we must call each element's
		// destructor.
		T* dst = reinterpret_cast<T*>(m_array);
		for(std::size_t i=0; i<m_size; ++i) {
			dst[i].~T();
		}

		// Then we can free the memory chunk.
		delete [] m_array;
	}

	/**
	 * Append a value in the vector. If the vector is full a new chunk
	 * will be allocated by doubling the current capacity. Then the old
	 * content is copied in the new one.
	 *
	 * @sa reserve
	 */
	virtual void push_back(const T &value)
	{
		// If the size is more or equal to the current capacity, we must
		// reserve a bigger memory chunk.
		if(m_size >= m_capacity) {
			reserve(m_capacity*2);
		}

		// Call the copy ctor to append the new element. This is done by
		// using the placement new op.
		T* dst = reinterpret_cast<T*>(m_array);
		new (dst + m_size++) T(value);
	}

	/**
	 * Reserve a new memory chunk to store vector's elements. If the
	 * given capacity is less or equal than the current capacity, it
	 * won't do anything.
	 */
	virtual void reserve(std::size_t capacity)
	{
		if(capacity > m_capacity) {
			m_capacity = capacity;

			// The old array was already allocated, so we just need to
			// memcopy bytes in the new memory chunk. Because we used a
			// builtin type to allocate the memory chunk, freeing it
			// won't call the destructor of each array element. The
			// memcopy is safe in this case.
			char * old = m_array;
			m_array = new char [m_capacity*sizeof(T)];
			std::memcpy(m_array, old, m_size*sizeof(T));
			delete [] old;
		}
	}

	/**
	 * Begin iterator.
	 */
	virtual iterator begin() const
	{
		T* dst = reinterpret_cast<T*>(m_array);
		return dst;
	}

	/**
	 * Begin const_iterator.
	 */
	virtual const_iterator cbegin() const
	{
		return begin();
	}

	/**
	 * End iterator.
	 */
	virtual iterator end() const
	{
		T* dst = reinterpret_cast<T*>(m_array);
		return dst+m_size;
	}

	/**
	 * End const_iterator.
	 */
	virtual const_iterator cend() const
	{
		return end();
	}

	/**
	 * Return the element stored at the given index. No out of bounds
	 * check is done here.
	 */
	virtual T & operator[](std::size_t index) const
	{
		T* dst = reinterpret_cast<T*>(m_array);
		return dst[index];
	}

	/**
	 * Return the number of elements stored in the vector.
	 */
	virtual std::size_t size() const
	{
		return m_size;
	}

	/**
	 * Return the capacity (in term of number of elements) of the vector.
	 */
	virtual std::size_t capacity() const
	{
		return m_capacity;
	}

private:
	std::size_t m_capacity;
	std::size_t m_size;
	char *m_array;
};



/**
 * A small test case
 */
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

	for(const auto &v: v1) {
		std::cout << v << " ";
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

	for(const auto &v: v2) {
		std::cout << v << " ";
	}
	std::cout << std::endl;

	//---------------------------------------
	std::cout << "Vector 3 Test" << std::endl;
	Vector<int> v3(v2);
	v2.push_back(9);

	for(const auto &v: v3) {
		std::cout << v << " ";
	}
	std::cout << std::endl;

	//---------------------------------------
	std::cout << "Vector 2x2 Test" << std::endl;
	Vector< Vector<int> > v2x2;

	v2x2.push_back(v1);
	v2x2.push_back(v2);
	v2x2.push_back(v3);

	for(const auto &vline: v2x2) {
		for(const auto &v: vline) {
			std::cout << v << " ";
		}
		std::cout << std::endl;
	}

	//--------------------------------------
	Vector<int> v2p (std::move(v2));
	v2p.push_back(10);

	for(const auto &v: v2p) {
		std::cout << v << " ";
	}
	std::cout << std::endl;

	return 0;
}
