#include <string>
#include <vector>
#include <iostream>
#include "llama/llama.h"
#include "AI/Loading_model.h"

using namespace std;
vector<llama_token> LModel::Dialogue(const string& Phrase) {
    if (!vocab) {
        throw runtime_error("Error: Vocab is null. Model not loaded.");
    }
    int n_tokens = -llama_tokenize(LModel::vocab, Phrase.c_str(), Phrase.size(), NULL, 0, false, true);
    vector<llama_token> tokens(n_tokens);
    llama_tokenize(vocab, Phrase.c_str(), Phrase.size(), tokens.data(), tokens.size(), false, true);
    return tokens;
}