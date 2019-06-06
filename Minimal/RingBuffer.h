#pragma once

template <typename T, size_t SIZE>
static int incRingIdx(std::array<T, SIZE> & v, int head) {
	if (head < 0)
		head = head + (int)v.size();
	return (head + 1) % v.size();
}

template <typename T, size_t SIZE>
static T& getRingAt(std::array<T, SIZE> & v, int idx) {
	if (idx < 0)
		idx = idx + (int)v.size();
	return  v.at(idx % v.size());
}
