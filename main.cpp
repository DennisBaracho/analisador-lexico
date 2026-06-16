#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <stdexcept>

namespace Constantes {
    enum class Token { NUM, ABCOL, ABPAR, ABCHA, FECOL, FEPAR, FECHA, OP, VAR, END_OF_FILE };

    std::string tokenToString(Token t) {
        switch(t) {
            case Token::NUM: return "NUM";
            case Token::ABCOL: return "ABCOL";
            case Token::ABPAR: return "ABPAR";
            case Token::ABCHA: return "ABCHA";
            case Token::FECOL: return "FECOL";
            case Token::FEPAR: return "FEPAR";
            case Token::FECHA: return "FECHA";
            case Token::VAR: return "VAR";
            case Token::OP: return "OP";
            case Token::END_OF_FILE: return "EOF";
            default: return "UNKNOWN";
        }
    }

    const std::string DIGITOS = "0123456789";
    const std::string LETRAS_MIN = "abcdefghijklmnopqrstuvwxyz";
    const std::string LETRAS_MAI = "ABCDEFGHIJKLMNOPQRSTUVWXYZ";
    const std::string OPERADORES = "+-*/";
    const std::string VAZIOS = " \r\n\t";

    const char EOF_CHAR = 0;
    const char ABPAR = '(';
    const char FEPAR = ')';
    const char ABCHA = '{';
    const char FECHA = '}';
    const char ABCOL = '[';
    const char FECOL = ']';
    const char UNDER = '_';

    const std::string NOME_DEFAULT_ARQUIVO_ENTRADA = "entrada.txt";
}

class ErroLexico : public std::runtime_error {
private:
    std::string msg;
public:
    ErroLexico(char caractereEncontrado, const std::string& caracteresEsperados)
        : std::runtime_error("Erro lexico") {
        msg = "caractere encontrado: " + std::string(1, caractereEncontrado) +
              "\nera(m) esperado(s): " + caracteresEsperados;
    }
    
    const char* what() const noexcept override {
        return msg.c_str();
    }
};

class Analisador {
protected:
    std::string nomeArquivoEntrada;
public:
    Analisador(std::string _nomeArquivoEntrada) : nomeArquivoEntrada(_nomeArquivoEntrada) {} 
    Analisador() : nomeArquivoEntrada(Constantes::NOME_DEFAULT_ARQUIVO_ENTRADA) {}
    std::string getNomeArquivoEntrada() const {
        return nomeArquivoEntrada;
    }
};

class AnalisadorLexico : public Analisador {
protected:
    char proxCaractere;
    std::string entrada;
    size_t posicao = 0;
public:
    Constantes::Token tokenReconhecido;

    AnalisadorLexico(std::string _nomeArquivoEntrada) : Analisador(_nomeArquivoEntrada) {
        std::ifstream file(nomeArquivoEntrada);
        if (!file.is_open()) {
            throw std::runtime_error("Erro de leitura no arquivo " + nomeArquivoEntrada);
        }
        
        std::stringstream buffer;
        buffer << file.rdbuf();
        entrada = buffer.str();
        file.close();
        
        leProxCaractere();
    }

    void leProxCaractere() {
        if (posicao < entrada.length()) {
            proxCaractere = entrada[posicao++];
        } else {
            proxCaractere = Constantes::EOF_CHAR;
        }
    }

    bool proxCaractereIs(const std::string& s) {
        return s.find(proxCaractere) != std::string::npos;
    }
};

class MyAnalisadorLexico : public AnalisadorLexico {
public:
    MyAnalisadorLexico(std::string _nomeArquivoEntrada) : AnalisadorLexico(_nomeArquivoEntrada) {}
    void s0() {
        
        if (proxCaractereIs(Constantes::DIGITOS)) {
            leProxCaractere();
            s1();  
        } 

        else if (proxCaractere == Constantes::ABCOL) {
            leProxCaractere();
            s2();
        }

        else if (proxCaractere == Constantes::ABPAR) {
            leProxCaractere();
            s3();   
        }

        else if (proxCaractere == Constantes::ABCHA) {
            leProxCaractere();
            s4();
        }

        else if (proxCaractere == Constantes::FECOL) {
            leProxCaractere();
            s5();
        }

        else if (proxCaractere == Constantes::FEPAR) {
            leProxCaractere();
            s6();
        }

        else if (proxCaractere == Constantes::FECHA) {
            leProxCaractere();
            s7();
        }

        else if (proxCaractereIs(Constantes::OPERADORES)) {
            leProxCaractere();
            s8();
        }

        else if (proxCaractere == Constantes::EOF_CHAR) {
            leProxCaractere();
            s9();
        }
        
        else if (proxCaractereIs(Constantes::LETRAS_MIN + Constantes::LETRAS_MAI)) {
            leProxCaractere();
            s10();
        }

        else if (proxCaractereIs(Constantes::VAZIOS)) {
            leProxCaractere();
            s0();

        } else {
            throw ErroLexico(proxCaractere, Constantes::DIGITOS + Constantes::ABCOL + Constantes::ABPAR + Constantes::ABCHA + Constantes::FECOL + Constantes::FEPAR + Constantes::FECHA + Constantes::OPERADORES + Constantes::EOF_CHAR + Constantes::LETRAS_MIN + Constantes::LETRAS_MAI + Constantes::VAZIOS);
        }
    }

    void s1() {
        tokenReconhecido = Constantes::Token::NUM;

        if (proxCaractereIs(Constantes::DIGITOS)) {
            leProxCaractere();
            s1();
        }
    }

    void s2() {
        tokenReconhecido = Constantes::Token::ABCOL;
    }

    void s3() {
        tokenReconhecido = Constantes::Token::ABPAR;
    }

    void s4() {
        tokenReconhecido = Constantes::Token::ABCHA;
    }

    void s5() {
        tokenReconhecido = Constantes::Token::FECOL;
    }

    void s6() {
        tokenReconhecido = Constantes::Token::FEPAR;
    }
    
    void s7() {
        tokenReconhecido = Constantes::Token::FECHA;
    }
    
    void s8() {
        tokenReconhecido = Constantes::Token::OP;

        if (proxCaractereIs(Constantes::OPERADORES)) {
            leProxCaractere();
            s8();
        }
    }
    void s9() {
        tokenReconhecido = Constantes::Token::END_OF_FILE;
    }
    
    void s10() {
        tokenReconhecido = Constantes::Token::VAR;
        std::string caracteresAceitos = Constantes::LETRAS_MIN + Constantes::LETRAS_MAI + Constantes::DIGITOS + Constantes::UNDER;

        if (proxCaractereIs(caracteresAceitos)) {
            leProxCaractere();
            s10();
        }
    }
};

int main(int argc, char* argv[]) {
    if (argc != 2) {
        std::cerr << "Erro: esqueceu de escrever o nome do arquivo de entrada!" << std::endl;
        std::cerr << "Uso: ./analisador_lexico <arquivo_de_entrada>" << std::endl;
        return 1;
    }

    try {
        MyAnalisadorLexico scanner(argv[1]);
        
        do {
            scanner.s0();
            std::cout << tokenToString(scanner.tokenReconhecido) << std::endl;
        } while (scanner.tokenReconhecido != Constantes::Token::END_OF_FILE);

        std::cout << "Analise lexica realizada com sucesso no arquivo " << scanner.getNomeArquivoEntrada() << std::endl;

    } catch (const ErroLexico& e) {
        std::cout << "Erro lexico: " << e.what() << std::endl;
    } catch (const std::exception& e) {
        std::cout << "Erro: " << e.what() << std::endl;
    }

    return 0;
}