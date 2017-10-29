#include "corpus.h"

void CorpusNode::add_word(const QString &word_fragment, const long &score) {
    // We havent exhausted the word_fragment to store it's popularity.
    if (word_fragment.size() > 0) {
        QChar first_char = word_fragment[0];
        auto corpus_node_it = nextMap.find(word_fragment[0]);
        CorpusNode *nextCorpus;
        // The next character isn't in the map yet.
        if (corpus_node_it == nextMap.end()) {
            nextCorpus = new CorpusNode();
            nextMap.insert(first_char, nextCorpus);
        } else {
            nextCorpus = corpus_node_it.value();
        }
        QString new_fragment = word_fragment.right(word_fragment.size() - 1);
        nextCorpus->add_word(new_fragment, score);
    // We have traversed the map tree to the bottom, we can now add the data.
    } else {
        this->score = score;
    }
}

void CorpusNode::get_words_starting_with(const QString &prefix,
                           int hit_count,
                           QVector<QString> &top_words) {
    CorpusNode* temp = this;
    for (int i = 0; i < prefix.size(); i++) {
        auto corpus_node_it = temp->nextMap.find(prefix[i]);
        // then we have hit the end of the map,
        // and there are no hits for our prefix.
        if (corpus_node_it == temp->nextMap.end()) {
            return;
        } else {
            temp = corpus_node_it.value();
        }
    }
    // we are looking for %hit_count% words. initialize our
    // "most popular words list" with words that have zero popularity.
    QVector< QPair<QString, long> > top_pairs(hit_count, qMakePair(QChar(), 0));
    // all words within temp's maps
    // & descendants of temp are possible suggestions.
    temp->get_top_words(prefix, top_pairs);
    for (QPair<QString, long> p : top_pairs) {
        if (p.second > 0) {
            top_words.insert(top_words.begin(), p.first);
        }
    }
}

void CorpusNode::get_top_words(const QString& prefix,
        QVector< QPair<QString, long> >& top_pairs) {
    if (score != -1) {
        int best_index = 0;
        for (; best_index < top_pairs.size(); best_index++) {
            if (score < top_pairs[best_index].second) {
                break;
            }
        }
        // we found a value higher then our current values.
        if (best_index > 0) {
            // put him in the correct place, we are sorting in ascending order.
            top_pairs.insert(
                    top_pairs.begin() + best_index - 1,
                    qMakePair(prefix, score));
            // erase the new lowest value.
            top_pairs.erase(top_pairs.begin());
        }
    }
    for (auto it = nextMap.begin(); it != nextMap.end(); it++) {
        CorpusNode* nextCorpusNode = it.value();
        nextCorpusNode->get_top_words(prefix + it.key(), top_pairs);
    }
}


void build_corpus(const QString& filename, CorpusNode &head) {
    QFile f(filename);
    f.open(QIODevice::ReadOnly);
    QTextStream dict(&f);
    QString word;
    long frequency;
    char tmp;
    while (!dict.atEnd()) {
        dict >> word >> frequency;
        while (dict.device()->peek(2) == "\n") {
            dict.device()->getChar(&tmp);
        }
        head.add_word(word, frequency);
    }
    f.close();
}
