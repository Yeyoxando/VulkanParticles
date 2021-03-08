/*
 *	Author: Diego Ochando Torres
 *  Date: 09/01/2020
 *  e-mail: c0022981@my.shu.ac.uk | yeyoxando@gmail.com
 */

#ifndef __SYSTEM_H__
#define __SYSTEM_H__

// ------------------------------------------------------------------------- //

#include "components/component.h"

#include <vector>

// ------------------------------------------------------------------------- //

class System {
public:

protected:
  std::vector<Component::ComponentKind> required_components_;

};
/* 
  - Systems will need to be executed by each entity which has certain components
  
*/

// ------------------------------------------------------------------------- //

#endif // __SYSTEM_H__