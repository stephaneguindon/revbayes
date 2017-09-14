#include "MrcaIndexStatistic.h"
#include "RbConstants.h"
#include "RbException.h"

using namespace RevBayesCore;

MrcaIndexStatistic::MrcaIndexStatistic(const TypedDagNode<Tree> *t, const Clade &c) : TypedFunction<long>( new long(-1) ),
    tree( t ),
    clade( c ),
    index( RbConstants::Size_t::nan )
{
    // add the tree parameter as a parent
    addParameter( tree );
    
    initialize();
    update();
}


MrcaIndexStatistic::~MrcaIndexStatistic( void )
{
    // We don't delete the parameters, because they might be used somewhere else too. The model needs to do that!
    
}



MrcaIndexStatistic* MrcaIndexStatistic::clone( void ) const
{
    
    return new MrcaIndexStatistic( *this );
}


void MrcaIndexStatistic::initialize( void )
{
    clade.resetTaxonBitset( tree->getValue().getTaxonBitSetMap() );
    taxa_count = clade.size();
    index = RbConstants::Size_t::nan;
    
}


void MrcaIndexStatistic::update( void )
{
    
    const std::vector<TopologyNode*> &n = tree->getValue().getNodes();
    size_t min_clade_size = n.size() + 2;
    
    bool found = false;
    if ( index != RbConstants::Size_t::nan )
    {
        
        TopologyNode *node = n[index];
        size_t cladeSize = size_t( (node->getNumberOfNodesInSubtree(true) + 1) / 2);
        if ( node->containsClade( clade, false ) == true )
        {
            
            if ( taxa_count == cladeSize )
            {
                found = true;
            }
            else
            {
                min_clade_size = cladeSize;
            }
            
        }
        
    }
    
    
    if ( found == false )
    {
        
        for (size_t i = tree->getValue().getNumberOfTips(); i < n.size(); ++i)
        {
            
            TopologyNode *node = n[i];
            size_t clade_size = size_t( (node->getNumberOfNodesInSubtree(true) + 1) / 2);
            if ( clade_size < min_clade_size && clade_size >= taxa_count && node->containsClade( clade, false ) )
            {
                
                index = node->getIndex();
                min_clade_size = clade_size;
                if ( taxa_count == clade_size )
                {
                    break;
                }
                
            }
            
        }
        
    }
    
    if ( index == RbConstants::Size_t::nan )
    {
        throw RbException("MrcaIndex-Statistics can only be applied if clade is present.");
    }
    
    int mrcaIndexIdx = int(index);
    *value = mrcaIndexIdx;
}



void MrcaIndexStatistic::swapParameterInternal(const DagNode *oldP, const DagNode *newP)
{
    
    if (oldP == tree)
    {
        tree = static_cast<const TypedDagNode<Tree>* >( newP );
        index = RbConstants::Size_t::nan;
    }
    
}


