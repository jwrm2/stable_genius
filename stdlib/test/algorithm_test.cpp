#include <iostream>

#include "../include/algorithm"
#include "../include/vector"

using namespace klib;

// Types needed for testing.
struct dummy_container {
    using pointer = void*;
};

int main()
{
    size_t fail_count = 0;

    // max
    {
        int i1 = 1;
        int i2 = 2;
        if (max(i1, i2) != 2)
        {
            ++fail_count;
            std::cout << "FAILED max with default compare\n";
        }
        auto comp = [](const int& lhs, const int& rhs) { return lhs < rhs; };
        if (max(i1, i2, comp) != 2)
        {
            ++fail_count;
            std::cout << "FAILED max with custom compare\n";
        }
    }
    // min
    {
        int i1 = 1;
        int i2 = 2;
        if (min(i1, i2) != 1)
        {
            ++fail_count;
            std::cout << "FAILED min with default compare\n";
        }
        auto comp = [](const int& lhs, const int& rhs) { return lhs < rhs; };
        if (min(i1, i2, comp) != 1)
        {
            ++fail_count;
            std::cout << "FAILED min with custom compare\n";
        }
    }
    // distance
    {
        // normal_iterator
        int* i1 = new int[4];
        if (distance(helper::normal_iterator<int*, dummy_container>{i1},
            helper::normal_iterator<int*, dummy_container>{&i1[3]}) != 3)
        {
            ++fail_count;
            std::cout << "FAILED distance with normal_iterator\n";
        }
        delete[] i1;
        // vector iterator
        vector<int> v1 = {1, 2, 3, 4};
        if (distance(v1.begin(), v1.end()) != 4)
        {
            ++fail_count;
            std::cout << "FAILED distance with vector::iterator\n";
        }
    }
    // equal
    {
        vector<int> v1 = {1, 2, 3, 4};
        vector<int> v2 = {1, 2, 3};
        vector<int> v3 = {2, 3, 4};
        auto comp = [](int i1, int i2) { return i1 == i2; };
        // Assumed second range
        if (!equal(v2.begin(), v2.end(), v1.begin()))
        {
            ++fail_count;
            std::cout << "FAILED equal assumed second range\n";
        }
        if (equal(v3.begin(), v3.end(), v1.begin()))
        {
            ++fail_count;
            std::cout << "FAILED not equal assumed second range\n";
        }
        // Explicit second range
        if (!equal(v1.begin(), v1.end() - 1, v2.begin(), v2.end()))
        {
            ++fail_count;
            std::cout << "FAILED equal explicit second range\n";
        }
        if (equal(v1.begin(), v1.end() - 1, v3.begin(), v3.end()))
        {
            ++fail_count;
            std::cout << "FAILED equal explicit same length second range\n";
        }
        if (equal(v1.begin(), v1.end(), v3.begin(), v3.end()))
        {
            ++fail_count;
            std::cout << "FAILED equal explicit different length second range\n";
        }
        // Assumed second range with comparison
        if (!equal(v2.begin(), v2.end(), v1.begin(), comp))
        {
            ++fail_count;
            std::cout << "FAILED equal assumed second range with comparison\n";
        }
        if (equal(v3.begin(), v3.end(), v1.begin(), comp))
        {
            ++fail_count;
            std::cout << "FAILED not equal assumed second range with comparison\n";
        }
        // Explicit second range with comparison
        if (!equal(v1.begin(), v1.end() - 1, v2.begin(), v2.end(), comp))
        {
            ++fail_count;
            std::cout << "FAILED equal explicit second range with comparison\n";
        }
        if (equal(v1.begin(), v1.end() - 1, v3.begin(), v3.end(), comp))
        {
            ++fail_count;
            std::cout << "FAILED equal explicit same length second range with comparison\n";
        }
        if (equal(v1.begin(), v1.end(), v3.begin(), v3.end(), comp))
        {
            ++fail_count;
            std::cout << "FAILED equal explicit different length second range with comparison\n";
        }
    }
    // lexicographical_compare
    {
        vector<int> v1 = {1, 2, 3, 4};
        vector<int> v2 = {1, 2, 3};
        auto comp = [](int i1, int i2) { return i1 < i2; };
        // Compare equal
        if (lexicographical_compare(v1.begin(), v1.end() - 1,
            v2.begin(), v2.end()))
        {
            ++fail_count;
            std::cout << "FAILED lexicographical_compare equal\n";
        }
        // Left is less because an element is less
        if (!lexicographical_compare(v1.begin(), v1.end() - 2,
            v2.begin() + 1, v2.end()))
        {
            ++fail_count;
            std::cout << "FAILED lexicographical_compare left less elementwise\n";
        }
        // Left is greater because an element is greater
        if (lexicographical_compare(v2.begin() + 1, v2.end(),
            v1.begin(), v1.end() - 2))
        {
            ++fail_count;
            std::cout << "FAILED lexicographical_compare left greater elementwise\n";
        }
        // Left is less because the sequence is shorter
        if (!lexicographical_compare(v2.begin(), v2.end(),
            v1.begin(), v1.end()))
        {
            ++fail_count;
            std::cout << "FAILED lexicographical_compare left less shorter\n";
        }
        // Left is greater because the sequence is longer
        if (lexicographical_compare(v1.begin(), v1.end(), v2.begin(), v2.end()))
        {
            ++fail_count;
            std::cout << "FAILED lexicographical_compare left greater longer\n";
        }
        // Left empty
        if (!lexicographical_compare(v1.begin(), v1.begin(),
            v2.begin(), v2.end()))
        {
            ++fail_count;
            std::cout << "FAILED lexicographical_compare left empty\n";
        }
        // Right empty
        if (lexicographical_compare(v1.begin(), v1.end(),
            v2.begin(), v2.begin()))
        {
            ++fail_count;
            std::cout << "FAILED lexicographical_compare right empty\n";
        }
        // Both empty
        if (lexicographical_compare(v1.begin(), v1.begin(),
            v2.begin(), v2.begin()))
        {
            ++fail_count;
            std::cout << "FAILED lexicographical_compare both empty\n";
        }
        // Compare equal with comparison
        if (lexicographical_compare(v1.begin(), v1.end() - 1,
            v2.begin(), v2.end(), comp))
        {
            ++fail_count;
            std::cout << "FAILED lexicographical_compare equal with comparison\n";
        }
        // Left is less because an element is less with comparison
        if (!lexicographical_compare(v1.begin(), v1.end() - 2,
            v2.begin() + 1, v2.end(), comp))
        {
            ++fail_count;
            std::cout << "FAILED lexicographical_compare left less elementwise with comparison\n";
        }
        // Left is greater because an element is greater with comparison
        if (lexicographical_compare(v2.begin() + 1, v2.end(),
            v1.begin(), v1.end() - 2, comp))
        {
            ++fail_count;
            std::cout << "FAILED lexicographical_compare left greater elementwise with comparison\n";
        }
        // Left is less because the sequence is shorter with comparison
        if (!lexicographical_compare(v2.begin(), v2.end(),
            v1.begin(), v1.end(), comp))
        {
            ++fail_count;
            std::cout << "FAILED lexicographical_compare left less shorter with comparison\n";
        }
        // Left is greater because the sequence is longer with comparison
        if (lexicographical_compare(v1.begin(), v1.end(),
            v2.begin(), v2.end(), comp))
        {
            ++fail_count;
            std::cout << "FAILED lexicographical_compare left greater longer with comparison\n";
        }
        // Left empty with comparison
        if (!lexicographical_compare(v1.begin(), v1.begin(),
            v2.begin(), v2.end(), comp))
        {
            ++fail_count;
            std::cout << "FAILED lexicographical_compare left empty with comparison\n";
        }
        // Right empty with comparison
        if (lexicographical_compare(v1.begin(), v1.end(),
            v2.begin(), v2.begin(), comp))
        {
            ++fail_count;
            std::cout << "FAILED lexicographical_compare right empty with comparisonn";
        }
        // Both empty with comparison
        if (lexicographical_compare(v1.begin(), v1.begin(),
            v2.begin(), v2.begin(), comp))
        {
            ++fail_count;
            std::cout << "FAILED lexicographical_compare both empty with comparison\n";
        }
    }
    // iter_swap
    {
        vector<int> v1 {1, 2, 3};
        vector<int> v2 {4, 5, 6};
        // Swap within container.
        iter_swap(v1.begin(), v1.begin() + 2);
        if (v1[0] != 3 || v1[1] != 2 || v1[2] != 1)
        {
            ++fail_count;
            std::cout << "FAILED iter_swap same container\n";
        }
        // Swap between containers.
        iter_swap(v1.begin() + 1, v2.begin() + 1);
        if (v1[0] != 3 || v1[1] != 5 || v1[2] != 1 || v2[0] != 4 ||
            v2[1] != 2 || v2[2] != 6)
        {
            ++fail_count;
            std::cout << "FAILED iter_swap different container\n";
        }
    }
    // reverse
    {
        // Odd number
        vector<int> v1 {1, 2, 3};
        reverse(v1.begin(), v1.end());
        if (v1[0] != 3 || v1[1] != 2 || v1[2] != 1)
        {
            ++fail_count;
            std::cout << "FAILED reverse odd number\n";
        }
        // Even number
        vector<int> v2 {1, 2, 3, 4};
        reverse(v2.begin(), v2.end());
        if (v2[0] != 4 || v2[1] != 3 || v2[2] != 2 || v2[3] != 1)
        {
            ++fail_count;
            std::cout << "FAILED reverse even number\n";
        }
    }
    // fill_n
    {
        // Complete vector.
        vector<int> v1 {1, 2, 3};
        auto it1 = fill_n(v1.begin(), 3, 4);
        if (v1.size() != 3 || v1[0] != 4 || v1[1] != 4 || v1[2] != 4 ||
            it1 != v1.end())
        {
            ++fail_count;
            std::cout << "FAILED fill_n whole vector\n";
        }
        // Start of vector
        vector<int> v2 {1, 2, 3};
        auto it2 = fill_n(v2.begin(), 2, 4);
        if (v1.size() != 3 || v2[0] != 4 || v2[1] != 4 || v2[2] != 3 ||
            it2 != --v2.end())
        {
            ++fail_count;
            std::cout << "FAILED fill_n start of vector\n";
        }
        // End of vector
        vector<int> v3 {1, 2, 3};
        auto it3 = fill_n(++v3.begin(), 2, 4);
        if (v3.size() != 3 || v3[0] != 1 || v3[1] != 4 || v3[2] != 4 ||
            it3 != v3.end())
        {
            ++fail_count;
            std::cout << "FAILED fill_n end of vector\n";
        }
        // Middle of vector
        vector<int> v4 {1, 2, 3, 4};
        auto it4 = fill_n(++v4.begin(), 2, 5);
        if (v4.size() != 4 || v4[0] != 1 || v4[1] != 5 || v4[2] != 5 ||
            v4[3] != 4 ||it4 != --v4.end())
        {
            ++fail_count;
            std::cout << "FAILED fill_n middle of vector\n";
        }
        // Zero count
        vector<int> v5 {1, 2, 3};
        auto it5 = fill_n(v5.begin(), 0, 4);
        if (v5.size() != 3 || v5[0] != 1 || v5[1] != 2 || v5[2] != 3 ||
            it5 != v5.begin())
        {
            ++fail_count;
            std::cout << "FAILED fill_n zero count\n";
        }
        // Negative count
        vector<int> v6 {1, 2, 3};
        auto it6 = fill_n(v6.begin(), -2, 4);
        if (v6.size() != 3 || v6[0] != 1 || v6[1] != 2 || v6[2] != 3 ||
            it6 != v6.begin())
        {
            ++fail_count;
            std::cout << "FAILED fill_n negative count\n";
        }
        // Empty vector
        vector<int> v7 {};
        auto it7 = fill_n(v7.begin(), 0, 4);
        if (!v7.empty() || it7 != v7.begin() || it7 != v7.end())
        {
            ++fail_count;
            std::cout << "FAILED fill_n empty vector\n";
        }
    }
    // swap_ranges
    {
        // Complete vector.
        vector<int> v1 {1, 2, 3, 4};
        vector<int> v2 {5, 6, 7, 8};
        auto it1 = swap_ranges(v1.begin(), v1.end(), v2.begin());
        if (v1.size() != 4 || v1[0] != 5 || v1[1] != 6 || v1[2] != 7 ||
            v1[3] != 8 || v2.size() != 4 || v2[0] != 1 || v2[1] != 2 ||
            v2[2] != 3 || v2[3] != 4 || it1 != v2.end())
        {
            ++fail_count;
            std::cout << "FAILED swap_ranges whole vector\n";
        }
        // Start of vector.
        vector<int> v3 {1, 2, 3, 4};
        vector<int> v4 {5, 6, 7, 8};
        auto it2 = swap_ranges(v3.begin(), --v3.end(), v4.begin());
        if (v3.size() != 4 || v3[0] != 5 || v3[1] != 6 || v3[2] != 7 ||
            v3[3] != 4 || v4.size() != 4 || v4[0] != 1 || v4[1] != 2 ||
            v4[2] != 3 || v4[3] != 8 || it2 != --v4.end())
        {
            ++fail_count;
            std::cout << "FAILED swap_ranges start of vector\n";
        }
        // Middle of vector.
        vector<int> v5 {1, 2, 3, 4};
        vector<int> v6 {5, 6, 7, 8};
        auto it3 = swap_ranges(++v5.begin(), --v5.end(), ++v6.begin());
        if (v5.size() != 4 || v5[0] != 1 || v5[1] != 6 || v5[2] != 7 ||
            v5[3] != 4 || v6.size() != 4 || v6[0] != 5 || v6[1] != 2 ||
            v6[2] != 3 || v6[3] != 8 || it3 != --v6.end())
        {
            ++fail_count;
            std::cout << "FAILED swap_ranges middle of vector\n";
        }
        // End of vector.
        vector<int> v7 {1, 2, 3, 4};
        vector<int> v8 {5, 6, 7, 8};
        auto it4 = swap_ranges(++v7.begin(), v7.end(), ++v8.begin());
        if (v7.size() != 4 || v7[0] != 1 || v7[1] != 6 || v7[2] != 7 ||
            v7[3] != 8 || v8.size() != 4 || v8[0] != 5 || v8[1] != 2 ||
            v8[2] != 3 || v8[3] != 4 || it4 != v8.end())
        {
            ++fail_count;
            std::cout << "FAILED swap_ranges end of vector\n";
        }
        // Zero range.
        vector<int> v9 {1, 2, 3, 4};
        vector<int> v10 {5, 6, 7, 8};
        auto it5 = swap_ranges(++v9.begin(), ++v9.begin(), ++v10.begin());
        if (v9.size() != 4 || v9[0] != 1 || v9[1] != 2 || v9[2] != 3 ||
            v9[3] != 4 || v10.size() != 4 || v10[0] != 5 || v10[1] != 6 ||
            v10[2] != 7 || v10[3] != 8 || it5 != ++v10.begin())
        {
            ++fail_count;
            std::cout << "FAILED swap_ranges zero range\n";
        }
        // Empty source vector, zero range.
        vector<int> v11;
        vector<int> v12 {5, 6, 7, 8};
        auto it6 = swap_ranges(v11.begin(), v11.end(), ++v12.begin());
        if (!v11.empty() || v12.size() != 4 || v12[0] != 5 || v12[1] != 6 ||
            v12[2] != 7 || v12[3] != 8 || it6 != ++v12.begin())
        {
            ++fail_count;
            std::cout << "FAILED swap_ranges empty vector\n";
        }
    }
    // transform
    {
        struct add_one {
            int operator()(int a) { return ++a; }
        };
        // Whole range, overwrite.
        vector<int> v1 {1, 2, 3};
        transform(v1.begin(), v1.end(), v1.begin(), add_one {});
        if (v1.size() != 3 || v1[0] != 2 || v1[1] != 3 || v1[2] != 4)
        {
            ++fail_count;
            std::cout << "FAILED transform whole range, overwrite\n";
        }
        // Partial range, start, overwrite.
        vector<int> v2 {1, 2, 3};
        transform(v2.begin(), --v2.end(), v2.begin(), add_one {});
        if (v2.size() != 3 || v2[0] != 2 || v2[1] != 3 || v2[2] != 3)
        {
            ++fail_count;
            std::cout << "FAILED transform partial range, start, overwrite\n";
        }
        // Partial range, end, overwrite.
        vector<int> v3 {1, 2, 3};
        transform(++v3.begin(), v3.end(), ++v3.begin(), add_one {});
        if (v3.size() != 3 || v3[0] != 1 || v3[1] != 3 || v3[2] != 4)
        {
            ++fail_count;
            std::cout << "FAILED transform partial range, end, overwrite\n";
        }
        // Whole range, other destination.
        vector<int> v4 {1, 2, 3};
        vector<int> v5 {0, 0, 0};
        transform(v4.begin(), v4.end(), v5.begin(), add_one {});
        if (v4.size() != 3 || v4[0] != 1 || v4[1] != 2 || v4[2] != 3 ||
            v5.size() != 3 || v5[0] != 2 || v5[1] != 3 || v5[2] != 4)
        {
            ++fail_count;
            std::cout << "FAILED transform whole range, other destination\n";
        }
    }

    // End
    if (fail_count == 0)
        std::cout << "All tests passed\n";
    else
        std::cout << fail_count << " tests failed\n";
}
