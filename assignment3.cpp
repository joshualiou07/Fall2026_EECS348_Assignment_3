// EECS 348 Assignment 3
// C++ program that prioritizes emails for a CEO using a MaxHeap
// Input: a text file containing the commands EMAIL, NEXT, READ, and COUNT
// Output: a terminal output between user and program. Contains next email and amount of unread
// Collaborators: Only the GenAIs
// Source of Code: Both original and revisions by ChatGPT
// Author: Joshua Liou
// Creation date: 10/1/26
// Revision date: 10/1/26
// Revisions: Add parent and child pointers to better traverse the list. Modified MaxHeap functions so inserting and removing takes O(log n) time. Added comments. 

/*
All blocks of code below were generated and modified with ChatGPT.
Blocks marked REVISED by ChatGPT show changes from the original program.
*/

#include <cstddef> //For size_t, used for sizes and positions
#include <fstream> //For reading file input
#include <iostream> //For standard input/output
#include <list> //Includes list to store the heap nodes
#include <sstream> //Used to separate the command from the rest of the line
#include <string> //Includes working with strings with "std::string" 
#include <utility> //ChatGPT uses this for swapping and moving emails

//Stores email information and compares email priorities
class Email {
private:
    std::string sender; //sender category
    std::string subject; //subject text
    std::string date; //date text
    int priority; //sender priority number
    int dateValue; //date as a number

    //input a sender string and return its rank. Larger number is read first.
    static int senderPriority(const std::string& category) {
        if (category == "Boss") return 5; //compares category with each sender string
        if (category == "Subordinate") return 4;
        if (category == "Peer") return 3;
        if (category == "ImportantPerson") return 2;
        return 1; //For OtherPerson. 
    }

public:
    //Constructor gets email info and gets its priority
    //Changes date format to YYYYMMDD
    //"std::stoi" is used to convert each part to a number.
    Email(const std::string& senderCategory, const std::string& subjectLine,
          const std::string& emailDate)
        : sender(senderCategory), subject(subjectLine), date(emailDate),
          priority(senderPriority(senderCategory)),
          dateValue(std::stoi(emailDate.substr(6, 4)) * 10000   //year first makes newer years have greater value.
                    + std::stoi(emailDate.substr(0, 2)) * 100
                    + std::stoi(emailDate.substr(3, 2))) {}

    //compare sender priority, if equal then compare date. return True if email comes first.
    bool higherPriorityThan(const Email& other) const {
        if (priority != other.priority) return priority > other.priority;
        return dateValue > other.dateValue;
    }

    //code below prints sender, subject, and date formatted
    void display(std::ostream& output) const {
        output << "Next email:\n"
               << "       Sender: " << sender << '\n'
               << "       Subject: " << subject << '\n'
               << "       Date: " << date << "\n\n";
    }
};

//REVISED by ChatGPT: added links to parent and children instead of searching the list one by one.
//List stores nodes by heap level, from left to right. Swaps only change the emails.
class MaxHeap {
private:
    //heap node stores email and pointers to its parent and children.
    struct HeapNode {
        Email email; //stored email
        HeapNode* parent; //pointer to parent node
        HeapNode* left; //pointer to left child
        HeapNode* right; //pointer to right child

        //saves email and parent. nullptr for if no child yet.
        HeapNode(const Email& value, HeapNode* parentNode)
            : email(value), parent(parentNode), left(nullptr), right(nullptr) {}
    };

    std::list<HeapNode> nodes; //list of nodes. Goes from root to last node.

    //REVISED by ChatGPT: finds a heap position using its binary digits. Positions start at 1.
    //Skip the first 1. 0 means go left and 1 means go right.
    HeapNode* nodeAt(std::size_t index) {
        HeapNode* node = &nodes.front(); //starts at root
        std::size_t bit = 1; //used to check one binary digit at a time
        while (bit <= index / 2) bit <<= 1; //shift left to find the first binary digit
        for (bit >>= 1; bit != 0; bit >>= 1) { //skip first digit, then check the rest
            node = (index & bit) != 0 ? node->right : node->left; //1 goes right, 0 goes left
        }
        return node; //return pointer to the node
    }

    //REVISED by ChatGPT: uses parent pointers to move a higher-priority email upward.
    void siftUp(HeapNode* node) {
        while (node->parent != nullptr) { //stop if node has no parent
            HeapNode* parent = node->parent; //gets parent node
            if (!node->email.higherPriorityThan(parent->email)) break; //stop if already ordered
            std::swap(node->email, parent->email); //swap stored emails
            node = parent; //continue checking from parent
        }
    }

