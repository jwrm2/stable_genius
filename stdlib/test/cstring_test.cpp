#include <iostream>

#include "../include/cstring"

using namespace klib;

int main()
{
    size_t fail_count = 0;

    // strcmp
    {
        const char s1[] = "abc";
        const char s2[] = "abc";
        const char s3[] = "ab";
        const char s4[] = "def";
        const char s5[] = "";
        const char s6[] = "";
        if (strcmp(s1, s2) != 0)
        {
            ++fail_count;
            std::cout << "FAILED strcmp(const char*, const char*) equal\n";
        }
        if (strcmp(s1, s3) <= 0)
        {
            ++fail_count;
            std::cout << "FAILED strcmp(const char*, const char*) second shorter\n";
        }
        if (strcmp(s3, s1) >= 0)
        {
            ++fail_count;
            std::cout << "FAILED strcmp(const char*, const char*) second longer\n";
        }
        if (strcmp(s1, s4) >= 0)
        {
            ++fail_count;
            std::cout << "FAILED strcmp(const char*, const char*) second greater\n";
        }
        if (strcmp(s4, s1) <= 0)
        {
            ++fail_count;
            std::cout << "FAILED strcmp(const char*, const char*) second less\n";
        }
        if (strcmp(s1, s5) <= 0)
        {
            ++fail_count;
            std::cout << "FAILED strcmp(const char*, const char*) second empty\n";
        }
        if (strcmp(s5, s1) >= 0)
        {
            ++fail_count;
            std::cout << "FAILED strcmp(const char*, const char*) first empty\n";
        }
        if (strcmp(s5, s6) != 0)
        {
            ++fail_count;
            std::cout << "FAILED strcmp(const char*, const char*) both empty\n";
        }
    }
    // strncmp
    {
        const char s1[] = "abc";
        const char s2[] = "abc";
        const char s3[] = "ab";
        const char s4[] = "def";
        const char s5[] = "";
        const char s6[] = "";
        if (strncmp(s1, s2, 3) != 0)
        {
            ++fail_count;
            std::cout << "FAILED strncmp(const char*, const char*, size_t) equal\n";
        }
        if (strncmp(s1, s3, 3) <= 0)
        {
            ++fail_count;
            std::cout << "FAILED strncmp(const char*, const char*, size_t) second shorter\n";
        }
        if (strncmp(s3, s1, 3) >= 0)
        {
            ++fail_count;
            std::cout << "FAILED strncmp(const char*, const char*, size_t) second longer\n";
        }
        if (strncmp(s1, s4, 3) >= 0)
        {
            ++fail_count;
            std::cout << "FAILED strncmp(const char*, const char*, size_t) second greater\n";
        }
        if (strncmp(s4, s1, 3) <= 0)
        {
            ++fail_count;
            std::cout << "FAILED strncmp(const char*, const char*, size_t) second less\n";
        }
        if (strncmp(s1, s5, 3) <= 0)
        {
            ++fail_count;
            std::cout << "FAILED strncmp(const char*, const char*, size_t) second empty\n";
        }
        if (strncmp(s5, s1, 3) >= 0)
        {
            ++fail_count;
            std::cout << "FAILED strncmp(const char*, const char*, size_t) first empty\n";
        }
        if (strncmp(s5, s6, 3) != 0)
        {
            ++fail_count;
            std::cout << "FAILED strncmp(const char*, const char*, size_t) both empty\n";
        }
        if (strncmp(s1, s3, 2) != 0)
        {
            ++fail_count;
            std::cout << "FAILED strncmp(const char*, const char*, size_t) equal, limited\n";
        }
        if (strncmp(s1, s3, 0) != 0)
        {
            ++fail_count;
            std::cout << "FAILED strncmp(const char*, const char*, size_t) equal, zero length\n";
        }
    }
    // strcat
    {
        char s1[10] = "he";
        const char* s2 = "llo";
        char* s3 = strcat(s1, s2);
        if (strcmp(s1, "hello") != 0 || s1 != s3 || strcmp(s2, "llo") != 0)
        {
            ++fail_count;
            std::cout << "FAILED strcat(char*, const char*) simple\n";
        }
        char s4[10] = "he";
        const char* s5 = "";
        char* s6 = strcat(s4, s5);
        if (strcmp(s4, "he") != 0 || s4 != s6 || strcmp(s5, "") != 0)
        {
            ++fail_count;
            std::cout << "FAILED strcat(char*, const char*) second empty\n";
        }
        char s7[10] = "";
        const char* s8 = "llo";
        char* s9 = strcat(s7, s8);
        if (strcmp(s7, "llo") != 0 || s7 != s9 || strcmp(s8, "llo") != 0)
        {
            ++fail_count;
            std::cout << "FAILED strcat(char*, const char*) first empty\n";
        }
        char s10[10] = "";
        const char* s11 = "";
        char* s12 = strcat(s10, s11);
        if (strcmp(s10, "") != 0 || s10 != s12 || strcmp(s11, "") != 0)
        {
            ++fail_count;
            std::cout << "FAILED strcat(char*, const char*) both empty\n";
        }
        char* s13 = nullptr;
        const char* s14 = "llo";
        char* s15 = strcat(s13, s14);
        if (s13 || s15 || strcmp(s14, "llo") != 0)
        {
            ++fail_count;
            std::cout << "FAILED strcat(char*, const char*) dest nullptr\n";
        }
        char s16[10] = "he";
        const char* s17 = nullptr;
        char* s18 = strcat(s16, s17);
        if (strcmp(s16, "he") != 0 || s17 || s18)
        {
            ++fail_count;
            std::cout << "FAILED strcat(char*, const char*) source nullptr\n";
        }
        char* s19 = nullptr;
        const char* s20 = nullptr;
        char* s21 = strcat(s19, s20);
        if (s19 || s20 || s21)
        {
            ++fail_count;
            std::cout << "FAILED strcat(char*, const char*) both nullptr\n";
        }
    }
    // strcpy
    {
        const char* s1 = "hello";
        char s2[10];
        char* s3 = strcpy(s2, s1);
        if (strcmp(s1, "hello") != 0 || strcmp(s2, "hello") != 0 || s2 != s3)
        {
            ++fail_count;
            std::cout << "FAILED strcpy(char*, const char*) simple\n";
        }
        const char* s4 = "";
        char s5[10];
        char* s6 = strcpy(s5, s4);
        if (strcmp(s4, "") != 0 || strcmp(s5, "") != 0 || s5 != s6)
        {
            ++fail_count;
            std::cout << "FAILED strcpy(char*, const char*) empty\n";
        }
        const char* s7 = "he\0llo";
        char s8[10];
        char* s9 = strcpy(s8, s7);
        if (strcmp(s7, "he") != 0 || strcmp(s8, "he") != 0 || s8 != s9)
        {
            ++fail_count;
            std::cout << "FAILED strcpy(char*, const char*) contains null\n";
        }
        const char* s10 = nullptr;
        char s11[10] = "llo";
        char* s12 = strcpy(s11, s10);
        if (s10 || strcmp(s11, "llo") != 0 || s12)
        {
            ++fail_count;
            std::cout << "FAILED strcpy(char*, const char*) source nullptr\n";
        }
        const char* s13 = "hello";
        char* s14 = nullptr;
        char* s15 = strcpy(s14, s13);
        if (strcmp(s13, "hello") != 0 || s14 || s15)
        {
            ++fail_count;
            std::cout << "FAILED strcpy(char*, const char*) dest nullptr\n";
        }
        const char* s16 = nullptr;
        char* s17 = nullptr;
        char* s18 = strcpy(s17, s16);
        if (s16 || s17 || s18)
        {
            ++fail_count;
            std::cout << "FAILED strcpy(char*, const char*) both nullptr\n";
        }
    }
    // strncpy
    {
        const char* s1 = "hello";
        char s2[10];
        char* s3 = strncpy(s2, s1, 6);
        if (strcmp(s1, "hello") != 0 || strcmp(s2, "hello") != 0 || s2 != s3)
        {
            ++fail_count;
            std::cout << "FAILED strncpy(char*, const char*, size_t) exact\n";
        }
        const char* s4 = "hello";
        char s5[10] = "\0\0\0p\0\0\0\0\0";
        char* s6 = strncpy(s5, s4, 3);
        if (strcmp(s4, "hello") != 0 || strcmp(s5, "help") != 0 || s5 != s6)
        {
            std::cout << "s5 == " << s5 << '\n';
            ++fail_count;
            std::cout << "FAILED strncpy(char*, const char*, size_t) too few\n";
        }
        const char* s7 = "hello";
        char s8[10] = "ppppppppp";
        char* s9 = strncpy(s8, s7, 8);
        if (strcmp(s7, "hello") != 0 || strcmp(s8, "hello") != 0 || s8 != s9 ||
            s8[6] != '\0' || s8[7] != '\0' || strcmp(&s8[8], "p") != 0)
        {
            ++fail_count;
            std::cout << "FAILED strncpy(char*, const char*, size_t) too many\n";
        }
        const char* s10 = "he\0llo";
        char s11[10];
        char* s12 = strncpy(s11, s10, 6);
        if (strcmp(s10, "he") != 0 || strcmp(s11, "he") != 0 || s11 != s12)
        {
            ++fail_count;
            std::cout << "FAILED strncpy(char*, const char*, size_t) contains null\n";
        }
        const char* s13 = nullptr;
        char s14[10] = "llo";
        char* s15 = strncpy(s14, s13, 3);
        if (s13 || strcmp(s14, "llo") != 0 || s15)
        {
            ++fail_count;
            std::cout << "FAILED strncpy(char*, const char*, size_t) source nullptr\n";
        }
        const char* s16 = "hello";
        char* s17 = nullptr;
        char* s18 = strncpy(s17, s16, 3);
        if (strcmp(s16, "hello") != 0 || s17 || s18)
        {
            ++fail_count;
            std::cout << "FAILED strncpy(char*, const char*, size_t) dest nullptr\n";
        }
        const char* s19 = nullptr;
        char* s20 = nullptr;
        char* s21 = strncpy(s20, s19, 3);
        if (s19 || s20 || s21)
        {
            ++fail_count;
            std::cout << "FAILED strncpy(char*, const char*, size_t) both nullptr\n";
        }
        const char* s22 = "hello";
        char s23[10] = "goodbye";
        char* s24 = strncpy(s23, s22, 0);
        if (strcmp(s22, "hello") != 0 || strcmp(s23, "goodbye") != 0 ||
            s23 != s24)
        {
            ++fail_count;
            std::cout << "FAILED strncpy(char*, const char*, size_t) zero\n";
        }
    }
    // strlen
    {
        const char* s1 = "hello";
        if (strlen(s1) != 5)
        {
            ++fail_count;
            std::cout << "FAILED strlen(const char*) basic\n";
        }
        const char* s2 = "";
        if (strlen(s2) != 0)
        {
            ++fail_count;
            std::cout << "FAILED strlen(const char*) empty\n";
        }
        const char* s3 = "he\0llo";
        if (strlen(s3) != 2)
        {
            ++fail_count;
            std::cout << "FAILED strlen(const char*) contains null\n";
        }
    }
    // memchr
    {
        char s1[6] = "hello";
        if (memchr(static_cast<void*>(s1), 'e', 5) !=
            static_cast<void*>(&s1[1]))
        {
            ++fail_count;
            std::cout << "FAILED memchr(void*, int, size_t) simple\n";
        }
        if (memchr(static_cast<void*>(s1), 'h', 5) !=
            static_cast<void*>(&s1[0]))
        {
            ++fail_count;
            std::cout << "FAILED memchr(void*, int, size_t) at start\n";
        }
        if (memchr(static_cast<void*>(s1), 'o', 5) !=
            static_cast<void*>(&s1[4]))
        {
            ++fail_count;
            std::cout << "FAILED memchr(void*, int, size_t) at end\n";
        }
        if (memchr(static_cast<void*>(s1), 'l', 5) !=
            static_cast<void*>(&s1[2]))
        {
            ++fail_count;
            std::cout << "FAILED memchr(void*, int, size_t) multiple occurence\n";
        }
        if (memchr(static_cast<void*>(s1), 'a', 5) != nullptr)
        {
            ++fail_count;
            std::cout << "FAILED memchr(void*, int, size_t) not present\n";
        }
        if (memchr(static_cast<void*>(s1), 'o', 4) != nullptr)
        {
            ++fail_count;
            std::cout << "FAILED memchr(void*, int, size_t) present after count\n";
        }
        if (memchr(static_cast<void*>(s1), 'h', 0) != nullptr)
        {
            ++fail_count;
            std::cout << "FAILED memchr(void*, int, size_t) zero count\n";
        }
        if (memchr(static_cast<void*>(nullptr), 'h', 0) != nullptr)
        {
            ++fail_count;
            std::cout << "FAILED memchr(void*, int, size_t) nullptr\n";
        }
        const char s2[6] = "hello";
        if (memchr(static_cast<const void*>(s2), 'e', 5) !=
            static_cast<const void*>(&s2[1]))
        {
            ++fail_count;
            std::cout << "FAILED memchr(const void*, int, size_t) simple\n";
        }
        if (memchr(static_cast<const void*>(s2), 'h', 5) !=
            static_cast<const void*>(&s2[0]))
        {
            ++fail_count;
            std::cout << "FAILED memchr(const void*, int, size_t) at start\n";
        }
        if (memchr(static_cast<const void*>(s2), 'o', 5) !=
            static_cast<const void*>(&s2[4]))
        {
            ++fail_count;
            std::cout << "FAILED memchr(const void*, int, size_t) at end\n";
        }
        if (memchr(static_cast<const void*>(s2), 'l', 5) !=
            static_cast<const void*>(&s2[2]))
        {
            ++fail_count;
            std::cout << "FAILED memchr(const void*, int, size_t) multiple occurence\n";
        }
        if (memchr(static_cast<const void*>(s2), 'a', 5) != nullptr)
        {
            ++fail_count;
            std::cout << "FAILED memchr(const void*, int, size_t) not present\n";
        }
        if (memchr(static_cast<const void*>(s2), 'o', 4) != nullptr)
        {
            ++fail_count;
            std::cout << "FAILED memchr(const void*, int, size_t) present after count\n";
        }
        if (memchr(static_cast<const void*>(s2), 'h', 0) != nullptr)
        {
            ++fail_count;
            std::cout << "FAILED memchr(const void*, int, size_t) zero count\n";
        }
        if (memchr(static_cast<const void*>(nullptr), 'h', 0) != nullptr)
        {
            ++fail_count;
            std::cout << "FAILED memchr(cont void*, int, size_t) nullptr\n";
        }
    }
    // memcmp
    {
        const char s1[] = "abc";
        const char s2[] = "abc";
        const char s3[] = "abd";
        const char s4[] = "def";
        const char s5[] = "";
        const char s6[] = "";
        if (memcmp(static_cast<const void*>(s1),
            static_cast<const void*>(s2), 3) != 0)
        {
            ++fail_count;
            std::cout << "FAILED memcmp(const void*, const void*, size_t) equal\n";
        }
        if (memcmp(static_cast<const void*>(s1),
            static_cast<const void*>(s4), 3) >= 0)
        {
            ++fail_count;
            std::cout << "FAILED memcmp(const void*, const void*, size_t) second greater\n";
        }
        if (memcmp(static_cast<const void*>(s4),
            static_cast<const void*>(s1), 3) <= 0)
        {
            ++fail_count;
            std::cout << "FAILED memcmp(const void*, const void*, size_t) second less\n";
        }
        if (memcmp(static_cast<const void*>(s5),
            static_cast<const void*>(s6), 0) != 0)
        {
            ++fail_count;
            std::cout << "FAILED memcmp(const void*, const void*, size_t) both empty\n";
        }
        if (memcmp(static_cast<const void*>(s1),
            static_cast<const void*>(s3), 2) != 0)
        {
            ++fail_count;
            std::cout << "FAILED memcmp(const void*, const void*, size_t) equal, limited\n";
        }
        if (memcmp(static_cast<const void*>(s1),
            static_cast<const void*>(s3), 0) != 0)
        {
            ++fail_count;
            std::cout << "FAILED memcmp(const void*, const void*, size_t) equal, zero length\n";
        }
    }
    // memcpy
    {
        const char* s1 = "hello";
        char s2[10];
        char* s3 = static_cast<char*>(memcpy(static_cast<void*>(s2),
            static_cast<const void*>(s1), 6));
        if (strcmp(s1, "hello") != 0 || strcmp(s2, "hello") != 0 || s2 != s3)
        {
            ++fail_count;
            std::cout << "FAILED memcpy(void*, const void*, size_t) simple\n";
        }
        const char* s4 = "hello";
        char s5[10];
        char* s6 = static_cast<char*>(memcpy(static_cast<void*>(s5),
            static_cast<const void*>(s4), 2));
        if (strcmp(s4, "hello") != 0 || strncmp(s5, "hello", 2) != 0 ||
            s5 != s6)
        {
            ++fail_count;
            std::cout << "FAILED memcpy(void*, const void*, size_t) partial\n";
        }
        const char* s7 = "he\0ll\0o";
        char s8[10];
        char* s9 = static_cast<char*>(memcpy(static_cast<void*>(s8),
            static_cast<const void*>(s7), 8));
        if (memcmp(s7, "he\0ll\0o", 8) != 0 ||
            memcmp(s8, "he\0ll\0o", 8) != 0 || s8 != s9)
        {
            ++fail_count;
            std::cout << "FAILED memcpy(void*, const void*, size_t) with nulls\n";
        }
        const char* s10 = "hello";
        void* s11 = nullptr;
        char* s12 = static_cast<char*>(memcpy(s11,
            static_cast<const void*>(s10), 6));
        if (strcmp(s10, "hello") != 0 || s11 || s12)
        {
            ++fail_count;
            std::cout << "FAILED memcpy(void*, const void*, size_t) dest null\n";
        }
        const void* s13 = nullptr;
        char s14[10] = "hello";
        char* s15 = static_cast<char*>(memcpy(static_cast<void*>(s14), s13, 6));
        if (strcmp(s14, "hello") != 0 || s13 || s15)
        {
            ++fail_count;
            std::cout << "FAILED memcpy(void*, const void*, size_t) src null\n";
        }
        const void* s16 = nullptr;
        void* s17 = nullptr;
        char* s18 = static_cast<char*>(memcpy(s17, s16, 6));
        if (s16 || s17 || s18)
        {
            ++fail_count;
            std::cout << "FAILED memcpy(void*, const void*, size_t) both null\n";
        }
        const char* s19 = "hello";
        char s20[10] = "goodbye";
        char* s21 = static_cast<char*>(memcpy(static_cast<void*>(s20),
            static_cast<const void*>(s19), 0));
        if (strcmp(s19, "hello") != 0 || strcmp(s20, "goodbye") != 0 ||
            s21 != s20)
        {
            ++fail_count;
            std::cout << "FAILED memcpy(void*, const void*, size_t) zero count\n";
        }
    }
    // memmove
    {
        const char* s1 = "hello";
        char s2[10];
        char* s3 = static_cast<char*>(memmove(static_cast<void*>(s2),
            static_cast<const void*>(s1), 6));
        if (strcmp(s1, "hello") != 0 || strcmp(s2, "hello") != 0 || s2 != s3)
        {
            ++fail_count;
            std::cout << "FAILED memmove(void*, const void*, size_t) simple\n";
        }
        const char* s4 = "hello";
        char s5[10];
        char* s6 = static_cast<char*>(memmove(static_cast<void*>(s5),
            static_cast<const void*>(s4), 2));
        if (strcmp(s4, "hello") != 0 || strncmp(s5, "hello", 2) != 0 ||
            s5 != s6)
        {
            ++fail_count;
            std::cout << "FAILED memmove(void*, const void*, size_t) partial\n";
        }
        const char* s7 = "he\0ll\0o";
        char s8[10];
        char* s9 = static_cast<char*>(memmove(static_cast<void*>(s8),
            static_cast<const void*>(s7), 8));
        if (memcmp(s7, "he\0ll\0o", 8) != 0 ||
            memcmp(s8, "he\0ll\0o", 8) != 0 || s8 != s9)
        {
            ++fail_count;
            std::cout << "FAILED memmove(void*, const void*, size_t) with nulls\n";
        }
        const char* s10 = "hello";
        void* s11 = nullptr;
        char* s12 = static_cast<char*>(memmove(s11,
            static_cast<const void*>(s10), 6));
        if (strcmp(s10, "hello") != 0 || s11 || s12)
        {
            ++fail_count;
            std::cout << "FAILED memmove(void*, const void*, size_t) dest null\n";
        }
        const void* s13 = nullptr;
        char s14[10] = "hello";
        char* s15 = static_cast<char*>(memmove(static_cast<void*>(s14), s13, 6));
        if (strcmp(s14, "hello") != 0 || s13 || s15)
        {
            ++fail_count;
            std::cout << "FAILED memmove(void*, const void*, size_t) src null\n";
        }
        const void* s16 = nullptr;
        void* s17 = nullptr;
        char* s18 = static_cast<char*>(memmove(s17, s16, 6));
        if (s16 || s17 || s18)
        {
            ++fail_count;
            std::cout << "FAILED memmove(void*, const void*, size_t) both null\n";
        }
        const char* s19 = "hello";
        char s20[10] = "goodbye";
        char* s21 = static_cast<char*>(memmove(static_cast<void*>(s20),
            static_cast<const void*>(s19), 0));
        if (strcmp(s19, "hello") != 0 || strcmp(s20, "goodbye") != 0 ||
            s21 != s20)
        {
            ++fail_count;
            std::cout << "FAILED memmove(void*, const void*, size_t) zero count\n";
        }
        char s22[10] = "hello";
        char* s23 = static_cast<char*>(memmove(static_cast<void*>(&s22[2]),
            static_cast<const void*>(s22), 6));
        if (strcmp(s22, "hehello") != 0 || strcmp(s23, "hello") != 0 ||
            s23 != &s22[2])
        {
            ++fail_count;
            std::cout << "FAILED memmove(void*, const void*, size_t) overlap end\n";
        }
        char s24[10] = "\0\0hello";
        char* s25 = static_cast<char*>(memmove(static_cast<void*>(s24),
            static_cast<const void*>(&s24[2]), 5));
        if (strcmp(s24, "hellolo") != 0 || strcmp(s25, "hellolo") != 0 ||
            s24 != s25)
        {
            ++fail_count;
            std::cout << "FAILED memmove(void*, const void*, size_t) overlap start\n";
        }
    }
    // memset
    {
        char s1[10] = "hello";
        char* s2 = static_cast<char*>(memset(s1, 'a', 5));
        if (strcmp(s1, "aaaaa") != 0 || s1 != s2)
        {
            ++fail_count;
            std::cout << "FAILED memset(void*, int, size_t) simple\n";
        }
        char s3[10] = "hello";
        char* s4 = static_cast<char*>(memset(s3, 'a', 0));
        if (strcmp(s3, "hello") != 0 || s3 != s4)
        {
            ++fail_count;
            std::cout << "FAILED memset(void*, int, size_t) zero count\n";
        }
        char* s5 = nullptr;
        char* s6 = static_cast<char*>(memset(s5, 'a', 5));
        if (s5 || s6)
        {
            ++fail_count;
            std::cout << "FAILED memset(void*, int, size_t) nullptr\n";
        }
    }

    // End
    if (fail_count == 0)
        std::cout << "All tests passed\n";
    else
        std::cout << fail_count << " tests failed\n";
}
