#include "feenox.h"
#include "thermal.h"
extern feenox_t feenox;
thermal_t thermal;

int feenox_problem_init_parser_thermal(void) {
  
  feenox.pde.dofs = 1;
  feenox_check_alloc(feenox.pde.unknown_name = calloc(feenox.pde.dofs, sizeof(char *)));
  feenox_check_alloc(feenox.pde.unknown_name[0] = strdup("T"));
  
  feenox_call(feenox_problem_define_solutions());
  
///va+T_max+name T_max
///va+T_max+detail The maximum temperature\ $T_\text{max}$ of the thermal problem.
  feenox.pde.vars.T_max = feenox_define_variable_get_ptr("T_max");

///va+T_min+name T_min
///va+T_min+detail The minimum temperature\ $T_\text{min}$ of the thermal problem.
  feenox.pde.vars.T_min = feenox_define_variable_get_ptr("T_min");
  
  return FEENOX_OK;
}

int feenox_problem_init_runtime_thermal(void) {
  
#ifdef HAVE_PETSC
  
  // check if we were given an initial solution
  if ((feenox.pde.initial_condition = feenox_get_function_ptr("T_0")) != NULL) {
    if (feenox.pde.initial_condition->n_arguments != feenox.pde.dim) {
      feenox_push_error_message("initial condition function T_0 ought to have %d arguments instead of %d", feenox.pde.dim, feenox.pde.initial_condition->n_arguments);
      return FEENOX_OK;
    }
  }

  // we are FEM not FVM
  feenox.pde.spatial_unknowns = feenox.pde.mesh->n_nodes;
  feenox.pde.mesh->data_type = data_type_node;
  feenox.pde.global_size = feenox.pde.spatial_unknowns * feenox.pde.dofs;
  
  
  // TODO: check end_time and quasistatic
  feenox.pde.has_rhs = PETSC_TRUE;
  // TODO: depends on end_time
  feenox.pde.has_mass = (feenox_var_value(feenox_special_var(end_time)) > 0) ? PETSC_TRUE : PETSC_FALSE;

  // initialize distributions
  // TODO: document distributions
  // here we just initialize everything, during build we know which
  // of them are mandatory and which are optional
  feenox_call(feenox_distribution_init(&thermal.k, "k"));
  if (thermal.k.defined == 0) {
    feenox_push_error_message("undefined thermal conductivity 'k'");
    return FEENOX_ERROR;
  }
  if (thermal.k.full == 0) {
    // TODO: tell which are the missing volumes
    feenox_push_error_message("thermal conductivity 'k' is not defined over all volumes");
    return FEENOX_ERROR;
  }
  
  feenox_call(feenox_distribution_init(&thermal.q, "q'''"));
  if (thermal.q.defined == 0) {
    feenox_call(feenox_distribution_init(&thermal.q, "q"));
  }
  // TODO: check that this does not depend on space
  feenox_call(feenox_distribution_init(&thermal.Q, "Q"));
  
  if (feenox.pde.has_mass) {
    feenox_call(feenox_distribution_init(&thermal.kappa, "kappa"));
    if (thermal.kappa.defined == 0) {
      feenox_call(feenox_distribution_init(&thermal.rhocp, "rhocp"));
      if (thermal.rhocp.defined == 0) {
        feenox_call(feenox_distribution_init(&thermal.rho, "rho"));
        feenox_call(feenox_distribution_init(&thermal.cp, "cp"));

        if (thermal.rhocp.defined == 0 || thermal.cp.defined == 0) {
          feenox_push_error_message("either 'kappa', 'rhocp' or both 'rho' and 'cp' are needed for transient");
          return FEENOX_ERROR;
        }
        if (thermal.rhocp.full == 0 || thermal.cp.full == 0) {
          feenox_push_error_message("either 'rho' or 'cp' is not defined over all volumes");
          return FEENOX_ERROR;
        }
      } else if (thermal.rhocp.full == 0) {
        feenox_push_error_message("product 'rhocp' is not defined over all volumes");
        return FEENOX_ERROR;
      }
    } else if (thermal.kappa.full == 0) {
      feenox_push_error_message("thermal diffusivity 'kappa' is not defined over all volumes");
      return FEENOX_ERROR;
    }
  }

  
  thermal.volumetric_space_dependent |= feenox_expression_depends_on_space(thermal.k.dependency_variables);
  thermal.volumetric_space_dependent |= feenox_expression_depends_on_space(thermal.q.dependency_variables);
  thermal.volumetric_space_dependent |= feenox_expression_depends_on_space(thermal.Q.dependency_variables);
  thermal.volumetric_space_dependent |= feenox_expression_depends_on_space(thermal.kappa.dependency_variables);
  thermal.volumetric_space_dependent |= feenox_expression_depends_on_space(thermal.rho.dependency_variables);
  thermal.volumetric_space_dependent |= feenox_expression_depends_on_space(thermal.cp.dependency_variables);
  thermal.volumetric_space_dependent |= feenox_expression_depends_on_space(thermal.rhocp.dependency_variables);
  
  // TODO: automatic
  if (feenox.pde.math_type == math_type_nonlinear) {
    feenox.pde.solve_petsc = feenox_solve_petsc_nonlinear;
  } else {
    feenox.pde.solve_petsc = feenox_solve_petsc_linear;
  }
  
#endif  
  return FEENOX_OK;
}
