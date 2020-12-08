#include <list>
#include <vector>
#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <unordered_map>
#include <algorithm>
using namespace std;

class Graph
{
public:
	struct Movie
	{
		list<string> directors;
		list<string> writers;
		list<string> starring;
		list<string> producers;
		string name;
		unordered_map<Movie *, int> connections;
	};

	unordered_map<string, Movie *> moviemap;
	void createMovie(list<string> producers, list<string> directors, list<string> writers, list<string> starring, string name)
	{
		Movie *movie = new Movie;

		movie->producers = producers;
		movie->directors = directors;
		movie->writers = writers;
		movie->starring = starring;
		name.erase(remove(name.begin(), name.end(), '\r'), name.end());
		movie->name = name;
		generateWeights(movie);
		moviemap.emplace(name, movie);
	}

	void generateWeights(Movie *movie)
	{
		auto iter = moviemap.begin();
		for (; iter != moviemap.end(); iter++)
		{
			int weight = 0;
			int actorCount = 0;
			Movie *movie1 = movie;
			Movie *movie2 = iter->second;
			auto directiter = movie1->directors.begin();
			auto produceiter = movie1->producers.begin();
			auto writeiter = movie1->writers.begin();
			auto actoriter = movie1->starring.begin();
			for (; directiter != movie1->directors.end(); directiter++)
			{
				if (find(movie2->directors.begin(), movie2->directors.end(), *directiter) != movie2->directors.end())
					weight++;
			}
			for (; produceiter != movie1->producers.end(); produceiter++)
			{
				if (find(movie2->producers.begin(), movie2->producers.end(), *produceiter) != movie2->producers.end())
					weight++;
			}
			for (; writeiter != movie1->writers.end(); writeiter++)
			{
				if (find(movie2->writers.begin(), movie2->writers.end(), *writeiter) != movie2->writers.end())
					weight++;
			}
			for (; actoriter != movie1->starring.end(); actoriter++)
			{
				if (find(movie2->starring.begin(), movie2->starring.end(), *actoriter) != movie2->starring.end())
					actorCount++;
			}
			weight += actorCount / 2;
			if (weight != 0)
				addEdge(movie1, movie2, weight);
		}
	}

	void addEdge(Movie *from, Movie *to, int weight)
	{
		from->connections.emplace(to, weight);
		to->connections.emplace(from, weight);
	}

	void printInfo(Movie movie)
	{

		for (auto i : movie.producers)
		{
			std::cout << i << "\n";
		}

		for (auto i : movie.directors)
		{
			std::cout << i << "\n";
		}

		for (auto i : movie.writers)
		{
			std::cout << i << "\n";
		}

		for (auto i : movie.starring)
		{
			std::cout << i << "\n";
		}

		cout << movie.name << endl;
		auto iter = movie.connections.begin();
		for (; iter != movie.connections.end(); iter++)
		{
			cout << iter->first->name << " " << iter->second << endl;
		}
	}

	void printGraph()
	{
		auto it = moviemap.begin();
		for (; it != moviemap.end(); it++)
		{
			cout << it->first << endl;
		}
	}

	int size()
	{
		return moviemap.size();
	}
};

struct App
{
	Graph movieGraph;
	list<string> parseListField(string cell)
	{
		list<string> ret;
		string temp;
		stringstream items(cell);

		while (getline(items, temp, '|'))
		{
			ret.emplace_back(temp);
		}
		return ret;
	}

	App()
	{
		fstream file;
		file.open("./moviecsv.csv");
		string row;

		while (getline(file, row))
		{
			vector<string> itemized;
			string temp;
			std::stringstream cells;
			cells.str(row);

			while (getline(cells, temp, ','))
			{
				itemized.push_back(temp);
			}

			if (itemized.size() < 5)
			{
				continue;
			}

			list<string> producers = parseListField(itemized[0]);
			list<string> directors = parseListField(itemized[1]);
			list<string> writers = parseListField(itemized[2]);
			list<string> starring = parseListField(itemized[3]);
			producers.sort();
			producers.unique();
			directors.sort();
			directors.unique();
			writers.sort();
			writers.unique();
			starring.sort();
			starring.unique();
			string name = itemized[4];

			movieGraph.createMovie(producers, directors, writers, starring, name);
		}
	}
};

int main()
{
	App myapp;
	myapp.movieGraph.printGraph();
	string input;
	getline(cin, input);
	auto movie = myapp.movieGraph.moviemap.find(input);
	if (movie == myapp.movieGraph.moviemap.end())
	{
		cout << "kms" << endl;
	}
	else
	{
		myapp.movieGraph.printInfo(*movie->second);
	}
	

	cout << endl
		 << "SIZE OF GRAPH LIST: " << myapp.movieGraph.size();

	return 0;
}