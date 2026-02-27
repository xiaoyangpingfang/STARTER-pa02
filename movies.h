#ifndef MOVIES_H
#define MOVIES_H

#include <string>
#include <vector>
#include <array>
#include <algorithm>

struct Movie {
    std::string name;
    double rating;
    Movie(const std::string& n, double r) : name(n), rating(r) {}
};

// Comparator: descending rating, then ascending name
struct MovieCmp {
    bool operator()(const Movie& a, const Movie& b) const {
        if (a.rating != b.rating) return a.rating > b.rating;
        return a.name < b.name;
    }
};

// Trie node
struct TrieNode {
    // children indexed by character - we support lowercase letters and space and colon etc.
    // Use a map-like approach with array for printable ASCII (32-126)
    static const int BASE = 32;
    static const int SIZE = 96; // ASCII 32-127
    std::array<TrieNode*, SIZE> children;
    // All movies that have this node's prefix
    std::vector<Movie> movies;
    bool hasMovies;

    TrieNode() : hasMovies(false) {
        children.fill(nullptr);
    }
    ~TrieNode() {
        for (auto c : children) delete c;
    }
};

class Trie {
public:
    Trie();
    ~Trie();
    void insert(const std::string& name, double rating);
    // Returns pointer to node at end of prefix, or nullptr if not found
    TrieNode* search(const std::string& prefix) const;
    // Collect all movies in subtree rooted at node
    void collectMovies(TrieNode* node, std::vector<Movie>& result) const;
private:
    TrieNode* root;
};

#endif
