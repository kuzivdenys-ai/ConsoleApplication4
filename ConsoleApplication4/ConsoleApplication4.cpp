#include <iostream>
#include <string>
#include <vector>
#include <stdexcept>
#include <cstdlib>
#ifdef _WIN32
#include <windows.h>
#endif
using namespace std;

const int    MIN_POWER = 1, MAX_POWER = 2000;   // к.с.
const double MIN_VOLUME = 0.1, MAX_VOLUME = 20.0;  // літри

// ===== Безпечне введення: повторюємо запит, доки не введуть коректно =====
void checkEof(istream& in) {            // кінець потоку (Ctrl+Z / Ctrl+D) - виходимо
    if (in.eof()) { cout << "\nКінець введення.\n"; exit(0); }
}
int readInt(istream& in, const string& prompt, int lo, int hi) {
    int x;
    while (true) {
        cout << prompt;
        if (in >> x && in.peek() == '\n' && x >= lo && x <= hi) { in.ignore(1000, '\n'); return x; }
        checkEof(in);
        cout << "  Помилка: потрібне ціле число від " << lo << " до " << hi << ".\n";
        in.clear();                     // скидаємо стан помилки
        in.ignore(10000, '\n');         // викидаємо сміття з рядка
    }
}
double readDouble(istream& in, const string& prompt, double lo, double hi) {
    double x;
    while (true) {
        cout << prompt;
        if (in >> x && in.peek() == '\n' && x >= lo && x <= hi) { in.ignore(1000, '\n'); return x; }
        checkEof(in);
        cout << "  Помилка: потрібне число від " << lo << " до " << hi << " (через крапку).\n";
        in.clear();
        in.ignore(10000, '\n');
    }
}
string readString(istream& in, const string& prompt) {
    string s;
    while (true) {
        cout << prompt;
        getline(in, s);
        checkEof(in);
        if (s.find_first_not_of(" \t") != string::npos) return s;
        cout << "  Помилка: рядок не може бути порожнім.\n";
    }
}

// ===== Клас АВТОМОБІЛЬ =====
class Car {
    string brand, color;
    int    power;     // 0 = не задано
    double volume;    // 0 = не задано
public:
    Car() : power(0), volume(0) {}
    Car(const string& b, const string& c, int p, double v) : power(0), volume(0) {
        setBrand(b); setColor(c); setPower(p); setVolume(v);   // сеттери можуть кинути виняток
    }
    ~Car() {}

    void setBrand(const string& b) { if (b.empty()) throw invalid_argument("Марка не може бути порожньою."); brand = b; }
    void setColor(const string& c) { if (c.empty()) throw invalid_argument("Колір не може бути порожнім."); color = c; }
    void setPower(int p) {
        if (p < MIN_POWER || p > MAX_POWER) throw invalid_argument("Потужність має бути від 1 до 2000 к.с.");
        power = p;
    }
    void setVolume(double v) {
        if (!(v >= MIN_VOLUME && v <= MAX_VOLUME))   // така форма ловить і NaN
            throw invalid_argument("Об'єм двигуна має бути від 0.1 до 20 л.");
        volume = v;
    }

    string getBrand() const { return brand; }
    string getColor() const { return color; }
    int getPower() const { if (power == 0) throw runtime_error("Потужність ще не встановлена."); return power; }
    double getVolume() const { if (volume == 0) throw runtime_error("Об'єм двигуна ще не встановлений."); return volume; }

    Car& operator+=(int hp) {             // збільшення потужності
        if (power == 0) throw runtime_error("Потужність ще не встановлена.");
        if (hp <= 0 || hp > MAX_POWER - power)
            throw invalid_argument("Приріст має бути додатним, а потужність - не більше 2000 к.с.");
        power += hp;
        return *this;
    }

    // Дружні функції, бо зліва стоїть потік, а не Car
    friend ostream& operator<<(ostream& out, const Car& c) {
        return out << c.brand << ", " << c.color << ", " << c.power << " к.с., " << c.volume << " л";
    }
    friend istream& operator>>(istream& in, Car& c) {
        Car tmp;                          // вводимо у тимчасовий об'єкт
        tmp.setBrand(readString(in, "  Марка: "));
        tmp.setColor(readString(in, "  Колір: "));
        tmp.setPower(readInt(in, "  Потужність (к.с.): ", MIN_POWER, MAX_POWER));
        tmp.setVolume(readDouble(in, "  Об'єм двигуна (л): ", MIN_VOLUME, MAX_VOLUME));
        c = tmp;                          // і лише потім копіюємо
        return in;
    }
};

