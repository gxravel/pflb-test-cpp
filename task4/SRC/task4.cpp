#include <iostream>
#include <string>
#include <set>
#include <regex>
#include <cmath>
#include <fcntl.h>
#include <io.h>
#include <algorithm>

using namespace std;

void show_usage()
{
    wcerr << L"Сравнивает 2 строки: одинаковые ли они. Результат: вывод «ОК», если строки идентичны, «КО», если не идентичны\n\nUsage:\n\n"
          << L"\t ./task4.exe <string1> <string2>\n"
          << L"\nПримечания:\n\n"
          << L"\tВо второй строке может быть символ ‘*’ –он заменяет собой любое количество любых символов.\n\n";
}

int wmain(int argc, wchar_t *argv[])
{
    setlocale(LC_ALL, "rus");
    _setmode(_fileno(stdout), _O_U16TEXT);
    _setmode(_fileno(stdin), _O_U16TEXT);
    _setmode(_fileno(stderr), _O_U16TEXT);
    if (argc < 3)
    {
        show_usage();
        return 1;
    }
    else
    {
        wstring s1 = argv[1];
        wstring s2 = argv[2];
        if (s2 == L"task4.cpp")
        {
            wcerr << "Problem with passing asterisk as an argument\n";
            return 1;
        }
        wregex rgx_asterisks(L"\\*{2,}");
        s2 = regex_replace(s2, rgx_asterisks, L"*");
        wchar_t asterisk = L'*';
        wstring t1 = s1;
        wstring t2 = s2;

        wstring t;
        bool first_iteration = true;
        int i, shift;
        int i_asterisk = t2.find(asterisk);
        bool has_asterisk = i_asterisk >= 0;
        while (has_asterisk)
        {
            int t2_len = t2.length();
            if (t2_len == 1)
            {
                wcout << L"OK\n";
                return 0;
            }
            if (i_asterisk == 0)
                t2 = t2.substr(1, t2_len - 1);
            else
            {
                t = t2.substr(0, i_asterisk);
                i = t1.find(t);

                if (i < 0 || first_iteration && i != 0)
                {
                    wcout << L"KO\n";
                    return 0;
                }
                shift = t.length();
                t1 = t1.substr(i + shift, t1.length() - shift - i);
                if (i_asterisk == t2_len - 1)
                {
                    wcout << L"OK\n";
                    return 0;
                }
                t2 = t2.substr(shift + 1, t2_len - shift - 1);
            }
            first_iteration = false;
            i_asterisk = t2.find(asterisk);
            has_asterisk = i_asterisk >= 0;
        }
        if (t1.find_last_of(t2) != t1.length() - 1)
        {
            wcout << L"KO\n";
            return 0;
        }
        wcout << L"OK\n";
    }
    return 0;
};