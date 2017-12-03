#include <iostream>
#include <iomanip>
#include <algorithm>
#include <vector>
#include <cstdlib>
#include <ctime>
#include <cstdint>

template<typename T>
std::ostream &operator<<(std::ostream &os, const std::vector<T> &vector) {
	for(std::size_t s=0; s<vector.size(); s++) {
		os << vector[s];
		if ((s+1) != vector.size()) {
			os << " ";
		}
	}
	return os;
}

template<typename T>
bool check(const std::vector<T> &array, const std::vector<T> &ref) {
	if (array.size() != ref.size()) return false;
	for(std::size_t s=0; s<ref.size(); s++) {
		if (array[s] != ref[s]) return false;
	}
	return true;
}

template<typename T>
void mergesort_fusion(const std::vector<T> &array0, const std::vector<T> &array1, std::vector<T> &array) {
	std::size_t i0=0;
	std::size_t i1=0;
	std::size_t s=0;

	while(s < array0.size()+array1.size()) {
		if (i0 >= array0.size()) {
			array[s++] = array1[i1++];
		}
		else if (i1 >= array1.size()) {
			array[s++] = array0[i0++];
		}
		else {
			if(array0[i0] < array1[i1]) {
				array[s++] = array0[i0++];
			}
			else {
				array[s++] = array1[i1++];
			}
		}
	}

}

/**
 * Inplace Quick sort on vector.
 */
template<typename T>
void mergesort(std::vector<T> &array) {
	if (array.size() <= 1) return;

	const std::size_t mid = array.size() / 2;
	std::vector<T> lo {array.begin(), array.begin()+mid};
	std::vector<T> hi {array.begin()+mid, array.end()};

	mergesort(lo);
	mergesort(hi);

	mergesort_fusion(lo, hi, array);
}


int main(void) {
	std::srand(std::time(0));

	const std::vector<int> array = {-5, 5, -14, 13, 10, 8, -1, 10, -12, 7, 0, 9, 2, 14, -14, -15, -13};
	std::vector<int> sorted = array;
	std::vector<int> sorted_ref = array;

	std::sort(sorted_ref.begin(), sorted_ref.end());

	mergesort(sorted);

	std::cout << "Source:     " << array << std::endl;
	std::cout << "Sorted:     " << sorted << std::endl;
	std::cout << "Sorted ref: " << sorted_ref << std::endl;

	return check(sorted, sorted_ref) ? 0 : 1;
}
