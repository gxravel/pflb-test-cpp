#include <iostream>
#include <fstream>
#include <regex>
#include <sstream>
#include <ctime>
#include <iomanip>
#include <array>

using namespace std;

array<string, 2> actions{"top up", "scoop"};
array<string, 2> status{"фейл", "успех"};

void show_usage()
{
    cerr << "Usage: \n\n\t./task3.exe <log_path> <period_start> <period_end>\n\n"
         << "где\n\n\tlog_path\tпуть к лог файлу\n"
         << "\tperiod_start\tначало желаемого периода\n"
         << "\tperiod_end\tконец желаемого периода\n\n"
         << "Пример лог файла:\n\n"
         << "META DATA:\n200 (объем бочки)\n32 (текущий объем воды в бочке)\n"
         << "2020-01-01Т12:51:32 – [username1] - wanna top up 10l (успех)\n"
         << "2020-01-01Т12:51:34 – [username2] - wanna scoop 50l (фейл)\n\n";
}

tm get_tm(const string &s)
{
    tm t{};
    istringstream ss(s);

    ss >> get_time(&t, "%Y-%m-%dT%H:%M:%S");
    if (ss.fail())
        throw invalid_argument("wrong time format");

    return t;
}

int determine_period_state(const string &ts, time_t s1, time_t s2)
{
    tm t = get_tm(ts);
    auto s = mktime(&t);
    if (s < s1)
        return -1;
    if (s > s2)
        return 1;
    return 0;
}

int main(int argc, char *argv[])
{
    if (argc < 4)
    {
        show_usage();
        return 1;
    }
    const string log_path = argv[1];
    const string period_start = argv[2];
    const string period_end = argv[3];
    tm start_tm, end_tm;
    try
    {
        start_tm = get_tm(period_start);
        end_tm = get_tm(period_end);
    }
    catch (const exception &e)
    {
        cerr << e.what() << '\n';
        show_usage();
        return 1;
    }
    time_t start_s = mktime(&start_tm);
    time_t end_s = mktime(&end_tm);

    ifstream log_file(argv[1]);
    if (!log_file.is_open())
    {
        show_usage();
        return 1;
    }
    string line;
    int barrel_volume, current_volume;
    getline(log_file, line);
    try
    {
        getline(log_file, line);
        barrel_volume = stoi(line);
        getline(log_file, line);
        current_volume = stoi(line);
    }
    catch (const exception &e)
    {
        cerr << e.what() << '\n';
        show_usage();
        log_file.close();
        return 1;
    }

    regex rgx_log(R"(([0-9-]+T[0-9:.Z]+) - \[.+\] - wanna (.+) ([0-9]+)l \((.+)\))");
    smatch match;
    int topup_try_number = 0, topup_fail_number = 0, topup_volume = 0, topup_volume_failed = 0;
    int scoop_try_number = 0, scoop_fail_number = 0, scoop_volume = 0, scoop_volume_failed = 0;
    int volume_start = current_volume;
    int volume_end = 0;
    bool is_in_past = true;
    bool at_least_one_log = false;
    bool is_start_in_past = true;

    while (getline(log_file, line))
    {
        const string s = line;
        if (!regex_search(s.begin(), s.end(), match, rgx_log))
        {
            show_usage();
            log_file.close();
            return 1;
        }
        bool is_topup = match[2] == actions[0];
        bool is_success = match[4] == status[1];
        int litres, period_state;
        try
        {
            litres = stoi(match[3]);
            period_state = determine_period_state(match[1], start_s, end_s);
        }
        catch (const exception &e)
        {
            cerr << e.what() << '\n';
            show_usage();
            log_file.close();
            return 1;
        }

        if (period_state == -1)
        {
            is_start_in_past = false;
            if (is_success)
            {
                if (is_topup)
                    volume_start += litres;
                else
                    volume_start -= litres;
            }
        }
        else if (period_state == 0)
        {
            if (!at_least_one_log)
            {
                volume_end = volume_start;
                at_least_one_log = true;
            }
            if (is_topup)
            {
                topup_try_number++;
                if (is_success)
                {
                    topup_volume += litres;
                    volume_end += litres;
                }
                else
                {
                    topup_fail_number++;
                    topup_volume_failed += litres;
                }
            }
            else
            {
                scoop_try_number++;
                if (is_success)
                {
                    scoop_volume += litres;
                    volume_end -= litres;
                }
                else
                {
                    scoop_fail_number++;
                    scoop_volume_failed += litres;
                }
            }
        }
        else if (period_state == 1)
        {
            break;
        }

        is_in_past = false;
    }

    log_file.close();
    ifstream read_file("./task3.csv");
    if (!read_file.is_open())
    {
        show_usage();
        return 1;
    }

    bool is_empty = read_file.peek() == ifstream::traits_type::eof();
    read_file.close();

    ofstream result_file("./task3.csv", ios_base::app);
    if (!result_file.is_open())
    {
        show_usage();
        return 1;
    }
    if (is_empty)
        result_file << "Начало периода,"
                    << "Конец периода,"
                    << "Количество попыток налить воду в бочку,"
                    << "Процент ошибок (налить воду),"
                    << "Объем воды был налит в бочку,"
                    << "Объем воды был не налит в бочку,"
                    << "Количество попыток зачерпнуть воду из бочки,"
                    << "Процент ошибок (зачерпнуть воду),"
                    << "Объем воды зачерпнули из бочки,"
                    << "Объем воды не зачерпнули из бочки,"
                    << "Объем воды в начале периода,"
                    << "Объем воды в конце периода\n";

    if (is_in_past)
    {
        volume_start = 0;
        volume_end = 0;
    }
    else if (is_start_in_past)
    {
        volume_start = 0;
    }
    else if (!at_least_one_log)
    {
        volume_end = volume_start;
    }

    int topup_fail_percentage = (topup_try_number != 0) ? topup_fail_number * 100.0 / topup_try_number : 0;
    int scoop_fail_percentage = (scoop_try_number != 0) ? scoop_fail_number * 100.0 / scoop_try_number : 0;

    result_file << period_start << ','
                << period_end << ','
                << topup_try_number << ','
                << topup_fail_percentage << ','
                << topup_volume << ','
                << topup_volume_failed << ','
                << scoop_try_number << ','
                << scoop_fail_percentage << ','
                << scoop_volume << ','
                << scoop_volume_failed << ','
                << volume_start << ','
                << volume_end << '\n';

    result_file.close();
}