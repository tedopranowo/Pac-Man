#include <fstream>
#include <map>
#include <string>
#include <vector>

#define WIN_32_LEAN_AND_MEAN
#include <Windows.h>

#include "..\OS\Source\OpSys.h"

#define TARGET_FILEPATH "..\\King\\Source\\Generated\\"

namespace LuaCGenerator
{
    struct Argument
    {
        std::string m_type;
        std::string m_name;
    };

    struct FunctionSignature
    {
        bool m_isVirtual;
        bool m_isStatic;
        std::string m_returnType;
        std::string m_name;
        std::vector<Argument> m_arguments;
    };

    struct GeneratedClass
    {
        std::vector<FunctionSignature> m_functions;
        std::string m_name;

        const std::map<std::string, std::string> m_conversionTable
        {
            {"int", "integer"},
            {"long long", "number" },
            {"double", "number"},
            {"std::string", "string"},
            {"const char*", "string"}
        };
    };

    void ReadHeaderTimeStamps(std::map<std::string, time_t>* Map, std::ifstream* inFile);
    void WriteHeaderTimeStamps(std::map<std::string, time_t>* Map);
    void ParseFunctionSignature(std::string signature, FunctionSignature& toFill);
    void ParseArgument(std::string& expression, FunctionSignature& toFill);
    void WriteGeneratedLua(GeneratedClass& toWrite);
    void StripWhiteSpace(std::string& text);
    void RemoveAllCharactersOfType(std::string text, const char remove);
}

int main(int argc, char** argv)
{
    std::ifstream inHeaderTimestamps;

    std::string targetDirectory;

    if (argc < 2)
    {
        targetDirectory = "..\\King\\Source\\*";
    }
    else
    {
        targetDirectory = argv[1];
    }

    std::map<std::string, time_t> timestampMap;

    LuaCGenerator::ReadHeaderTimeStamps(&timestampMap, &inHeaderTimestamps);

    std::string validMacros[]
    {
        "LuaFunction"
    };

    OpSys* pOperatingSystem = OpSys::Create();
    pOperatingSystem->SetSearchFilter(".h");
    pOperatingSystem->ReadDevelopmentResourceDirectory(targetDirectory);

    for (auto fileIt = pOperatingSystem->GetContentMapBegin();
    fileIt != pOperatingSystem->GetContentMapEnd();
        ++fileIt)
    {
        std::ifstream parseFileIn;
        std::string line;
        std::string className = "";
        LuaCGenerator::GeneratedClass genClass;

        if (fileIt->first == "..\\king\\source\\luamanager\\typedeflua.h")
          continue; 

        parseFileIn.open(fileIt->first);

        if (!parseFileIn.is_open())
            continue;

        while (std::getline(parseFileIn, line))
        {
            if (line.find("class") != line.npos && line.find("//") == line.npos &&
                line.find(";") == line.npos && line.find("enum") == line.npos)
            {
                if (genClass.m_name.empty())
                {
                    size_t classIndex = line.find("class");

                    genClass.m_name = line.substr(classIndex + 6,
                        line.find(":") - classIndex - 6);
                    LuaCGenerator::StripWhiteSpace(genClass.m_name);
                }
                else
                {
                    // Log error
                }
            }

            for (auto& macro : validMacros)
            {
                if (line.find(macro) != std::string::npos)
                {
                    // Found a macro
                    std::string expression;

                    // Read until ';'
                    do
                    {
                        std::getline(parseFileIn, line);
                        expression += line;
                    } while (expression.find(';') == -1);
            
                    // Remove all tabs
                    LuaCGenerator::RemoveAllCharactersOfType(expression, '\t');

                    LuaCGenerator::FunctionSignature signature;
                    LuaCGenerator::ParseFunctionSignature(expression, signature);
                    genClass.m_functions.push_back(signature);
                }
            }
        }

        if (genClass.m_name.find(";") == std::string::npos &&
            genClass.m_functions.size() > 0)
        {
            LuaCGenerator::WriteGeneratedLua(genClass);
        }

        parseFileIn.close();
    }

    return 0;
}

