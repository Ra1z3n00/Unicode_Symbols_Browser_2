#ifndef DATABASE_H
#define DATABASE_H

#include <iostream>
#include <vector>
#include <string>
#include <sstream>
#include <algorithm>
#include <tuple>
#include <set>

using namespace std;

// --- Data Structures ---
struct Symbol {
  string name;     // Nome leggibile
  string keywords; // Parole chiave
  string hex;      // Codice esadecimale (es. "221E")
  string ch;       // Carattere UTF-8
  string latex;    // Comando LaTeX
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

inline bool containsCaseInsensitive(const string &text, const string &pat) {
  if (pat.empty())
    return true;
  string a = text, b = pat;
  transform(a.begin(), a.end(), a.begin(), ::tolower);
  transform(b.begin(), b.end(), b.begin(), ::tolower);
  return a.find(b) != string::npos;
}


inline vector<Symbol> buildDatabase() {
  vector<tuple<string, string, string, string>> raw = {
      // --- OPERATORI BASE ---
      {"Segno piu", "Plus add", "002B", "+"},
      {"Segno meno", "Minus subtract dash", "2212", "-"},
      {"Segno di moltiplicazione", "Times x multiply", "00D7", "\\times"},
      {"Operatore punto", "Dot multiply dotopt middot", "22C5", "\\cdot"},
      {"Segno di divisione", "Division divide", "00F7", "\\div"},
      {"Uguale", "Equals", "003D", "="},
      {"Diverso", "Neq not equal", "2260", "\\neq"},
      {"Minore di", "Less then 003C", "003C", "<"},
      {"Maggiore di", "Greater than 003E", "003E", ">"},
      {"Minore o uguale", "Le 2264", "2264", "\\leq"},
      {"Maggiore o uguale", "Ge 2265", "2265", "\\geq"},
      {"Piu o meno", "Plus-minus sign", "00B1", "\\pm"},
      {"Meno o piu", "Minus-plus sign", "2213", "\\mp"},
      
      // --- FUNZIONI E CALCOLO (Nuovi!) ---
      {"Composizione funzione", "Compose function ring operator", "2218", "\\circ"},
      {"Pavimento (Floor Sinistro)", "Floor left floor rounding", "230A", "\\lfloor"},
      {"Pavimento (Floor Destro)", "Floor right floor rounding", "230B", "\\rfloor"},
      {"Soffitto (Ceiling Sinistro)", "Ceiling left ceil rounding", "2308", "\\lceil"},
      {"Soffitto (Ceiling Destro)", "Ceiling right ceil rounding", "2309", "\\rceil"},
      {"Definito come (Delta)", "Defined as delta equal", "225C", "\\triangleq"},
      {"Definito come (Due punti)", "Defined as coloneq assignment", "2254", "\\coloneqq"},
      
      // --- ALGEBRA LINEARE E VETTORI (Nuovi!) ---
      {"Parentesi Angolare Sx", "Angle bracket left bra inner product", "27E8", "\\langle"},
      {"Parentesi Angolare Dx", "Angle bracket right ket inner product", "27E9", "\\rangle"},
      {"Norma (Doppia barra)", "Norm double vertical bar", "2016", "\\|"},
      {"Prodotto Tensore", "Tensor product otimes", "2297", "\\otimes"},
      {"Somma Diretta (XOR)", "Direct sum oplus xor", "2295", "\\oplus"},
      {"Prodotto scalare (Punto)", "Dot product scalar", "22C5", "\\cdot"},

      // --- INSIEMI NUMERICI (Blackboard Bold) ---
      {"Numeri Naturali", "Natural set N double struck", "2115", "\\mathbb{N}"},
      {"Numeri Interi", "Integers set Z double struck", "2124", "\\mathbb{Z}"},
      {"Numeri Razionali", "Rationals set Q double struck", "211A", "\\mathbb{Q}"},
      {"Numeri Reali", "Reals set R double struck", "211D", "\\mathbb{R}"},
      {"Numeri Complessi", "Complex set C double struck", "2102", "\\mathbb{C}"},
      {"Numeri Primi", "Primes set P double struck", "2119", "\\mathbb{P}"},

      // --- LOGICA E INSIEMISTICA ---
      {"Per ogni", "Forall all", "2200", "\\forall"},
      {"Esiste", "Exists", "2203", "\\exists"},
      {"Non esiste", "Not exists", "2204", "\\nexists"},
      {"Insieme vuoto", "Empty null set", "2205", "\\emptyset"},
      {"Elemento di", "In set element belongs", "2208", "\\in"},
      {"Non appartiene", "Not in set element", "2209", "\\notin"},
      {"Unione", "Union set", "222A", "\\cup"},
      {"Intersezione", "Intersection set", "2229", "\\cap"},
      {"Sottoinsieme", "Subset of", "2282", "\\subset"},
      {"Sottoinsieme o uguale", "Subset of or equal to", "2286", "\\subseteq"},
      {"Contiene", "Superset", "2283", "\\supset"},
      {"Contiene o e uguale a", "Superset of or equal", "2287", "\\supseteq"},

      {"Implica", "Implies arrow right double", "21D2", "\\implies"},
      {"Se e solo se", "Iff double arrow", "21D4", "\\iff"},
      {"Pertanto", "Therefore ergo", "2234", "\\therefore"},
      {"Poiche", "Because since", "2235", "\\because"},
      {"Dimostra (Tee)", "Proves turnstile", "22A2", "\\vdash"},
      {"Non Dimostra", "Does not yield", "22AC", "\\nvdash"},
      {"Modella (Double Tee)", "Models entails", "22A8", "\\models"},
      {"Top (Verita/Tautologia)", "Top true tautology down tack", "22A4", "\\top"},
      {"Bottom (Falso/Contraddizione)", "Bottom false contradiction up tack", "22A5", "\\bot"},
      {"Negazione (NOT)", "Negation not logic", "00AC", "\\neg"},
      {"AND Logico", "And conjunction wedge", "2227", "\\land"},
      {"OR Logico", "Or disjunction vee", "2228", "\\lor"},

      // --- CALCOLO E GEOMETRIA ---
      {"Infinito", "Infty infinite", "221E", "\\infty"},
      {"Integrale", "Integral", "222B", "\\int"},
      {"Integrale doppio", "Double integral", "222C", "\\iint"},
      {"Sommatoria", "Summation sum", "2211", "\\sum"},
      {"Produttoria", "Product prod", "220F", "\\prod"},
      {"Radice quadrata", "Sqrt radical", "221A", "\\sqrt{}"},
      {"Nabla (Gradiente)", "Nabla gradient del", "2207", "\\nabla"},
      {"Derivata parziale", "Partial differential", "2202", "\\partial"},
      {"Angolo", "Angle geometry", "2220", "\\angle"},
      {"Parallelo", "Parallel", "2225", "\\parallel"},
      {"Perpendicolare", "Perp orthogonal", "27C2", "\\perp"},
      {"Grado", "Degree temperature", "00B0", "^\\circ"},
      {"Circa uguale", "Approx approx", "2248", "\\approx"},
      {"Proporzionale", "Proportional prop", "221D", "\\propto"},
      {"Congruente", "Congruent", "2245", "\\cong"},
      
      // --- FRECCE ---
      {"Freccia destra", "Arrow right", "2192", "\\rightarrow"},
      {"Freccia sinistra", "Arrow left", "2190", "\\leftarrow"},
      {"Freccia alto", "Arrow up", "2191", "\\uparrow"},
      {"Freccia basso", "Arrow down", "2193", "\\downarrow"},
      {"Freccia doppia", "Arrow left-right", "2194", "\\leftrightarrow"},
      {"Freccia lunga dx", "Long arrow right", "27F6", "\\longrightarrow"},
      {"Mappa a", "Maps to", "21A6", "\\mapsto"},

      // --- LETTERE GRECHE (Selezione) ---
      {"Alpha", "Alpha greek", "03B1", "\\alpha"},
      {"Beta", "Beta greek", "03B2", "\\beta"},
      {"Gamma", "Gamma greek", "03B3", "\\gamma"},
      {"Gamma (Maiuscolo)", "Gamma greek upper", "0393", "\\Gamma"},
      {"Delta", "Delta greek", "03B4", "\\delta"},
      {"Delta (Maiuscolo)", "Delta greek upper", "0394", "\\Delta"},
      {"Epsilon", "Epsilon greek", "03B5", "\\epsilon"},
      {"Theta", "Theta greek", "03B8", "\\theta"},
      {"Lambda", "Lambda greek", "03BB", "\\lambda"},
      {"Lambda (Maiuscolo)", "Lambda greek upper", "039B", "\\Lambda"},
      {"Mu (Micro)", "Mu greek micro", "03BC", "\\mu"},
      {"Pi", "Pi greek", "03C0", "\\pi"},
      {"Pi (Maiuscolo)", "Pi greek upper", "03A0", "\\Pi"},
      {"Sigma", "Sigma greek", "03C3", "\\sigma"},
      {"Sigma (Maiuscolo)", "Sigma greek upper", "03A3", "\\Sigma"},
      {"Phi", "Phi greek", "03C6", "\\phi"},
      {"Psi", "Psi greek", "03C8", "\\psi"},
      {"Omega", "Omega greek", "03C9", "\\omega"},
      {"Omega (Maiuscolo)", "Omega greek upper", "03A9", "\\Omega"},

      // --- INFORMATICA E VARI ---
      {"Command (Apple)", "Command key apple", "2318", ""},
      {"Option (Alt)", "Option key alt", "2325", ""},
      {"Shift", "Shift key up", "21E7", ""},
      {"Return (Invio)", "Return enter key", "23CE", ""},
      {"Aleph", "Aleph hebrew cardinal", "2135", "\\aleph"},
      {"Puntini", "Ellipsis dots", "2026", "\\dots"},
      {"Pallino", "Bullet dot point", "2022", "\\bullet"},
      {"Diametro", "Diameter empty set var", "2300", "\\diameter"},
      {"Spunta", "Check mark tick correct", "2713", "\\checkmark"},
      {"L calligrafica di Laplace", "Laplace transform symbol", "2112", "\\mathcal{L}"},

      {"Tau", "Tau", "03c4", "\\tau"},
      {"Tau (Maiuscolo)", "Tau greek upper", "03A4", "\\Tau"}
      
  };

  vector<Symbol> out;
  out.reserve(raw.size());
  for (auto &t : raw) {
    string name = get<0>(t);
    string keywords = get<1>(t);
    string hex = get<2>(t);
    string latex = get<3>(t);
    unsigned int cp = hexToUInt(hex);
    string ch = codepointToUtf8(cp);
    out.push_back({name, keywords, hex, ch, latex});
  }
  return out;
}

#endif