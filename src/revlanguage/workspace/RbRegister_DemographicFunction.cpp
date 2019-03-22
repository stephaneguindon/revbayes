/**
 * @file
 * This file contains the Workspace function that adds types and functions
 * to the global workspace, registering them with the interpreter/compiler
 * during the process.
 *
 * @brief Function registering language objects
 *
 * Instructions
 *
 * This is the central registry of Rev objects. It is a large file and needs
 * to be properly organized to facilitate maintenance. Follow these simple
 * guidelines to ensure that your additions follow the existing structure.
 *
 * 1. All headers are added in groups corresponding to directories in the
 *    revlanguage code base.
 * 2. All objects (types, distributions, and functions) are registered in
 *    groups corresponding to directories in the revlanguage code base.
 * 3. All entries in each group are listed in alphabetical order.
 *
 * Some explanation of the directory structure is provided in the comments
 * in this file. Consult these comments if you are uncertain about where
 * to add your objects in the code.
 */

#include <sstream>
#include <vector>
#include <set>
#include <cstdlib>

/* Files including helper classes */
#include "AddWorkspaceVectorType.h"
#include "AddVectorizedWorkspaceType.h"
#include "RbException.h"
#include "RlUserInterface.h"
#include "Workspace.h"


/// Demographic Functions ///

/* Demographic functions types (in folder "demography) */
#include "RlDemographicFunction.h"
#include "RlConstantDemographicFunction.h"
#include "RlExponentialDemographicFunction.h"

/** Initialize global workspace */
void RevLanguage::Workspace::initializeDemographicFunctionGlobalWorkspace(void)
{
    
    try
    {
        ////////////////////////////////////////////////////////////////////////////////
        /* Add demographic function (in folder "datatypes/phylogenetic/demography") (alphabetic order) */
        ////////////////////////////////////////////////////////////////////////////////
        
        addTypeWithConstructor( new ConstantDemographicFunction() );
        addTypeWithConstructor( new ExponentialDemographicFunction() );



        addFunction( new Func_workspaceVector<DemographicFunction>() );
        
    }
    catch(RbException& rbException)
    {
        
        RBOUT("Caught an exception while initializing demographic functions in the workspace\n");
        std::ostringstream msg;
        rbException.print(msg);
        msg << std::endl;
        RBOUT(msg.str());
        
        RBOUT("Please report this bug to the RevBayes Development Core Team");
        
        RBOUT("Press any character to exit the program.");
        getchar();
        exit(1);
    }
}


