#include <iostream>
#include <fstream>
#include <chrono>
#include <random>
#include <array>
#include <sstream>
#include <ctime>
#include <iomanip>

using namespace std;

const string log_path = "./log.log";
const int barrel_volume = 200;
const int current_volume = 50;
const int n_users = 5;
const int min_seconds_difference = 1;
const int max_seconds_difference = 3600 * 6;
const int iterations = (1 << 20) / 60;
const string period_start = "2021-02-09T12:00:00";
array<string, n_users> users{"rabbit", "wolf", "duck", "donkey", "monkey"};
array<string, 2> actions{"wanna top up", "wanna scoop"};
array<string, 2> status{"фейл", "успех"};

tm get_tm(const string &s)
{
    tm t{};
    istringstream ss(s);

    ss >> get_time(&t, "%Y-%m-%dT%H:%M:%S");
    if (ss.fail())
        throw invalid_argument("wrong time format");

    return t;
}

bool is_in_period(tm t, tm t1, tm t2)
{
    auto s = mktime(&t);
    auto s1 = mktime(&t1);
    auto s2 = mktime(&t2);
    if (s < s1 || s > s2)
        return false;

    return true;
}

int main(int argc, char *argv[])
{
    ofstream log_file(log_path);
    if (!log_file.is_open())
        return 1;

    log_file << "META DATA:\n"
             << barrel_volume << " (объем бочки)\n"
             << current_volume << " (текущий объем воды в бочке)\n";

    random_device rd;
    mt19937 rng(rd());
    uniform_int_distribution<int> dist_seconds(min_seconds_difference, max_seconds_difference);
    uniform_int_distribution<int> dist_users(0, n_users - 1);
    uniform_int_distribution<int> dist_action(0, 1);
    uniform_int_distribution<int> dist_litres(1, barrel_volume / 2);

    tm t;
    try
    {
        t = get_tm(period_start);
    }
    catch (const exception &e)
    {
        cerr << e.what() << "\n";
        return 1;
    }
    time_t ts = mktime(&t);
    int current = current_volume;
    int stat, temp;

    for (int i = 0; i < iterations; i++)
    {
        ts += dist_seconds(rng);
        tm *t_upd = localtime(&ts);
        int action = dist_action(rng);
        int litres = dist_litres(rng);
        log_file << put_time(t_upd, "%Y-%m-%dT%H:%M:%S")
                 << " - [" << users[dist_users(rng)] << "] - "
                 << actions[action] << " "
                 << litres << "l";

        stat = 1;
        if (action == 0)
        {
            temp = current + litres;
            if (temp > barrel_volume)
                stat = 0;
        }
        else
        {
            temp = current - litres;
            if (temp < 0)
                stat = 0;
        }
        if (stat == 1)
            current = temp;

        log_file << " (" << status[stat] << ")\n";
    }

    log_file.close();
}