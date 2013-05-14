//
//  AdmixtureNearestNeighborInterchange.h
//  rb_mlandis
//
//  Created by Michael Landis on 4/12/13.
//  Copyright (c) 2013 Michael Landis. All rights reserved.
//

#ifndef __rb_mlandis__AdmixtureNearestNeighborInterchange__
#define __rb_mlandis__AdmixtureNearestNeighborInterchange__


#include <ostream>
#include <set>
#include <string>

#include "SimpleMove.h"
#include "ContinuousStochasticNode.h"
#include "StochasticNode.h"
#include "AdmixtureTree.h"

namespace RevBayesCore {
    
    class AdmixtureNearestNeighborInterchange : public SimpleMove {
        
    public:
        AdmixtureNearestNeighborInterchange( StochasticNode<AdmixtureTree> *n, std::vector<ContinuousStochasticNode*> br, double d, bool tuning, double weight);                                                         //!<  constructor
        
        // Basic utility functions
        AdmixtureNearestNeighborInterchange*  clone(void) const;                                                                  //!< Clone object
        void                            swapNode(DagNode *oldN, DagNode *newN);
        
    protected:
        const std::string&              getMoveName(void) const;                                                            //!< Get the name of the move for summary printing
        double                          performSimpleMove(void);                                                            //!< Perform move
        void                            rejectSimpleMove(void);
        void                            tune(void);
        
    private:
        
        // member variables
        StochasticNode<AdmixtureTree>*  variable;
        std::vector<ContinuousStochasticNode*> branchRates;
        
        // stored objects to undo proposal
        bool                            failed;
        bool                            changed;
        
        AdmixtureNode*                  storedNode;
        AdmixtureNode*                  storedBrother;
        AdmixtureNode*                  storedBrotherParent;
        AdmixtureNode*                  storedNodeParent;
        AdmixtureNode*                  storedNodeChildFix;
        AdmixtureNode*                  storedNodeChildMove;
        AdmixtureNode*                  newNodeParent;
        AdmixtureNode*                  newNodeChild;

        double                          storedAge;
        
        int                             storedNodeRateIndex;
        int                             storedBrotherRateIndex;
        double                          storedNodeRate;
        double                          storedBrotherRate;
        
        double                          delta;
    };
}


#endif /* defined(__rb_mlandis__AdmixtureNearestNeighborInterchange__) */
