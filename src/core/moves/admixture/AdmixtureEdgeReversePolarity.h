//
//  AdmixtureEdgeReversePolarity.h
//  rb_mlandis
//
//  Created by Michael Landis on 3/6/13.
//  Copyright (c) 2013 Michael Landis. All rights reserved.
//

#ifndef __rb_mlandis__AdmixtureEdgeReversePolarity__
#define __rb_mlandis__AdmixtureEdgeReversePolarity__

#include <ostream>
#include <set>
#include <string>

#include "SimpleMove.h"
#include "StochasticNode.h"
#include "AdmixtureTree.h"

namespace RevBayesCore {
    
    class AdmixtureEdgeReversePolarity : public SimpleMove {
        
    public:
        AdmixtureEdgeReversePolarity( StochasticNode<AdmixtureTree> *n, double l, double weight);                                                          //!<  constructor
        
        // Basic utility functions
        AdmixtureEdgeReversePolarity*          clone(void) const;                                                                  //!< Clone object
        void                            swapNode(DagNode *oldN, DagNode *newN);
        
    protected:
        const std::string&              getMoveName(void) const;                                                            //!< Get the name of the move for summary printing
        double                          performSimpleMove(void);                                                            //!< Perform move
        void                            rejectSimpleMove(void);
         void                           acceptSimpleMove(void);
        
    private:
        
        // member variables
        StochasticNode<AdmixtureTree>*  variable;
        
        // stored objects to undo proposal
        AdmixtureNode*                  storedAdmixtureParent;
        AdmixtureNode*                  storedAdmixtureChild;
        double                          storedWeight;
        
        double lambda;
        bool failed;
        
    };
    
}




#endif /* defined(__rb_mlandis__AdmixtureEdgeReversePolarity__) */