void LuaCGenerator::ReadHeaderTimeStamps(std::map<std::string, time_t>* map, std::ifstream* inFile)
{
    std::string headerName;
    std::string timestamp;

    while (!inFile->eof())
    {
        std::string line;
        std::getline(*inFile, line);

        headerName = line.substr(0, line.find(' '));
        timestamp = line.substr(line.find(' ') + 1);
    }

    // TODO: fill out map??
}

void LuaCGenerator::WriteHeaderTimeStamps(std::map<std::string, time_t>* map)
{

}

void LuaCGenerator::ParseFunctionSignature(std::string signature, FunctionSignature& toFill)
{
    size_t findIndex = signature.find("static");
    if (findIndex != signature.npos)
    {
        toFill.m_isStatic = true;
        signature = signature.erase(findIndex, 7);
    }

    findIndex = signature.find("virtual");
    if (findIndex != signature.npos)
    {
        toFill.m_isVirtual = true;
        signature = signature.erase(findIndex, 8);
    }

    // Handle override here
    LuaCGenerator::StripWhiteSpace(signature);

    // Get return type
    findIndex = signature.find(' ');

    // Edge case, no space between return type and function name
    if (findIndex == signature.npos)
        findIndex = signature.find('\t');

    toFill.m_returnType = signature.substr(0, findIndex);

    // Cut from string
    signature = signature.erase(0, findIndex + 1);
    LuaCGenerator::StripWhiteSpace(signature);

    // Get Function Name
    findIndex = signature.find('(');
    toFill.m_name = signature.substr(0, findIndex);

    signature = signature.erase(0, findIndex + 1);
    LuaCGenerator::StripWhiteSpace(signature);

    // Arguments
    findIndex = signature.find(')');
    if (findIndex == 0)
    {
        // This function signature takes no arguments
        return;
    }
    
    // Cut the signature down to just the argument list
    signature = signature.substr(0, findIndex);

    findIndex = signature.find(',');
    if (findIndex != signature.npos)
    {
        // More than 1 argument
        do
        {
            std::string argument = signature.substr(0, findIndex);
            LuaCGenerator::ParseArgument(argument, toFill);
            signature = signature.erase(0, findIndex + 1);
            LuaCGenerator::StripWhiteSpace(signature);
            findIndex = signature.find(',');
        } while (findIndex != signature.npos);
    }

    // Parse last argument
    LuaCGenerator::ParseArgument(signature, toFill);
}

void LuaCGenerator::ParseArgument(std::string& expression, FunctionSignature& toFill)
{
    size_t findIndex = expression.find_last_of(' ');
    Argument argument;
    argument.m_name = expression.substr(findIndex + 1);
    argument.m_type = expression.substr(0, findIndex);

    toFill.m_arguments.push_back(argument);
}

