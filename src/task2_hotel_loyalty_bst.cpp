// Task 2 - Hotel loyalty program implemented as a binary search tree (BST)
//
// Each guest is a node keyed by loyalty points (plus first and last name) with
// left/right/parent pointers. Extra functionality: for a given guest,
// find the guest with the closest higher number of points.

#include <iostream>
#include <fstream>
#include <limits>
#include <vector>
#include <string>
#include <sstream>
#include <queue>
#include <stack>
#include <algorithm>

using std::cin;
using std::cout;
using std::endl;
using std::string;
using std::vector;

class Node {
public:
    int points;
    string firstName;
    string lastName;
    Node* left;
    Node* right;
    Node* parent;

    Node(int p, const string& fn, const string& ln)
        : points(p), firstName(fn), lastName(ln), left(nullptr), right(nullptr), parent(nullptr) {}
};

struct Guest {
    int points;
    string firstName;
    string lastName;
    Guest(int p, const string& fn, const string& ln) : points(p), firstName(fn), lastName(ln) {}
};

class BST {
private:
    Node* root;

public:
    BST() : root(nullptr) {}
    ~BST() { clear(); }
    BST(const BST&) = delete;
    BST& operator=(const BST&) = delete;

    void insert(int points, const string& fn, const string& ln) {
        Node* newNode = new Node(points, fn, ln);
        if (!root) {
            root = newNode;
            return;
        }
        Node* curr = root;
        Node* parent = nullptr;
        while (curr) {
            parent = curr;
            if (points < curr->points)
                curr = curr->left;
            else
                curr = curr->right;
        }
        newNode->parent = parent;
        if (points < parent->points)
            parent->left = newNode;
        else
            parent->right = newNode;
    }

    Node* searchByPoints(int points) {
        Node* curr = root;
        while (curr) {
            if (points == curr->points)
                return curr;
            else if (points < curr->points)
                curr = curr->left;
            else
                curr = curr->right;
        }
        return nullptr;
    }

    void buildMinimalBST(const vector<Guest>& guests) {
        clear();
        if (guests.empty()) return;

        struct Range {
            int start, end;
            Node* parent;
            bool isLeftChild;
        };

        std::queue<Range> q;
        q.push({0, (int)guests.size() - 1, nullptr, false});
        while (!q.empty()) {
            Range r = q.front();
            q.pop();

            int mid = (r.start + r.end) / 2;
            Node* newNode = new Node(guests[mid].points, guests[mid].firstName, guests[mid].lastName);
            if (!root)
                root = newNode;
            else {
                if (r.isLeftChild)
                    r.parent->left = newNode;
                else
                    r.parent->right = newNode;
                newNode->parent = r.parent;
            }

            if (mid - 1 >= r.start)
                q.push({r.start, mid - 1, newNode, true});
            if (mid + 1 <= r.end)
                q.push({mid + 1, r.end, newNode, false});
        }
    }

    Node* searchByName(const string& firstName, const string& lastName) {
        std::stack<Node*> s;
        Node* curr = root;
        while (curr || !s.empty()) {
            while (curr) {
                s.push(curr);
                curr = curr->left;
            }
            curr = s.top();
            s.pop();
            if (curr->firstName == firstName && curr->lastName == lastName) {
                return curr;
            }
            curr = curr->right;
        }
        return nullptr;
    }

    Node* minimum(Node* start) {
        Node* curr = start;
        while (curr && curr->left)
            curr = curr->left;
        return curr;
    }

    void transplant(Node* u, Node* v) {
        if (!u->parent)
            root = v;
        else if (u == u->parent->left)
            u->parent->left = v;
        else
            u->parent->right = v;
        if (v)
            v->parent = u->parent;
    }

    void deleteNode(Node* z) {
        if (!z) return;
        if (!z->left)
            transplant(z, z->right);
        else if (!z->right)
            transplant(z, z->left);
        else {
            Node* y = minimum(z->right);
            if (y->parent != z) {
                transplant(y, y->right);
                y->right = z->right;
                if (y->right) y->right->parent = y;
            }
            transplant(z, y);
            y->left = z->left;
            if (y->left) y->left->parent = y;
        }
        delete z;
    }

