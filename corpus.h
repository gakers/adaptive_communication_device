#ifndef CORPUS_H
#define CORPUS_H

#include <QFile>
#include <QTextStream>
#include <QVector>
#include <QMap>
#include <QPair>
#include <QDebug>


class CorpusNode {
    public:
        CorpusNode() : score(-1) {}
        long score;
        QMap<QChar, CorpusNode*> nextMap;

        void add_word(const QString& str, const long& score);
        void get_words_starting_with(const QString& prefix,
                       int hit_count,
                       QVector<QString>& top_words);
        void get_top_words(const QString& prefix,
                           QVector< QPair<QString, long> >& top_pairs);
};

void build_corpus(const QString& filename, CorpusNode &head);

#endif // CORPUS_H
