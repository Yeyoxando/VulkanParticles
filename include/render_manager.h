/*
 *	Author: Diego Ochando Torres
 *  Date: 30/12/2020
 *  e-mail: c0022981@my.shu.ac.uk | yeyoxando@gmail.com
 */

#ifndef __RENDER_MANAGER_H__
#define __RENDER_MANAGER_H__

// ------------------------------------------------------------------------- // 

// Includes

// ------------------------------------------------------------------------- // 
/*
 - Change uniforms by object (One set changed by object), 
   and by world, one time for scene (as camera mat or lights)
 -
   
*/

class RenderManager {
public:
  // Empty
  RenderManager();
  // Empty
  ~RenderManager();

  // Initialize here all the things to call them when needed and not in object construction
  void init();
  // Delete allocated resources and finishes here to control the destruction order of the managers
  void shutDown();

private:

  // All the members here
  // or inside an internal struct initialized and deleted at the beginning
};

// ------------------------------------------------------------------------- // 

#endif __RENDER_MANAGER_H__