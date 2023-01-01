// Name: Deep Walke
// BT19CSE122

// Assumptions
// consider fully connected graph

#include <iostream>
#include <stdio.h>
#include <vector>
#include <set>
#include <unordered_set>
#include <queue>
#include <map>
#include <unordered_map>
#include <cstdlib>
#include <ctime>
#include <algorithm>
#include <fstream>
#include <random>
using namespace std;

// Storing the path and cost for the path
struct path_cost
{
    int cost;
    vector<int> path;
};

// Comparator class for priority queue
// such that minimum value is returned every time at the top
class myComparator
{
public:
    int operator()(path_cost a, path_cost b)
    {
        return a.cost > b.cost;
    }
};

priority_queue<path_cost, vector<path_cost>, myComparator> population;
// set to store the paths already visited/ added to the population
set<vector<int>> path_visited;

// Returing the cost of the given path
int get_cost(vector<int> path, vector<vector<int>> adj_matrix)
{
    int cost = 0.0;
    for (int i = 0; i < path.size() - 1; i++)
    {
        cost += adj_matrix[path[i]][path[i + 1]];
    }
    cost += adj_matrix[path[path.size() - 1]][path[0]];
    return cost;
}

// Fitness function basically calculates the cost of the each path generated
void calculate_fitness(vector<vector<int>> pop, vector<vector<int>> adj_matrix)
{
    for (int i = 0; i < pop.size(); i++)
    {
        int cost = get_cost(pop[i], adj_matrix);
        population.push({cost, pop[i]});
        
    }
}

// generating the population of size 100 i.e. 100 random paths
// stroing the paths in a file named population.txt
void generate_population(vector<vector<int>> adj_matrix)
{
    int n = adj_matrix.size();
    vector<vector<int>> pop;
    ofstream fout;
    fout.open("population.txt");
    for (int i = 0; i < 100; i++)
    {
        vector<int> temp;
        srand(time(NULL));
        for (int j = 0; j < n; j++)
        {
            temp.push_back(j);
        }
        random_shuffle(temp.begin(), temp.end());

        // checking if the path is already visited or not
        if (path_visited.find(temp) == path_visited.end())
        {
            for (int j = 0; j < n; j++)
            {
                fout << temp[j] << " ";
            }
            fout << endl;

            pop.push_back(temp);
            path_visited.insert(temp);
        }
    }
    fout.close();
    // after generating the population calculating the fitness of each path
    calculate_fitness(pop, adj_matrix);
}

// selection of top 10 paths from the population
void select_best()
{   
    //storing the best 10 paths
    priority_queue<path_cost, vector<path_cost>, myComparator> selected;
    cout << "POPULATION SIZE: " << population.size() << endl;
    for (int i = 0; i < 10 && !population.empty(); i++)
    {
        selected.push(population.top());
        population.pop();
    }
    // printing the selected paths and also pushing it back to the population
    cout<<"COST\tPATH"<<endl;
    while (!selected.empty())
    {
        cout << selected.top().cost<<"\t";
        for (int i = 0; i < selected.top().path.size(); i++)
        {
            cout << selected.top().path[i] << " ";
        }
        cout <<"\n";
        population.push(selected.top());
        selected.pop();
    }
}

// generating the children from the selected parents
// Basic crossover is used
// Both the parents are divided into 2 parts, depending on the random number generated
// and then the children are generated by combining the 2 parts of the parents
// p1:1 2 3 | 4 5 6
// p2:4 6 2 | 5 1 3
// of:1 2 3 4 6 5
void crossover(vector<vector<int>> adj_matrix)
{
    int n = adj_matrix.size();

    // storing the population which is being used for crossover 
    // so that we can push it back to the population later
    priority_queue<path_cost, vector<path_cost>, myComparator> population_popped;
    for (int i = 0; i < 10 && !population.empty(); i++)
    {
        vector<int> temp1 = population.top().path;
        int c1 = population.top().cost;
        population_popped.push(population.top());
        population.pop();
        vector<int> temp2 = population.top().path;
        int c2 = population.top().cost;
        population_popped.push(population.top());
        population.pop();
        srand(time(NULL));

        // random number generated to divide the parents
        int cross_point = rand() % n;

        vector<int> offspring1;
        set<int> s;

        // pushing all the elements after the cross point to the set
        for (int k = cross_point + 1; k < n; k++)
        {
            s.insert(temp1[k]);
        }

        // order od elements after crossover point in the offspring1 is same as the parent2
        queue<int> q;
        for (int k = 0; k < n; k++)
        {
            if (s.find(temp2[k]) != s.end())
            {
                q.push(temp2[k]);
            }
        }
        // till crossover point we will keep the order same as in 1st parent
        for(int k=0;k<cross_point+1;k++)
        {
            offspring1.push_back(temp1[k]);
        }
        // after crossover point we will keep the order same as in 2nd parent
        while (!q.empty())
        {
            offspring1.push_back(q.front());
            q.pop();
        }
        // pushing the offspring1 to the population if it is not already visited
        if (path_visited.find(offspring1) == path_visited.end())
        {
            int cost = get_cost(offspring1, adj_matrix);
            population.push({cost,offspring1});
            path_visited.insert(offspring1);
        }
    }

    // pushing back the population which was used for crossover
    while (!population_popped.empty())
    {
        population.push(population_popped.top());
        population_popped.pop();
    }
}

