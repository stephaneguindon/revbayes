#ifndef ModelContainer_H
#define ModelContainer_H

#include "Container.h"
#include "RlDeterministicNode.h"
#include "RlUserInterface.h"
#include "TypedDagNode.h"
#include "TypedUserFunction.h"
#include "TypeSpec.h"

#include <iostream>
#include <vector>


namespace RevLanguage {
    

    /**
     * @brief ModelContainer: base class for Rev containers of model objects
     *
     * The most important difference between this class and WorkspaceContainer
     * is that this class keeps an internal DAG node that is in charge of the
     * value. The distinction between WorkspaceContainer and ModelContainer is
     * analogous to that between WorkspaceObject and ModelObject.
     * 
     * The template type is the Rev model object type (rlType) and dim, the dimension
     * of the container (1 for vector, 2 for matrix, etc).
     *
     * Because of the need for a specialized value type for containers of abstract
     * elements, the template arguments also include the desired value type.
     *
     * Because of the dependency of some functionality on the number of
     * dimensions, we provide specializations for supported number of dimensions
     * (currently 1 and 2) for some functions, but no generic implementation, ensuring
     * that this class can only be used with those dimensions.
     */
    
    // Generic template
    template <typename rlType, size_t dim, typename valueType>
    class ModelContainer : public Container {

    public:
    
        typedef typename valueType::iterator        iterator;
        typedef typename valueType::const_iterator  const_iterator;
        
        virtual                                    ~ModelContainer(void);                                               //!< Destructor
        
        // STL vector-like functions
        iterator                                    begin(void);                                                        //!< Iterator to the beginning of the Vector
        const_iterator                              begin(void) const;                                                  //!< Const-iterator to the beginning of the Vector
        iterator                                    end(void);                                                          //!< Iterator to the end of the Vector
        const_iterator                              end(void) const;                                                    //!< Const-iterator to the end of the Vector
        
        // Basic utility functions you have to override
        virtual Container*                          clone(void) const = 0;                                              //!< Clone object
        static const std::string&                   getClassType(void);                                                 //!< Get class name
        static const TypeSpec&                      getClassTypeSpec(void);                                             //!< Get class type spec
        virtual const TypeSpec&                     getTypeSpec(void) const = 0;                                        //!< Get the object type spec of the instance
        
        // Basic utility functions you should not have to override
        RevBayesCore::TypedDagNode<valueType>*      getDagNode(void) const;                                             //!< Get the DAG node
        const valueType&                            getValue(void) const;                                               //!< Get the internal value
        bool                                        hasDagNode(void) const;                                             //!< Do we have a DAG node?
        bool                                        isConstant(void) const;                                             //!< Is this variable and the internally stored deterministic node constant?
        void                                        makeCompositeValue();                                               //!< Convert the container to a composite value
        void                                        makeConstantValue();                                                //!< Convert the container to a constant variable
        void                                        makeDeterministicValue(UserFunctionCall* call, UserFunctionArgs* args);     //!< Convert to deterministic object with a userdefined Rev function
        RevObject*                                  makeIndirectReference(void);                                        //!< Make an object referencing the dag node of this object
        void                                        printStructure(std::ostream& o) const;                              //!< Print structure of language object for user
        void                                        printValue(std::ostream& o) const;                                  //!< Print value for user
        void                                        replaceVariable(RevObject *newVar);                                 //!< Prepare to replace the internal DAG node
        void                                        setName(const std::string &n);                                      //!< Set the name of the variable (if applicable)
        
        // Member object functions you may want to override
        virtual RevPtr<Variable>                    executeMethod(const std::string& name, const std::vector<Argument>& args);  //!< Override to map member methods to internal functions
        virtual const MethodTable&                  getMethods(void) const;                                                     //!< Get member methods (const)
        
        // Container functions that you need to override to protect from assignment or external modification
        virtual RevPtr<Variable>                    findOrCreateElement(const std::vector<size_t>& oneOffsetIndices);   //!< Find or create element variable
        virtual RevPtr<Variable>                    getElement(const std::vector<size_t>& oneOffsetIndices);            //!< Get element variable

