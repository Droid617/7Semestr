#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <sstream>
#include <immintrin.h>

int find_max_value_and_index(std::vector<float> arr, float& max_value, int& max_index, int arr_size) {
    // variable initialization
    max_value = arr[0];
    max_index = 0;

    // finding max value and its index
    __m128 xmm_max_value = _mm_load_ps(&arr[0]);//load first 4 elems from arr
    __m128i xmm_max_index = _mm_setzero_si128();//zeroing

    for (int i = 0; i < arr_size; i += 4) {
        __m128 xmm_values = _mm_load_ps(&arr[i]);//shift by one elem every loop
        __m128i xmm_indices = _mm_set_epi32(i + 3, i + 2, i + 1, i);//loading 4 integers/indexes

        //compare 2 registers like if first[i] < second[i] then result[i] = 1 else 0 -> result casting to __m128i
        __m128i xmm_mask = _mm_castps_si128(_mm_cmpgt_ps(xmm_values, xmm_max_value));
        //from 2 registers getting new like new_register[i]=max(first[i], second[i])
        xmm_max_value = _mm_max_ps(xmm_values, xmm_max_value);
        //from 2 registers getting new like new_register[i]=(xmm_mask == 0) ? first[i] : second[i]
        xmm_max_index = _mm_blendv_epi8(xmm_max_index, xmm_indices, xmm_mask);
    }

    // getting max value and its index
    float values[4];
    int indices[4];

    _mm_store_ps(values, xmm_max_value);//loading from register to array
    _mm_store_si128(reinterpret_cast<__m128i*>(indices), xmm_max_index);//the same but indexes

    // searching max value and its index from previous result
    for (int i = 0; i < 4; i++) {
        if (values[i] > max_value) {
            max_value = values[i];
            max_index = indices[i];
        }
    }

    return 0;
}

int main() {
    /*TODO: this code does not read file in vs2022, but open file correctly at another places*/

    //const char* array_file = "input_one_array.txt";
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
