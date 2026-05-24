#include <iostream>
#include <vector>
#include <string>
#include <random>
#include <algorithm>
#include <chrono>
#include <utility>

// Глобальный счетчик для отслеживания количества посимвольных сравнений во всех алгоритмах
long long char_comparisons = 0;

// Базовая функция посимвольного сравнения строк для стандартных сортировок
bool standardStringLess(const std::string& a, const std::string& b) {
    size_t i = 0;
    size_t len_a = a.length();
    size_t len_b = b.length();
    while (i < len_a && i < len_b) {
        char_comparisons++;
        if (a[i] != b[i]) {
            return a[i] < b[i];
        }
        i++;
    }
    char_comparisons++;
    return len_a < len_b;
}

// ГЕНЕРАТОР СТРОК
class StringGenerator {
private:
    std::string alphabet;
    std::mt19937 rng;

public:
    StringGenerator() {
        alphabet = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789!@#%:;^&*()-._";
        std::random_device rd;
        rng = std::mt19937(rd());
    }

    // Генерация одной случайной строки заданной длины
    std::string generateSingleString(size_t length) {
        std::uniform_int_distribution<size_t> dist(0, alphabet.size() - 1);
        std::string s = "";
        for (size_t i = 0; i < length; ++i) {
            s += alphabet[dist(rng)];
        }
        return s;
    }

    // Полностью случайный набор строк
    std::vector<std::string> generateRandom(size_t count) {
        std::vector<std::string> res(count);
        std::uniform_int_distribution<size_t> len_dist(10, 200);
        for (size_t i = 0; i < count; ++i) {
            res[i] = generateSingleString(len_dist(rng));
        }
        return res;
    }

    // Обратно отсортированный набор строк
    std::vector<std::string> generateReversed(size_t count) {
        std::vector<std::string> res = generateRandom(count);
        // Сортируем в прямом порядке, используя стандартное посимвольное сравнение
        std::sort(res.begin(), res.end(), standardStringLess);
        // Переворачиваем в обратный
        std::reverse(res.begin(), res.end());
        return res;
    }

    // Почти отсортированный набор строк
    std::vector<std::string> generateAlmostSorted(size_t count) {
        std::vector<std::string> res = generateRandom(count);
        std::sort(res.begin(), res.end(), standardStringLess);
        
        // Делаем небольшое количество случайных перестановок пар (около 2% от размера)
        size_t swaps = count / 50;
        if (swaps == 0 && count > 1) swaps = 1;
        
        std::uniform_int_distribution<size_t> idx_dist(0, count - 1);
        for (size_t i = 0; i < swaps; ++i) {
            size_t idx1 = idx_dist(rng);
            size_t idx2 = idx_dist(rng);
            std::swap(res[idx1], res[idx2]);
        }
        return res;
    }
};


// РЕАЛИЗАЦИЯ ВСЕХ АЛГОРИТМОВ СОРТИРОВКИ ДЛЯ ТЕСТЕРА

namespace Sorts {
    // 1. СТАНДАРТНЫЙ MERGE SORT
    void standardMerge(std::vector<std::string>& arr, int left, int mid, int right) {
        int n1 = mid - left + 1;
        int n2 = right - mid;
        std::vector<std::string> L(n1), R(n2);
        for (int i = 0; i < n1; i++) L[i] = arr[left + i];
        for (int j = 0; j < n2; j++) R[j] = arr[mid + 1 + j];

        int i = 0, j = 0, k = left;
        while (i < n1 && j < n2) {
            if (standardStringLess(L[i], R[j])) {
                arr[k++] = L[i++];
            } else {
                arr[k++] = R[j++];
            }
        }
        while (i < n1) arr[k++] = L[i++];
        while (j < n2) arr[k++] = R[j++];
    }

    void standardMergeSort(std::vector<std::string>& arr, int left, int right) {
        if (left < right) {
            int mid = left + (right - left) / 2;
            standardMergeSort(arr, left, mid);
            standardMergeSort(arr, mid + 1, right);
            standardMerge(arr, left, mid, right);
        }
    }