        // Container functions you should not have to override
        size_t                                      getDim(void) const;                                                 //!< Get the dimensions
        RevObject*                                  makeElementLookup(const std::vector< RevPtr<Variable> >& oneOffsetIndices); //!< Get dynamic element variable lookup
        size_t                                      size(void) const;                                                   //!< Get the number of elements
        
        // Container function you have to override
        virtual void                                setElements(std::vector<RevObject*> elems, const std::vector<size_t>& lengths) = 0; //!< Set elements from Rev objects
        
    protected:
        ModelContainer(void);                                           //!< Construct empty container
        ModelContainer(const valueType& v);                             //!< Construct constant model container
        ModelContainer(RevBayesCore::TypedDagNode<valueType>* n);       //!< Construct model container from DAG node
        ModelContainer(const ModelContainer<rlType,dim,valueType>& c);  //!< Copy constructor
        
        // Assignment operator
        ModelContainer&                             operator=(const ModelContainer& x);                             //!< Assignment operator
        
        // ModelContainer helper function defined here
        void                                        setDagNode(RevBayesCore::DagNode *newNode);                     //!< Set or replace the internal dag node (and keep me)
        
        // ModelContainer helper function you have to override
        virtual RevPtr<Variable>                    getElementFromValue(const std::vector<size_t>& oneOffsetIndices) = 0;   //!< Get element from value (and not from container node)
        
        // Member variable (protected and not private to make it available to derived classes)
        RevBayesCore::TypedDagNode<valueType>*      dagNode;                                                        //!< The DAG node keeping the value

    };

}


#include "ArgumentRule.h"
#include "ArgumentRules.h"
#include "ConstantNode.h"
#include "ContainerNode.h"
#include "MemberFunction.h"
#include "MethodTable.h"
#include "RbException.h"
#include "RlUtils.h"
#include "StochasticNode.h"

using namespace RevLanguage;


/**
 * Default constructor. Construct empty model container. We do not
 * need to store the type of element because that can be obtained
 * from the type spec of the derived class instance.
 */
template <typename rlType, size_t dim, typename valueType>
ModelContainer<rlType, dim, valueType>::ModelContainer( void ) :
    Container(),
    dagNode( new RevBayesCore::ConstantNode<valueType>( "", new valueType() ) )
{
    dagNode->incrementReferenceCount();
}


/**
 * Construct constant model container from appropriate container value,
 * which needs to be of valueType.
 */
template <typename rlType, size_t dim, typename valueType>
ModelContainer<rlType, dim, valueType>::ModelContainer( const valueType& v ) :
    Container(),
    dagNode( new RevBayesCore::ConstantNode<valueType>( "", new valueType( v ) ) )
{
    dagNode->incrementReferenceCount ();
}


/**
 * Construct a container wrapper around an appropriate DAG node.
 */
template <typename rlType, size_t dim, typename valueType>
ModelContainer<rlType, dim, valueType>::ModelContainer( RevBayesCore::TypedDagNode<valueType>* n ) :
    Container(),
    dagNode( n )
{
    dagNode->incrementReferenceCount();
}


/**
 * Copy constructor. We make an independent copy of the DAG node
 * here, and deal with DAG node memory management.
 */
template <typename rlType, size_t dim, typename valueType>
ModelContainer<rlType, dim, valueType>::ModelContainer( const ModelContainer& c) :
    Container( c ),
    dagNode( NULL )
{
    if ( c.dagNode != NULL )
    {
        dagNode = c.dagNode->clone();
        dagNode->incrementReferenceCount();
    }
}


/** Destructor needs to deal with DAG node memory management */
template <typename rlType, size_t dim, typename valueType>
ModelContainer<rlType, dim, valueType>::~ModelContainer()
{
    if ( dagNode->decrementReferenceCount() == 0 )
        delete dagNode;
}


/**
 * Assignment operator. We make sure we have an independent clone of the
 * DAG node here, and deal with DAG node memory management.
 */
