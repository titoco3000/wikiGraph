/*
Funções relacionadas a busca e tratamento na internet
*/
#ifndef __HTML_HPP__
#define __HTML_HPP__

#include <iostream>
#include <string>
#include <regex>
#include <algorithm>
/*
Para usar curl, foi necessario:
    > sudo apt-get install libcurl4-nss-dev
*/
#include <curl/curl.h>

/*
Essa tabela e constante foram gerados pelo script "htmlEscape.py"
*/
#define tamanhoTabelaSubs 95
const std::string tabelaSubs[tamanhoTabelaSubs][2] = {
    {"%C2%A1", "¡"},
    {"%C2%A2", "¢"},
    {"%C2%A3", "£"},
    {"%C2%A4", "¤"},
    {"%C2%A5", "¥"},
    {"%C2%A6", "¦"},
    {"%C2%A7", "§"},
    {"%C2%A8", "¨"},
    {"%C2%A9", "©"},
    {"%C2%AA", "ª"},
    {"%C2%AB", "«"},
    {"%C2%AC", "¬"},
    {"%C2%AD", "­"},
    {"%C2%AE", "®"},
    {"%C2%AF", "¯"},
    {"%C2%B0", "°"},
    {"%C2%B1", "±"},
    {"%C2%B2", "²"},
    {"%C2%B3", "³"},
    {"%C2%B4", "´"},
    {"%C2%B5", "µ"},
    {"%C2%B6", "¶"},
    {"%C2%B7", "·"},
    {"%C2%B8", "¸"},
    {"%C2%B9", "¹"},
    {"%C2%BA", "º"},
    {"%C2%BB", "»"},
    {"%C2%BC", "¼"},
    {"%C2%BD", "½"},
    {"%C2%BE", "¾"},
    {"%C2%BF", "¿"},
    {"%C3%80", "À"},
    {"%C3%81", "Á"},
    {"%C3%82", "Â"},
    {"%C3%83", "Ã"},
    {"%C3%84", "Ä"},
    {"%C3%85", "Å"},
    {"%C3%86", "Æ"},
    {"%C3%87", "Ç"},
    {"%C3%88", "È"},
    {"%C3%89", "É"},
    {"%C3%8A", "Ê"},
    {"%C3%8B", "Ë"},
    {"%C3%8C", "Ì"},
    {"%C3%8D", "Í"},
    {"%C3%8E", "Î"},
    {"%C3%8F", "Ï"},
    {"%C3%90", "Ð"},
    {"%C3%91", "Ñ"},
    {"%C3%92", "Ò"},
    {"%C3%93", "Ó"},
    {"%C3%94", "Ô"},
    {"%C3%95", "Õ"},
    {"%C3%96", "Ö"},
    {"%C3%97", "×"},
    {"%C3%98", "Ø"},
    {"%C3%99", "Ù"},
    {"%C3%9A", "Ú"},
    {"%C3%9B", "Û"},
    {"%C3%9C", "Ü"},
    {"%C3%9D", "Ý"},
    {"%C3%9E", "Þ"},
    {"%C3%9F", "ß"},
    {"%C3%A0", "à"},
    {"%C3%A1", "á"},
    {"%C3%A2", "â"},
    {"%C3%A3", "ã"},
    {"%C3%A4", "ä"},
    {"%C3%A5", "å"},
    {"%C3%A6", "æ"},
    {"%C3%A7", "ç"},
    {"%C3%A8", "è"},
    {"%C3%A9", "é"},
    {"%C3%AA", "ê"},
    {"%C3%AB", "ë"},
    {"%C3%AC", "ì"},
    {"%C3%AD", "í"},
    {"%C3%AE", "î"},
    {"%C3%AF", "ï"},
    {"%C3%B0", "ð"},
    {"%C3%B1", "ñ"},
    {"%C3%B2", "ò"},
    {"%C3%B3", "ó"},
    {"%C3%B4", "ô"},
    {"%C3%B5", "õ"},
    {"%C3%B6", "ö"},
    {"%C3%B7", "÷"},
    {"%C3%B8", "ø"},
    {"%C3%B9", "ù"},
    {"%C3%BA", "ú"},
    {"%C3%BB", "û"},
    {"%C3%BC", "ü"},
    {"%C3%BD", "ý"},
    {"%C3%BE", "þ"},
    {"%C3%BF", "ÿ"}};

/*
Usa a tabela para fazer as substituições de caracteres especias, para a string ficar legivel
*/
std::string unEscapeHTML(std::string original)
{
    for (int i = 0; i < tamanhoTabelaSubs; i++)
        original = std::regex_replace(original, std::regex(tabelaSubs[i][0]), tabelaSubs[i][1]);

    return original;
}

/*
Substitui os caracteres especiais e underscore
*/
std::string wikipediaEscape(std::string original)
{
    original = unEscapeHTML(original);
    std::replace(original.begin(), original.end(), '_', ' ');
    return original;
}

/*
Callback para CURL
*/
static size_t WriteCallback(void *contents, size_t size, size_t nmemb, void *userp)
{
    ((std::string *)userp)->append((char *)contents, size * nmemb);
    return size * nmemb;
}

/*
Busca o site e retorna ele como uma string
*/
std::string obterSite(std::string addr)
{
    CURL *curl;
    CURLcode res;
    std::string readBuffer{""};

    curl = curl_easy_init();
    if (curl)
    {
        curl_easy_setopt(curl, CURLOPT_URL, addr.c_str());
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &readBuffer);
        res = curl_easy_perform(curl);
        curl_easy_cleanup(curl);
    }
    return readBuffer;
}

#endif