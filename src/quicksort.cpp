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
inline void swap(std::vector<T> &array, size_t indexA, size_t indexB) {
	if (indexA != indexB) {
		T tmp;
		tmp = array[indexA];
		array[indexA] = array[indexB];
		array[indexB] = tmp;
	}
}

template<typename T>
void quicksort_base(std::vector<T> &array, int64_t start, int64_t last) {
	if (start >= last) return;

	int64_t pivot = start + std::rand() % (last-start+1);

	swap(array, pivot, last);

	int64_t j=start;
	for(int64_t i=start; i<last; i++) {
		if(array[i] <= array[last]) {
			swap(array, i, j);
			j++;
		}
	}

	swap(array, last, j);

	quicksort_base(array, start, j-1);
	quicksort_base(array, j+1, last);
}

/**
 * Inplace Quick sort on vector.
 */
template<typename T>
void quicksort(std::vector<T> &array) {
	quicksort_base(array, 0, array.size()-1);
}

int main(void) {
	std::srand(std::time(0));

	std::vector<int> array = {-5, 5, -14, 13, 10, 8, -1, 10, -12, 7, 0, 9, 2, 14, -14, -15, -13};
	std::vector<int> sorted = array;
	std::vector<int> sorted_ref = array;

	std::sort(sorted_ref.begin(), sorted_ref.end());

	quicksort(sorted);

	std::cout << "Source:     " << array << std::endl;
	std::cout << "Sorted:     " << sorted << std::endl;
	std::cout << "Sorted ref: " << sorted_ref << std::endl;

	return check(sorted, sorted_ref) ? 0 : 1;
}
