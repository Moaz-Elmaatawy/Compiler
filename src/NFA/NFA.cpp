#include "NFA.h"

int NFA::Node::UNIQUE_ID = 0;

NFA::Node::Node() : id(UNIQUE_ID++) {}

int NFA::Node::get_id() const {
    return id;
}

void NFA::Node::addTransition(const char c, shared_ptr<Node> ptr) {
    this->trans[c].push_back(move(ptr));
}

NFA::NFA() : start(make_shared<Node>()), end(make_shared<Node>()) {
    start->addTransition(EPSILON, end);
}

NFA::NFA(const char c) : start(make_shared<Node>()), end(make_shared<Node>()) {
    start->addTransition(c, end);
}

NFA::NFA(shared_ptr<Node> start, shared_ptr<Node> end) : start(move(start)), end(move(end)) {

}


NFA::NFA(const NFA &cpy) {
    unordered_map<int, shared_ptr<Node>> allocated;
    unordered_set<int> constructed{cpy.start->id};
    queue<Node *> q;
    q.push(cpy.start.get());
    while (!q.empty()) {
        Node *cur_old = q.front();
        q.pop();

        if (auto it = allocated.find(cur_old->id); it == allocated.end()) {
            allocated[cur_old->id] = make_shared<Node>();
        }

        Node *cur_new = allocated[cur_old->id].get();
        for (const auto &[c, vec]: cur_old->trans) {
            for (const auto &node_ptr: vec) {

                if (auto it = allocated.find(node_ptr->id); it == allocated.end()) {
                    allocated[node_ptr->id] = make_shared<Node>();
                }

                cur_new->trans[c].push_back(allocated[node_ptr->id]);
                if (!constructed.count(node_ptr->id)) {
                    constructed.insert(node_ptr->id);
                    q.push(node_ptr.get());
                }
            }
        }
    }
    this->start = move(allocated[cpy.start->id]);
    this->end = move(allocated[cpy.end->id]);
}

NFA &NFA::operator=(const NFA &cpy) {
    NFA temp(cpy);
    this->start = move(temp.start);
    this->end = move(temp.end);
    return *this;
}

NFA::NFA(NFA &&rhs) noexcept {
    this->start = move(rhs.start);
    this->end = move(rhs.end);
}

NFA &NFA::operator=(NFA &&rhs) noexcept {
    this->start = move(rhs.start);
    this->end = move(rhs.end);
    return *this;
}

NFA::Set getEpsilonClosure(const NFA::Set &states) {
    NFA::Set closure = states;
    queue<const NFA::Node *> q;
    for (auto &state: states) {
        q.push(state);
    }
    while (!q.empty()) {
        auto front = q.front();
        q.pop();
        if (!front->trans.count(EPSILON)) {
            continue;
        }
        for (const auto &child: front->trans.at(EPSILON)) {
            if (!closure.count(child.get())) {
                closure.insert(child.get());
                q.push(child.get());
            }
        }
    }
    return closure;
}


NFA::Set Move(const NFA::Set &states, const char c) {
    NFA::Set new_set;
    for (auto &state: states) {
        if (!state->trans.count(c)) {
            continue;
        }
        for (const auto &child: state->trans.at(c)) {
            new_set.insert(child.get());
        }
    }
    return new_set;
}
