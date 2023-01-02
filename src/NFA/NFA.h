#ifndef LEXICAL_ANALYZER_NFA_H
#define LEXICAL_ANALYZER_NFA_H

#include <memory>
#include <vector>
#include <unordered_map>
#include <queue>
#include <unordered_set>
#include <set>
#include "../Utils/Component.h"

class NFA {

public:

    NFA();

    explicit NFA(char c);

    class Node;

    explicit NFA(shared_ptr<Node> start, shared_ptr<Node> end);

    NFA(const NFA &cpy);

    NFA &operator=(const NFA &cpy);

    NFA(NFA &&rhs) noexcept;

    NFA &operator=(NFA &&rhs) noexcept;

    using Set = set<const NFA::Node *>;

    class Node {
    public:
        using Transitions = unordered_map<char, vector<shared_ptr<Node>>>;
        static int UNIQUE_ID;

        Node();

        int get_id() const;

        void addTransition(char c, shared_ptr<Node> ptr);

        friend class NFA;

        friend class NFABuilder;

        friend NFA::Set getEpsilonClosure(const NFA::Set &states);

        friend NFA::Set Move(const NFA::Set &states, const char c);

    private:
        const int id;
        Transitions trans;
    };

    friend class NFABuilder;

    const Node *getStart() const {
        return start.get();
    }

    const Node *getEnd() const {
        return end.get();
    }

private:
    shared_ptr<Node> start, end;

};

NFA::Set getEpsilonClosure(const NFA::Set &states);

NFA::Set Move(const NFA::Set &states, char c);

#endif //LEXICAL_ANALYZER_NFA_H
