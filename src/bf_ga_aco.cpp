#include <list>
#include <iostream>
#include <cmath>
#include <vector>
#include <random>
#include<unordered_map>
#include <map>
#include <ctime>


struct Solve
{
    int number_of_nodes = 0;
    std::vector<int> nodes;
    std::vector<std::vector<int>> matrix;
    std::vector<int> min_path;
    int min = std::numeric_limits<int>::max();
    Solve() {}
    void set_matrix()
    {
        nodes.resize(number_of_nodes);
        for (size_t i = 0; i < number_of_nodes; ++i) nodes[i] = i;
        matrix.resize(number_of_nodes);
        for (size_t i = 0; i < number_of_nodes; ++i) matrix[i].resize(number_of_nodes);
        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_real_distribution<> dist(1, 10);
        for (size_t i = 0; i < number_of_nodes; ++i)
        {
            for (size_t j = 0; j < number_of_nodes; ++j)
            {
                if (i != j)
                {

                    matrix[i][j] = dist(gen);
                    matrix[j][i] = matrix[i][j];
                }
                else matrix[i][j] = 0;
            }
        }
    }
    void show_matrix()
    {
        for (const auto& i : matrix)
        {
            for (const auto& j : i) std::cout << j << " ";
            std::cout << std::endl;
        }
    }
    void show_solve_path()
    {
        std::cout << std::endl << "solve path: ";
        for (const auto& i : min_path) std::cout << i << " ";
        std::cout << std::endl;
    }
    virtual void solve_path(int number_of_start_node) = 0;
};

struct Brut_force : Solve
{
    std::list<std::vector<int>> paths;
    std::vector<int> perm;
    std::vector<int> perm_nodes; //????? ??? ????????, ????? ?????????, ????? ?? ???? ? ??? ????????????, ? ?????? ???????????? ?? ????????? ??????? 
    Brut_force(int number_of_nodes_)
    {
        number_of_nodes = number_of_nodes_;
        set_matrix();
    }
    void permutation(std::vector<int>& perm_nodes, std::vector<int>& perm, std::list<std::vector<int>>& paths) //????? nodes ??? perm_nodes
    {
        if (perm_nodes.empty())
        {
            paths.push_back(perm);
        }
        else
        {
            for (int i = 0; i < perm_nodes.size(); ++i)
            {

                std::vector<int> perm_nodes_copy = perm_nodes;
                perm_nodes_copy.erase(perm_nodes_copy.begin() + i);
                std::vector<int> perm_copy = perm;
                perm_copy.push_back(perm_nodes[i]);
                permutation(perm_nodes_copy, perm_copy, paths);
            }
        }
    }
    void solve_path(int number_of_start_node) override
    {
        for (size_t i = 0; i < number_of_nodes; ++i)
        {
            if (nodes[i] != number_of_start_node)
            {
                perm_nodes.push_back(nodes[i]);
            }
        }
        permutation(perm_nodes, perm, paths);
        for (auto& i : paths)
        {
            std::vector<int> tmp_vector(i.size() + 1);
            tmp_vector[0] = number_of_start_node;
            for (size_t j = 0; j < i.size(); ++j) tmp_vector[j + 1] = i[j];
            tmp_vector.push_back(number_of_start_node);
            i = std::move(tmp_vector);
        }
        for (auto& i : paths)
        {
            int tmp = 0;
            for (size_t j = 0; j + 1 < i.size(); ++j)
            {
                tmp += matrix[i[j]][i[j + 1]];
            }
            if (tmp < min and *i.begin() == number_of_start_node)
            {
                min = tmp;
                min_path = i;
            }
        }
    }
};

struct Greedy_Algorithm : Solve
{
    int n = 0;
    std::vector<int> visited;
    Greedy_Algorithm(int number_of_nodes_) : n(number_of_nodes_)
    {
        number_of_nodes = n;
        visited.resize(number_of_nodes, 0);

    }
    void solve_path(int number_of_start_node) override
    {
        min = 0;
        visited[number_of_start_node] = 1;
        int next = number_of_start_node;
        int k = 0;
        int tmp = 0;
        while (k < number_of_nodes)
        {
            int min_distance_to_near = std::numeric_limits<int>::max();
            tmp = next;
            for (size_t j = 0; j < number_of_nodes; ++j)
            {
                if (tmp != j and visited[j] == 0 and matrix[tmp][j] < min_distance_to_near)
                {
                    min_distance_to_near = matrix[tmp][j];
                    next = j;
                }
            }
            visited[tmp] = 1;
            min_path.push_back(tmp);
            min += matrix[next][tmp];
            ++k;
        }
        min_path.push_back(number_of_start_node);
        min += matrix[tmp][number_of_start_node];
    }
};

