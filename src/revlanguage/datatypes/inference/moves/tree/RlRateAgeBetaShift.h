#ifndef RlRateAgeBetaShift_H
#define RlRateAgeBetaShift_H

#include "RateAgeBetaShift.h"
#include "RlMove.h"
#include "TypedDagNode.h"

#include <ostream>
#include <string>

namespace RevLanguage {
    
    /**
     * @brief Rev wrapper class for the rate-age-beta-shift move.
     *
     * This class is the Rev wrapper class for the rate-age-beta-shift move,
     * a move that proposes a new node age and branch rates so that the actual
     * branch lengths remain constant. The new node age is proposed from a scaled
     * beta distribution between its parent age and its oldest child age.
     *
     *
     * @author The RevBayes Development Core Team (Sebastian Hoehna)
     * @copyright GPL version 3
     */
    class RateAgeBetaShift : public Move {
        
    public:
        
        RateAgeBetaShift(void);                                                                                                //!< Default constructor
        
        // Basic utility functions
        virtual RateAgeBetaShift*                   clone(void) const;                                                      //!< Clone object
        void                                        constructInternalObject(void);                                          //!< We construct the a new internal Move.
        static const std::string&                   getClassName(void);                                                     //!< Get class name
        static const TypeSpec&                      getClassTypeSpec(void);                                                 //!< Get class type spec
        const MemberRules&                          getMemberRules(void) const;                                             //!< Get member rules (const)
        virtual const TypeSpec&                     getTypeSpec(void) const;                                                //!< Get language type of the object
        virtual void                                printValue(std::ostream& o) const;                                      //!< Print value (for user)
        
    protected:
        
        void                                        setConstMemberVariable(const std::string& name, const RbPtr<const Variable> &var);              //!< Set member variable
        
        RbPtr<const Variable>                       tree;
        RbPtr<const Variable>                       delta;
        RbPtr<const Variable>                       rates;
        RbPtr<const Variable>                       tune;
        
    };
    
}

#endif
