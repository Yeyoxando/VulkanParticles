/*
* Author: Diego Ochando Torres
* Date: 31/12/2020
* e-mail: c0022981@my.shu.ac.uk | yeyoxando@gmail.com
*/

#ifndef __COMPONENT_MATERIAL_H__
#define __COMPONENT_MATERIAL_H__

// ------------------------------------------------------------------------- // 

#include "component.h"

// ------------------------------------------------------------------------- //

/*
  - associated with a pipeline like Unreal
  
  - research pipelines variation,
    same pipeline for all kind of objects, only 2 now -> same shader
  - opaque
  - translucent
  
  - How to change a shader then?

*/

class ComponentMaterial : public Component{
public:
  ComponentMaterial();

protected:
  ~ComponentMaterial();

};

// ------------------------------------------------------------------------- //

#endif // __COMPONENT_MATERIAL_H__
