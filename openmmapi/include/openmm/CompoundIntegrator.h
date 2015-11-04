#ifndef OPENMM_COMPOUNDINTEGRATOR_H_
#define OPENMM_COMPOUNDINTEGRATOR_H_

/* -------------------------------------------------------------------------- *
 *                                   OpenMM                                   *
 * -------------------------------------------------------------------------- *
 * This is part of the OpenMM molecular simulation toolkit originating from   *
 * Simbios, the NIH National Center for Physics-Based Simulation of           *
 * Biological Structures at Stanford, funded under the NIH Roadmap for        *
 * Medical Research, grant U54 GM072970. See https://simtk.org.               *
 *                                                                            *
 * Portions copyright (c) 2015 Stanford University and the Authors.           *
 * Authors: Peter Eastman                                                     *
 * Contributors:                                                              *
 *                                                                            *
 * Permission is hereby granted, free of charge, to any person obtaining a    *
 * copy of this software and associated documentation files (the "Software"), *
 * to deal in the Software without restriction, including without limitation  *
 * the rights to use, copy, modify, merge, publish, distribute, sublicense,   *
 * and/or sell copies of the Software, and to permit persons to whom the      *
 * Software is furnished to do so, subject to the following conditions:       *
 *                                                                            *
 * The above copyright notice and this permission notice shall be included in *
 * all copies or substantial portions of the Software.                        *
 *                                                                            *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR *
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,   *
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL    *
 * THE AUTHORS, CONTRIBUTORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM,    *
 * DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR      *
 * OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE  *
 * USE OR OTHER DEALINGS IN THE SOFTWARE.                                     *
 * -------------------------------------------------------------------------- */

#include "Integrator.h"
#include "openmm/Kernel.h"
#include "internal/windowsExport.h"

namespace OpenMM {

/**
 * This class allows you to use multiple integration algorithms within a single simulation,
 * switching back and forth between them.  To use it, create whatever other Integrators
 * you need, then add all of them to a CustomIntegrator:
 * 
 * <tt><pre>
 * CompoundIntegrator compoundIntegrator;
 * compoundIntegrator.addIntegrator(new VerletIntegrator(0.001));
 * compoundIntegrator.addIntegrator(new LangevinIntegrator(300.0, 1.0, 0.001));
 * </pre></tt>
 * 
 * Next create a Context, specifying the CompoundIntegrator as the Integrator to use for
 * the Context:
 * 
 * <tt><pre>
 * Context context(system, compoundIntegrator);
 * </pre></tt>
 * 
 * Finally, call setCurrentIntegrator() to set which Integrator is active.  That one will
 * be used for all calls to step() until the next time you change it.
 * 
 * <tt><pre>
 * compoundIntegrator.setCurrentIntegrator(0);
 * compoundIntegrator.step(1000); // Take 1000 steps of Verlet dynamics
 * compoundIntegrator.setCurrentIntegrator(1);
 * compoundIntegrator.step(1000); // Take 1000 steps of Langevin dynamics
 * </pre></tt>
 */

class OPENMM_EXPORT CompoundIntegrator : public Integrator {
public:
    /**
     * Create a CompoundIntegrator.
     */
    explicit CompoundIntegrator();
    ~CompoundIntegrator();
    /**
     * Get the number of Integrators that have been added to this CompoundIntegrator.
     */
    int getNumIntegrators() const;
    /**
     * Add an Integrator to this CompoundIntegrator.  The Integrator object should have
     * been created on the heap with the "new" operator.  The CompoundIntegrator takes over
     * ownership of it, and deletes it when the CompoundIntegrator itself is deleted.
     * All Integrators must be added before the Context is created.
     * 
     * @param integrator    the Integrator to add
     * @return the index of the Integrator that was added
     */
    int addIntegrator(Integrator* integrator);
    /**
     * Get a reference to one of the Integrators that have been added to this CompoundIntegrator.
     * 
     * @param index     the index of the Integrator to get
     */
    Integrator& getIntegrator(int index);
    /**
     * Get a const reference to one of the Integrators that have been added to this CompoundIntegrator.
     * 
     * @param index     the index of the Integrator to get
     */
    const Integrator& getIntegrator(int index) const;
    /**
     * Get the index of the current Integrator.
     */
    int getCurrentIntegrator() const;
    /**
     * Set the current Integrator.
     * 
     * @param index    the index of the Integrator to use
     */
    void setCurrentIntegrator(int index);
    /**
     * Get the size of each time step, in picoseconds.  This method calls getStepSize() on
     * whichever Integrator has been set as current.
     * 
     * @return the step size, measured in ps
     */
    double getStepSize() const;
    /**
     * Set the size of each time step, in picoseconds.  This method calls setStepSize() on
     * whichever Integrator has been set as current.
     * 
     * @param size    the step size, measured in ps
     */
    void setStepSize(double size);
    /**
     * Get the distance tolerance within which constraints are maintained, as a fraction of the constrained distance.
     * This method calls getConstraintTolerance() on whichever Integrator has been set as current.
     */
    double getConstraintTolerance() const;
    /**
     * Set the distance tolerance within which constraints are maintained, as a fraction of the constrained distance.
     * This method calls setConstraintTolerance() on whichever Integrator has been set as current.
     */
    void setConstraintTolerance(double tol);
   /**
     * Advance a simulation through time by taking a series of time steps.  This method
     * calls step() on whichever Integrator has been set as current.
     * 
     * @param steps   the number of time steps to take
     */
    void step(int steps);
protected:
    /**
     * This will be called by the Context when it is created.  It informs the Integrator
     * of what context it will be integrating, and gives it a chance to do any necessary initialization.
     * It will also get called again if the application calls reinitialize() on the Context.
     * 
     * The implementation calls initialize() on each Integrator that has been added to this CompoundIntegrator.
     */
    void initialize(ContextImpl& context);
    /**
     * This will be called by the Context when it is destroyed to let the Integrator do any necessary
     * cleanup.  It will also get called again if the application calls reinitialize() on the Context.
     * 
     * The implementation calls cleanup() on each Integrator that has been added to this CompoundIntegrator.
     */
    void cleanup();
    /**
     * Get the names of all Kernels used by this Integrator.
     * 
     * The implementation returns the union of all kernel names required by all Integrators that have been added.
     */
    std::vector<std::string> getKernelNames();
    /**
     * Compute the kinetic energy of the system at the current time.
     * 
     * The implementation calls computeKineticEnergy() on whichever Integrator has been set as current.
     */
    double computeKineticEnergy();
private:
    int currentIntegrator;
    std::vector<Integrator*> integrators;
};

} // namespace OpenMM

#endif /*OPENMM_COMPOUNDINTEGRATOR_H_*/
