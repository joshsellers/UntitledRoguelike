#include "Bygro.h"
#include "Tokenizer.h"
#include <map>
#include "Expression.h"
#include "Parser.h"
#include <fstream>
#include <functional>
#include "Compiler.h"
#include <filesystem>
#include <iostream>
#include "../../core/Util.h"
#include "../../core/MessageManager.h"

const std::string BYGRO_VERSION = "1.16";

void Bygro::loadAndCompile(std::string filePath, bool debug) {
    MessageManager::displayMessage("Bygro version " + BYGRO_VERSION, 5, DEBUG);

    std::ifstream in(filePath);
    std::string rawProgram = "";
    if (in.good()) {
        std::string line;
        while (getline(in, line)) {
            bool foundNonComment = false;
            bool shouldSkipLine = false;
            for (auto& character : line) {
                if (character == ' ') continue;
                else if (character == '#' && !foundNonComment) {
                    shouldSkipLine = true;
                    break;
                } else {
                    foundNonComment = true;
                    break;
                }
            }
            if (shouldSkipLine) continue;
            rtrim(line);
            rawProgram += line + " ";
        }
    } else {
        MessageManager::displayMessage("Could not open " + filePath, 5, ERR);
        return;
    }

    rtrim(rawProgram);

    std::vector<std::string> tokens = Tokenizer::tokenize(rawProgram);

    std::vector<std::vector<std::string>> statements;
    std::vector<std::string> currentStatement;
    for (std::string token : tokens) {
        if (token != ";") currentStatement.push_back(token);
        else {
            statements.push_back(currentStatement);
            currentStatement.clear();
        }
    }

    Parser parser;
    std::vector<EXPR> expressions = parser.parse(tokens);

    std::string assembly = "";
    for (auto& expression : expressions) {
        assembly += expression->evaluate() + ":";
    }
    replaceAll(assembly, "::", ":");
    if (stringEndsWith(assembly, ":")) {
        assembly.pop_back();
    }

    if (debug) {
        MessageManager::displayMessage("Assembly:", 5, DEBUG);
        MessageManager::displayMessage(assembly, 5, DEBUG);
    }

    std::string output = Compiler::compile(assembly);

    std::string outputPath = splitString(filePath, ".")[0] + ".rf";
    try {
        if (!std::filesystem::remove(outputPath)) {
            MessageManager::displayMessage("Could not replace output file", 0, DEBUG);
        }
    } catch (const std::filesystem::filesystem_error& err) {
        MessageManager::displayMessage("Could not replace output file: " + (std::string)err.what(), 5, ERR);
    }

    try {
        std::ofstream out(outputPath, std::ios::binary);
        out << output;
        out.close();
    } catch (std::exception ex) {
        MessageManager::displayMessage("Error writing to output file: " + (std::string)ex.what(), 5, ERR);
    }

    MessageManager::displayMessage("Build successful", 5, DEBUG);
}
