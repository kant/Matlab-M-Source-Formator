//#include "stdafx.h"
#include <string>
#include <iostream>
#include <fstream>
#include <regex>
using std::string;
using std::ifstream;
using std::ofstream;
using std::regex;
using std::regex_replace;

void ShowHelp() {
	std::cerr << "------------------------------------------------------" << std::endl;
	std::cerr << "The commandline tool \"Matlab formator\" is used to format matlab source file." << std::endl;
	std::cerr << "The syntax is MFormator[.exe] input [output]" << std::endl;
	std::cerr << "Or in an alternative way: " << std::endl;
	std::cerr << "type input | MFormator [output]" << std::endl;
	std::cerr << "type input | MFormator >output" << std::endl;
	std::cerr << "Explanatation:" << std::endl;
	std::cerr << "\tinput\t the [path/]filename of a specific/existing file as input" << std::endl;
	std::cerr << "\toutput\t the [path/]filename for outputing, if no outptu filename is given, the content are redirect to the console" << std::endl;
	std::cerr << "------------------------------------------------------" << std::endl;
	std::cerr << "Now, you can input your contents below:" << std::endl;
}

class MATLABFormatEngine {
private:
	long _nIndentLevel;
	bool _bLineContinue;
public:
	MATLABFormatEngine() :_nIndentLevel(0), _bLineContinue(false) {}
	~MATLABFormatEngine() {}
	string ProcessLine(const string& line) {
		string content = line;
		bool bIndent;
		//std::regex _operator("([-+*/%:=]+)");
		content = std::regex_replace(content, std::regex("^[ \t]+"), "");
		if (!std::regex_search(content, std::regex("^%"))) {
			content = std::regex_replace(content, std::regex("([*/%:=<>]+)"), " $1 "); 
			content = std::regex_replace(content, std::regex("[ \t]*,"), ", ");
			content = std::regex_replace(content, std::regex(" {2,}"), " "); // cut the spare spaces

			// Special treatment should be consider for +-(Plus/Minus) operatiors
			// However, I don't have any idea about what to do with +-
			//content = std::regex_replace(content, std::regex("(\d)\s*(e)\s*[-+]?(\d))"), "$1$2$3");
			//content = std::regex_replace(content, std::regex("(\d)\s*(e)\s*[-+]?(\d)"), "$1$2$3");

			// Add the indent
			if (std::regex_search(content, std::regex("^end"))) {
				_nIndentLevel = _nIndentLevel > 0 ? _nIndentLevel - 1 : 0;
			}
			bool bIndent = std::regex_search(content, std::regex("^(if|for|while|switch|do|function)"));

			if(std::regex_search(content, std::regex("^(else|case)")))
				content = std::string(_nIndentLevel > 0? _nIndentLevel - 1 : 0, '\t') + content;
			else
				content = std::string(_nIndentLevel + static_cast<long>(_bLineContinue), '\t') + content;

			_bLineContinue = std::regex_search(content, std::regex("\\.{3}$"));
			if(bIndent) _nIndentLevel++;
		} else {
			content = std::string(_nIndentLevel, '\t') + content;
		}
		return content;
	}
};

int main(int argc, char * argv[])
{
	using std::cout;
	using std::endl;
#ifdef _DEBUG
	if (argc < 2) ShowHelp();
	for (int i = 0; i < argc; ++i) {
		std::cerr << "[" << i << "]\t" << argv[i] << std::endl;
	}
#endif
	// ------------------------------------
	// Input stream
	// ------------------------------------
	// The following works in visual studio 2019, however, it doesn't work for MinGW g++
	// So, I rewrite it in another form
	// std::istream & fin = argc < 2 ? std::cin : (std::istream&) ifstream(argv[1]);
	std::ifstream _fin;
	if(argc >= 2) _fin.open(argv[1]);
	std::istream & fin = argc < 2 ? std::cin : _fin;
	if (fin.bad()) {
		std::cerr << "Cannot access the file for reading:\n" << argv[1] << std::endl;
		ShowHelp();
		exit(-1);
	}

	// ------------------------------------
	// Input stream
	// ------------------------------------
	// std::ostream & fout =argc < 3? std::cout : (std::ostream&)ofstream(argv[2]);
	std::ofstream _fout;
	if(argc >= 3) _fout.open(argv[2]);
	std::ostream & fout =argc < 3? std::cout : _fout;
	if (fout.bad()) {
		std::cerr << "Cannot access the file for writing:\n" << argv[1] << std::endl;
		ShowHelp();
		exit(-1);
	}
	MATLABFormatEngine engine;
	string line;
	int nLineCount = 0;
	while (!fin.eof()) {
		std::getline(fin, line);
		fout << engine.ProcessLine(line) << std::endl;
	}
	fclose(stdout);
#ifdef _DEBUG
	//system("pause");
#endif
	return 0;
}

