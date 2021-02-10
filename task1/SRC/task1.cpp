#include <iostream>
#include <string>
#include <set>
#include <regex>
#include <cmath>
#include <fcntl.h>
#include <io.h>
#include <algorithm>

using namespace std;

const int max_base_length = 10 + 26 + 33; // = 69 (digits, english and russian letters)

void show_usage()
{
    wcerr << L"Конвертирует число (без знака) из одной системы счисления в любую другую.\n\nUsage:\n\n"
          << L"\t ./task1.exe <nb> [base_src = 0123456789] <base_dst>\n"
          << L"\nгде\n\n"
          << L"\tnb\tподаваемое число [0; 4'294'967'295]\n"
          << L"\tbase\tсистема счисления.\n\n"
          << L"Формат системы счисления:\n\n"
          << L"\t01 - двоичная, 012 - троичная, 0123456789abcdef - шестнадцатеричная,\n"
          << L"\tкотики - система счисления в котиках.\n\nПримечания:\n\n"
          << L"\tПрограмма не чувствительна к регистру.\n"
          << L"\tВ качестве алфавита могут быть использованы цифры, латинские знаки и кириллица.\n"
          << L"\tТаким образом, максимальный размер системы счисления - 69.\n"
          << L"\tПовторные значения системы счисления отбрасываются.\n"
          << L"\tnb может содержать только значения из base_src.\n\n";
}

bool is_format_ok(const wstring &base)
{
    const wregex format(L"[0-9a-zA-Zа-яА-ЯёЁ]{1," + to_wstring(max_base_length) + L'}');
    return regex_match(base, format);
}

bool is_nb_correct(const wstring &nb, const wstring &alpha_src)
{
    int n_alpha = alpha_src.length();
    int max_nb_length = floor(log(double(UINT_MAX)) / log(double(n_alpha))) + 1;

    if (nb.length() > max_nb_length)
        return false;

    wregex only_alpha(L'[' + alpha_src + L"]+");
    return regex_match(nb, only_alpha);
}

void check_nb_boundaries(long long nb)
{
    if (nb < 0 || nb > UINT_MAX)
        throw exception();
}

wstring to_lower(const wstring &s)
{
    wstring result = s;
    transform(result.begin(), result.end(), result.begin(), towlower);
    return result;
}

wstring get_alphabet(const wstring &base)
{
    wstring base_lower = to_lower(base);
    set<wchar_t> alpha_set(base_lower.begin(), base_lower.end());
    if (alpha_set.size() == base_lower.length())
    {
        return base_lower;
    }
    wstring result;
    vector<int> found_first_indexes;
    for (const wchar_t &c : alpha_set)
    {
        found_first_indexes.push_back(base_lower.find_first_of(c));
    }
    sort(found_first_indexes.begin(), found_first_indexes.end());
    for (const int &i : found_first_indexes)
    {
        result.push_back(base_lower.at(i));
    }
    return result;
}

unsigned to_base10(const wstring &nb, const wstring &alpha_src)
{
    long long nb_long = 0;
    int n_alpha = alpha_src.length();
    int counter = nb.length();
    for (const wchar_t &c : nb)
    {
        counter--;
        int i = alpha_src.find_first_of(c);
        nb_long += i * pow(n_alpha, counter);
    }
    check_nb_boundaries(nb_long);

    return unsigned(nb_long);
}

wstring from_base10(unsigned nb, const wstring &alpha)
{
    int n_alpha = alpha.length();
    wstring result;
    do
    {
        int remainder = nb % n_alpha;
        nb /= n_alpha;
        result.insert(result.begin(), alpha[remainder]);
    } while (nb > 0);

    return result;
}

wstring i_to_base(unsigned nb, const wstring &base)
{
    wstring alpha = get_alphabet(base);
    return from_base10(nb, alpha);
}

wstring i_to_base(const wstring &nb, const wstring &base_src, const wstring &base_dst)
{
    wstring nb_lower = to_lower(nb);
    wstring alpha_src = get_alphabet(base_src);

    if (!is_nb_correct(nb_lower, alpha_src))
        throw exception();

    wstring alpha_dst = get_alphabet(base_dst);
    if (alpha_src == alpha_dst)
        return nb_lower;

    unsigned nb_uint = to_base10(nb_lower, alpha_src);
    return from_base10(nb_uint, alpha_dst);
}

int wmain(int argc, wchar_t *argv[])
{
    setlocale(LC_ALL, "rus");
    _setmode(_fileno(stdout), _O_U16TEXT);
    _setmode(_fileno(stdin), _O_U16TEXT);
    _setmode(_fileno(stderr), _O_U16TEXT);
    wstring result;
    if (argc < 3)
    {
        show_usage();
        return 1;
    }
    else if (argc == 3)
    {
        long long nb_long;
        try
        {
            nb_long = stoll(argv[1]);
            check_nb_boundaries(nb_long);
        }
        catch (const exception &e)
        {
            show_usage();
            return 1;
        }

        if (!is_format_ok(argv[2]))
        {
            show_usage();
            return 1;
        }

        unsigned nb = nb_long;
        result = i_to_base(nb, argv[2]);
    }
    else
    {
        if (!is_format_ok(argv[1]) || !is_format_ok(argv[2]) || !is_format_ok(argv[3]))
        {
            show_usage();
            return 1;
        }

        try
        {
            result = i_to_base(argv[1], argv[2], argv[3]);
        }
        catch (const exception &e)
        {
            show_usage();
            return 0;
        }
    }
    wcout << result << L'\n';
    return 0;
};