struct ACO : Solve
{
    const double k_start_pheramon = 0.01;
    const int Q = 100;
    const int time_max = 4;
    const double alpha = 1.0;
    const double betta = 1.0;
    int start = 0;
    const double p = 0.2;
    int ant_count;
    std::list<int> gPath;
    std::vector<std::vector<double>> pheramon_matrix;
    double min_path_length = std::numeric_limits<double>::max();
    ACO(int number_of_nodes_)
    {
        number_of_nodes = number_of_nodes_;
        set_matrix();
        pheramon_matrix.resize(number_of_nodes);
        for (auto& i : pheramon_matrix) i.resize(number_of_nodes, k_start_pheramon);
        ant_count = number_of_nodes;
    }
    void solve_path(int number_of_start_node) override
    {
        start = number_of_start_node;
        std::random_device rd;
        std::mt19937 e(rd());
        std::uniform_real_distribution<> dist(0, 1);
        for (size_t t = 1; t < time_max; ++t)
        {
            std::map<std::pair<int, int>, double> delta_pheramon;
            for (size_t ant = 0; ant < ant_count; ++ant)
            {
                double aLenght = 0.0;
                std::unordered_map<int, bool> visited;
                std::list<int> aPath; //???? ???????
                for (size_t i = 0; i < number_of_nodes; ++i) visited[i] = false;
                int visited_cnt = 0;
                int from = number_of_start_node; //???????? ?????
                while (visited_cnt < number_of_nodes)
                {
                    visited[from] = true;
                    aPath.push_back(from);
                    ++visited_cnt;
                    auto znam = 0.0;
                    for (size_t i = 0; i < matrix[from].size(); ++i)
                    {
                        if (!visited[i])
                        {
                            znam += std::pow((1.0 / matrix[from][i]), betta) * std::pow(pheramon_matrix[from][i], alpha);
                        }
                    }
                    std::map<std::pair<int, int>, double> Prob;
                    for (size_t i = 0; i < matrix[from].size(); ++i)
                    {
                        if (!visited[i]) Prob[std::make_pair(from, i)] = std::pow((1.0 / matrix[from][i]), betta) * std::pow(pheramon_matrix[from][i], alpha) / znam;
                    }
                    std::map<std::pair<int, int>, double> Pchoice;

                    auto prev = -1.0;
                    for (auto& it : Prob)
                    {
                        if (prev == -1.0) Pchoice[it.first] = Prob[it.first];
                        else Pchoice[it.first] = Prob[it.first] + prev;
                        prev = Pchoice[it.first];
                    }
                    /*std::cout << "\nPchoice:\n";
                    for (const auto& it : Pchoice) std::cout << it.first.first << " -> " << it.first.second << "    " << it.second << std::endl;*/
                    auto random_choice = dist(e);
                    for (auto& it : Pchoice)
                    {
                        if (random_choice <= it.second)
                        {
                            from = it.first.second;
                            break;
                        }
                    }

                }
                aPath.push_back(start);
                for (auto it = aPath.begin(); it != std::prev(aPath.end()); ++it)
                {
                    aLenght += matrix[*it][*std::next(it)];
                }
                for (auto it = aPath.begin(); it != std::prev(aPath.end()); ++it)
                {
                    delta_pheramon[std::make_pair(*it, *std::next(it))] += Q / aLenght;
                }
                if (aLenght < min_path_length)
                {
                    min_path_length = aLenght;
                    gPath = std::move(aPath);
                }
            }
            for (auto& it : delta_pheramon)
            {
                pheramon_matrix[it.first.second][it.first.first] += it.second;
                pheramon_matrix[it.first.first][it.first.second] += it.second;
            }

        }
        for (const auto& i : gPath)
        {
            min_path.push_back(i);
        }
        min = min_path_length;
    }
};

int main()
{
    int n;
    int start;
    std::cout << "Enter the number of nodes\n";
    std::cin >> n;
    std::cout << "Enter the start node\n";
    std::cin >> start;
    auto start_time1 = clock();
    Brut_force A(n);
    std::cout << "matrix:\n";
    A.show_matrix();
    A.solve_path(start);
    auto end_time1 = clock();
    auto start_time2 = clock();
    Greedy_Algorithm B(n);
    B.matrix = A.matrix;
    B.solve_path(start);
    auto end_time2 = clock();
    auto start_time3 = clock(); 
    ACO C(n);
    C.matrix = A.matrix;
    C.solve_path(start);
    auto end_time3 = clock();
    std::cout << "\nBF:";
    A.show_solve_path();
    std::cout << "length: " << A.min << std::endl;
    std::cout << "time in nanosecond(1*10**-9second): " << end_time1 - start_time1 << std::endl;
    std::cout << "\nGA:";
    B.show_solve_path();
    std::cout << "length: " << B.min << std::endl;
    std::cout << "time in nanosecond(1*10**-9second): " << end_time2 - start_time2 << std::endl;
    std::cout << "\nACO:";
    C.show_solve_path();
    std::cout << "length: " << C.min << std::endl;
    std::cout << "time in nanosecond(1*10**-9second): " << end_time3 - start_time3 << std::endl;
}
