#include <iostream>
#include <string>
#include <fstream>
#include <sstream>
#include <random>
#include <chrono>
#include <algorithm>
#include "Deklaracje.h"

/** \brief Funkcja losujaca.
 *
 *  funkcja przyjmuje przedzial z ktorego ma wylosowac liczbe i zwrocic ta liczbe
 *
 * @param minimum
 * @param maximum
 * @return losowa_liczba
 */
int losowa(int minimum, int maximum) {
    if (minimum > maximum) {
        return 0;
    }
    static std::default_random_engine silnik(std::chrono::system_clock::now().time_since_epoch().count());
    std::uniform_int_distribution<int> s(minimum, maximum);
    return s(silnik);
}
/** \brief Funkcja ocenia plecak.
 *
 * Funkcja ocenia plecak w zaleznosci od jego wartosci i tego jak jest wypelniony
 *
 * @param plecak
 * @param L_PLECAKA
 * @return ocena
 */
double ocen_plecak(const plecak& plecak, const double& L_PLECAKA){
    if(plecak.waga > L_PLECAKA){
        return 0;
    }
    return (plecak.wartosc*1.5 + (L_PLECAKA - plecak.waga));
}

/** \breif Funkcja wczytuje przedmioty z pliku do listy przedmiotow.
 *
 * Funkcja wczytuje przedmiot z kazdej linijki czyli wczytuje jego:
 *      - nazwe
 *      - wage
 *      - wartosc
 * Nastepnie dodaje przedmiot do wynikowego vectora
 *
 * @param NAZWA_PLIKU
 * @return vector_przedmiotow
 */
std::vector<przedmiot> wczytaj_przedmioty(const std::string& NAZWA_PLIKU) {
    std::vector<przedmiot> przedmioty;
    std::ifstream plik(NAZWA_PLIKU);  /// @todo jakas zgrabniejsza nazwa strumienia
    std::string linia;
    while (plik)
    {
        while (getline(plik, linia)) 
        {
            przedmiot item;
            std::stringstream n;
            n << linia;
            if (n >> item.nazwa >> item.waga >> item.wartosc) 
            {
                if (item.waga > 0 && item.wartosc > 0)
                    przedmioty.push_back(item);
            }
            n.clear();  // KS: kasuje flagi bledow 
        }
    }
    return przedmioty;
}
/** \brief Funkcja generuje populacje czyli tworzy losowe poprawne plecaki i zwraca je jako vector plecakow.
 *
 *  Funkcja kopiuje pule_przedmiotow bo wykorzystuje tasowanie a nie chce wplywac na oryginal
 *
 * Dzialanie funkcji:
 *      1. Stworzenie listyy wynikowej
 *      2. Stworzenie nowego pustego plecaka
 *      3. Przetasowanie puli przedmitow - ze wzgledu na ten punkt funkcja tworzy kopie puli
 *      4. Wkladamy do plecaka przedmiotu z puli az jakis sie nie zmiesci
 *      5. Przekazuje gotowy plecak do listy wynikowej
 *      6. jezeli lista < wymagana_liczebnosc_populacji wracam do punktu 2
 *
 * @param L_OSOBNIKOW
 * @param L_PLECAKA
 * @param pula_przedmiotow
 * @return vector_plecakow
 */
std::vector < plecak > generator_populacji(const int& L_OSOBNIKOW, const double& L_PLECAKA, std::vector<przedmiot> pula_przedmiotow) {
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

        std::shuffle(std::begin(pula_przedmiotow), std::end(pula_przedmiotow), silnik);

        int index_przedmiotu = 0; // zmienna potrzebna do petli

        // 4. iteruje od gory po przedmiotach i wkladam do plecaka az skonczy sie w nim miejsce

        while ((L_PLECAKA - plecak.waga) >= pula_przedmiotow[index_przedmiotu].waga) {

            // wkladam przedmiot do plecaka i doliczam jego statystyki
            plecak.przedmioty.push_back(pula_przedmiotow[index_przedmiotu]);
            plecak.waga += pula_przedmiotow[index_przedmiotu].waga;
            plecak.wartosc += pula_przedmiotow[index_przedmiotu].wartosc;

            // zmieniam index dla kolejnego przedmiotu
            index_przedmiotu++;
            // jezeli skoncza nam sie przedmioty konczymy wkladanie do plecaka
            if (index_przedmiotu > pula_przedmiotow.size() - 1) break;
        }

        //5. przekazuje plecak do listy plecakow
        plecak.ocena = ocen_plecak(plecak, L_PLECAKA);
        populacja.push_back(plecak);
    }
    // 6. jezeli mam wszystkie plecaki zwracam liste - jezeli nie - wracam do punktu 2
    return populacja;
}
/** \brief Funkcja losuje przedzial wewnatrz plecaka
 *
 * Funkcja przyjmuje plecak - liczy jego elementy i losuje przedzial:
 * czyli losuje lewa i prawa grancie przedzialu
 * zwraca granice w parze
 *
 * @param plecak
 * @return para(lewa_granica, prawa_granica)
 */
