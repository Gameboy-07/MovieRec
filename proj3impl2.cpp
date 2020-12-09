#include <list>
#include <vector>
#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <unordered_map>
#include <map>
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

	unordered_map<string, Movie *> moviemap;	//main structure to hold graph
	void createMovie(list<string> producers, list<string> directors, list<string> writers, list<string> starring, string name)
	{
		Movie *movie = new Movie;

		movie->producers = producers;
		movie->directors = directors;
		movie->writers = writers;
		movie->starring = starring;
		name.erase(remove(name.begin(), name.end(), '\r'), name.end());	//erases hidden terminal character from movie name
		movie->name = name;
		moviemap.emplace(name, movie);
	}

	void generateWeights(Movie *movie)	//compares selected movie to the rest of the loaded data, noting any similarities
	{
		auto iter = moviemap.begin();
		for (; iter != moviemap.end(); iter++)
		{
			if (iter->second == movie)
				continue;
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

	void addEdge(Movie *from, Movie *to, int weight)	//creates edge in graph with selected weight
	{
		from->connections.emplace(to, weight);
		to->connections.emplace(from, weight);
	}

	void printInfo(Movie movie)	//lists all cast positions and related movies for selected film
	{
		cout << "Producer(s) in: " << movie.name << endl;
		if (movie.producers.size() == 0)
			cout << "Unlisted" << endl;
		for (auto i : movie.producers)
		{
			std::cout << i << "\n";
		}
		cout << endl << "Director(s) in: " << movie.name << endl;
		if (movie.directors.size() == 0)
			cout << "Unlisted" << endl;
		for (auto i : movie.directors)
		{
			std::cout << i << "\n";
		}
		cout << endl << "Writer(s) in: " << movie.name << endl;
		if (movie.writers.size() == 0)
			cout << "Unlisted" << endl;
		for (auto i : movie.writers)
		{
			std::cout << i << "\n";
		}
		cout << endl << "Starring Role(s) in: " << movie.name << endl;
		if (movie.starring.size() == 0)
			cout << "Unlisted" << endl;
		for (auto i : movie.starring)
		{
				std::cout << i << "\n";
		}
		cout << endl << "Movies with similar staff or cast include :" << endl;
		auto iter = movie.connections.begin();
		if (iter == movie.connections.end())
			cout << "No similar matches found!" << endl;
		else
		{
			for (; iter != movie.connections.end(); iter++)
			{
				cout << iter->first->name << " with " << iter->second << " similar positions." << endl;
			}
		}
	}

	void printGraph()	//creates an ordered map to print out movies alphabetically
	{
		map<string, Movie*> ordered(moviemap.begin(), moviemap.end());
		auto it = ordered.begin();
		for (; it != ordered.end(); it++)
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

		while (getline(items, temp, '|'))	//separates multiple values in one field 
		{
			ret.emplace_back(temp);
		}
		return ret;
	}

	App()
	{
		fstream file;
		file.open("./moviecsv.csv");	//opens .csv file with dad
		string row;

		while (getline(file, row))
		{
			vector<string> itemized;
			string temp;
			std::stringstream cells;
			cells.str(row);

			while (getline(cells, temp, ','))	//parses data to an itemized list
			{
				itemized.push_back(temp);
			}

			if (itemized.size() < 5)
			{
				continue;
			}
			
			//sorts the list into each category of data
			list<string> producers = parseListField(itemized[0]);	
			list<string> directors = parseListField(itemized[1]);
			list<string> writers = parseListField(itemized[2]);
			list<string> starring = parseListField(itemized[3]);

			//removes any repeated values in the categories
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
	string input;
	while (input != "4")
	{
		cout << "Number of Movies: " << myapp.movieGraph.size() << endl;
		cout << "1. Select Movie From List" << endl << "2. Select Random Movie From List" << endl << "3. Print Full Alphabetical List" << endl << "4. Exit" << endl << "Select an option: ";
		getline(cin, input);
		if (input == "1")
		{
			cout << "Select a movie: ";
			getline(cin, input);
			auto movie = myapp.movieGraph.moviemap.find(input);
			if (movie == myapp.movieGraph.moviemap.end())
			{
				cout << "Selection not found! Please try again." << endl;
			}
			else
			{
				myapp.movieGraph.generateWeights(movie->second);
				myapp.movieGraph.printInfo(*movie->second);
			}
		}
		else if (input == "2")
		{
			auto iter = myapp.movieGraph.moviemap.begin();
			advance(iter, (rand() % myapp.movieGraph.size()));
			myapp.movieGraph.generateWeights(iter->second);
			myapp.movieGraph.printInfo(*iter->second);
		}
		else if (input == "3")
		{
			myapp.movieGraph.printGraph();
		}
		else if (input != "4")
		{
			cout << "Invalid Selection! Please try again." << endl;
		}
	}



	return 0;
}