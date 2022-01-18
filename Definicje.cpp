#include <iostream>
#include <string>
#include <fstream>
#include <sstream>
#include <random>
#include <chrono>
#include <algorithm>
#include "Deklaracje.h"

/* TO CHECK:x

    1. zmienic generacje plecaka na generacje populacji:
        funkcja generacja populacji shuffleuje przedmioty i po kolei wklada je do plecaka
        funkcja generacja plecaka po prostu wklada przedmiot do plecaka i zwraca plecak

    2. Zaimplementowac algorytm krzyzowania
        - znalezc algorytm

    3. Zmienic algorytm selekcji:
        - shuffle po potomkach - sprawdzam po dwa - jeden z nich zostaje

    4. zmienic bubblesort na zwykle sortowanie

    5. zaaktualizowac funkcje algorytm zeby dzialala dobrze

*/

// ====================== generator liczb losowych ==================
// funkcja ktora zwraca nam losowa liczbe z podanego przedzialu
int losowa(int minimum, int maximum) {
    if (minimum > maximum) {
        return 0;
    }
    static std::default_random_engine silnik(std::chrono::system_clock::now().time_since_epoch().count());
    std::uniform_int_distribution<int> s(minimum, maximum);
    return s(silnik);
}

// ======================== tworzenie puli przedmiotow =====================
// funkcja przedmioty tworzy pule przedmiotow
std::vector<przedmiot> wczytaj_przedmioty(const std::string& NAZWA_PLIKU) { //wektor z przedmiotami z pliku
    std::vector<przedmiot> przedmioty;
    std::ifstream plik(NAZWA_PLIKU);  /// @todo jakas zgrabniejsza nazwa strumienia
    przedmiot item;
    std::string linia;
    while (plik)
    {
        while (getline(plik, linia)) 
        {
            std::stringstream n;
            n << linia;
            if (n >> item.nazwa >> item.waga >> item.wartosc) 
            {
                if (item.waga > 0 && item.wartosc > 0)
                    przedmioty.push_back(item);
            //    std::cout << item.nazwa << " " << item.wartosc << " " << item.waga << std::endl;
            }
            n.clear();  // KS: kasuje flagi bledow 
        }
    }
   // std::cout << std::endl << std::endl;
    return przedmioty;
}

// ================ GENERACJA POPULACJI PLECAKOW ================
// 
// Funkcja generator_pupulacji tworzy liste plecakow (struct plecak)
// 
// 1. tworzeliste plecakow
// 2. tworze plecak
// 3. generuje losowa permutacje przedmiotow
// 4. iteruje od gory po przedmiotach i wkladam do plecaka az skonczy sie w nim miejsce
// 5. przekazuje plecak do listy plecakow
// 6. jezeli mam wszystkie plecaki zwracam liste - jezeli nie - wracam do punktu 2

std::vector < plecak > generator_populacji(int L_OSOBNIKOW, double L_PLECAKA, std::vector<przedmiot> tablica) {
    // 1. Tworze liste placakow
    std::vector < plecak > populacja;
    static std::default_random_engine silnik(std::chrono::system_clock::now().time_since_epoch().count());

    for (int istniejace_plecaki = 0; istniejace_plecaki < L_OSOBNIKOW; istniejace_plecaki++) {
        // 2. tworze plecak
        plecak plecak;
        plecak.waga = 0;
        plecak.wartosc = 0;
        plecak.ocena = 0;
        plecak.przedmioty;
        // 3. losuje permutacje zbioru przedmiotow

        std::shuffle(std::begin(tablica), std::end(tablica), silnik);

        int index_przedmiotu = 0; // zmienna potrzebna do petli

        // 4. iteruje od gory po przedmiotach i wkladam do plecaka az skonczy sie w nim miejsce

        while ((L_PLECAKA - plecak.waga) >= tablica[index_przedmiotu].waga) {

            // wkladam przedmiot do plecaka i doliczam jego statystyki
            plecak.przedmioty.push_back(tablica[index_przedmiotu]);
            plecak.waga += tablica[index_przedmiotu].waga;
            plecak.wartosc += tablica[index_przedmiotu].wartosc;

            // zmieniam index dla kolejnego przedmiotu
            index_przedmiotu++;
            // jezeli skoncza nam sie przedmioty konczymy wkladanie do plecaka
            if (index_przedmiotu > tablica.size() - 1) break;
        }

        //5. przekazuje plecak do listy plecakow
        plecak.ocena = plecak.wartosc/plecak.waga;
        populacja.push_back(plecak);
    }
    // 6. jezeli mam wszystkie plecaki zwracam liste - jezeli nie - wracam do punktu 2
    return populacja;
}