// ===== Колекція автомобілів =====
class Garage {
    vector<Car> cars;
public:
    void add(const Car& c) { cars.push_back(c); }
    int size() const { return (int)cars.size(); }
    Car& at(int i) { if (i < 0 || i >= size()) throw invalid_argument("Немає автомобіля з таким номером."); return cars[i]; }
    const vector<Car>& all() const { return cars; }

    // Перевантажений метод пошуку: однакова назва, різні параметри
    vector<Car> find(const string& brand) const {                       // за маркою
        vector<Car> r; for (const Car& c : cars) if (c.getBrand() == brand) r.push_back(c); return r;
    }
    vector<Car> find(const string& brand, const string& color) const {  // за маркою і кольором
        vector<Car> r; for (const Car& c : cars) if (c.getBrand() == brand && c.getColor() == color) r.push_back(c); return r;
    }
    vector<Car> find(int minP, int maxP) const {                        // за потужністю
        if (minP > maxP) throw invalid_argument("Мінімум більший за максимум.");
        vector<Car> r; for (const Car& c : cars) if (c.getPower() >= minP && c.getPower() <= maxP) r.push_back(c); return r;
    }
    vector<Car> find(double minV, double maxV) const {                  // за об'ємом
        if (minV > maxV) throw invalid_argument("Мінімум більший за максимум.");
        vector<Car> r; for (const Car& c : cars) if (c.getVolume() >= minV && c.getVolume() <= maxV) r.push_back(c); return r;
    }
};

void printList(const vector<Car>& list) {
    if (list.empty()) cout << "  (нічого немає)\n";
    for (size_t i = 0; i < list.size(); i++) cout << "  " << i + 1 << ". " << list[i] << "\n";
}

void demo() {                             
    Car empty;
    try { empty.setPower(-5); }
    catch (const invalid_argument& e) { cout << "  setPower(-5): " << e.what() << "\n"; }
    try { empty.setVolume(0); }
    catch (const invalid_argument& e) { cout << "  setVolume(0): " << e.what() << "\n"; }
    try { empty.getPower(); }
    catch (const runtime_error& e) { cout << "  getPower(): " << e.what() << "\n"; }
    try { Car c("BMW", "синій", 100, 1.6); c += 5000; }
    catch (const invalid_argument& e) { cout << "  += 5000: " << e.what() << "\n"; }
}

int main() {
#ifdef _WIN32
    SetConsoleOutputCP(65001);            // українські літери в консолі Windows
    SetConsoleCP(65001);
#endif
    Garage garage;
    while (true) {
        cout << "\n1-Додати  2-Показати  3-Збільшити потужність (+=)  4-Пошук за маркою\n"
            "5-За маркою і кольором  6-За потужністю  7-За об'ємом  8-Винятки  0-Вихід\n";
        int choice = readInt(cin, "Ваш вибір: ", 0, 8);
        if (choice == 0) break;
        try {
            if (choice == 1) { Car c; cin >> c; garage.add(c); }
            else if (choice == 2) printList(garage.all());
            else if (choice == 3) {
                if (garage.size() == 0) { cout << "Список порожній.\n"; continue; }
                printList(garage.all());
                int n = readInt(cin, "Номер авто: ", 1, garage.size());
                int hp = readInt(cin, "На скільки к.с. збільшити: ", 1, MAX_POWER);
                garage.at(n - 1) += hp;
                cout << "Тепер: " << garage.at(n - 1) << "\n";
            }
            else if (choice == 4) printList(garage.find(readString(cin, "Марка: ")));
            else if (choice == 5) {
                string b = readString(cin, "Марка: "), c = readString(cin, "Колір: ");
                printList(garage.find(b, c));
            }
            else if (choice == 6) {
                int lo = readInt(cin, "Потужність від: ", 0, MAX_POWER), hi = readInt(cin, "Потужність до: ", 0, MAX_POWER);
                printList(garage.find(lo, hi));
            }
            else if (choice == 7) {
                double lo = readDouble(cin, "Об'єм від: ", 0, MAX_VOLUME), hi = readDouble(cin, "Об'єм до: ", 0, MAX_VOLUME);
                printList(garage.find(lo, hi));
            }
            else demo();
        }
        catch (const invalid_argument& e) { cout << "Некоректне значення: " << e.what() << "\n"; }
        catch (const runtime_error& e) { cout << "Помилка читання поля: " << e.what() << "\n"; }
        catch (const exception& e) { cout << "Інша помилка: " << e.what() << "\n"; }
    }
    cout << "Кінець програми.\n";
}