    //REVISED by ChatGPT: uses child pointers to move replacement email down.
    void siftDown(HeapNode* node) {
        while (node->left != nullptr) { //keep going while node has children
            HeapNode* highest = node->left; //start by choosing left child
            if (node->right != nullptr
                && node->right->email.higherPriorityThan(highest->email)) {
                highest = node->right; //choose right child if it has higher priority
            }
            if (!highest->email.higherPriorityThan(node->email)) break; //stop if already ordered
            std::swap(node->email, highest->email); //swap parent email with higher-priority child
            node = highest; //continue checking downward
        }
    }

public:
    MaxHeap() = default; //create a heap with an empty list
    //REVISED by ChatGPT: prevents copying the heap because copied pointers would point to old nodes.
    MaxHeap(const MaxHeap&) = delete;
    MaxHeap& operator=(const MaxHeap&) = delete;

    //REVISED by ChatGPT: add email, connect the new node, then restore heap order.
    void insert(const Email& email) {
        if (nodes.empty()) { //first email becomes the root
            nodes.emplace_back(email, nullptr); //creates node at the back with no parent
            return; //no other emails to compare
        }
        std::size_t index = nodes.size() + 1; //position of new node, starting at 1
        HeapNode* parent = nodeAt(index / 2); //finds parent
        nodes.emplace_back(email, parent); //creates new node at back of list
        HeapNode* added = &nodes.back(); //gets pointer to new node
        if (index % 2 == 0) parent->left = added; //even means left child
        else parent->right = added; //odd means right child
        siftUp(added); //move email up if needed
    }

    //REVISED by ChatGPT: returns pointer to top email
    const Email* peek() const {
        return nodes.empty() ? nullptr : &nodes.front().email; //nullptr if inbox is empty
    }

    //REVISED by ChatGPT: removes highest-priority email and updates the node links.
    void removeMax() {
        if (nodes.empty()) return; //if no emails remain, do nothing
        if (nodes.size() == 1) { //if only one email remains
            nodes.pop_back(); //remove the only node
            return; //no heap order to fix
        }
        HeapNode* last = &nodes.back(); //gets last node
        nodes.front().email = std::move(last->email); //move last email to the top
        //Remove the parent's link to the last node before deleting that node.
        if (last->parent->left == last) last->parent->left = nullptr;
        else last->parent->right = nullptr;
        nodes.pop_back(); //remove last node from list
        siftDown(&nodes.front()); //move replacement email down
    }

    //REVISED by ChatGPT: gets unread count from the node list in O(1).
    std::size_t count() const {
        return nodes.size();
    }
};

//Reads commands and uses MaxHeap to manage the emails
class EmailPrioritizer {
private:
    MaxHeap inbox; //heap for unread emails

    //Code below removes spaces and line endings from both ends 
    static std::string trim(const std::string& value) {
        std::size_t first = value.find_first_not_of(" \t\r\n"); 
        if (first == std::string::npos) return ""; 
        std::size_t last = value.find_last_not_of(" \t\r\n"); 
        return value.substr(first, last - first + 1); 
    }

public:
    //reads commands from input and prints results
    void run(std::istream& input, std::ostream& output) {
        std::string line; //one input line
        while (std::getline(input, line)) { //reads one line at a time until input ends
            std::istringstream record(line); //lets us read separate parts of the line
            std::string command; //stores command name
            record >> command; //gets first word from line

            if (command == "EMAIL") { //if input starts with EMAIL
                //Below stores each field read from the line.
                std::string sender;
                std::string subject;
                std::string date;
                std::getline(record, sender, ','); //read sender until first comma
                std::getline(record, subject, ','); //read subject until second comma
                std::getline(record, date); //read date
                inbox.insert(Email(trim(sender), trim(subject), trim(date))); //trim fields and add email
            } else if (command == "NEXT") { //shows top email without removing it
                const Email* next = inbox.peek(); //gets top email
                if (next != nullptr) next->display(output); //print email if one exists
                else output << "There are no emails to read.\n\n"; //prints message if inbox is empty
            } else if (command == "READ") { //removes top email
                inbox.removeMax();
            } else if (command == "COUNT") { //prints number of unread emails
                output << "There are " << inbox.count()
                       << " emails to read.\n\n";
            }
        }
    }
};


int main(int argc, char* argv[]) { //Main function. Select input and run EmailPrioritizer object
    EmailPrioritizer program; //create program object
    if (argc > 1) { //if a filename is entered after the program name
        std::ifstream input(argv[1]); //open file
        if (!input) { //if file cannot be opened, stop
            std::cerr << "Unable to open test file.\n";
            return 1;
        }
        program.run(input, std::cout); //read commands from file and print results
    } else {
        program.run(std::cin, std::cout); //read from terminal or redirected file
    }
    return 0; //program finished
}