void drukuj_plecaki(plecak* plecak_a, plecak* plecak_b, std::string coto) {
    int max_index = 0;
    int min_index = 0;
    if (plecak_a->przedmioty.size() - 1 > plecak_b->przedmioty.size() - 1) {
        max_index = plecak_a->przedmioty.size() - 1;
        min_index = plecak_b->przedmioty.size() - 1;
    }
    else {
        max_index = plecak_b->przedmioty.size() - 1;
        min_index = plecak_a->przedmioty.size() - 1;
    }

    // std::cout << "\n1" << coto << "\t" << plecak_a->waga << "\t" << plecak_a->wartosc << "\t2" << coto << "\t" << plecak_b->waga << "\t" << plecak_b->wartosc << std::endl;
    for (int i = 0; i <= max_index; i++) {
        if (plecak_a->przedmioty.size() - 1 >= i) {
            std::cout << plecak_a->przedmioty[i].nazwa << "\t" << plecak_a->przedmioty[i].waga << "\t" << plecak_a->przedmioty[i].wartosc;
        }
        else {
            std::cout << "\t\t";
        }
        if (plecak_b->przedmioty.size() - 1 >= i) {
            std::cout << "\t" << plecak_b->przedmioty[i].nazwa << "\t" << plecak_b->przedmioty[i].waga << "\t" << plecak_b->przedmioty[i].wartosc << std::endl;
        }
        else {
            std::cout << "\t\t\t" << std::endl;
        }
    }
}


// ======================== GENERACJA POTOMKA ============================
// Wykorzystuje algortym Order Crossover Operator (OX).
// wybieram losowy przedzial wewnatrz osobnikow (w odbydwu osobnikach te same indeksy nawet jezeli nie istnieja)
// przedmioty z tego przedzialu zostaja w swoim potomku (osobnik_a - potomek_a, osobnik_b - potomek_b)
// pozostale odciete przedmioty wpisujemy do przeciwnego potomka (osobnika_a - potomek_b, osobnik_b - potomek_a) o ile sie zmieszcza
// jezeli sie nie zmieszcza to nie przekazujemy dalej
//
// void krzyzowanie (plecak & osobnik_a, ...);

// std::pair<plecak, plecak> krzyzowanie (const plecak & osobnik_a, const plecak & osobnik_b, const  double L_PLECAKA);

