#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <cstring>
#include <cstdlib>
#include <unistd.h>
#include <map>
#include <vector>
#include <algorithm>
#include <stdbool.h>

#include "Options.h"
#include "Configuration.h"
#include "RbClient.h"
#include "RevLanguageMain.h"
#include "RbOptions.h"
#include "Parser.h"
#include "Workspace.h"
#include "FunctionTable.h"
#include "RlFunction.h"
#include "WorkspaceUtils.h"
#include "CommandLineUtils.h"
#include "libs/filesystem.h"

#include <boost/filesystem.hpp>
#include "boost/filesystem/operations.hpp"
#include "boost/filesystem/path.hpp"

namespace fs = boost::filesystem;

const std::string inifile = expandUserDir("~") + directorySeparator() + "revbayes.ini";



int main(int argc, const char* argv[]) {
    
    // parse command line arguments
    Options options;
    options.parseOptions(argc, argv);    
    std::cout << options.getMessage() << std::endl;
    if(options.isHelp()){
        exit(0);
    }
    
    // read / create settings file
    Configuration configuration(inifile);
    configuration.parseInifile();
    std::cout << configuration.getMessage() << std::endl;
    

    /* initialize environment */
    RevLanguageMain rl;
    // pass input files to Rev
    rl.startRevLanguageEnvironment(options.getInputFiles());
    // exit after processing the input files if not interactive
    if (!options.isInteractive()) {
        exit(0);
    }   
    
    if (!options.isDisableReadline()) {        
        RbClient c;
        c.startInterpretor();
        return 0;
    }


    /* Start the very basic RB interpreter if disable-readline has been set */
    std::cout << "Starting RevBayes with basic interpretor" << std::endl;
    char *default_prompt = (char *) "RevBayes > ";
    char *incomplete_prompt = (char *) "RevBayes + ";
    char *prompt = default_prompt;
    int result = 0;
    std::string commandLine;
    std::string line;

    while(true) {

        std::cout << prompt;
        std::istream& retStream = getline(std::cin, line);

        if (!retStream)
            exit(0);

        if (result == 0 || result == 2) {
            prompt = default_prompt;
            commandLine = line;
        } else if (result == 1) {
            prompt = incomplete_prompt;
            commandLine += line;
        }

        result = RevLanguage::Parser::getParser().processCommand(commandLine, &RevLanguage::Workspace::userWorkspace());
    }

    return 0;

}


