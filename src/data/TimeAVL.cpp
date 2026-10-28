// 담당: 방지혁
#include "data/TimeAVL.h"

// --------------------------------------------------------
// Private Static Helper 멤버 함수 구현 (TimeAVL.h 헤더 명세 100% 일치)
// --------------------------------------------------------
int TimeAVL::heightOf(Node* n) {
    return n ? n->height : 0;
}

// 두 정수 중 큰 값 (STL 알고리즘 std::max 대신 직접 비교)
static int maxOf(int a, int b) {
    return a > b ? a : b;
}

TimeAVL::Node* TimeAVL::rotateRight(Node* y) {
    Node* x = y->left;
    Node* T2 = x->right;

    x->right = y;
    y->left = T2;

    y->height = 1 + maxOf(heightOf(y->left), heightOf(y->right));
    x->height = 1 + maxOf(heightOf(x->left), heightOf(x->right));

    return x;
}

TimeAVL::Node* TimeAVL::rotateLeft(Node* x) {
    Node* y = x->right;
    Node* T2 = y->left;

    y->left = x;
    x->right = T2;

    x->height = 1 + maxOf(heightOf(x->left), heightOf(x->right));
    y->height = 1 + maxOf(heightOf(y->left), heightOf(y->right));

    return y;
}

TimeAVL::Node* TimeAVL::rebalance(Node* n) {
    if (!n) return nullptr;

    n->height = 1 + maxOf(heightOf(n->left), heightOf(n->right));
    int balance = heightOf(n->left) - heightOf(n->right);

    // LL Case
    if (balance > 1 && (heightOf(n->left->left) >= heightOf(n->left->right))) {
        return rotateRight(n);
    }
    // LR Case
    if (balance > 1 && (heightOf(n->left->left) < heightOf(n->left->right))) {
        n->left = rotateLeft(n->left);
        return rotateRight(n);
    }
    // RR Case
    if (balance < -1 && (heightOf(n->right->right) >= heightOf(n->right->left))) {
        return rotateLeft(n);
    }
    // RL Case
    if (balance < -1 && (heightOf(n->right->right) < heightOf(n->right->left))) {
        n->right = rotateRight(n->right);
        return rotateLeft(n);
    }

    return n;
}

TimeAVL::Node* TimeAVL::insertNode(Node* n, const Record& r) {
    long long k = r.time.key();
    if (!n) {
        Node* newNode = new Node();
        newNode->key = k;
        newNode->records.push_back(r);
        newNode->height = 1;
        newNode->left = nullptr;
        newNode->right = nullptr;
        return newNode;
    }

    if (k < n->key) {
        n->left = insertNode(n->left, r);
    }
    else if (k > n->key) {
        n->right = insertNode(n->right, r);
    }
    else {
        // 동일한 시각이면 해당 노드의 records 동적 배열에 추가
        n->records.push_back(r);
        return n;
    }

    return rebalance(n);
}

// --------------------------------------------------------
// Public 멤버 함수 구현
// --------------------------------------------------------
TimeAVL::TimeAVL() : root_(nullptr), size_(0) {}

TimeAVL::~TimeAVL() {
    // 람다 함수를 사용해 멤버 범위 안에서 안전하게 노드 메모리 해제
    auto clearNodes = [](auto self, Node* n) -> void {
        if (!n) return;
        self(self, n->left);
        self(self, n->right);
        delete n;
        };
    clearNodes(clearNodes, root_);
    root_ = nullptr;
}

void TimeAVL::insert(const Record& r) {
    root_ = insertNode(root_, r);
    size_++;
}

void TimeAVL::rangeSearch(const DateTime& from, const DateTime& to, DynamicArray<Record>& out) const {
    out.clear();
    long long fromKey = from.key();
    long long toKey = to.key();

    // 람다 함수를 사용해 멤버 범위 안에서 안전하게 rangeSearch 수행
    auto searchNodes = [](auto self, Node* n, long long fk, long long tk, DynamicArray<Record>& result) -> void {
        if (!n) return;

        if (fk < n->key) {
            self(self, n->left, fk, tk, result);
        }

        if (n->key >= fk && n->key <= tk) {
            for (int i = 0; i < static_cast<int>(n->records.size()); ++i) {
                result.push_back(n->records[i]);
            }
        }

        if (tk > n->key) {
            self(self, n->right, fk, tk, result);
        }
        };

    searchNodes(searchNodes, root_, fromKey, toKey, out);
}

int TimeAVL::size() const {
    return size_;
}

int TimeAVL::height() const {
    return heightOf(root_);
}