void krzyzowanie(plecak* osobnik_a, plecak* osobnik_b, std::vector<plecak>* potomkowie, double L_PLECAKA) { //krzyzowanie osobnikow
    int max_index_a = osobnik_a->przedmioty.size()-1;
    int max_index_b = osobnik_b->przedmioty.size()-1;

    int lewa_granica_a = losowa(0, max_index_a);
    int prawa_granica_a = losowa(0, max_index_a);
    if(lewa_granica_a > prawa_granica_a){
        int temp = lewa_granica_a;
        lewa_granica_a = prawa_granica_a;
        prawa_granica_a = temp;
    }

    int lewa_granica_b = losowa(0, max_index_b);
    int prawa_granica_b = losowa(0, max_index_b);
    if(lewa_granica_b > prawa_granica_b){
        int temp = lewa_granica_b;
        lewa_granica_b = prawa_granica_b;
        prawa_granica_b = temp;
    }

    std::vector< przedmiot > do_wpisania_a;
    std::vector< przedmiot > do_wpisania_b;

    plecak potomek_a;
    potomek_a.wartosc = 0;
    potomek_a.waga = 0;
    potomek_a.ocena = 0;
    plecak potomek_b;
    potomek_b.wartosc = 0;
    potomek_b.waga = 0;
    potomek_b.ocena = 0;

    for(int i = 0; i <= max_index_a; i++){
        if( lewa_granica_a <= i && i <= prawa_granica_a){
            potomek_a.przedmioty.push_back( osobnik_a->przedmioty[i] );
            potomek_a.wartosc += osobnik_a->przedmioty[i].wartosc;
            potomek_a.waga += osobnik_a->przedmioty[i].waga;
        }else{
            do_wpisania_b.push_back( osobnik_a->przedmioty[i] );
        }
    }
    for(int i = 0; i <= max_index_b; i++){
        if( lewa_granica_b <= i && i <= prawa_granica_b){
            potomek_b.przedmioty.push_back( osobnik_b->przedmioty[i] );
            potomek_b.wartosc += osobnik_b->przedmioty[i].wartosc;
            potomek_b.waga += osobnik_b->przedmioty[i].waga;
        }else{
            do_wpisania_a.push_back( osobnik_b->przedmioty[i] );
        }
    }

    for(int i = 0; i < do_wpisania_a.size(); i++){
        bool powtarza_sie = false;
        for(int j = 0; j < potomek_a.przedmioty.size(); j++){
            if(potomek_a.przedmioty[j].nazwa == do_wpisania_a[i].nazwa){
                powtarza_sie = true;
            }
        }
        if(powtarza_sie){
            potomek_b.przedmioty.push_back(do_wpisania_a[i]);
            potomek_b.wartosc += do_wpisania_a[i].wartosc;
            potomek_b.waga += do_wpisania_a[i].waga;
        }else{
            potomek_a.przedmioty.push_back(do_wpisania_a[i]);
            potomek_a.wartosc += do_wpisania_a[i].wartosc;
            potomek_a.waga += do_wpisania_a[i].waga;
        }
    }

    for(int i = 0; i < do_wpisania_b.size(); i++){
        bool powtarza_sie = false;
        for(int j = 0; j < potomek_b.przedmioty.size(); j++){
            if(potomek_b.przedmioty[j].nazwa == do_wpisania_b[i].nazwa){
                powtarza_sie = true;
            }
        }
        if(powtarza_sie){
            potomek_a.przedmioty.push_back(do_wpisania_b[i]);
            potomek_a.wartosc += do_wpisania_b[i].wartosc;
            potomek_a.waga += do_wpisania_b[i].waga;
        }else{
            potomek_b.przedmioty.push_back(do_wpisania_b[i]);
            potomek_b.wartosc += do_wpisania_b[i].wartosc;
            potomek_b.waga += do_wpisania_b[i].waga;
        }
    }

    potomek_b.ocena = potomek_b.wartosc / potomek_b.waga;
    potomek_a.ocena = potomek_a.wartosc / potomek_a.waga;

    potomkowie->push_back(potomek_b);
    potomkowie->push_back(potomek_a);
}

// ======================= KRZYZOWANIE POPULACJI ==========================
// krzyzujemy rodzicow miedzy soba w taki sposob zeby otrzymac wiecej dzieci niz rodzicow (niz max liczba osobnikow)
// po to by nastepnie w selekcji populacji odrzucic najslabszych potomkow
//
// po kolei kazdego rodzica krzyzuje z losowym osobnikami - dzieki temu uzyskamy dwa razy wiecej dzieci niz rodzicow
// nastepnie potomkow dodajemy do listy potomkowie
// na koncu zwracamy liste potomkowie
//
std::vector<plecak> krzyzowanie_populacji(std::vector< plecak > populacja, int L_PLECAKA) 
{
    std::vector<plecak> potomkowie;

    for (int i = 0; i < populacja.size(); i++) 
    {
        int pierwszy = losowa(0, populacja.size() - 1);
        int drugi    = losowa(0, populacja.size() - 1);
        
        int index = losowa(0, populacja.size() - 1);
        while (index == i) {
            index = losowa(0, populacja.size() - 1);
        }
        krzyzowanie(&populacja[i], &populacja[index], &potomkowie, L_PLECAKA);
        // ocen osobniki w krzyzowaniu
    }
    return potomkowie;
}

// ======================= SORTOWANIE POPULACJI ==========================
// 
bool porownanie_plecakow(plecak plecak1, plecak plecak2) {
    return(plecak1.ocena > plecak2.ocena);
}

std::vector<plecak> sortowanie_populacji(std::vector< plecak > populacja) { //sortowanie populacji;
    std::sort(populacja.begin(), populacja.end(), porownanie_plecakow);
    return populacja;
}

// ======================== SELEKCJA POPULACJI =========================
// 1. losujemy dwa osobniki
// 2. do populacji potomnej przechodzi lepszy z nich.
// 3. powtarzamy, az populacja potomna bedzie miala odpowiednia wielkosc
std::vector<plecak> selekcja_populacji_poprawna(std::vector< plecak > populacja, int L_OSOBNIKOW) {
    // tworzenie obiektu wynikowego
    std::vector<plecak> populacja_potomna;

    while (populacja_potomna.size() < L_OSOBNIKOW) {
        int index1 = losowa(0, populacja.size() - 1);
        int index2 = losowa(0, populacja.size() - 1);

        if (porownanie_plecakow(populacja[index1], populacja[index2])) {
            populacja_potomna.push_back(populacja[index1]);
        }
        else {
            populacja_potomna.push_back(populacja[index2]);
        }
    }
    //     zwrot obiektu wynikowego
    return populacja_potomna;
}

