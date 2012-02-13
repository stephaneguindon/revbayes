/**
 * @file
 * This file contains the implementation of Argument, which is
 * used to hold a potentially labeled argument passed to a
 * function.
 *
 * @brief Implementation of Argument
 *
 * (c) Copyright 2009-
 * @date Last modified: $Date$
 * @author The RevBayes Development Core Team
 * @license GPL version 3
 * @version 1.0
 * @since 2009-11-20, version 1.0
 *
 * $Id$
 */

#include <sstream>

#include "Argument.h"
#include "DAGNode.h"
#include "RbUtil.h"
#include "VectorString.h"


// Definition of the static type spec member
const TypeSpec Argument::typeSpec(Argument_name);

/** Construct from argument label and DAG node */
Argument::Argument(Variable* v) : RbInternal() {
    
    label   = "";
    var     = v;
}


/** Construct from argument label and DAG node */
Argument::Argument(const std::string& argLabel, Variable* v) : RbInternal() {

    label   = argLabel;
    var     = v;
}

/** Copy Constructor. We keep the same pointer to the variable stored inside this argument. */
Argument::Argument(const Argument &x) : RbInternal(x) {
    
    label   = x.label;
    if (x.var != NULL)
        var     = x.var->clone();
}


/** Destructor */
Argument::~Argument() {
    delete var;
}


Argument& Argument::operator=(const Argument &x) {
    
    if ( &x != this ) {
        
        if (var != NULL) {
            delete var;
        }
        // Copy the new variable
        if (x.var == NULL) {
            var = NULL;
        }
        else {
            var     = x.var->clone();
        }
        label   = x.label;
    }
    
    return (*this);
}


/** Get class vector describing type of object */
const VectorString& Argument::getClass(void) const { 

    static VectorString rbClass = VectorString(Argument_name) + RbInternal::getClass();
	return rbClass; 
}


RbDagNodePtr Argument::getDagNode(void) const {
    return var->getDagNode();
}


RbDagNodePtr Argument::getDagNode(void) {
    return var->getDagNode();
}


const std::string& Argument::getLabel(void) const {
    return label;
}


/** Get the type spec of this class. We return a static class variable because all instances will be exactly from this type. */
const TypeSpec& Argument::getTypeSpec(void) const {
    return typeSpec;
}


const Variable& Argument::getVariable(void) const {
    return *var;
}


Variable& Argument::getVariable(void) {
    return *var;
}


/** Complete info about object */
void Argument::printValue(std::ostream &o) const {

    o << label << " = ";
    var->printValue(o);

}

/** Set the DAG node of the argument */
void Argument::setDagNode(RbDagNodePtr newNode) {
    var->setDagNode(newNode);
}


/** Set the variable of the argument */
void Argument::setVariable(Variable* newVar) {
    
    // delete the old variable
    if (var != NULL) {
        delete var;
    }
    var = newVar;
}

