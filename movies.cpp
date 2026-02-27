#include "movies.h"

Trie::Trie() {
    root = new TrieNode();
}

Trie::~Trie() {
    delete root;
}

void Trie::insert(const std::string& name, double rating) {
    TrieNode* cur = root;
    for (char ch : name) {
        int idx = (int)ch - TrieNode::BASE;
        if (idx < 0 || idx >= TrieNode::SIZE) continue; // skip out-of-range chars
        if (!cur->children[idx]) {
            cur->children[idx] = new TrieNode();
        }
        cur = cur->children[idx];
        cur->movies.push_back(Movie(name, rating));
        cur->hasMovies = true;
    }
}

TrieNode* Trie::search(const std::string& prefix) const {
    TrieNode* cur = root;
    for (char ch : prefix) {
        int idx = (int)ch - TrieNode::BASE;
        if (idx < 0 || idx >= TrieNode::SIZE) return nullptr;
        if (!cur->children[idx]) return nullptr;
        cur = cur->children[idx];
    }
    return cur;
}

void Trie::collectMovies(TrieNode* node, std::vector<Movie>& result) const {
    if (!node) return;
    result = node->movies;
}
