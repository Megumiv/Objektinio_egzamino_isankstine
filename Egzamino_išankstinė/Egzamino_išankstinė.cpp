#include "Header.h"
/* header.h:
 #pragma once
#include <iostream>
#include <fstream>
#include <sstream>
#include <map>
#include <set>
#include <regex>
#include <algorithm>
#include <locale>
#include <string>

using std::string;
using std::wcout;
using std::endl;
using std::cerr;
using std::locale;
using std::ofstream;
using std::ifstream;

*/


// Pagalbinė funkcija žodžiams valyti nuo skyrybos ženklų ir skaičių
string normalizeWord(const string& word) {
    string clean;
    for (char c : word) {
        if (std::isalpha(static_cast<unsigned char>(c))) {
            clean += std::tolower(static_cast<unsigned char>(c));
        }
    }
    return clean;
}


int main() {

    // Nustatome lokalę į UTF-8
    try {
        locale loc("lt-LT.UTF-8");
        locale::global(loc);
        wcout.imbue(loc);
    }
    catch (const std::runtime_error&) {
        wcout << L"Lokacijos nustatyti nepavyko, naudojama numatytoji.\n";
    }



    ifstream inFile("tekstas.txt");
    ofstream wordCountOut("zodziu_pasikartojimai.txt");
    ofstream vietaOut("zodziu_vietos");
    ofstream urlOut("url_adresai.txt");

    if (!inFile) {
        cerr << "Nepavyko atidaryti failo tekstas.txt\n";
        return 1;
    }
    /*
    if (inFile.peek() == ifstream::traits_type::eof()) {
        std::wcout << L"Pateiktas failas yra tuščias.\n";
        return 0;
    }
    */


    std::map<string, int> wordCount;
    std::map<string, std::set<int>> wordLines;
    std::set<string> urlSet;

    string line;
    int lineNumber = 0;

    
    // Bendresnis URL radimas (uŽskaitytų ir ne tik URL, pvz. T.XVII, nors ir yra galimas toks URL variantas)
    std::regex URL(R"(\b((https?:\/\/|www\.)?[\w\-.]+\.[a-z]{2,}([\/?#][^\s]*)?)\b)", std::regex::icase);
    // URL radimas - neužskaito bet kokių "raidės"+"."+"raidės", tačiau limituoja, nes nenuskaitytų pvz.: "pvz.lv"
    // std::regex URL(R"(\b((https?:\/\/|www\.)?[\w\-.]+\.(lt|gov|com|org|net|edu)(\/[^\s]*)?)\b)", std::regex::icase);



    while (std::getline(inFile, line)) {
        ++lineNumber;
        std::istringstream iss(line);
        string word;

        // Ieškom URL'
        auto words_begin = std::sregex_iterator(line.begin(), line.end(), URL);
        auto words_end = std::sregex_iterator();
        for (std::sregex_iterator i = words_begin; i != words_end; ++i) {
            urlSet.insert(i->str());
        }

        // Valomi nereikalingi skyrybos ženklai
        while (iss >> word) {
            string normalized = normalizeWord(word);
            if (!normalized.empty()) {
                wordCount[normalized]++;
                wordLines[normalized].insert(lineNumber);
            }
        }
    }

    // Rašomas žodžių pasikartojimų sąrašas
    wordCountOut <<"Žodžių, pasikartojusių dažniau nei 1 kartą, sąrašas:\n" << string(21, '-') << "|" << string(35, '-') << endl;
    //if (word.empty()) wordCountOut << "(nieko nerasta.)";
    for (const auto& [word, count] : wordCount) {
        if (count > 1) {
            wordCountOut << std::setw(20) << std::left << word << " : " << count << " kartus\n";
        }
    }
    if (!wordCountOut) {
        cerr << "Nepavyko sukurti failo zodziu_pasikartojimai.txt\n";
    }

    // Rašoma cross-reference lentelė su nurodytomis žodžių vietomis tekste
    vietaOut << "Žodžių, pasikartojusių dažniau nei 1 kartą, vietų sąrašas (kuriose jis teksto eilutėse):\n" << string(19, '-') <<"|" << string(19, '-') << "|" << string(50, '-') << endl;
    //if (wordCount.empty()) vietaOut << "(nieko nerasta.)";
    for (const auto& [word, lines] : wordLines) {
        if (wordCount[word] > 1) {
            vietaOut << std::setw(20) << std::left << word << " pasirodė eilutėse:  ";
            for (int lineNum : lines) {
                vietaOut << lineNum << " ";
            }
            vietaOut << "\n";
        }
    }
    if (!vietaOut) {
        cerr << "Nepavyko sukurti failo zodziu_vietos.txt\n";
    }

    // Rašomi URL'ai
    urlOut << "Rastų URL sąrašas:\n" << string(80, '-') << endl;
    if (urlSet.empty()) urlOut << "(nieko nerasta.)";
    else {
        for (const auto& url : urlSet) { urlOut << url << "\n";}
    }
    if (!urlOut) {
        cerr << "Nepavyko sukurti failo url_adresai.txt\n";
    }

    wcout << L"Analizė baigta. \nRezultatai išsaugoti failuose: zodziu_pasikartojimai.txt, zodziu_vietos, url_adresai.txt.\n";
    
    return 0;
}
