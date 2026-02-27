// Winter'26
// Instructor: Diba Mirza
// Student name: Xiaoyang Zhang
#include <iostream>
#include <fstream>
#include <string>
#include <ctime>
#include <vector>
#include <cstring>
#include <algorithm>
#include <limits.h>
#include <iomanip>
#include <set>
#include <queue>
#include <sstream>
#include <map>
using namespace std;
#include "utilities.h"
#include "movies.h"

bool parseLine(string &line, string &movieName, double &movieRating);

int main(int argc, char** argv){
    if (argc < 2){
        cerr << "Not enough arguments provided (need at least 1 argument)." << endl;
        cerr << "Usage: " << argv[0] << " moviesFilename prefixFilename " << endl;
        exit(1);
    }

    ifstream movieFile(argv[1]);
    if (movieFile.fail()){
        cerr << "Could not open file " << argv[1];
        exit(1);
    }

    // Part 1: use map for alphabetical ordering
    map<string, double> movieMap;
    // Part 2: use Trie for fast prefix lookup
    Trie trie;

    string line, movieName;
    double movieRating;

    while (getline(movieFile, line) && parseLine(line, movieName, movieRating)){
        movieMap[movieName] = movieRating;
        trie.insert(movieName, movieRating);
    }
    movieFile.close();

    if (argc == 2){
        // Part 1: print all movies in alphabetical order
        for (const auto& kv : movieMap) {
            cout << kv.first << ", " << fixed << setprecision(1) << kv.second << "\n";
        }
        return 0;
    }

    ifstream prefixFile(argv[2]);
    if (prefixFile.fail()) {
        cerr << "Could not open file " << argv[2];
        exit(1);
    }

    vector<string> prefixes;
    while (getline(prefixFile, line)) {
        if (!line.empty()) {
            prefixes.push_back(line);
        }
    }
    prefixFile.close();

    vector<pair<string, Movie*>> bestMovies; // prefix -> best movie (nullptr if none)

    MovieCmp cmp;

    for (const string& prefix : prefixes) {
        TrieNode* node = trie.search(prefix);

        if (!node || !node->hasMovies) {
            cout << "No movies found with prefix " << prefix << "\n";
            bestMovies.push_back({prefix, nullptr});
            continue;
        }
        vector<Movie> matches = node->movies;
        sort(matches.begin(), matches.end(), cmp);

        for (const Movie& m : matches) {
            cout << m.name << ", " << fixed << setprecision(1) << m.rating << "\n";
        }
        cout << "\n";
        bestMovies.push_back({prefix, new Movie(matches[0].name, matches[0].rating)});
    }

    // Print best movies at end
    for (auto& p : bestMovies) {
        if (p.second == nullptr) {
            continue;
        }
        cout << "Best movie with prefix " << p.first << " is: "
             << p.second->name << " with rating "
             << fixed << setprecision(1) << p.second->rating << "\n";
        delete p.second;
    }

    return 0;
}

bool parseLine(string &line, string &movieName, double &movieRating) {
    int commaIndex = line.find_last_of(",");
    movieName = line.substr(0, commaIndex);
    movieRating = stod(line.substr(commaIndex + 1));
    if (movieName[0] == '\"') {
        movieName = movieName.substr(1, movieName.length() - 2);
    }
    return true;
}