    // Стандартный Quick Sort
    void standardQuickSort(std::vector<std::string>& arr, int lo, int hi) {
        if (lo >= hi) return;
        int i = lo, j = hi;
        std::string pivot = arr[lo];
        while (i <= j) {
            while (standardStringLess(arr[i], pivot)) i++;
            while (standardStringLess(pivot, arr[j])) j--;
            if (i <= j) {
                std::swap(arr[i++], arr[j--]);
            }
        }
        standardQuickSort(arr, lo, j);
        standardQuickSort(arr, i, hi);
    }

    // Вспомогательная функция для извлечения символа
    int charAt(const std::string& s, int d) {
        if (d < (int)s.length()) return static_cast<unsigned char>(s[d]);
        return -1;
    }

    // Адаптированный string merge sort
    int lcpCompare(const std::string& a, const std::string& b, int start_pos, int& lcp) {
        int i = start_pos;
        int len_a = a.length();
        int len_b = b.length();
        while (i < len_a && i < len_b) {
            char_comparisons++;
            if (a[i] != b[i]) {
                lcp = i;
                return (a[i] < b[i]) ? -1 : 1;
            }
            i++;
        }
        lcp = i;
        if (len_a == len_b) return 0;
        return (len_a < len_b) ? -1 : 1;
    }

    void stringMerge(std::vector<std::string>& arr, int left, int mid, int right) {
        int n1 = mid - left + 1;
        int n2 = right - mid;
        std::vector<std::string> L(n1), R(n2);
        for (int i = 0; i < n1; i++) L[i] = arr[left + i];
        for (int j = 0; j < n2; j++) R[j] = arr[mid + 1 + j];

        int i = 0, j = 0, k = left, current_lcp = 0;
        while (i < n1 && j < n2) {
            int res = lcpCompare(L[i], R[j], current_lcp, current_lcp);
            if (res <= 0) {
                arr[k++] = L[i++];
                current_lcp = 0;
            } else {
                arr[k++] = R[j++];
                current_lcp = 0;
            }
        }
        while (i < n1) arr[k++] = L[i++];
        while (j < n2) arr[k++] = R[j++];
    }

    void stringMergeSort(std::vector<std::string>& arr, int left, int right) {
        if (left < right) {
            int mid = left + (right - left) / 2;
            stringMergeSort(arr, left, mid);
            stringMergeSort(arr, mid + 1, right);
            stringMerge(arr, left, mid, right);
        }
    }

    // Тернарный quick sort для строк
    void stringQuickSort(std::vector<std::string>& arr, int lo, int hi, int d) {
        if (hi <= lo) return;
        int lt = lo, gt = hi, i = lo + 1;
        int pivot = charAt(arr[lo], d);
        while (i <= gt) {
            int t = charAt(arr[i], d);
            char_comparisons++;
            if (t < pivot) std::swap(arr[lt++], arr[i++]);
            else if (t > pivot) std::swap(arr[i], arr[gt--]);
            else i++;
        }
        stringQuickSort(arr, lo, lt - 1, d);
        if (pivot >= 0) stringQuickSort(arr, lt, gt, d + 1);
        stringQuickSort(arr, gt + 1, hi, d);
    }

    // MSD RADIX SORT
    void msdRadixSortOnly(std::vector<std::string>& arr, int lo, int hi, int d, std::vector<std::string>& aux) {
        if (hi <= lo) return;
        int count[258] = {0};
        for (int i = lo; i <= hi; i++) count[charAt(arr[i], d) + 2]++;
        for (int r = 0; r < 257; r++) count[r + 1] += count[r];
        for (int i = lo; i <= hi; i++) aux[count[charAt(arr[i], d) + 1]++] = arr[i];
        for (int i = lo; i <= hi; i++) arr[i] = aux[i - lo];
        for (int r = 0; r < 256; r++) {
            msdRadixSortOnly(arr, lo + count[r], lo + count[r + 1] - 1, d + 1, aux);
        }
    }