std::pair<int, int> licz_granice_plecaka(const plecak& plecak){
    int lewa, prawa;
    int max_index_a = plecak.przedmioty.size()-1;

    lewa = losowa(0, max_index_a);
    prawa = losowa(0, max_index_a);

    if(lewa > prawa){
        int temp = lewa;
        lewa = prawa;
        prawa = temp;
    }

    return std::make_pair(lewa, prawa);
}
/** \brief Funkcja dzieli przedmioty z plecaka na te do przekazania do potomka_a i potomka_b
 *
 * Funkcja losuje przedzial wedlug ktorego dzieli przedmioty na te do przekazania do dwoch potomkow
 * Te ze srodka przedzialu podaje do swojego potomka a te z zewnatrz przedzialu wpisuje do listy
 * z ktorej przypiszemy przedmioty do drugiego potomka
 *
 * @param rodzic
 * @param potomek
 * @param do_przekazania
 */
void przygotowanie_krzyzowania(plecak & rodzic, plecak & potomek, std::vector<przedmiot> & do_przekazania){
    std::pair<int, int> granice = licz_granice_plecaka(rodzic);

    for(int i = 0; i <= rodzic.przedmioty.size()-1; i++){
        if( granice.first <= i && i <= granice.second){
            potomek.przedmioty.push_back( rodzic.przedmioty[i] );
            potomek.wartosc += rodzic.przedmioty[i].wartosc;
            potomek.waga += rodzic.przedmioty[i].waga;
        }else{
            do_przekazania.push_back( rodzic.przedmioty[i] );
        }
    }
}
/** \brief Funkcja wpisuje do potomkow przedmioty z list do wpisania
 *
 * Funkcja wpisuje do potomkow przedmioty z list utworzonych z rodzicow
 * Jezeli jakais przedmiot z listy nie zmiesci sie do wlasciwego potomka
 * przekazujemy go do drugiego potomka
 *
 * @param potomek_wlasciwy
 * @param potomek_odpad
 * @param do_wpisania
 */
void przypisanie_przedmiotow_potomkom(plecak & potomek_wlasciwy, plecak & potomek_odpad, std::vector<przedmiot> & do_wpisania){
    for(int i = 0; i < do_wpisania.size(); i++){
        bool powtarza_sie = false;
        for(int j = 0; j < potomek_wlasciwy.przedmioty.size(); j++){
            if(potomek_wlasciwy.przedmioty[j].nazwa == do_wpisania[i].nazwa){
                powtarza_sie = true;
            }
        }
        if(powtarza_sie){
            potomek_odpad.przedmioty.push_back(do_wpisania[i]);
            potomek_odpad.wartosc += do_wpisania[i].wartosc;
            potomek_odpad.waga += do_wpisania[i].waga;
        }else{
            potomek_wlasciwy.przedmioty.push_back(do_wpisania[i]);
            potomek_wlasciwy.wartosc += do_wpisania[i].wartosc;
            potomek_wlasciwy.waga += do_wpisania[i].waga;
        }
    }
}
/** \brief Funkcja sterujaca calym procesem krzyzowania
 *
 * Funkcja przyjmuje dwa plecaki i krzyzuje je zwracajac dwa potomki
 * Funkjca tworzy listy wykorzystywane w kolejnych funkjcach
 *
 * @param osobnik_a
 * @param osobnik_b
 * @param potomkowie
 * @param L_PLECAKA
 */
void krzyzowanie(plecak & osobnik_a, plecak & osobnik_b, std::vector<plecak> & potomkowie,const double & L_PLECAKA) { //krzyzowanie osobnikow
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

    przygotowanie_krzyzowania(osobnik_a, potomek_a, do_wpisania_b);
    przygotowanie_krzyzowania(osobnik_b, potomek_b, do_wpisania_a);

    przypisanie_przedmiotow_potomkom(potomek_a, potomek_b, do_wpisania_a);
    przypisanie_przedmiotow_potomkom(potomek_b, potomek_a, do_wpisania_b);

    potomek_a.ocena = ocen_plecak(potomek_a, L_PLECAKA);
    potomek_b.ocena = ocen_plecak(potomek_b, L_PLECAKA);

    potomkowie.push_back(potomek_b);
    potomkowie.push_back(potomek_a);
}