template <typename rlType, size_t dim, typename valueType>
ModelContainer<rlType, dim, valueType>& ModelContainer<rlType, dim, valueType>::operator=(const ModelContainer& c)
{
    
    if ( this != &c )
    {
        // Free the memory
        if ( dagNode->decrementReferenceCount() == 0 )
            delete dagNode;

        dagNode = NULL;
        
        // Create my own copy
        if ( c.dagNode != NULL )
        {
            dagNode = c.dagNode->clone();
            dagNode->incrementReferenceCount();
        }
    }
    
    return *this;
}


/**
 * Get iterator to the beginning of the container value elements. We
 * assume that the valueType class has an iterator.
 *
 * @todo Check that this is safe. Is it not possible to assign to value using this iterator?
 */
template <typename rlType, size_t dim, typename valueType>
typename valueType::iterator ModelContainer<rlType, dim, valueType>::begin( void ) {
    
    return dagNode->getValue().begin();
}


/**
 * Get const_iterator to the beginning of the container value elements. We
 * assume that the valueType class has a const_iterator.
 */
template <typename rlType, size_t dim, typename valueType>
typename valueType::const_iterator ModelContainer<rlType, dim, valueType>::begin( void ) const {
    
    return dagNode->getValue().begin();
}


/**
 * Get iterator to the end of the container value elements. We assume
 * that the valueType class has an iterator.
 *
 * @todo Check that this is safe. Is it not possible to assign to value using this iterator?
 */
template <typename rlType, size_t dim, typename valueType>
typename valueType::iterator ModelContainer<rlType, dim, valueType>::end( void ) {
    
    return dagNode->getValue().end();
}


/**
 * Get const_iterator to the end of the container value elements. We assume
 * that the valueType class has a const_iterator.
 */
template <typename rlType, size_t dim, typename valueType>
typename valueType::const_iterator ModelContainer<rlType, dim, valueType>::end( void ) const {
    
    return dagNode->getValue().end();
}


/**
 * Map calls to member methods. This deals with the stochastic variable methods.
 */
template <typename rlType, size_t dim, typename valueType>
RevPtr<Variable> ModelContainer<rlType, dim, valueType>::executeMethod( std::string const &name, const std::vector<Argument> &args )
{
    if ( name == "clamp" )
    {
        // Check whether the variable is actually a stochastic node
        if ( !dagNode->isStochastic() ) {
            throw RbException( "Only stochastic variables can be clamped." );
        }
        
        // Convert the node
        RevBayesCore::StochasticNode<valueType>* stochNode = static_cast< RevBayesCore::StochasticNode<valueType>* >( dagNode );
        
        // Get the observation
        const valueType &observation = static_cast< const ModelContainer<rlType, 1, valueType>& >( args[0].getVariable()->getRevObject() ).getValue();
        
        // Clamp
        stochNode->clamp( new valueType( observation ) );
        
        return NULL;
    }
    else if (name == "redraw")
    {
        // Check whether the variable is actually a stochastic node
        if ( !dagNode->isStochastic() )
            throw RbException("You can only set the value for stochastic variables.");

        // Convert the node
        RevBayesCore::StochasticNode<valueType>* stochNode = static_cast< RevBayesCore::StochasticNode<valueType>* >( dagNode );
        
        // Redraw the value
        stochNode->redraw();
        
        return NULL;
    }
    else if ( name == "setValue" )
    {
        // Check whether the variable is actually a stochastic node
        if ( !dagNode->isStochastic() )
            throw RbException( "You can only set the value of a stochastic variable." );

        // Convert the node
        RevBayesCore::StochasticNode<valueType>* stochNode = static_cast< RevBayesCore::StochasticNode<valueType>* >( dagNode );
        
        // Get the observation
        const valueType &observation = static_cast< const ModelContainer<rlType, 1, valueType>& >( args[0].getVariable()->getRevObject() ).getValue();
        
        // Set the value
        stochNode->setValue( new valueType(observation) );
        
        return NULL;
    }
    else if ( name == "unclamp" )
    {
        // Check whether the DAG node is actually a stochastic node
        if ( !dagNode->isStochastic() )
            throw RbException( "Only stochastic variables can be unclamped." );
        
        // Convert the node
        RevBayesCore::StochasticNode<valueType>* stochNode = static_cast< RevBayesCore::StochasticNode<valueType>* >( dagNode );
        
        // Unclamp
        stochNode->unclamp();
        
        return NULL;
    }

    return Container::executeMethod( name, args );
}