// ===================== DRUKUJ PLECAK =============
// tutaj po prostu drukujemy jeden plecak
//
//void drukuj_plecak(std::ofstream & plik, const plecak & plecak_dobry);

void drukuj_plecak(std::ofstream* plik, int numer_generacji, plecak plecak_dobry, plecak plecak_zly) {
    if ((*plik).is_open()) {
        *plik << "generacja " << numer_generacji << ", waga " << plecak_dobry.waga << ", wartosc" << " " << plecak_dobry.wartosc << ":" << std::endl;
        for (int index = 0; index < plecak_dobry.przedmioty.size(); index++) {
            *plik << "\t\t" << plecak_dobry.przedmioty[index].nazwa << " " << plecak_dobry.przedmioty[index].waga << " " << plecak_dobry.przedmioty[index].wartosc << std::endl;
        }
    }
}

plecak najlepszy( std::vector<plecak> populacja ){
    plecak najlepszy;
    najlepszy.wartosc = 0;
    najlepszy.ocena = 0;
    for(int i = 0; i < populacja.size(); i++){
        if(populacja[i].wartosc > najlepszy.wartosc){
            najlepszy = populacja[i];
        }
    }
    return najlepszy;
}

// ===================== FUNKCJA ROZRUCHOWA ALGORYTMU ===================
// to wlasnie ta funkcja wykona cala reszte funkcji w odpowiedniej kolejnosci
// w niej tez determinujemy liczbe generacji
//
// dzieki dobrym nazwom funkcji dalsze komentowanie algorytmu nie ma sensu
//
// zwracamy najlepszego osobnika
//
plecak algorytm(std::vector<przedmiot> tablica, int L_OSOBNIKOW, double L_PLECAKA, int L_POKOLEN, std::string NAZWA_PLIKU_WYJSCIOWEGO)
{
    // KS: Tablica: czy musi kopia?
    // KS: NAZWA_PLIKU_WYJSCIOWEGO: czy musi byc kopia?
    
    std::vector<plecak> populacja;
    std::ofstream plik;
    plik.open(NAZWA_PLIKU_WYJSCIOWEGO);

    populacja = generator_populacji(L_OSOBNIKOW, L_PLECAKA, tablica);
    int numer_populacji = 0;

    // zabezpieczenie przed nieskonczona petla
    if (populacja.size() < 2) 
        return populacja[0];  // KS: Co gdy populacja.size() == 0?

    populacja = sortowanie_populacji(populacja); // KS: To nie jest dobry pomysl.
    // wypisac numer pokolenia, a potem plecak
    drukuj_plecak(&plik, numer_populacji, populacja[0], populacja[populacja.size() - 1]);

    struct plecak najlepsiejszy;
    najlepsiejszy.wartosc = 0;
    najlepsiejszy.ocena = 0;

    for (int i = 0; i < L_POKOLEN; i++)
    {
        std::cout << "\nliczee generacje: " << numer_populacji << " LICZB elementow: " << populacja.size() << std::endl;
        std::vector<plecak> nowa_populacja;
        nowa_populacja = krzyzowanie_populacji(populacja, L_PLECAKA);
        nowa_populacja = selekcja_populacji_poprawna(nowa_populacja, L_OSOBNIKOW);
        plecak debesciak;
        debesciak = najlepszy(nowa_populacja);
        numer_populacji++;
        populacja = nowa_populacja;
        drukuj_plecak(&plik, numer_populacji, debesciak, populacja[populacja.size() - 1]);
        if (debesciak.wartosc > najlepsiejszy.wartosc) {
            najlepsiejszy = debesciak;
        }
    }
 //   std::cout << "\nnajlepszy plecak:\twaga - " << najlepszy.waga << "\twartosc - " << najlepszy.wartosc << std::endl;
  //  for (int i = 0; i < najlepszy.przedmioty.size(); i++) {
  //      std::cout << najlepszy.przedmioty[i].nazwa << "\t" << najlepszy.przedmioty[i].waga << "\t" << najlepszy.przedmioty[i].wartosc << std::endl;
  //  }
    plik.close();
    return najlepsiejszy;
}


//5.wypisac najlepszy osobnik

// Wygenerowac duza liczbe (>1000) przedmiotow (calkowicie losowo).