// ======================= KRZYZOWANIE POPULACJI ==========================
// krzyzujemy rodzicow miedzy soba w taki sposob zeby otrzymac wiecej dzieci niz rodzicow (niz max liczba osobnikow)
// po to by nastepnie w selekcji populacji odrzucic najslabszych potomkow
//
// po kolei kazdego rodzica krzyzuje z losowym osobnikami - dzieki temu uzyskamy dwa razy wiecej dzieci niz rodzicow
// nastepnie potomkow dodajemy do listy potomkowie
// na koncu zwracamy liste potomkowie
//

/** \brief Funkcja decyduje ktory plecak nalezy skrzyzowac z ktorym
 *
 * Funkcja losuje dwa plecaki i krzyzuje je ze soba wedlug oceny
 *
 * @param populacja
 * @param L_PLECAKA
 * @return
 */
std::vector<plecak> krzyzowanie_populacji(std::vector< plecak > & populacja, const int & L_PLECAKA)
{
    std::vector<plecak> potomkowie;

    for (int i = 0; i < populacja.size(); i++) 
    {
        int index = losowa(0, populacja.size() - 1);
        krzyzowanie(populacja[i], populacja[index], potomkowie, L_PLECAKA);
    }
    return potomkowie;
}

// Funkcja porownojaca plecaki
bool porownanie_plecakow(plecak plecak1, plecak plecak2) {
    return(plecak1.ocena > plecak2.ocena);
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

void drukuj_plecak(std::ofstream* plik, int numer_generacji, plecak plecak_dobry) {
    if ((*plik).is_open()) {
        *plik << "generacja " << numer_generacji << ", waga " << plecak_dobry.waga << ", wartosc" << " " << plecak_dobry.wartosc << ", ocena "<< plecak_dobry.ocena << ":" << std::endl;
        for (int index = 0; index < plecak_dobry.przedmioty.size(); index++) {
            *plik << "\t\t" << plecak_dobry.przedmioty[index].nazwa << " " << plecak_dobry.przedmioty[index].waga << " " << plecak_dobry.przedmioty[index].wartosc << std::endl;
        }
    }
}

// Funkcja wybiera najlepszy element sposrod
plecak najlepszy( std::vector<plecak> populacja ){
    plecak najlepszy;
    najlepszy.wartosc = 0;
    najlepszy.ocena = 0;
    for(int i = 0; i < populacja.size(); i++){
        if(porownanie_plecakow(populacja[i], najlepszy)){
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
    if(populacja.empty()){
        std::cout << "\nBlad danych wejsciowych";
        plik << "\nBlad danych wejsciowych";
        plecak plecak;
        return plecak;
    }
    else if (populacja.size() < 2) {
        drukuj_plecak(&plik, numer_populacji, najlepszy(populacja));
        return populacja[0];
    }
    std::cout << "\nlicze generacje: " << numer_populacji << " LICZB elementow: " << populacja.size() << std::endl;
    drukuj_plecak(&plik, numer_populacji, najlepszy(populacja));

    struct plecak najlepsiejszy;
    najlepsiejszy.wartosc = 0;
    najlepsiejszy.ocena = 0;

    for (int i = 0; i < L_POKOLEN; i++)
    {
        std::cout << "\nlicze generacje: " << numer_populacji+1 << " LICZB elementow: " << populacja.size() << std::endl;
        std::vector<plecak> nowa_populacja;
        plecak debesciak;
        nowa_populacja = krzyzowanie_populacji(populacja, L_PLECAKA);
        nowa_populacja = selekcja_populacji_poprawna(nowa_populacja, L_OSOBNIKOW);
        debesciak = najlepszy(nowa_populacja);
        numer_populacji++;
        populacja = nowa_populacja;
        drukuj_plecak(&plik, numer_populacji, debesciak);
        if (debesciak.ocena > najlepsiejszy.ocena) {
            najlepsiejszy = debesciak;
        }
    }
    std::cout << "\nnajlepszy plecak:\twaga - " << najlepsiejszy.waga << "\twartosc - " << najlepsiejszy.wartosc << std::endl;
    for (int i = 0; i < najlepsiejszy.przedmioty.size(); i++) {
        std::cout << najlepsiejszy.przedmioty[i].nazwa << "\t" << najlepsiejszy.przedmioty[i].waga << "\t" << najlepsiejszy.przedmioty[i].wartosc << std::endl;
    }
    plik.close();
    return najlepsiejszy;
}