void LuaCGenerator::WriteGeneratedLua(GeneratedClass& toWrite)
{
    std::ofstream outFile(TARGET_FILEPATH + toWrite.m_name + ".gen.h");

    if (!outFile.is_open())
    {
        // Log Error
        return;
    }

    using std::endl;

    /*
    *** Header File ***
    */
    outFile << "#pragma once" << endl << endl;
    outFile << "#include <lua.hpp>" << endl << endl;
    outFile << "namespace " << toWrite.m_name << "Lua" << endl << "{" << endl;
    for (auto& function : toWrite.m_functions)
    {
        outFile << "\tint " << function.m_name
            << "(lua_State* pState);" << endl;
    }
    outFile << "};";

    outFile.close();

    /*
    *** Source File ***
    */
    outFile.open(TARGET_FILEPATH + toWrite.m_name + ".gen.cpp");

    if (!outFile.is_open())
    {
        // Log Error
        return;
    }

    // [TODO] add includes base off of file??
    outFile << "#include \"" << toWrite.m_name << ".gen.h\"" << endl;
    outFile << "#include \"../Components/" << toWrite.m_name << ".h\"\n";

    for (auto& function : toWrite.m_functions)
    {
        // Function Signature
        outFile << "int " << toWrite.m_name << "Lua::" << function.m_name
            << "(lua_State* pState)" << endl;
        outFile << "{" << endl;

        int argCount = (int)function.m_arguments.size();

        // Function Body
        for (int argumentIndex = 0; argumentIndex < argCount; ++argumentIndex)
        {
            Argument* pArgument = &function.m_arguments[argumentIndex];
            outFile << '\t' << pArgument->m_type << " " << pArgument->m_name
                << " = static_cast<";
            outFile << pArgument->m_type << ">(lua_to";

            auto convIt = toWrite.m_conversionTable.find(pArgument->m_type);

            if (convIt == toWrite.m_conversionTable.end())
            {
                // Log error
                outFile.close();
                return;
            }

            outFile << convIt->second.c_str() << "(pState, ";
            outFile << -argCount + argumentIndex << "));";
            outFile << endl;
        }

        // Get 'this'
        outFile << '\t' << toWrite.m_name << "* p" << toWrite.m_name;
        outFile << " = static_cast<" << toWrite.m_name
            << "*>(lua_touserdata(pState, -" << argCount + 1;
        outFile << "));" << endl << endl;
        outFile << "\tlua_pop(pState, " << argCount + 1 << ");" << endl << endl;

        // Calling the member functions
        if (function.m_returnType != "void")
            outFile << "\t" << function.m_returnType << " RetVal = ";
        else
            outFile << "\t";

        outFile << "p" << toWrite.m_name << "->" << function.m_name << "(";

        for (int argumentIndex{ 0 }; argumentIndex < argCount; ++argumentIndex)
        {
            Argument* pArgument = &function.m_arguments[argumentIndex];

            outFile << pArgument->m_name;

            if (argumentIndex != argCount - 1)
                outFile << ", ";
        }

        outFile << ");" << endl << endl;

        // Dealing with return values
        if (function.m_returnType == "void")
            outFile << "\treturn 0;" << endl;
        else
        {
            outFile << "\tlua_push";
            
            auto convIt = toWrite.m_conversionTable.find(function.m_returnType);

            if (convIt == toWrite.m_conversionTable.end())
                outFile << "lightuserdata";
            else
                outFile << convIt->second.c_str();

            outFile << "(pState, RetVal);\n";
            outFile << "\treturn 1;\n";
        }

        outFile << "}" << endl << endl;
    }

    // Expose to lua
    outFile << "void " << toWrite.m_name << "::ExposeToLua(lua_State* pState)" << endl;
    outFile << "{" << endl;
    outFile << "\tlua_createtable(pState, 0, " << toWrite.m_functions.size() + 1 << ");\n\n";
    outFile << "\tlua_pushlightuserdata(pState, this);\n";
    outFile << "\tlua_setfield(pState, -2, \"this\");\n\n";

    for (auto& function : toWrite.m_functions)
    {
        outFile << "\tlua_pushcfunction(pState, " << toWrite.m_name << "Lua::"
            << function.m_name << ");\n";
        outFile << "\tlua_setfield(pState, -2, \"" << function.m_name << "\");\n\n";
    }

    outFile << "\tlua_setfield(pState, -2, \"" << toWrite.m_name << "\");\n";
    outFile << "}\n";

    outFile.close();
}



void LuaCGenerator::StripWhiteSpace(std::string& text)
{
    auto stringIt = text.begin();

    while (*stringIt == ' ' || *stringIt == '\t')
    {
        stringIt = text.erase(stringIt);
    }

    stringIt = text.end() - 1;

    while (*stringIt == ' ' || *stringIt == '\t')
    {
        stringIt = text.erase(stringIt);
        --stringIt;
    }

}

void LuaCGenerator::RemoveAllCharactersOfType(std::string text, const char remove)
{
    for (auto charIt = text.begin(); charIt != text.end(); ++charIt)
    {
        if (*charIt == remove)
            charIt = text.erase(charIt);
    }
}