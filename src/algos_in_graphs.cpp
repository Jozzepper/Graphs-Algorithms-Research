#include <iostream>
#include <forward_list>
#include <unordered_map>
#include <vector>
#include <queue>
#include <stack>
#include <limits>
#include <algorithm>
#include <list>
#define INF 10000000


enum Color { White, Gray, Black };


struct Node {
	std::string name;
	Node* prev = nullptr;
	int distance = 0;
	Node() = default;
	Node(std::string _name) : name(_name) {
	}
	bool operator == (const Node& other) const {
		return name == other.name;
	}
	size_t metka = std::numeric_limits<size_t>::max();
	int input_time = 0;
	int output_time = 0;
	int color = White;
};



void BFS(std::unordered_map<Node*, std::forward_list<Node*>>& G, Node& start)
{
	start.color = Gray;
	std::vector<std::pair<Node*, Node*>> parents;
	std::queue<Node*> q;
	start.distance = 0;
	q.push(&start);
	while (!q.empty())
	{
		auto top = q.front();
		if (top->name != start.name) parents.push_back(std::make_pair(top, top->prev));
		q.pop();
		std::cout << top->name << " ";
		for (auto& node : G[top])
		{
			if (node->color == White)
			{
				node->color = Gray;
				node->distance = top->distance + 1;
				node->prev = top;
				q.push(node);
			}
		}
		//std::cout << top->distance << std::endl;
		top->color = Black;
	}
	std::cout << std::endl;
	//for (const auto& i : parents) std::cout << i.first->name << " : " << i.second->name << std::endl;
}


void DFS(std::unordered_map<Node*, std::forward_list<Node*>> G, Node& start)
{
	start.color = Gray;
	++start.input_time;
	std::stack<Node*> s;
	s.push(&start);
	while (!s.empty())
	{
		auto top = s.top();
		s.pop();
		std::cout << top->name << " ";
		for (auto& node : G[top])
		{
			if (node->color == White)
			{
				s.push(node);
				node->color = Gray;
				++node->input_time;
			}
		}
		top->color = Black;
		++top->output_time;
	}
}

std::vector<std::vector<int64_t>> floyd(std::vector<std::vector<int64_t>>& D)
{
	std::vector<std::vector<int64_t>> path(D.size(), std::vector<int64_t>(D.size()));
	for (size_t u = 0; u < D.size(); ++u) {
		for (size_t v = 0; v < D.size(); ++v) {
			if ((u == v) or (D[u][v] == INF)) path[u][v] = -1;
			else path[u][v] = u;
		}
	}
	for (size_t i = 0; i < D.size(); ++i)
	{
		for (size_t u = 0; u < D.size(); ++u)
		{
			for (size_t v = 0; v < D.size(); ++v)
			{
				if (D[u][i] + D[i][v] < D[u][v])
				{
					D[u][v] = D[u][i] + D[i][v];
					path[u][v] = i;
					if(u == v) D[u][v] = 0;
				}
			}
		}
	}
	for (auto& vect : D) {
		for (auto& p : vect) {
			std::cout << p << " ";
		}
		std::cout << "\n";
	}
// 	std::cout << std::endl;
// 	for (auto& vect : path) {
// 		for (auto& p : vect) {
// 			std::cout << p << " ";
// 		}
// 		std::cout << "\n";
// 	}
	return path;
}

void short_path(size_t u, size_t v, const std::vector<std::vector<int64_t>>& D, const std::vector<std::vector<int64_t>>& path)
{
	std::cout << "Short_path\n";
	if (D[u][v] == INF) std::cout << "No\n";
	std::list<int> p;
	p.push_back(v);
	int tmp = v;
	while (tmp != u)
	{
		tmp = path[u][tmp];
		p.push_back(tmp);
	}
	for (const auto& i : p) std::cout << i+1 << " ";
}

void resetNodeColors(std::unordered_map<Node*, std::forward_list<Node*>>& G) {
	for (auto const& [node, neighbors] : G) {
		node->color = 0; // White
	}
}


iint main()
{
    std::cout << "=== 1. ДЕМОНСТРАЦИЯ ОБХОДОВ (BFS и DFS) ===\n";
    std::cout << "Граф: 1 соединен с 2 и 3. 2 и 3 соединены с 4.\n\n";

    std::unordered_map<Node*, std::forward_list<Node*>> G;
    Node n1("1"), n2("2"), n3("3"), n4("4");

    // Простая структура "ромб": 1 -> (2, 3) -> 4
    G[&n1] = {&n2, &n3};
    G[&n2] = {&n1, &n4};
    G[&n3] = {&n1, &n4};
    G[&n4] = {&n2, &n3};

    std::cout << "DFS (в глубину, через стек): ";
    DFS(G, n1);
    std::cout << "\n";

    resetNodeColors(G);

    std::cout << "BFS (в ширину, через очередь, по уровням): ";
    BFS(G, n1);
    std::cout << "\n\n";


    std::cout << "=== 2. ДЕМОНСТРАЦИЯ ФЛОЙДА-УОРШЕЛЛА ===\n";
    std::cout << "Граф: 0 --10--> 1 --10--> 3  (путь длиной 20)\n";
    std::cout << "      0 --5---> 2 --10--> 3  (путь длиной 15, это кратчайший!)\n\n";

    // Матрица смежности для 4 вершин (0, 1, 2, 3)
    std::vector<std::vector<int64_t>> G2 = {
        {INF,  10,   5, INF}, // вершина 0
        { 10, INF, INF,  10}, // вершина 1
        {  5, INF, INF,  10}, // вершина 2
        {INF,  10,  10, INF}  // вершина 3
    };

    std::cout << "Матрица кратчайших расстояний:\n";
    auto path = floyd(G2);

    std::cout << "\nКратчайший путь из вершины 0 в вершину 3:\n";
    // Внимание: твоя функция short_path выводит индексы + 1 в обратном порядке восстановления
    short_path(0, 3, G2, path);
    std::cout << "\n(Смысл: алгоритм нашел 'секретный короткий путь' через вершину 2, длина 15 вместо 20)\n";

    return 0;
}