    // MSD RADIX SORT + QUICK SORT
    void msdRadixSortHybrid(std::vector<std::string>& arr, int lo, int hi, int d, std::vector<std::string>& aux) {
        if (hi <= lo) return;
        if ((hi - lo + 1) < 74) {
            stringQuickSort(arr, lo, hi, d);
            return;
        }
        int count[258] = {0};
        for (int i = lo; i <= hi; i++) count[charAt(arr[i], d) + 2]++;
        for (int r = 0; r < 257; r++) count[r + 1] += count[r];
        for (int i = lo; i <= hi; i++) aux[count[charAt(arr[i], d) + 1]++] = arr[i];
        for (int i = lo; i <= hi; i++) arr[i] = aux[i - lo];
        for (int r = 0; r < 256; r++) {
            msdRadixSortHybrid(arr, lo + count[r], lo + count[r + 1] - 1, d + 1, aux);
        }
    }
}


// ТЕСТЕР СОРТИРОВОК
class StringSortTester {
public:
    struct Metrics {
        double duration_ms;
        long long comparisons;
    };

    // Обобщенный метод для замера одной сортировки
    Metrics runTest(const std::vector<std::string>& initial_arr, int sort_type) {
        const int RUNS = 5;
        double total_time = 0;
        long long total_comps = 0;

        for (int run = 0; run < RUNS; ++run) {
            std::vector<std::string> arr_copy = initial_arr;
            char_comparisons = 0; // сбрасываем глобальный счетчик

            auto start = std::chrono::high_resolution_clock::now();
            
            if (sort_type == 1) Sorts::standardMergeSort(arr_copy, 0, arr_copy.size() - 1);
            else if (sort_type == 2) Sorts::standardQuickSort(arr_copy, 0, arr_copy.size() - 1);
            else if (sort_type == 3) Sorts::stringMergeSort(arr_copy, 0, arr_copy.size() - 1);
            else if (sort_type == 4) Sorts::stringQuickSort(arr_copy, 0, arr_copy.size() - 1, 0);
            else if (sort_type == 5) {
                std::vector<std::string> aux(arr_copy.size());
                Sorts::msdRadixSortOnly(arr_copy, 0, arr_copy.size() - 1, 0, aux);
            }
            else if (sort_type == 6) {
                std::vector<std::string> aux(arr_copy.size());
                Sorts::msdRadixSortHybrid(arr_copy, 0, arr_copy.size() - 1, 0, aux);
            }

            auto end = std::chrono::high_resolution_clock::now();
            std::chrono::duration<double, std::milli> duration = end - start;
            
            total_time += duration.count();
            total_comps += char_comparisons;
        }

        return { total_time / RUNS, total_comps / RUNS };
    }
};

int main() {
    StringGenerator generator;
    StringSortTester tester;

    // Генерируем максимальные базовые массивы по 3000 элементов согласно ТЗ
    std::vector<std::string> base_random = generator.generateRandom(3000);
    std::vector<std::string> base_reversed = generator.generateReversed(3000);
    std::vector<std::string> base_almost = generator.generateAlmostSorted(3000);

    // Вывод шапки таблицы результатов для удобного копирования в Excel
    std::cout << "Size;DataType;StdMerge_ms;StdMerge_comps;StdQuick_ms;StdQuick_comps;StrMerge_ms;StrMerge_comps;StrQuick_ms;StrQuick_comps;MSD_ms;MSD_comps;MSDHybrid_ms;MSDHybrid_comps\n";

    // Итерируемся по размерам подмассивов от 100 до 3000 с шагом 100
    for (int size = 100; size <= 3000; size += 100) {
        // Вырезаем подмассивы нужного размера из базовых наборов данных
        std::vector<std::string> rand_sub(base_random.begin(), base_random.begin() + size);
        std::vector<std::string> rev_sub(base_reversed.begin(), base_reversed.begin() + size);
        std::vector<std::string> almost_sub(base_almost.begin(), base_almost.begin() + size);

        // Массив типов данных для прогона цикла
        std::vector<std::pair<std::string, std::vector<std::string>>> datasets = {
            {"Random", rand_sub},
            {"Reversed", rev_sub},
            {"AlmostSorted", almost_sub}
        };

        for (const auto& data : datasets) {
            std::cout << size << ";" << data.first;
            for (int type = 1; type <= 6; ++type) {
                StringSortTester::Metrics m = tester.runTest(data.second, type);
                std::cout << ";" << m.duration_ms << ";" << m.comparisons;
            }
            std::cout << "\n";
        }
    }

    return 0;
}