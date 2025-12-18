#include <algorithm>
#include <cstdio> // fopen, fscanf, fclose
#include <fstream>
#include <iostream>
#include <limits>
#include <numeric>
#include <ranges>
#include <sstream>
#include <stdexcept>
#include <unordered_map>
#include <vector>

using namespace std;

// ====================
//  1 zadanie: 01.2024
// ====================

void zadanie1_a(vector< int > left, vector< int > right)
{
    sort(left.begin(), left.end());
    sort(right.begin(), right.end());

    auto distances =
        views::iota(0u, left.size()) | views::transform([&](int i) { return abs(left[i] - right[i]); }) | views::common;

    const auto total = accumulate(distances.begin(), distances.end(), 0);

    cout << "zadanie 01.2024 A = " << total << "\n";
}

void zadanie1_b(const vector< int >& left, const vector< int >& right)
{
    unordered_map< int, int > right_counts;
    ranges::for_each(right, [&](int num) { right_counts[num]++; });

    int similarity_score =
        accumulate(left.begin(), left.end(), 0, [&](int acc, int num) { return acc + num * right_counts[num]; });

    cout << "zadanie 01.2024 B = " << similarity_score << "\n";
}

void adwent_01_2024()
{
    vector< int > left, right;
    int           a, b;

    FILE* file = nullptr;
    if (fopen_s(&file, "pierwszy_adwent.txt", "r") != 0 || !file)
        throw runtime_error("Nie mozna otworzyc pliku pierwszy_adwent.txt");

    while (fscanf_s(file, "%d %d", &a, &b) == 2)
    {
        left.push_back(a);
        right.push_back(b);
    }

    fclose(file);

    zadanie1_a(left, right);
    zadanie1_b(left, right);
}

// ====================
//  2 zadanie: 02.2024
// ====================

bool isSafe(const vector< int >& seq)
{
    auto diffs = seq | views::slide(2) | views::transform([](auto pair) { return pair[1] - pair[0]; });

    auto valid = [](int d) {
        return abs(d) >= 1 && abs(d) <= 3;
    };

    if (!ranges::all_of(diffs, valid))
        return false;

    bool allPos = ranges::all_of(diffs, [](int d) { return d > 0; });
    bool allNeg = ranges::all_of(diffs, [](int d) { return d < 0; });

    return allPos || allNeg;
}

void zadanie2_a(const vector< vector< int > >& input)
{
    auto safe_counter = ranges::count_if(input, [](const auto& levels) { return isSafe(levels); });

    cout << "zadanie 02.2024 A = " << safe_counter << "\n";
}

void zadanie2_b(const vector< vector< int > >& input)
{
    auto safe_counter = ranges::count_if(input, [](const auto& levels) {
        if (isSafe(levels))
            return true;

        return ranges::any_of(views::iota(0u, levels.size()), [&](int i) {
            vector< int > reduced;
            ranges::copy(views::iota(0u, levels.size()) | views::filter([&](int j) { return j != i; }) |
                             views::transform([&](int j) { return levels[j]; }),
                         back_inserter(reduced));
            return isSafe(reduced);
        });
    });

    cout << "zadanie 02.2024 B = " << safe_counter << "\n";
}

void adwent_02_2024()
{
    ifstream file("drugi_adwent.txt");
    if (!file.is_open())
        throw runtime_error("Nie mozna otworzyc pliku drugi_adwent.txt");

    string                  line;
    vector< vector< int > > input_zad_2;
    vector< int >           levels;

    while (getline(file, line))
    {
        if (line.empty())
            continue;
        stringstream ss(line);
        int          x;
        while (ss >> x)
            levels.push_back(x);
        input_zad_2.push_back(levels);
        levels.clear();
    }

    zadanie2_a(input_zad_2);
    zadanie2_b(input_zad_2);
}

// ====================
//  3 zadanie: 13.2024
// ====================

struct Machine
{
    int       Ax, Ay;
    int       Bx, By;
    long long Px, Py;
};

vector< int > parseLine(const string& line)
{
    auto rng =
        line | views::transform([](unsigned char c) { return (isdigit(c) || c == '+' || c == '-') ? char(c) : ' '; });

    string cleaned;
    ranges::copy(rng, back_inserter(cleaned));

    vector< int > nums;
    stringstream  ss(cleaned);
    int           x;
    while (ss >> x)
        nums.push_back(x);

    return nums;
}

long long solveMachine(const Machine& m)
{
    long long det = 1LL * m.Ax * m.By - 1LL * m.Ay * m.Bx;

    if (det != 0)
    {
        long long a_num = 1LL * m.Px * m.By - 1LL * m.Py * m.Bx;
        long long b_num = 1LL * m.Ax * m.Py - 1LL * m.Ay * m.Px;

        if (a_num % det != 0 || b_num % det != 0)
            throw logic_error("Brak całkowitego rozwiązania dla maszyny");

        long long a = a_num / det;
        long long b = b_num / det;
        if (a < 0 || b < 0)
            throw logic_error("Rozwiązanie ujemne dla maszyny");

        return 3 * a + b;
    }
    else
    {
        if (1LL * m.Px * m.Ay != 1LL * m.Py * m.Ax)
            throw logic_error("Brak rozwiązania w przypadku osobliwym");

        long long t = (m.Ax != 0) ? m.Px / m.Ax : m.Py / m.Ay;
        long long k = (m.Ax != 0) ? m.Bx / m.Ax : m.By / m.Ay;

        auto costs = views::iota(0LL, t + 1) | views::transform([&](long long b) {
                         long long a = t - k * b;
                         return (a < 0) ? numeric_limits< long long >::max() : 3 * a + b;
                     });

        return ranges::min(costs);
    }
}

void zadanie3_a(const vector< Machine >& machines)
{
    long long totalTokens = 0;
    for (const auto& m : machines)
    {
        try
        {
            totalTokens += solveMachine(m);
        }
        catch (const exception&)
        {
            //  maszyny bez rozwiązania
        }
    }
    cout << "zadanie 13.2024 A = " << totalTokens << "\n";
}

void zadanie3_b(const vector< Machine >& machines)
{
    long long totalTokens = 0;
    for (auto m : machines)
    {
        m.Px += 10000000000000LL;
        m.Py += 10000000000000LL;
        try
        {
            totalTokens += solveMachine(m);
        }
        catch (const exception&)
        {
            // maszyny bez rozwiązania
        }
    }
    cout << "zadanie 13.2024 B = " << totalTokens << "\n";
}

void adwent_13_2024()
{
    ifstream file("trzeci_adwent.txt");
    if (!file.is_open())
        throw runtime_error("Nie mozna otworzyc pliku trzeci_adwent.txt");

    string            line;
    vector< Machine > machines;

    while (true)
    {
        string aLine, bLine, pLine;
        if (!getline(file, aLine))
            break;
        getline(file, bLine);
        getline(file, pLine);

        auto Avals = parseLine(aLine);
        auto Bvals = parseLine(bLine);
        auto Pvals = parseLine(pLine);

        machines.push_back(Machine{Avals[0], Avals[1], Bvals[0], Bvals[1], Pvals[0], Pvals[1]});

        getline(file, bLine); // pusta linia
    }

    
    zadanie3_a(machines);
    zadanie3_b(machines);
}

// ====================
//  main
// ====================

int main()
{
    try
    {
        adwent_01_2024();
        cout << "\n";
        adwent_02_2024();
        cout << "\n";
        adwent_13_2024();
    }
    catch (const exception& e)
    {
        cerr << "Wystąpił wyjątek: " << e.what() << "\n";
        return 1; 
    }
    return 0; 
}