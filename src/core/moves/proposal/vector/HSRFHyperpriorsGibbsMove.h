#ifndef HSRFHyperpriorsGibbsMove_H
#define HSRFHyperpriorsGibbsMove_H

#include "AbstractGibbsMove.h"
#include "AbstractMove.h"
#include "StochasticNode.h"

#include <set>
#include <vector>

namespace RevBayesCore {

    class HSRFHyperpriorsGibbsMove : public AbstractGibbsMove {

    public:
        HSRFHyperpriorsGibbsMove( StochasticNode<double> *g, std::vector< StochasticNode<double> *> l, std::vector< StochasticNode<double> *> n, double z, double w);                                                         //!<  constructor
        virtual                                ~HSRFHyperpriorsGibbsMove(void);                                                             //!< Destructor

        // Basic utility functions
        HSRFHyperpriorsGibbsMove*            clone(void) const;                                                                  //!< Clone object
        const std::string&                      getMoveName(void) const;                                                            //!< Get the name of the move for summary printing
        bool                                     heatsAreAllowable(double prHeat, double lHeat, double pHeat); //<! A check we can call for whether it is okay to use this move with given prior/likelihood/posterior heats
        
    protected:
        void                                    performGibbsMove(void);                                 //!< Perform move
        void                                    swapNodeInternal(DagNode *oldN, DagNode *newN);                                             //!< Swap the pointers to the variable on which the move works on.

    private:

        // member variables
        StochasticNode<double>*                 global_scale;                                          //!< The half-Cauchy variable the Proposal is working on
        std::vector< StochasticNode<double> *>  local_scales;                                          //!< The half-Cauchy variables the Proposal is working on
        std::vector< StochasticNode<double> *>  normals;                                               //!< The normal random variables on which this is a hyperprior
        double                                  zeta;                                                  //!< Controls total scale of field, x is halfCauchy(0,zeta)

    };

}

#endif