// mutation is done by swapping 2 random elements in the path
void mutate(vector<vector<int>> adj_matrix)
{
    int n = adj_matrix.size();
    priority_queue<path_cost, vector<path_cost>, myComparator> population_popped;
    for (int i = 0; i < 10; i++)
    {
        // getting 2 random numbers
        srand(time(NULL));
        int cross_point1 = rand() % n;
        int cross_point2 = rand() % n;
        vector<int> temp = population.top().path;
        int c1 = population.top().cost;
        population_popped.push(population.top());
        population.pop();
        // swapping the 2 elements at index cross_point1 and cross_point2
        swap(temp[cross_point1], temp[cross_point2]);
        // pushing the mutated path to the population if it is not already visited
        if (path_visited.find(temp) == path_visited.end())
        {
            int cost = get_cost(temp, adj_matrix);
            population.push({cost,temp});
            path_visited.insert(temp);
        }
    }
    // pushing back the population which was used for mutation
    while (!population_popped.empty())
    {
        population.push(population_popped.top());
        population_popped.pop();
    }
}

void genetic_algorithm(vector<vector<int>> adj_matrix)
{
    // Generating the initial population
    generate_population(adj_matrix);
    // selecting the best 10 paths from the population
    cout<<"GENERATION 0"<<endl;
    select_best();
    int cost = 0;
    vector<int> path;
    cost = population.top().cost;
    path = population.top().path;
    cout << "\nMinimum Cost for Generation 0: " << cost << "\n\n";

    // performing crossover and mutation for 100 times 
    // and selecting the best 10 paths after each iteration
    for (int i = 0; i < 100; i++)
    {
        crossover(adj_matrix);
        mutate(adj_matrix);
        cout<<"GENERATION "<<i+1<<endl;
        select_best();
        cost = population.top().cost;
        path = population.top().path;
        cout << "\nMinimum Cost for Generation "<<i+1<<": " << cost << "\n\n";
    }

    cout<<"COST: "<<cost<<endl;
    cout<<"FINAL PATH: \n";
    for (int i = 0; i < path.size(); i++)
    {
        cout << path[i] << "->";
    }
}

// To check if given graph is fully connected
bool isFullyConnected(vector<vector<int>> adj_matrix)
{
    int n = adj_matrix.size();
    for (int i = 0; i < n; i++)
    {
        for (int j = 0; j < n; j++)
        {
            if (adj_matrix[i][j] == 0 && i != j)
            {
                return false;
            }
        }
    }
    return true;
}

// main function
int main(int argc, char const *argv[])
{
    // Creating initial population.
    // Calculating fitness.
    // Selecting the best genes.
    // Crossing over.
    // Mutating to introduce variations.

    string input_file(argv[1]);
    fstream file(input_file);

    int n, a, b, w;
    file >> n;

    vector<vector<int>> adj_matrix(n, vector<int>(n, 0));
    while (file >> a >> b >> w)
    {
        adj_matrix[a - 1][b - 1] = w;
        adj_matrix[b - 1][a - 1] = w;
    }

    if (!isFullyConnected(adj_matrix))
    {
        cout << "Given graph is not fully connected\n";
        return 0;
    }

    genetic_algorithm(adj_matrix);

    return 0;
}

