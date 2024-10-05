#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <sstream>
#include <limits>
#include <immintrin.h>

int find_max_value_and_index(std::vector<float> arr, float& max_value, int& max_index, int arr_size) 
{
    // variable initialization
    max_value = arr[0];
    max_index = 0;

    __m128 xmm_max_value = _mm_loadu_ps(&arr[0]);// load first 4 floats from arr
    //__m128i xmm_max_index = _mm_setzero_si128();
    __m128i xmm_max_index = _mm_set_epi32(3, 2, 1, 0);// telling that first 4 elems are maximum

    for (int i = 4; i < (arr_size / 4) * 4; i += 4)
    {
        __m128 xmm_values = _mm_loadu_ps(&arr[i]);//load next 4 elements
        __m128i xmm_indices = _mm_set_epi32(i + 3, i + 2, i + 1, i);// loading 4 indexes

        /*
        * xmm_values[i] > xmm_max_value ?
        * if true than -nan (set bit)
        * if false than 0 (bit not set)
        */
        __m128 cmp_res = _mm_cmpgt_ps(xmm_values, xmm_max_value);

        //casting
        __m128i xmm_mask = _mm_castps_si128(cmp_res);

        // from 2 registers getting new like new_register[i]=max(first[i], second[i])
        xmm_max_value = _mm_max_ps(xmm_values, xmm_max_value);
        // from 2 registers getting new like new_register[i]=(xmm_mask == 0) ? first[i] : second[i]
        xmm_max_index = _mm_blendv_epi8(xmm_max_index, xmm_indices, xmm_mask);
    }

    // compare 0 with 2 and 1 with 3 (xmm_max_value[0123] and xmm_max_value[2301])
    __m128 max1 = _mm_max_ps(xmm_max_value, _mm_shuffle_ps(xmm_max_value, xmm_max_value, _MM_SHUFFLE(2, 3, 0, 1)));
    // compare max values from the previous comparing and xmm_max_value[1032]
    __m128 max2 = _mm_max_ps(max1, _mm_shuffle_ps(max1, max1, _MM_SHUFFLE(1, 0, 3, 2))); 

    // getting first elem of max2 (maximum)
    _mm_store_ss(&max_value, max2);

    // getting index of max value from xmm_max_index
    __m128 maxMask = _mm_cmpeq_ps(max1, _mm_set1_ps(max_value)); // max1 mask
    int indexMask = _mm_movemask_ps(maxMask); // retrieving mask
    int index = -1;

    if (indexMask) 
    {
        // max value in max1
        int idx = _tzcnt_u32(indexMask); // first bit
        index = idx; // (0 or 1 indexes)
    }
    else 
    {
        // max value in max2
        maxMask = _mm_cmpeq_ps(max2, _mm_set1_ps(max_value)); // max2 mask
        indexMask = _mm_movemask_ps(maxMask); // retrieving mask

        int idx = _tzcnt_u32(indexMask); // second bit
        index = idx + 2; // because it is second bit (2 or 3 indexes)
    }

    // now getting index of max elem; this index in xmm_max_index
    // loading all indexes from __m128i
    int32_t indicesArray[4];
    _mm_storeu_si128((__m128i*)indicesArray, xmm_max_index); // saving indexes in indicesArray

    // getting index from that saved array
    max_index = indicesArray[index];

    //rest of arr
    for (int j = (arr_size / 4) * 4; j < arr_size; ++j)
    {
        if (arr[j] > max_value)
        {
            max_value = arr[j];
            max_index = j;
        }
    }

    return 0;
}

int main() 
{
    /*TODO: this code does not read file in vs2022, but open file correctly at another places*/

    // const char* array_file = "input_one_array.txt";
    float max_value = 0;
    int max_index = 0;
    int array_size = 100;//to zero

    std::ifstream file("input_one_array.txt");
    /*if (!file.is_open()) {
        std::cout << "File opening error." << std::endl;
        return -1;
    }
    if (!(file >> array_size)) {
        std::cout << "Reading array size error." << std::endl;
        file.close();
        return -2;
    }
    if (array_size <= 0) {
        std::cout << "Empty input array." << std::endl;
        file.close();
        return -3;
    }*/

    std::vector<float> arr = { -2523.19472, -3657.90276, 157.7563, -6977.76519, -9287.91120, 2863.81234, 4622.25806, -3920.82501, 8058.30408, -169.34896, -1313.4057, 3236.67749, -3860.16987, 5156.39035, 6459.94313, -5259.64025, -7085.8769, -4170.32793, -7396.15054, -5236.80695, -2613.19445, -204.11246, -7412.76194, -7687.66747, -5484.8125, 5058.96757, 4810.99984, -7644.57325, -394.49539, 1493.91734, -1913.21214, -3546.74052, -790.27009, -710.86787, -6419.72956, 2941.7463, -1236.96613, 9642.19160, -3546.91887, 2921.94573, 2802.94770, 7142.95841, -8169.43233, -4512.67691, 9957.45280, 8050.41459, -8473.99075, 5136.78121, -3859.25908, 3292.78792, 5598.33351, 8955.26259, 5597.44743, 1427.66800, -838.77500, 6609.55602, -6131.24942, 2044.66862, 5447.50648, -2450.34548, 5928.11257, -9732.12134, -4852.63683, -2548.85420, 8740.21780, 3555.34153, -9331.86515, -1320.75282, 5556.21237, 4439.47491, 67.80235, 617.38259, -1894.91905, 3940.72167, -4468.21939, -9108.6604, 7820.53132, 2454.4564, -1696.57956, -4114.87448, -97.54983, -8160.75965, -3456.46105, -1717.18006, -4447.32995, -5733.38060, -943.76434, -2288.75345, -6511.35117, 5702.31610, -2605.96811, 1922.39074, 5369.50042, 7733.37814, 6662.49494, -5397.18500, -4350.54674, 287.11269, -4731.44888, -1227.34530 };
    /*std::string line;
    std::getline(file, line);
    while (std::getline(file, line)) {
        std::istringstream iss(line);
        std::string token;

        while (std::getline(iss, token, ';')) {
            size_t pos = token.find(',');
            std::string float_string = token.substr(0, pos) + '.' + token.substr(pos + 1);
            float number = std::stof(float_string);
            arr.push_back(number);
        }
    }*/

    file.close();

    int status = find_max_value_and_index(arr, max_value, max_index, array_size);
    if (status != 0) {
        std::cout << "Error: " << status << std::endl;
    }
    else {
        std::cout << "Max value: " << max_value << std::endl;
        std::cout << "Index: " << max_index << std::endl;
    }

    return 0;
}
