#include <iostream>
#include <fstream>
#include <string>
#include <regex>
#include <cmath>

using namespace std;

void show_usage()
{
    cerr << "Находит точки столкновения сферы и прямой линии. Если их нет, то выводится фраза: \"Коллизий не найдено\";\n"
         << "если есть, то выводятся координаты точек, ограниченные символом новой строки. \n\n"
         << "Координаты считываются из файла, который имеет следующий формат:\n\n"
         << "\t{sphere: {center: [0, 0, 0], radius: 10.67}, line: {[1, 0.5, 15], [43, -14.6, 0.04]}}\n\n"
         << "Примечание: объекты и ключи могут находится в свободной последовательности.\n\n"
         << "Usage: \n\n\t./task2.exe <filepath>\n\n";
}

struct Point
{
    double x, y, z;
};

struct Sphere
{
    Point center;
    double radius;
};

struct Line
{
    Point p1, p2;
};

smatch extract_with_regex(const string &data, const regex &rgx)
{
    smatch match;
    if (!regex_search(data.begin(), data.end(), match, rgx))
        throw exception();

    return match;
}

void print_point(const Point &p, bool double_new_line = false)
{
    cout << p.x << ", " << p.y << ", " << p.z << '\n';
    if (double_new_line)
        cout << '\n';
}

Point calculate_point(const Line &line, double u)
{
    Point point;
    point.x = line.p1.x + u * (line.p2.x - line.p1.x);
    point.y = line.p1.y + u * (line.p2.y - line.p1.y);
    point.z = line.p1.z + u * (line.p2.z - line.p1.z);
    return point;
}

int main(int argc, char *argv[])
{
    if (argc < 2)
    {
        show_usage();
        return 1;
    }
    ifstream file(argv[1]);
    if (!file.is_open())
    {
        show_usage();
        return 1;
    }
    string data;
    while (getline(file, data))
    {
        regex rgx_center(R"(.*center: \[([0-9+-.]+), ([0-9+-.]+), ([0-9+-.]+)\].*)");
        regex rgx_radius(R"(.*radius: ([0-9+-.]+)[},].*)");
        regex rgx_line(R"(.*line: \{\[([0-9+-.]+), ([0-9+-.]+), ([0-9+-.]+)\], \[([0-9+-.]+), ([0-9+-.]+), ([0-9+-.]+)\]\}.*)");
        smatch match;
        Sphere sphere;
        Line line;
        Point point;
        try
        {
            match = extract_with_regex(data, rgx_center);
            point.x = stod(match[1]);
            point.y = stod(match[2]);
            point.z = stod(match[3]);
            sphere.center = point;

            match = extract_with_regex(data, rgx_radius);
            sphere.radius = stod(match[1]);

            match = extract_with_regex(data, rgx_line);
            point.x = stod(match[1]);
            point.y = stod(match[2]);
            point.z = stod(match[3]);
            line.p1 = point;
            point.x = stod(match[4]);
            point.y = stod(match[5]);
            point.z = stod(match[6]);
            line.p2 = point;
        }
        catch (const exception &e)
        {
            file.close();
            show_usage();
            return 1;
        }

        double a = pow(line.p2.x - line.p1.x, 2) + pow(line.p2.y - line.p1.y, 2) + pow(line.p2.z - line.p1.z, 2);
        if (a == 0)
        {
            file.close();
            show_usage();
            return 1;
        }
        double b = 2 * ((line.p2.x - line.p1.x) * (line.p1.x - sphere.center.x) + (line.p2.y - line.p1.y) * (line.p1.y - sphere.center.y) + (line.p2.z - line.p1.z) * (line.p1.z - sphere.center.z));
        double c = pow(sphere.center.x, 2) + pow(sphere.center.y, 2) + pow(sphere.center.z, 2) + pow(line.p1.x, 2) + pow(line.p1.y, 2) + pow(line.p1.z, 2) - 2 * (sphere.center.x * line.p1.x + sphere.center.y * line.p1.y + sphere.center.z * line.p1.z) - pow(sphere.radius, 2);

        double root = b * b - 4 * a * c;
        double u;

        if (root < 0)
        {
            cout << "Коллизий не найдено\n\n";
        }
        else if (root == 0)
        {
            u = -b / 2 / a;
            point = calculate_point(line, u);
            print_point(point, true);
        }
        else
        {
            u = (-b + sqrt(root)) / 2 / a;
            point = calculate_point(line, u);
            print_point(point);

            u = (-b - sqrt(root)) / 2 / a;
            point = calculate_point(line, u);
            print_point(point, true);
        }
    }

    file.close();
}