/**
 * Find an element for assignment. We simply delegate this call to our
 * container node, if we have one. If we are a stochastic node or
 * deterministic node, we ask the user first if they want to break up
 * the previous association of the variable with a single dynamic DAG
 * node and make it a composite of several individual DAG nodes instead.
 * If we are a constant node, we simply go ahead quietly with the
 * replacement.
 */
template<typename rlType, size_t dim, typename valueType>
RevPtr<Variable> ModelContainer<rlType, dim, valueType>::findOrCreateElement( const std::vector<size_t>& oneOffsetIndices )
{
    ContainerNode<rlType, valueType>* theContainerNode = dynamic_cast< ContainerNode<rlType, valueType>* >( dagNode );
    
    if ( theContainerNode == NULL )
    {
        if ( dynamic_cast< RevBayesCore::ConstantNode<valueType>* >( dagNode ) == NULL )
        {
            bool answer = UserInterface::userInterface().ask( "Do you want to convert the container to a composite value?" );

            if ( answer == true )
                makeCompositeValue();
            else
                throw RbException( "Assignment to elements of a homogeneous container not allowed" );
        }
        else
            makeCompositeValue();

        theContainerNode = static_cast< ContainerNode<rlType, valueType>* >( dagNode );
    }
    
    return theContainerNode->findOrCreateElement( oneOffsetIndices );
}


/** Get class name of object */
template <typename rlType, size_t dim, typename valueType>
const std::string& ModelContainer<rlType, dim, valueType>::getClassType(void)
{
    static std::string rbClassName = "ModelContainer";
    
	return rbClassName;
}


/** Get class type spec describing type of object */
template <typename rlType, size_t dim, typename valueType>
const TypeSpec& ModelContainer<rlType, dim, valueType>::getClassTypeSpec(void)
{
    static TypeSpec rbClass = TypeSpec( getClassType(), &Container::getClassTypeSpec() );
    
	return rbClass;
}


/**
 * Get an element. We support reference assignment by giving out a smart pointer to the actual
 * element if we are a composite node. We do that by delegating the call to our container node.
 * If we are a constant node, we transform ourselves to a composite conatiner node first. If we
 * are a dynamic node, we simply give out a new temporary variable, a clone of the original
 * element. In this way, we can guard ourselves from attempts by others to modify the element
 * in cases where that would be inappropriate.
 *
 * Because we need to know something about the structure of our value type if we need to retrieve
 * the element from the value rather than from a container node, we delegate the task
 * to a derived class through the getElementFromValue function.
 *
 *
 */
template<typename rlType, size_t dim, typename valueType>
RevPtr<Variable> ModelContainer<rlType, dim, valueType>::getElement( const std::vector<size_t>& oneOffsetIndices )
{
    // First check if we want to return a slice
    size_t sliceDim = 0;
    for ( size_t i = 0; i < dim && i < oneOffsetIndices.size(); ++i )
    {
        if ( oneOffsetIndices[i] > 1 )
            sliceDim++;
    }

    // Full clone is easy, simply return a clone of ourselves
    if ( sliceDim == dim )
    {
        if ( oneOffsetIndices.size() > dim )
            throw RbException( "Slicing across Rev objects not supported" );
        
        return new Variable( this->clone() );
    }

    // Partial slice requires more work of us
    if ( sliceDim > 0 )
        throw RbException ( "Partial container slices not supported yet" );

    // We want a single element: delegate the work to our container node
    ContainerNode<rlType, valueType>* theContainerNode = dynamic_cast< ContainerNode<rlType, valueType>* >( dagNode );
    
    if ( theContainerNode == NULL )
    {
        if ( dynamic_cast< RevBayesCore::ConstantNode<valueType>* >( dagNode ) != NULL )
            makeCompositeValue();
        
        theContainerNode = static_cast< ContainerNode<rlType, valueType>* >( dagNode );
    }

    if ( theContainerNode != NULL )
        return theContainerNode->getElement( oneOffsetIndices );

    // That did not work. We need to do it ourselves.
    return getElementFromValue( oneOffsetIndices );
}


