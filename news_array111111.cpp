#include <iostream>
#include <fstream>
#include <sstream>
#include <algorithm>    // for std::transform()
#include <cctype>       // for isdigit()
#include <string>
#include <ctime>        // for time measurements (clock)
using namespace std;

// Maximum number of articles in the array
const int MAX_SIZE = 10000;

// --------------------------------------------------------
// Structure to store news articles
// --------------------------------------------------------
struct NewsArticle {
    string title;     // Article title
    string content;   // Article content
    string category;  // E.g., "politics", "technology", ...
    string date;      // Format: "YYYY-MM-DD"
};

// --------------------------------------------------------
// Global variables for the array
// --------------------------------------------------------
NewsArticle newsArray[MAX_SIZE];
int newsCount = 0;  // Number of articles actually read

// --------------------------------------------------------
// Singly Linked List
// --------------------------------------------------------
struct ListNode {
    NewsArticle data;
    ListNode* next;
};

ListNode* head = nullptr;
ListNode* tail = nullptr;
int listCount = 0;  // Number of nodes in the linked list

// --------------------------------------------------------
// Helper functions
// --------------------------------------------------------
string removeQuotes(const string& str) {
    if (str.size() >= 2 && str.front() == '"' && str.back() == '"') {
        return str.substr(1, str.size() - 2);
    }
    return str;
}

string trim(const string& str) {
    size_t first = str.find_first_not_of(" \t\r\n");
    if (first == string::npos) return "";
    size_t last = str.find_last_not_of(" \t\r\n");
    return str.substr(first, last - first + 1);
}

string toLowerCase(const string& str) {
    string result = str;
    transform(result.begin(), result.end(), result.begin(), ::tolower);
    return result;
}

// --------------------------------------------------------
// Extract the year from a string like "YYYY-MM-DD"
// --------------------------------------------------------
int extractYear(const string& date) {
    // We need at least 4 digits, all numeric
    if (date.size() < 4) return 0;
    if (!isdigit(date[0]) || !isdigit(date[1]) ||
        !isdigit(date[2]) || !isdigit(date[3])) {
        return 0;
    }
    return stoi(date.substr(0, 4));
}

// --------------------------------------------------------
// Add a new node to the tail of the linked list
// --------------------------------------------------------
void addToLinkedList(const NewsArticle& article) {
    ListNode* newNode = new ListNode;
    newNode->data = article;
    newNode->next = nullptr;

    if (head == nullptr) {
        head = newNode;
        tail = newNode;
    } else {
        tail->next = newNode;
        tail = newNode;
    }
    listCount++;
}

// --------------------------------------------------------
// Read a CSV file into both the array and the linked list
// --------------------------------------------------------
void readCSV(const string& filename) {
    ifstream file(filename);
    if (!file.is_open()) {
        cout << "Error: cannot open " << filename << endl;
        return;
    }

    // Skip the header line: (Title,Content,Category,Date)
    string header;
    getline(file, header);

    // Read each line
    string line;
    while (getline(file, line) && newsCount < MAX_SIZE) {
        stringstream ss(line);
        string title, content, category, date;

        getline(ss, title, ',');
        getline(ss, content, ',');
        getline(ss, category, ',');
        getline(ss, date, ',');

        // Remove quotes
        title    = removeQuotes(title);
        content  = removeQuotes(content);
        category = removeQuotes(category);
        date     = removeQuotes(date);

        // Store in the array
        newsArray[newsCount].title    = title;
        newsArray[newsCount].content  = content;
        newsArray[newsCount].category = category;
        newsArray[newsCount].date     = date;
        newsCount++;

        // Also store in the linked list
        NewsArticle tmpArticle = {title, content, category, date};
        addToLinkedList(tmpArticle);
    }

    file.close();
    cout << "File " << filename << " loaded. Total articles: " << newsCount << endl;
}

// --------------------------------------------------------
// MERGE SORT for the ARRAY
// --------------------------------------------------------

// Merge two sorted subarrays (arr[left..mid], arr[mid+1..right])
void merge(NewsArticle arr[], int left, int mid, int right) {
    int n1 = mid - left + 1;
    int n2 = right - mid;

    // Temporary arrays
    NewsArticle* L = new NewsArticle[n1];
    NewsArticle* R = new NewsArticle[n2];

    for (int i = 0; i < n1; i++)
        L[i] = arr[left + i];
    for (int j = 0; j < n2; j++)
        R[j] = arr[mid + 1 + j];

    // Merge them back into arr[]
    int i = 0; // index for L
    int j = 0; // index for R
    int k = left;

    while (i < n1 && j < n2) {
        if (extractYear(L[i].date) <= extractYear(R[j].date)) {
            arr[k] = L[i];
            i++;
        } else {
            arr[k] = R[j];
            j++;
        }
        k++;
    }

    // Copy the remaining elements
    while (i < n1) {
        arr[k] = L[i];
        i++;
        k++;
    }
    while (j < n2) {
        arr[k] = R[j];
        j++;
        k++;
    }

    delete[] L;
    delete[] R;
}

void mergeSortArray(NewsArticle arr[], int left, int right) {
    if (left < right) {
        int mid = left + (right - left) / 2;
        mergeSortArray(arr, left, mid);
        mergeSortArray(arr, mid + 1, right);
        merge(arr, left, mid, right);
    }
}

// --------------------------------------------------------
// MERGE SORT for the LINKED LIST
// --------------------------------------------------------

// Find the middle of a linked list using slow/fast pointers
ListNode* getMiddle(ListNode* headNode) {
    if (!headNode || !headNode->next)
        return headNode;

    ListNode* slow = headNode;
    ListNode* fast = headNode->next;

    while (fast && fast->next) {
        slow = slow->next;
        fast = fast->next->next;
    }
    return slow;
}

