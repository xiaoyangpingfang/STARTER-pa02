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

/*
Part 3: Time and Space Complexity Analysis
Data Structure: Trie
Each TrieNode at depth d (corresponding to a prefix of length d) stores a vector
of ALL movies whose names start with that prefix.

Part 3a: Time Complexity

Assumptions: n movies already stored in Trie, m prefixes already in array.
 
For each prefix of length |p| <= l:
1. Traverse Trie to the prefix node:      O(l)
2. Access the pre-stored movie list:       O(1)
3. Sort the k matching movies:             O(k log k)
4. Print the k movies:                     O(k)
 
Cost per prefix:  O(l + k log k)
Total for m prefixes: O(m * (l + k log k))

Relevant parameters: m, l, k
Final Big-O time complexity: O(m * (l + k log k))
 
Because l (max movie name length) and k (matches per prefix) are both
small constants relative to n, the runtime grows nearly linearly in m
and is largely independent of n -- consistent with the near-flat
benchmark curve observed for this implementation.
 
Measured runtimes on CSIL using prefix_large.txt:
input_20_random.csv:      11 ms
input_100_random.csv:       8 ms
input_1000_random.csv:     18 ms
input_76920_random.csv:  2051 ms
 
The first three datasets are small enough that runtime is dominated
by constant factors and OS overhead. The jump at n=76920 reflects
the cost of building the Trie and larger k values (more movies share
prefixes), but per-prefix query cost remains sub-linear in n.
 
Part 3b: Space Complexity
 
Assumptions: n movies already stored, m prefixes already in array.
 
Trie nodes:
Each of the n movie names of average length l creates at most l nodes,
giving at most O(n * l) nodes total (fewer with shared prefixes).
 
Movie storage:
Each movie is copied into every ancestor node along its path,
so each movie appears in O(l) nodes. Total movie copies: O(n * l).
 
Temporary sort buffer: O(k) per query, reused each iteration.

Relevant parameters: n, l
Final Big-O space complexity: O(n * l)

Part 3c: Tradeoffs between Time and Space Complexity
 
Design goal: LOW TIME COMPLEXITY.
 
By storing the full list of matching movies at every internal Trie node,
a prefix query requires only O(l) traversal plus O(k log k) sorting --
no subtree traversal is needed at query time. This is the key optimization
that keeps query cost independent of n.
 
Was low space complexity also achieved?
No. Storing movies at every ancestor node means each movie is duplicated
O(l) times, yielding O(n * l) total space -- significantly more than the
O(n) needed to store each movie once. For n=76920 and average name
length ~20, this results in roughly 1.5 million Movie objects in the Trie.
 
This is a deliberate space-for-time tradeoff: we accept higher memory
usage to eliminate subtree traversal and achieve fast prefix queries.
Low time complexity was the harder goal; it required the non-obvious
insight of pre-materializing results at internal nodes rather than
computing them lazily at query time.
 */

bool parseLine(string &line, string &movieName, double &movieRating) {
    int commaIndex = line.find_last_of(",");
    movieName = line.substr(0, commaIndex);
    movieRating = stod(line.substr(commaIndex + 1));
    if (movieName[0] == '\"') {
        movieName = movieName.substr(1, movieName.length() - 2);
    }
    return true;
}
