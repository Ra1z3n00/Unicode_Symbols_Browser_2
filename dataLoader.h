#pragma once

#include <iostream>
#include <vector>
#include <string>
#include <sstream>
#include <algorithm>
#include <tuple>
#include <set>
#include <fstream>

using namespace std;

// --- Data Structures ---
struct Symbol {
  string name_it;  // Nome leggibile in italiano
  string name_en;  // Parole leggibile in inglese
  string hex;      // Codice esadecimale (es. "221E")
  string ch;       // Carattere UTF-8
  string latex;    // Comando LaTeX
  string category;
};

// --- Helper Functions ---

inline unsigned int hexToUInt(const string &hex) {
  unsigned int val = 0;
  stringstream ss;
  ss << std::hex << hex;
  ss >> val;
  return val;
}

inline string codepointToUtf8(unsigned int cp) {
  string out;
  if (cp <= 0x7F) {
    out.push_back((char)cp);
  } else if (cp <= 0x7FF) {
    out.push_back((char)(0xC0 | ((cp >> 6) & 0x1F)));
    out.push_back((char)(0x80 | (cp & 0x3F)));
  } else if (cp <= 0xFFFF) {
    out.push_back((char)(0xE0 | ((cp >> 12) & 0x0F)));
    out.push_back((char)(0x80 | ((cp >> 6) & 0x3F)));
    out.push_back((char)(0x80 | (cp & 0x3F)));
  } else {
    out.push_back((char)(0xF0 | ((cp >> 18) & 0x07)));
    out.push_back((char)(0x80 | ((cp >> 12) & 0x3F)));
    out.push_back((char)(0x80 | ((cp >> 6) & 0x3F)));
    out.push_back((char)(0x80 | (cp & 0x3F)));
  }
  return out;
}

// Funzione per la ricerca Case Insensitive
inline bool containsCaseInsensitive(const std::string& str, const std::string& search) {
    auto it = std::search(
        str.begin(), str.end(),
        search.begin(), search.end(),
        [](char ch1, char ch2) { return std::toupper(ch1) == std::toupper(ch2); }
    );
    return (it != str.end());
}

// Funzione per ripulire le stringhe da virgolette e spazi vuoti
inline string cleanString(string str) {
    // 1. Rimuove tutte le virgolette "
    str.erase(remove(str.begin(), str.end(), '\"'), str.end());
    
    // 2. Rimuove gli spazi iniziali
    size_t start = str.find_first_not_of(" \t");
    if (start == string::npos) return ""; // Se è tutto vuoto, ritorna stringa vuota
    str = str.substr(start);
    
    // 3. Rimuove gli spazi finali
    size_t end = str.find_last_not_of(" \t");
    return str.substr(0, end + 1);
}

// --- IL NUOVO MOTORE DI LETTURA CSV ---
inline vector<Symbol> buildDatabase() {
    vector<Symbol> out;
    
    // 1. Apriamo il file (assicurati che si chiami così e sia nella cartella giusta)
    ifstream file("database.csv");
    
    // 2. Controllo errori: se il file non esiste, restituiamo una lista vuota
    if (!file.is_open()) {
        cerr << "ERRORE: Impossibile trovare il file database.csv!" << endl;
        return out; 
    }

    string line;
    // 3. Leggiamo il file riga per riga
    while (getline(file, line)) {
        
        // Pulizia per file salvati su Windows (rimuove il carattere invisibile \r)
        line.erase(remove(line.begin(), line.end(), '\r'), line.end());

        // Se la riga è vuota o è un commento (inizia con #), la saltiamo!
        if (line.empty() || line[0] == '#') {
            continue;
        }

        // 4. "Spezziamo" la riga usando il punto e virgola
        stringstream ss(line);
        string name, keywords, hex, latex, category;

        // Estraiamo e RIPULIAMO le colonne
        getline(ss, name, ';');     name = cleanString(name);
        getline(ss, keywords, ';'); keywords = cleanString(keywords);
        getline(ss, hex, ';');      hex = cleanString(hex);
        getline(ss, latex, ';');    latex = cleanString(latex);
        
        // Estraiamo e RIPULIAMO l'ultima colonna
        getline(ss, category);      category = cleanString(category);

        // 5. Calcoliamo il carattere UTF-8 dal codice Hex pulito
        unsigned int cp = hexToUInt(hex);
        string ch = codepointToUtf8(cp);

        // 6. Inseriamo il simbolo finito nella lista
        out.push_back({name, keywords, hex, ch, latex, category});
    }

    file.close();
    return out;
}