    void increasePointsByName(const string& firstName, const string& lastName, int increment) {
        Node* node = searchByName(firstName, lastName);
        if (!node) {
            cout << "Guest Not Found.\n";
            return;
        }
        long long newPoints = static_cast<long long>(node->points) + increment;
        if (newPoints > std::numeric_limits<int>::max() || newPoints < std::numeric_limits<int>::min()) {
            cout << "Resulting Points Are Out Of Range.\n";
            return;
        }
        deleteNode(node);
        insert(static_cast<int>(newPoints), firstName, lastName);
        cout << "Points Were Incremented.\n";
    }

    void print() {
        if (!root) {
            cout << "Tree Is Empty.\n";
            return;
        }
        std::stack<Node*> s;
        Node* curr = root;
        while (curr || !s.empty()) {
            while (curr) {
                s.push(curr);
                curr = curr->left;
            }
            curr = s.top();
            s.pop();
            cout << curr->points << " - " << curr->firstName << " " << curr->lastName << endl;
            curr = curr->right;
        }
    }

    void printParentChildren() {
        if (!root) {
            cout << "Tree Is Empty.\n";
            return;
        }
        std::queue<Node*> q;
        q.push(root);
        while(!q.empty()) {
            Node* node = q.front();
            q.pop();

            cout << "Parent: " << node->firstName << " " << node->lastName << " (" << node->points << ") ";

            if(node->left)
                cout << "Left Child: " << node->left->firstName << " " << node->left->lastName << " (" << node->left->points << ") ";
            else
                cout << "Left Child: None ";

            if(node->right)
                cout << "Right Child: " << node->right->firstName << " " << node->right->lastName << " (" << node->right->points << ")";
            else
                cout << "Right Child: None";

            cout << endl;

            if(node->left) q.push(node->left);
            if(node->right) q.push(node->right);
        }
    }

    void clear() {
        if (!root) return;
        std::stack<Node*> s;
        s.push(root);
        while (!s.empty()) {
            Node* node = s.top(); s.pop();
            if (node->left) s.push(node->left);
            if (node->right) s.push(node->right);
            delete node;
        }
        root = nullptr;
    }

    void closestHigherByName(const string& firstName, const string& lastName) {
        Node* node = searchByName(firstName, lastName);
        if (!node) {
            cout << "Guest Not Found.\n";
            return;
        }
        int points = node->points;
        Node* curr = root;
        Node* candidate = nullptr;
        while (curr) {
            if (curr->points > points) {
                candidate = curr;
                curr = curr->left;
            } else {
                curr = curr->right;
            }
        }
        if (candidate)
            cout << "Guest With Closest Higher Points:\n"
                 << candidate->points << " - " << candidate->firstName << " " << candidate->lastName << endl;
        else
            cout << "No Guest Has Higher Points.\n";
    }
};

static string trim(const string& text) {
    size_t begin = text.find_first_not_of(" \t\r\n");
    if (begin == string::npos) return "";
    size_t end = text.find_last_not_of(" \t\r\n");
    return text.substr(begin, end - begin + 1);
}

// Strict integer parsing: the whole string (ignoring surrounding spaces) must be one int.
static bool parseInt(const string& text, int& value) {
    string t = trim(text);
    if (t.empty()) return false;
    try {
        size_t pos = 0;
        long long v = std::stoll(t, &pos);
        if (pos != t.size()) return false;
        if (v < std::numeric_limits<int>::min() || v > std::numeric_limits<int>::max()) return false;
        value = static_cast<int>(v);
        return true;
    } catch (...) {
        return false;
    }
}

bool parseLine(const string& line, Guest& guest) {
    std::stringstream ss(line);
    string token;
    vector<string> tokens;
    while (std::getline(ss, token, ',')) {
        tokens.push_back(token);
    }
    if (tokens.size() != 3)
        return false;
    int points = 0;
    if (!parseInt(tokens[0], points))
        return false;
    string fn = trim(tokens[1]);
    string ln = trim(tokens[2]);
    if (fn.empty() || ln.empty())
        return false;
    guest = Guest(points, fn, ln);
    return true;
}