/**
 * Get method specifications. Here we provide argument rules for the stochastic
 * variable functions.
 */
template <typename rlType, size_t dim, typename valueType>
const MethodTable& ModelContainer<rlType, dim, valueType>::getMethods(void) const
{
    static MethodTable methods      = MethodTable();
    static bool        methodsSet   = false;
    
    if ( methodsSet == false )
    {
        // Stochastic variable member functions
        ArgumentRules* clampArgRules = new ArgumentRules();
        clampArgRules->push_back( new ArgumentRule("x", true, getTypeSpec() ) );
        methods.addFunction("clamp", new MemberFunction( RlUtils::Void, clampArgRules) );
        
        ArgumentRules* redrawArgRules = new ArgumentRules();
        methods.addFunction("redraw", new MemberFunction( RlUtils::Void, redrawArgRules) );
        
        ArgumentRules* setValueArgRules = new ArgumentRules();
        setValueArgRules->push_back( new ArgumentRule("x", true, getTypeSpec() ) );
        methods.addFunction("setValue", new MemberFunction( RlUtils::Void, setValueArgRules) );
        
        ArgumentRules* unclampArgRules = new ArgumentRules();
        methods.addFunction("unclamp", new MemberFunction( RlUtils::Void, unclampArgRules) );

        // Necessary call for proper inheritance
        methods.setParentTable( &Container::getMethods() );
        methodsSet = true;
    }
    
    return methods;
}


/** Get the DAG node holding the value */
template <typename rlType, size_t dim, typename valueType>
RevBayesCore::TypedDagNode<valueType>* ModelContainer<rlType, dim, valueType>::getDagNode( void ) const
{
    return dagNode;
}


/**
 * Get dimension of container. Returns 1 for vector, 2 for matrix etc.
 * Because the dimension is part of the template, we can do this in a
 * generic way.
 */
template <typename rlType, size_t dim, typename valueType>
size_t ModelContainer<rlType, dim, valueType>::getDim( void ) const
{
    return dim;
}


/**
 * Get the value of the node. We only give out a const reference to make
 * sure that nobody else messes with our value. Only the DAG node should
 * be able to do this.
 */
template <typename rlType, size_t dim, typename valueType>
const valueType& ModelContainer<rlType, dim, valueType>::getValue( void ) const
{
    return dagNode->getValue();
}


/** Make sure users understand we have an internal DAG node */
template <typename rlType, size_t dim, typename valueType>
bool ModelContainer<rlType, dim, valueType>::hasDagNode( void ) const
{
    return true;
}


/** Check whether this node has a constant value by asking the DAG node. */
template <typename rlType, size_t dim, typename valueType>
bool ModelContainer<rlType, dim, valueType>::isConstant( void ) const
{
    return dagNode->isConstant();
}


/**
 * Convert this node to a composite value, that is, a container where each element has
 * its own DAG node controlling its value. We do this by setting the DAG node to a
 * container node, if it is not already one.
 */
template <typename rlType, size_t dim, typename valueType>
void ModelContainer<rlType, dim, valueType>::makeCompositeValue( void )
{
    if ( dynamic_cast< ContainerNode<rlType, valueType>* >( dagNode ) != NULL )
        return;

    ContainerNode<rlType, valueType>* newNode = new ContainerNode<rlType, valueType>( "" );

    setDagNode( newNode );
}


/** Convert this node to a constant value if it is not already a constant value. */
template <typename rlType, size_t dim, typename valueType>
void ModelContainer<rlType, dim, valueType>::makeConstantValue( void )
{
    if ( !dagNode->isConstant() )
    {
        RevBayesCore::ConstantNode<valueType>* newNode = new RevBayesCore::ConstantNode<valueType>( dagNode->getName(), new valueType( dagNode->getValue() ) );

        setDagNode( newNode );
    }
}


/** Convert a model object to a deterministic object, the value of which is determined by a user-defined Rev function */
template <typename rlType, size_t dim, typename valueType>
void ModelContainer<rlType, dim, valueType>::makeDeterministicValue( UserFunctionCall* call, UserFunctionArgs* args )
{
    TypedUserFunction< valueType >*  fxn      = new TypedUserFunction< valueType >( call );
    DeterministicNode< valueType >*  detNode  = new DeterministicNode< valueType >("", fxn, args );
    
    setDagNode( detNode );
}


