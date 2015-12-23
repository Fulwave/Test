#include <arm_math.h>



uint8_t preset_0[] = {3, 80, 0, 36, 59, 1, 32, 31, 92, 48, 12, 32, 5, 0, 57, 6, 7, 40, 7, 15, 11, 6, 18, 18, 7, 26, 27, 10, 36, 10, 6, 42, 39, 3, 46, 27, 6, 49, 54, 11, 55, 17, 81, 2, 0, 42, 80, 101, 5, 0, 36, 80, 80, 0, 1, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 1, 1, 28, 0, 48, 65, 2, 0, 17, 80, 24, 38, 24, 32, 41, 3, 20, 87, 36};

uint8_t preset_1[] = {3, 80, 0, 41, 59, 1, 29, 31, 90, 48, 7, 32, 5, 0, 43, 6, 7, 41, 7, 12, 4, 6, 18, 18, 7, 26, 27, 10, 36, 10, 6, 42, 35, 3, 44, 27, 6, 49, 50, 11, 55, 22, 76, 2, 0, 42, 80, 101, 5, 0, 36, 80, 80, 0, 1, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 1, 1, 28, 0, 48, 38, 2, 0, 99, 80, 24, 37, 24, 32, 41, 3, 20, 87, 36};

uint8_t preset_2[] = {2, 80, 0, 41, 44, 1, 32, 31, 104, 48, 7, 32, 5, 0, 58, 6, 7, 36, 7, 11, 19, 11, 19, 17, 7, 26, 28, 10, 37, 16, 6, 42, 39, 3, 48, 27, 6, 49, 59, 11, 55, 19, 90, 2, 0, 42, 80, 101, 5, 0, 36, 80, 80, 0, 1, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 1, 1, 23, 0, 41, 39, 2, 0, 34, 80, 24, 47, 24, 32, 0, 3, 24, 76, 36};

uint8_t preset_3[] = {3, 80, 0, 48, 44, 1, 24, 23, 93, 48, 0, 32, 5, 0, 52, 6, 8, 53, 7, 12, 12, 11, 18, 9, 11, 27, 31, 10, 42, 16, 6, 42, 51, 3, 51, 27, 6, 49, 34, 11, 55, 20, 68, 2, 0, 58, 80, 87, 5, 0, 18, 80, 80, 0, 1, 1, 0, 0, 0, 0, 1, 0, 1, 1, 0, 0, 1, 1, 21, 0, 41, 32, 2, 0, 56, 80, 44, 44, 20, 28, 0, 3, 30, 92, 36};

uint8_t preset_4[] = {3, 80, 0, 48, 44, 1, 24, 23, 93, 48, 0, 32, 5, 0, 52, 6, 8, 53, 7, 12, 12, 11, 18, 9, 11, 27, 31, 10, 42, 16, 6, 42, 51, 3, 51, 27, 6, 49, 34, 11, 55, 23, 68, 2, 0, 58, 80, 84, 2, 0, 18, 80, 80, 0, 1, 1, 0, 0, 0, 0, 1, 0, 1, 1, 0, 0, 1, 1, 21, 0, 41, 32, 2, 0, 56, 80, 44, 44, 20, 28, 0, 3, 30, 92, 36};

uint8_t preset_5[128];