bool loadGuests(const string& filename, vector<Guest>& guests) {
    std::ifstream file(filename);
    if (!file.is_open()) {
        cout << "Cannot Open File: " << filename << endl;
        return false;
    }
    string line;
    int lineNum = 1;  // line 1 is the header
    guests.clear();
    std::getline(file, line);
    while (std::getline(file, line)) {
        lineNum++;
        if (trim(line).empty()) continue;
        Guest g(0, "", "");
        if (!parseLine(line, g)) {
            cout << "Wrong Format At Line " << lineNum << endl;
            return false;
        }
        guests.push_back(g);
    }
    if (guests.empty()) {
        cout << "File Is Empty Or No Valid Data Found." << endl;
        return false;
    }
    std::sort(guests.begin(), guests.end(), [](const Guest& a, const Guest& b) {
        if (a.points != b.points) return a.points < b.points;
        if (a.firstName != b.firstName) return a.firstName < b.firstName;
        return a.lastName < b.lastName;
    });
    for (size_t i = 1; i < guests.size(); i++) {
        if (guests[i].points == guests[i-1].points &&
            guests[i].firstName == guests[i-1].firstName &&
            guests[i].lastName == guests[i-1].lastName) {
            cout << "Duplicate Entry Found At Position " << i+1 << endl;
            return false;
        }
    }
    return true;
}

void printMenu() {
    cout << "-----MENU-----\n";
    cout << "1. Load Guests From Csv\n";
    cout << "2. Search Guest By Points\n";
    cout << "3. Increase Guest Points By Name\n";
    cout << "4. Print Tree Inorder\n";
    cout << "5. Print Tree Parent-Children\n";
    cout << "6. Delete Tree\n";
    cout << "7. Find Guest With Closest Higher Points By Name\n";
    cout << "0. Exit\n";
    cout << "Choose An Option: ";
}

int main() {
    BST hotelLoyalty;
    vector<Guest> guests;
    int choice = -1;

    while (choice != 0) {
        printMenu();
        string input;
        if (!std::getline(cin, input)) {  // end of input
            cout << "\n";
            break;
        }
        if (!parseInt(input, choice)) {
            cout << "Invalid Input\n";
            choice = -1;
            continue;
        }

        switch(choice) {
            case 1: {
                cout << "Enter Csv Filename: ";
                string filename;
                std::getline(cin, filename);
                if(filename.empty()) {
                    cout << "Filename Cannot Be Empty\n";
                    break;
                }
                if (loadGuests(filename, guests)) {
                    hotelLoyalty.buildMinimalBST(guests);
                    cout << "Tree Successfully Built With Minimal Height From Csv.\n";
                } else {
                    cout << "Loading Failed.\n";
                }
                break;
            }
            case 2: {
                cout << "Enter Points To Search: ";
                string sPoints;
                std::getline(cin, sPoints);
                if(sPoints.empty()) {
                    cout << "Input Cannot Be Empty\n";
                    break;
                }
                int points = 0;
                if (!parseInt(sPoints, points)) {
                    cout << "Invalid Points Input\n";
                    break;
                }
                Node* found = hotelLoyalty.searchByPoints(points);
                if (found) {
                    cout << "Guest Found: " << found->firstName << " " << found->lastName << " - " << found->points << " Points.\n";
                } else {
                    cout << "Guest Not Found.\n";
                }
                break;
            }
            case 3: {
                cout << "Enter Guest First Name: ";
                string fn; std::getline(cin, fn); fn = trim(fn);
                cout << "Enter Guest Last Name: ";
                string ln; std::getline(cin, ln); ln = trim(ln);
                cout << "Enter Points To Increase: ";
                string sInc; std::getline(cin, sInc);
                if(fn.empty() || ln.empty() || sInc.empty()) {
                    cout << "Inputs Cannot Be Empty\n";
                    break;
                }
                int inc = 0;
                if (!parseInt(sInc, inc)) {
                    cout << "Invalid Points Input\n";
                    break;
                }
                hotelLoyalty.increasePointsByName(fn, ln, inc);
                break;
            }
            case 4:
                hotelLoyalty.print();
                break;
            case 5:
                hotelLoyalty.printParentChildren();
                break;
            case 6:
                hotelLoyalty.clear();
                cout << "Tree Deleted.\n";
                break;
            case 7: {
                cout << "Enter Guest First Name: ";
                string fn; std::getline(cin, fn); fn = trim(fn);
                cout << "Enter Guest Last Name: ";
                string ln; std::getline(cin, ln); ln = trim(ln);
                if (fn.empty() || ln.empty()) {
                    cout << "Inputs Cannot Be Empty\n";
                    break;
                }
                hotelLoyalty.closestHigherByName(fn, ln);
                break;
            }
            case 0:
                cout << "Exiting Program.\n";
                break;
            default:
                cout << "Invalid Option.\n";
                break;
        }
    }

    return 0;
}