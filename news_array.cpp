p-0#include <iostream>
#include <fstream>
#include <sstream>
#include <algorithm>  // Для transform()
#include <cctype>     // Для isdigit()
#include <string>
#include <ctime>      // Для замера времени

using namespace std;

// 1) Максимальное количество статей
const int MAX_SIZE = 10000;

// 2) Структура для хранения статьи
struct NewsArticle {
    string title;     // Заголовок
    string content;   // Содержание
    string category;  // Категория (например, "politics")
    string date;      // Формат "YYYY-MM-DD"
};

// 3) Глобальный массив и счётчик
NewsArticle newsArray[MAX_SIZE];
int newsCount = 0;

// --------------------------------------------------------
// Вспомогательные функции: trim, toLowerCase
// --------------------------------------------------------
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
// Извлечение года из даты (с проверкой, чтобы не вылететь на stoi)
// --------------------------------------------------------
int extractYear(const string& date) {
    if (date.size() < 4) return 0; 
    if (!isdigit(date[0]) || !isdigit(date[1]) ||
        !isdigit(date[2]) || !isdigit(date[3])) {
        return 0;
    }
    return stoi(date.substr(0, 4));  
}

// --------------------------------------------------------
// QuickSort: сортируем по году публикации
// --------------------------------------------------------
int partition(NewsArticle arr[], int low, int high) {
    int pivot = extractYear(arr[high].date);
    int i = low - 1;

    for (int j = low; j < high; j++) {
        if (extractYear(arr[j].date) < pivot) {
            i++;
            swap(arr[i], arr[j]);
        }
    }
    swap(arr[i + 1], arr[high]);
    return i + 1;
}

void quickSort(NewsArticle arr[], int low, int high) {
    if (low < high) {
        int pi = partition(arr, low, high);
        quickSort(arr, low, pi - 1);
        quickSort(arr, pi + 1, high);
    }
}

// --------------------------------------------------------
// Функция чтения CSV
// --------------------------------------------------------
void readCSV(const string& filename) {
    ifstream file(filename);
    if (!file.is_open()) {
        cout << "Ошибка: Не удалось открыть " << filename << endl;
        return;
    }

    // Пропускаем строку-заголовок (Title,Content,Category,Date)
    string header;
    getline(file, header);

    // Считываем построчно
    string line;
    while (getline(file, line) && newsCount < MAX_SIZE) {
        stringstream ss(line);
        string title, content, category, date;

        getline(ss, title, ',');
        getline(ss, content, ',');
        getline(ss, category, ',');
        getline(ss, date, ',');

        newsArray[newsCount].title    = title;
        newsArray[newsCount].content  = content;
        newsArray[newsCount].category = category;
        newsArray[newsCount].date     = date;
        newsCount++;
    }

    file.close();
    cout << "Файл " << filename << " загружен. Статей: " << newsCount << endl;
}

// --------------------------------------------------------
// Поиск статей по категории и году (линейный поиск)
// --------------------------------------------------------
void searchByCategoryAndYear(const string& category, int year) {
    cout << "\nСтатьи в категории \"" << category
         << "\" за " << year << ":\n";

    // приводим искомую категорию к нижнему регистру и убираем пробелы
    string cleanCategory = toLowerCase(trim(category));

    bool found = false;
    for (int i = 0; i < newsCount; i++) {
        string articleCategory = toLowerCase(trim(newsArray[i].category));

        if (articleCategory.find(cleanCategory) != string::npos
            && extractYear(newsArray[i].date) == year) 
        {
            cout << "- " << newsArray[i].title
                 << " (" << newsArray[i].date << ")\n";
            found = true;
        }
    }
    if (!found) {
        cout << "❌ Не найдено ни одной статьи!\n";
    }
}

// --------------------------------------------------------
// Замер времени сортировки и поиска
// --------------------------------------------------------
void measureSortingTime() {
    clock_t start = clock();
    quickSort(newsArray, 0, newsCount - 1);
    clock_t end = clock();

    double duration = double(end - start) / CLOCKS_PER_SEC;
    cout << "\n⏳ Время сортировки: " << duration << " секунд.\n";
}

void measureSearchTime(const string& category, int year) {
    clock_t start = clock();
    searchByCategoryAndYear(category, year);
    clock_t end = clock();

    double duration = double(end - start) / CLOCKS_PER_SEC;
    cout << "\n⏳ Время поиска: " << duration << " секунд.\n";
}

// --------------------------------------------------------
// main()
// --------------------------------------------------------
int main() {
    // 1) Считываем два CSV-файла
    readCSV("true.csv");
    readCSV("fake.csv");

    // Показываем первые три статьи до сортировки
    cout << "\nДо сортировки (первые 3 статьи):\n";
    for (int i = 0; i < min(newsCount, 3); i++) {
        cout << "\"" << newsArray[i].title 
             << "\" - \"" << newsArray[i].date << "\"\n";
    }

    // 2) Замеряем время сортировки
    measureSortingTime();

    // Проверяем первые три статьи после сортировки
    cout << "\nПосле сортировки (первые 3 статьи):\n";
    for (int i = 0; i < min(newsCount, 3); i++) {
        cout << "\"" << newsArray[i].title 
             << "\" - \"" << newsArray[i].date << "\"\n";
    }

    // 3) Замеряем время поиска для "politics" в 2016 году
    measureSearchTime("politics", 2016);

    cout << "\nПрограмма завершена.\n";
    return 0;
}
