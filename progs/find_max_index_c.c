#include <limits.h>

int find_max_index_c(int *values, int len)
{
	int max_index = 0;
	int max = INT_MIN;

	for (int i = 0; i < len; i++)
		if (values[i] > max) {
			max_index = i;
			max = values[i];
		}

	return max_index;
}