// Merge two sorted linked lists
ListNode* mergeList(ListNode* leftHead, ListNode* rightHead) {
    if (!leftHead)  return rightHead;
    if (!rightHead) return leftHead;

    if (extractYear(leftHead->data.date) <= extractYear(rightHead->data.date)) {
        leftHead->next = mergeList(leftHead->next, rightHead);
        return leftHead;
    } else {
        rightHead->next = mergeList(leftHead, rightHead->next);
        return rightHead;
    }
}

// Recursive merge sort for a linked list
ListNode* mergeSortList(ListNode* headNode) {
    if (!headNode || !headNode->next) {
        return headNode;
    }

    // Find the middle node
    ListNode* middle = getMiddle(headNode);
    ListNode* nextOfMiddle = middle->next;

    // Split the list into two halves
    middle->next = nullptr;

    // Sort the left half
    ListNode* leftHead = mergeSortList(headNode);
    // Sort the right half
    ListNode* rightHead = mergeSortList(nextOfMiddle);

    // Merge both halves
    ListNode* sortedList = mergeList(leftHead, rightHead);
    return sortedList;
}

// --------------------------------------------------------
// LINEAR SEARCH in the ARRAY by category + year
// --------------------------------------------------------
void searchArrayByCategoryAndYear(const string& category, int year) {
    cout << "\n[ARRAY] Searching category \"" << category 
         << "\", year=" << year << ":\n";

    string catLower = toLowerCase(trim(category));
    bool foundAny = false;

    for (int i = 0; i < newsCount; i++) {
        int y = extractYear(newsArray[i].date);
        string c = toLowerCase(trim(newsArray[i].category));
        if (y == year && c.find(catLower) != string::npos) {
            cout << "- " << newsArray[i].title 
                 << " [" << newsArray[i].date << "]\n";
            foundAny = true;
        }
    }
    if (!foundAny) {
        cout << "❌ No articles found!\n";
    }
}

// --------------------------------------------------------
// LINEAR SEARCH in the LINKED LIST by category + year
// --------------------------------------------------------
void searchListByCategoryAndYear(const string& category, int year) {
    cout << "\n[LINKED LIST] Searching category \"" << category 
         << "\", year=" << year << ":\n";

    string catLower = toLowerCase(trim(category));
    bool foundAny = false;

    ListNode* current = head;
    while (current) {
        int y = extractYear(current->data.date);
        string c = toLowerCase(trim(current->data.category));
        if (y == year && c.find(catLower) != string::npos) {
            cout << "- " << current->data.title 
                 << " [" << current->data.date << "]\n";
            foundAny = true;
        }
        current = current->next;
    }
    if (!foundAny) {
        cout << "❌ No articles found!\n";
    }
}

// --------------------------------------------------------
// Time measurement for array merge sort
// --------------------------------------------------------
void measureMergeSortArray() {
    clock_t start = clock();
    mergeSortArray(newsArray, 0, newsCount - 1);
    clock_t end = clock();
    double duration = double(end - start) / CLOCKS_PER_SEC;
    cout << "\n⏳ Merge Sort (array) time: " << duration << " s.\n";
}

// --------------------------------------------------------
// Time measurement for linked-list merge sort
// --------------------------------------------------------
void measureMergeSortList() {
    clock_t start = clock();
    // Sorting might change the head pointer
    head = mergeSortList(head);
    clock_t end = clock();
    double duration = double(end - start) / CLOCKS_PER_SEC;
    cout << "\n⏳ Merge Sort (linked list) time: " << duration << " s.\n";
}

// --------------------------------------------------------
// main()
// --------------------------------------------------------
int main() {
    // Example: read from two CSV files (replace with your own)
    readCSV("true.csv");
    readCSV("fake.csv");

    // Print the first 3 articles from the ARRAY (before sorting)
    cout << "\n[ARRAY] First 3 articles (before sorting):\n";
    for (int i = 0; i < min(newsCount, 3); i++) {
        cout << "\"" << newsArray[i].title 
             << "\" - \"" << newsArray[i].date << "\"\n";
    }

    // Print the first 3 articles from the LINKED LIST (before sorting)
    cout << "\n[LINKED LIST] First 3 articles (before sorting):\n";
    {
        ListNode* cur = head;
        int countPrint = 0;
        while (cur && countPrint < 3) {
            cout << "\"" << cur->data.title 
                 << "\" - \"" << cur->data.date << "\"\n";
            cur = cur->next;
            countPrint++;
        }
    }

    // Sort the array by year (Merge Sort)
    measureMergeSortArray();

    // Sort the linked list by year (Merge Sort)
    measureMergeSortList();

    // Print first 3 articles from ARRAY (after sorting)
    cout << "\n[ARRAY] First 3 articles (after sorting):\n";
    for (int i = 0; i < min(newsCount, 3); i++) {
        cout << "\"" << newsArray[i].title 
             << "\" - \"" << newsArray[i].date << "\"\n";
    }

    // Print first 3 articles from LINKED LIST (after sorting)
    cout << "\n[LINKED LIST] First 3 articles (after sorting):\n";
    {
        ListNode* cur = head;
        int countPrint = 0;
        while (cur && countPrint < 3) {
            cout << "\"" << cur->data.title 
                 << "\" - \"" << cur->data.date << "\"\n";
            cur = cur->next;
            countPrint++;
        }
    }

    // Example: searching for articles in "politics" for year 2017
    searchArrayByCategoryAndYear("politics", 2017);
    searchListByCategoryAndYear("politics", 2017);

    cout << "\nTotal articles loaded: " << newsCount << endl;
    cout << "\nProgram finished.\n";
    return 0;
}