/** Make element lookup */
template <typename rlType, size_t dim, typename valueType>
RevObject* ModelContainer<rlType, dim, valueType>::makeElementLookup( const std::vector< RevPtr<Variable> >& oneOffsetIndices ) {
    
//    ElementLookupNode<typename rlType::valueType>* newNode = new ElementLookupNode< rlType::valueType >( "", this, oneOffsetIndices );
    
//    rlType* newObj = new rlType( newNode );
    
    return NULL; // newObj;
}


/** Make variable lookup */
template <typename rlType, size_t dim, typename valueType>
RevObject* ModelContainer<rlType, dim, valueType>::makeIndirectReference(void) {
    
//    IndirectReferenceNode< valueType >* newNode = new IndirectReferenceNode< valueType >( "", this );
    
    ModelContainer<rlType, dim, valueType>* newObj = this->clone();

//    newObj->setDagNode( newNode );

    return newObj;
}


/**
 * Print structure info for user  We simply delegate this to our
 * internal DAG node.
 */
template <typename rlType, size_t dim, typename valueType>
void ModelContainer<rlType, dim, valueType>::printStructure( std::ostream &o ) const
{
    dagNode->printStructureInfo( o );
}


/**
 * Print value for user. We just delegate this to our DAG node.
 *
 * The current hack works reasonably well for some vectors.
 *
 * @todo This needs a lot more work for nice printing of all container values.
 */
template <typename rlType, size_t dim, typename valueType>
void ModelContainer<rlType, dim, valueType>::printValue(std::ostream &o) const
{
    o << "[ ";
    dagNode->printValue( o,", " );
    o << " ]";
}


/**
 * Replace variable. This is called in Variable just prior to replacing this object
 * by another object with a different DAG node. We deal with this situation by telling
 * all children of the current DAG node that they are getting a new parent so that
 * they can adjust accordingly.
 */
template <typename rlType, size_t dim, typename valueType>
void ModelContainer<rlType, dim, valueType>::replaceVariable( RevObject *newObj )
{
    RevBayesCore::DagNode* newParent = newObj->getDagNode();
    
    while ( dagNode->getNumberOfChildren() > 0 )
    {
        dagNode->getFirstChild()->swapParent(dagNode, newParent);
    }
}


/**
 * Replace the DAG node. We throw an error if the new node does
 * not have the right value type. This is based on a dynamic
 * cast to the expected value type.
 */
template <typename rlType, size_t dim, typename valueType>
void RevLanguage::ModelContainer<rlType, dim, valueType>::setDagNode( RevBayesCore::DagNode* newNode )
{
    RevBayesCore::TypedDagNode<valueType>* newDagNode = dynamic_cast< RevBayesCore::TypedDagNode<valueType>* >( newNode );

    if ( newDagNode == NULL )
        throw RbException( "Illegal attempt to set type '" + this->getType() + "' with DAG node of wrong value type" );
    
    // Take care of the old value node
    if ( dagNode != NULL )
    {
        newDagNode->setName( dagNode->getName() );
        dagNode->replace( newDagNode );
        
        if ( dagNode->decrementReferenceCount() == 0 )
            delete dagNode;
    }
    
    // Set the new value node
    dagNode = newDagNode;
    
    // Increment the reference count to the new value node
    dagNode->incrementReferenceCount();
}


/** Name the object by naming the DAG node inside it. */
template <typename rlType, size_t dim, typename valueType>
void ModelContainer<rlType, dim, valueType>::setName( std::string const& n )
{
    if ( dagNode == NULL )
    {
        throw RbException( "Cannot set name of object with null DAG node." );
    }
    else
    {
        dagNode->setName( n );
    }
}


/**
 * Return size of container. This relies on there being a member
 * function size() in valueType.
 */
template <typename rlType, size_t dim, typename valueType>
size_t ModelContainer<rlType, dim, valueType>::size( void ) const
{
    return getValue().size();
}


#endif
