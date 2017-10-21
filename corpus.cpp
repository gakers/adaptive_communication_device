#include "corpus.h"



void CorpusNode::add_word(const QString &str, const long &score) {/*
    if(str.size()) {
        QChar first_char = str[0];
        auto corpus_node_it = nextMap.find(str[0]);
        CorpusNode *nextCorpus;
        if(corpus_node_it == nextMap.end()) {
            nextCorpus = new CorpusNode();
            nextMap.insert(first_char, nextCorpus);
        } else {
            nextCorpus = corpus_node_it->second;
        }
        nextCorpus->add_word(str.);
    }
    */
}

void CorpusNode::get_words(const QString &prefix,
                           int hit_count,
                           QVector<QString> &top_words) {

}

void CorpusNode::get_top_words(const QString &prefix,
                               QVector<QPair<QString, long> > &top_pairs) {

}
