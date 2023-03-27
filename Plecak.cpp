#include <iostream>
#include <string>


#include "Deklaracje.h"


int main(int argc, char** argv) { 
    std::string tab_komend[5] = { "-i", "-o", "-p", "-g", "-n" };
    std::string nazwa_pliku_wejsciowego;
    std::string NAZWA_PLIKU_WYJSCIOWEGO;
    int L_POKOLEN;
    int L_OSOBNIKOW;
    double L_PLECAKA;
    if (argc == 11)
    {
        for (int i = 1; i < argc - 1; i += 2) {
            std::string y = argv[i + 1];
            std::string x = argv[i];
            if (x == "-i") {
                nazwa_pliku_wejsciowego = y;
            }
            else if (x == "-o") {
                NAZWA_PLIKU_WYJSCIOWEGO = y;
            }
            else if (x == "-p") {
                L_PLECAKA = stod(y);
                if (L_PLECAKA <= 0) {
                    std::cout << "Niepoprawna waga plecaka";
                    return 1;
                }
            }
            else if (x == "-g") {
                L_POKOLEN = stoi(y);
                if (L_POKOLEN <= 0) {
                    std::cout << "Niepoprawna liczba pokolen";
                    return 1;
                }
            }
            else if (x == "-n") {
                L_OSOBNIKOW = stoi(y);
                if (L_OSOBNIKOW <= 0) {
                    std::cout << "Niepoprawna liczba osobnikow";
                    return 1;
                }
            }
            else 
            {
                std::cerr << "niepoprawnie podano parametry";
                return 1;
            }
        }
    }
    else
    {
        std::cerr << "nieprawidlowe uruchomienie programu" << std::endl;
        std::cerr << "Podaj -i " << '<' << "nazwa" << '>' << " jako nazwe pliku wejsciowego" << std::endl;
        std::cerr << "Podaj -o " << '<' << "nazwa" << '>' << " jako nazwe plik wyjsciowwego" << std::endl;
        std::cerr << "Podaj -p " << '<' << "liczba" << '>' << " jako ladownosc plecaka" << std::endl;
        std::cerr << "Podaj -g " << '<' << "liczba" << '>' << " jako liczba pokolen" << std::endl;
        std::cerr << "Podaj -n " << '<' << "liczba" << '>' << " jako liczba osobnikow w pokoleniu" << std::endl;
        
        return 0;
    }

    algorytm(wczytaj_przedmioty(nazwa_pliku_wejsciowego, L_PLECAKA), L_OSOBNIKOW, L_PLECAKA, L_POKOLEN, NAZWA_PLIKU_WYJSCIOWEGO);
    
}
