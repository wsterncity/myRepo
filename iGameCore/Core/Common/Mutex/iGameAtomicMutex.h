/*=========================================================================

  Program:   Visualization Toolkit
  Module:    iGameAtomicMutex.h

  Copyright (c) Ken Martin, Will Schroeder, Bill Lorensen
  All rights reserved.
  See Copyright.txt or http://www.kitware.com/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notice for more information.

=========================================================================*/
/**
 * @class   iGameAtomicMutex
 * @brief   mutual exclusion locking class using atomic operations
 *
 * iGameAtomicMutex allows the locking of variables which are accessed
 * through different threads using atomic operations. An atomic mutex
 * might be preferable over std::mutex, because it is faster when you want to spin lock and the
 * probability of acquiring the lock is high. The benefit of iGameAtomicMutex over
 * std::atomic<bool> is that is is copy constructible, and that is has predefined optimized
 * lock/unlock functions that can be used as a drop in replacement instead of std::mutex.
 */

#ifndef iGameAtomicMutex_h
#define iGameAtomicMutex_h

#include "iGameObject.h" // For export macro
#include <atomic>                // For std::atomic

IGAME_NAMESPACE_BEGIN
class iGameAtomicMutex  {

public:
    
    // left public purposely to allow for copy construction
    iGameAtomicMutex();
    ~iGameAtomicMutex() = default;
    iGameAtomicMutex(const iGameAtomicMutex& other);
    iGameAtomicMutex& operator=(const iGameAtomicMutex& other);

    ///@{
    /**
   * Lock/Unlock atomic operation.
   *
   * Note: lock/unlock is lowercase, to conform to the mutex wrapper std::lock_guard
   * which provides a convenient RAII-style mechanism  for owning a mutex for the
   * duration of a scoped block.
   */
    void lock();
    void unlock();
    ///@}
private:
    std::atomic_bool Locked;
};
IGAME_NAMESPACE_END
#endif 
