#pragma once
#include <iostream>
#include <vector>


///@todo skomentowac w doxygenie
struct przedmiot {
	std::string nazwa;
	double waga;
	int wartosc;
};

struct plecak {
	std::vector<przedmiot> przedmioty;
	double waga;
	int wartosc;
    double ocena;
};

/** Funkcja sczytuje przedmioty z pliku. */
std::vector<przedmiot> wczytaj_przedmioty(const std::string & NAZWA_PLIKU);

struct plecak algorytm(std::vector<przedmiot> tablica, int L_OSOBNIKOW, double L_PLECAKA, int L_POKOLEN, std::string NAZWA_PLIKU_WYJSCIOWEGO);
