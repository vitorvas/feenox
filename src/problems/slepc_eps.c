/*------------ -------------- -------- --- ----- ---   --       -            -
 *  feenox's eigen solver using SLEPc routines
 *
 *  Copyright (C) 2021 jeremy theler
 *
 *  This file is part of FeenoX <https://www.seamplex.com/feenox>.
 *
 *  feenox is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  feenox is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with feenox.  If not, see <http://www.gnu.org/licenses/>.
 *------------------- ------------  ----    --------  --     -       -         -
 */
#include "feenox.h"
extern feenox_t feenox;

int feenox_solve_slepc_eigen(void) {

#ifdef HAVE_SLEPC

  // we need the matrices to set the operators
  feenox_call(feenox_build());
  feenox_call(feenox_dirichlet_eval());
  feenox_call(feenox_dirichlet_set_K());  
  feenox_call(feenox_dirichlet_set_M());
  
  if (feenox.pde.eps == NULL) {
    petsc_call(EPSCreate(PETSC_COMM_WORLD, &feenox.pde.eps));
//    petsc_call(PetscObjectSetName((PetscObject)feenox.pde.eps, "eigenvalue-problem_solver"));
    if (feenox.pde.eps_type != NULL) {
      petsc_call(EPSSetType(feenox.pde.eps, feenox.pde.eps_type));
    }
    
    // get the associated contexts
    ST st;
    petsc_call(EPSGetST(feenox.pde.eps, &st));
    if (feenox.pde.st_type != NULL) {
      petsc_call(STSetType(st, feenox.pde.st_type));
    }

    if (feenox.pde.eigen_formulation == eigen_formulation_omega) {
      petsc_call(EPSSetOperators(feenox.pde.eps, feenox.pde.K_bc, feenox.pde.M_bc));
    } else {  
      petsc_call(EPSSetOperators(feenox.pde.eps, feenox.pde.M_bc, feenox.pde.K_bc));
    }

/*    
    if (feenox.pde.st_shift.items != NULL) {
      petsc_call(STSetShift(st, feenox_expression_eval(&feenox.pde.st_shift)));
    }
    if (feenox.pde.st_anti_shift.items != NULL) {
      petsc_call(STCayleySetAntishift(st, feenox_expression_eval(&feenox.pde.st_anti_shift)));
    }
*/    
    
    // tolerances
    petsc_call(EPSSetTolerances(feenox.pde.eps, feenox_var_value(feenox.pde.vars.eps_tol),
                                                (PetscInt)feenox_var_value(feenox.pde.vars.eps_max_it)));
    
    // TODO: make a virtual method
    feenox_call(feenox_problem_setup_eps_modal());
    
    KSP ksp;
    petsc_call(STGetKSP(st, &ksp));
    feenox_call(feenox_setup_ksp(ksp));

    // this should be faster but it is not
/*    
    if (feenox.pde.symmetric_K && feenox.pde.symmetric_M) {
      petsc_call(EPSSetProblemType(feenox.pde.eps, EPS_GHEP));
    }  
 */
    
    // convergence with respect to the matrix norm
//    petsc_call(EPSSetConvergenceTest(feenox.pde.eps, EPS_CONV_NORM));
    
    // specify how many eigenvalues (and eigenvectors) to compute.
    if (feenox.pde.eps_ncv.items != NULL) {
      petsc_call(EPSSetDimensions(feenox.pde.eps, feenox.pde.nev, (PetscInt)(feenox_expression_eval(&feenox.pde.eps_ncv)), PETSC_DEFAULT));
    } else {
      petsc_call(EPSSetDimensions(feenox.pde.eps, feenox.pde.nev, PETSC_DEFAULT, PETSC_DEFAULT));
    }
    
    feenox_check_alloc(feenox.pde.eigenvalue = calloc(feenox.pde.nev, sizeof(PetscScalar)));
    feenox_check_alloc(feenox.pde.eigenvector = calloc(feenox.pde.nev, sizeof(Vec)));

    petsc_call(EPSSetFromOptions(feenox.pde.eps));
    
  }
  
//  PetscViewerPushFormat(PETSC_VIEWER_STDOUT_WORLD, PETSC_VIEWER_ASCII_MATLAB);
//  MatView(feenox.pde.M, PETSC_VIEWER_STDOUT_WORLD);
  
  petsc_call(EPSSetInitialSpace(feenox.pde.eps, 1, &feenox.pde.phi));
  petsc_call(EPSSolve(feenox.pde.eps));
  PetscInt nconv = 0;
  petsc_call(EPSGetConverged(feenox.pde.eps, &nconv));
  if (nconv < feenox.pde.nev) {
    feenox_push_error_message("eigen-solver obtained only %d converged eigen-pairs (%d requested)", nconv, feenox.pde.nev);
    return FEENOX_ERROR;
  }
  
  PetscScalar imag = 0;
  unsigned int i = 0;
  for (i = 0; i < feenox.pde.nev; i++) {
    petsc_call(MatCreateVecs(feenox.pde.K, NULL, &feenox.pde.eigenvector[i]));
    petsc_call(EPSGetEigenpair(feenox.pde.eps, i, &feenox.pde.eigenvalue[i], &imag, feenox.pde.eigenvector[i], PETSC_NULL));
    if (fabs(imag) > feenox_var_value(feenox.pde.vars.eps_tol)) {
      feenox_push_error_message("the eigenvalue %d is complex (%g + i %g)", i+1, feenox.pde.eigenvalue[i], imag);
      return FEENOX_ERROR;
    }
  }
  
#endif
  
  return FEENOX_OK;
  
}