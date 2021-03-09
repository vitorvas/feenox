/*------------ -------------- -------- --- ----- ---   --       -            -
 *  FeenoX common framework header
 *
 *  Copyright (C) 2009--2021 jeremy theler
 *
 *  This file is part of FeenoX.
 *
 *  FeenoX is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  FeenoX is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with FeenoX.  If not, see <http://www.gnu.org/licenses/>.
 *------------------- ------------  ----    --------  --     -       -         -
 */

#ifndef FEENOX_H
#define FEENOX_H

#define HAVE_INLINE
#define GSL_RANGE_CHECK_OFF

// we need all the includes here so they all follow the inline directive above
#include <gsl/gsl_blas.h>
#include <gsl/gsl_errno.h>
#include <gsl/gsl_deriv.h>
#include <gsl/gsl_heapsort.h>
#include <gsl/gsl_integration.h>
#include <gsl/gsl_interp.h>
#include <gsl/gsl_linalg.h>
#include <gsl/gsl_math.h>
#include <gsl/gsl_matrix.h>
#include <gsl/gsl_min.h>
#include <gsl/gsl_multifit_nlin.h>
#include <gsl/gsl_multimin.h>
#include <gsl/gsl_multiroots.h>
#include <gsl/gsl_qrng.h>
#include <gsl/gsl_randist.h>
#include <gsl/gsl_rng.h>
#include <gsl/gsl_roots.h>
#include <gsl/gsl_spline.h>
#include <gsl/gsl_sf.h>
#include <gsl/gsl_statistics.h>
#include <gsl/gsl_sort_double.h>
#include <gsl/gsl_sort_vector_double.h>
#include <gsl/gsl_vector.h>
#include <gsl/gsl_version.h>

#if HAVE_IDA
 #include <ida/ida.h>
 #include <ida/ida_direct.h>
 #include <nvector/nvector_serial.h>
 #include <sundials/sundials_types.h>
 #include <sundials/sundials_math.h>
 #include <sunmatrix/sunmatrix_dense.h>
 #include <sunlinsol/sunlinsol_dense.h>
#endif


#include "contrib/uthash.h"
#include "contrib/utlist.h"
#include "contrib/kdtree.h"


#define FEENOX_OK         0
#define FEENOX_ERROR      1
#define FEENOX_UNHANDLED  2

#define BUFFER_TOKEN_SIZE        255
#define BUFFER_LINE_SIZE        4095


// no son enums porque hacemos operaciones con las mascaras de abajo
#define EXPR_UNDEFINED                            0 
#define EXPR_OPERATOR                             1
#define EXPR_CONSTANT                             2
#define EXPR_VARIABLE                             3
#define EXPR_VECTOR                               4
#define EXPR_MATRIX                               5
#define EXPR_BUILTIN_FUNCTION                     6
#define EXPR_BUILTIN_VECTORFUNCTION               7
#define EXPR_BUILTIN_FUNCTIONAL                   8
#define EXPR_FUNCTION                             9

#define EXPR_BASICTYPE_MASK                    1023
#define EXPR_CURRENT                              0
#define EXPR_INITIAL_TRANSIENT                 2048
#define EXPR_INITIAL_STATIC                    4096

// constants for custom error handling
#define ON_ERROR_NO_QUIT            1
#define ON_ERROR_NO_REPORT          2


// type of phase space componentes (the values come from SUNDIALs, do not change!)
#define DAE_ALGEBRAIC                      0.0
#define DAE_DIFFERENTIAL                   1.0


#define STEP_ALL                           0
#define STEP_BEFORE_DAE                    1
#define STEP_AFTER_DAE                     3


// reasonable defaults
#define DEFAULT_DT                         1.0/16.0
#define DEFAULT_REL_ERROR                  1e-6
#define DEFAULT_RANDOM_METHOD              gsl_rng_mt19937

#define DEFAULT_PRINT_FORMAT               "%g"
#define DEFAULT_PRINT_SEPARATOR            "\t"

#define DEFAULT_ROOT_MAX_TER               1024
#define DEFAULT_ROOT_TOLERANCE             (9.765625e-4)         // (1/2)^-10

#define DEFAULT_INTEGRATION_INTERVALS      1024
#define DEFAULT_INTEGRATION_TOLERANCE      (9.765625e-4)         // (1/2)^-10
#define DEFAULT_INTEGRATION_KEY            GSL_INTEG_GAUSS31

#define DEFAULT_DERIVATIVE_STEP            (9.765625e-4)         // (1/2)^-10

#define MINMAX_ARGS       10


// zero & infinite
#define ZERO          (8.881784197001252323389053344727e-16)  // (1/2)^-50
#define INFTY         (1125899906842624.0)                    // 2^50

#define MESH_INF 1e22
#define MESH_TOL 1e-6
#define MESH_FAILED_INTERPOLATION_FACTOR -1


// usamos los de gmsh, convertimos a vtk y frd con tablas
#define ELEMENT_TYPE_UNDEFINED      0
#define ELEMENT_TYPE_LINE2          1
#define ELEMENT_TYPE_TRIANGLE3      2
#define ELEMENT_TYPE_QUADRANGLE4    3
#define ELEMENT_TYPE_TETRAHEDRON4   4
#define ELEMENT_TYPE_HEXAHEDRON8    5
#define ELEMENT_TYPE_PRISM6         6
#define ELEMENT_TYPE_PYRAMID5       7
#define ELEMENT_TYPE_LINE3          8
#define ELEMENT_TYPE_TRIANGLE6      9
#define ELEMENT_TYPE_QUADRANGLE9    10
#define ELEMENT_TYPE_TETRAHEDRON10  11
#define ELEMENT_TYPE_HEXAHEDRON27   12 
#define ELEMENT_TYPE_POINT1         15
#define ELEMENT_TYPE_QUADRANGLE8    16
#define ELEMENT_TYPE_HEXAHEDRON20   17
#define ELEMENT_TYPE_PRISM15        18
#define NUMBER_ELEMENT_TYPE         19

#define M_SQRT5 2.23606797749978969640917366873127623544061835961152572427089


enum version_type {
  version_compact,
  version_copyright,
  version_info
};

// macro to check error returns in function calls
#define feenox_call(function)   if ((function) != FEENOX_OK) return FEENOX_ERROR
#ifdef HAVE_IDA
#define ida_call(function)      if ((err = function) < 0) { feenox_push_error_message("IDA returned error %d", err); return FEENOX_ERROR; }
#endif

// macro to check malloc() for NULL (ass means assignement, you ass!)
#define feenox_check_alloc(ass)         if ((ass) == NULL) { feenox_push_error_message("cannot allocate memory"); return FEENOX_ERROR; }
#define feenox_check_alloc_null(ass)    if ((ass) == NULL) { feenox_push_error_message("cannot allocate memory"); return NULL; }
#define feenox_check_minusone_null(ass) if ((ass) == -1) { feenox_push_error_message("cannot allocate memory"); return NULL; }

// macro to access internal special variables
#define feenox_special_var(var) (feenox.special_vars.var)

// value of a variable
#define feenox_var_value(var)   (*(var->value))

// value of a special variable
#define feenox_special_var_value(var)  feenox_var_value(feenox_special_var(var))

// pointer to the content of an object
#define feenox_value_ptr(obj)  (obj->value)



// number of internal functions, functional adn vector functions
#define N_BUILTIN_FUNCTIONS         53
#define N_BUILTIN_FUNCTIONALS       8
#define N_BUILTIN_VECTOR_FUNCTIONS  8



// forward declarations
typedef struct feenox_t feenox_t;
typedef struct builtin_function_t builtin_function_t;
typedef struct builtin_functional_t builtin_functional_t;
typedef struct builtin_vectorfunction_t builtin_vectorfunction_t;

typedef struct expr_t expr_t;
typedef struct expr_item_t expr_item_t;

typedef struct var_t var_t;
typedef struct vector_t vector_t;
typedef struct matrix_t matrix_t;
typedef struct function_t function_t;

typedef struct alias_t alias_t;

typedef struct instruction_t instruction_t;
typedef struct conditional_block_t conditional_block_t;

typedef struct assignment_t assignment_t;

typedef struct file_t file_t;
typedef struct print_t print_t;
typedef struct print_token_t print_token_t;
typedef struct print_function_t print_function_t;
typedef struct multidim_range_t multidim_range_t;

typedef struct sort_vector_t sort_vector_t;
typedef struct phase_object_t phase_object_t;
typedef struct dae_t dae_t;

typedef struct mesh_t mesh_t;
typedef struct physical_group_t physical_group_t;
typedef struct geometrical_entity_t geometrical_entity_t;
typedef struct physical_name_t physical_name_t;
typedef struct physical_property_t physical_property_t;
typedef struct property_data_t property_data_t;
typedef struct material_t material_t;
typedef struct material_list_item_t material_list_item_t;

typedef struct node_t node_t;
typedef struct node_relative_t node_relative_t;
typedef struct element_t element_t;
typedef struct element_list_item_t element_list_item_t;
typedef struct element_type_t element_type_t;
typedef struct cell_t cell_t;
typedef struct neighbor_t neighbor_t;
typedef struct gauss_t gauss_t;

typedef struct elementary_entity_t elementary_entity_t;
typedef struct bc_t bc_t;
typedef struct node_data_t node_data_t;

/*
typedef struct mesh_post_t mesh_post_t;
typedef struct mesh_post_dist_t mesh_post_dist_t;
typedef struct mesh_fill_vector_t mesh_fill_vector_t;
typedef struct mesh_find_minmax_t mesh_find_minmax_t;
typedef struct mesh_integrate_t mesh_integrate_t;
*/

// individual factor of an algebraic expression
struct expr_item_t {
  size_t n_chars;
  int type;        // defines #EXPR_ because we need to operate with masks
  
  size_t level;       // hierarchical level
  size_t tmp_level;   // for partial sums

  size_t oper;        // number of the operator if applicable
  double sign;
  double constant; // value of the numerical constant if applicable
  double value;    // current value
  
// vector with (optional) auxiliary stuff (last value, integral accumulator, rng, etc)
  double *aux;

  builtin_function_t *builtin_function;
  builtin_vectorfunction_t *builtin_vectorfunction;
  builtin_functional_t *builtin_functional;

  var_t *variable;
  vector_t *vector;
  matrix_t *matrix;
  function_t *function;

  vector_t **vector_arg;

  var_t *functional_var_arg;

  // algebraic expression of the arguments of the function
  expr_t *arg;

  expr_item_t *next;
};


// algebraic expression
struct expr_t {
  expr_item_t *items;
  double value;
  char *string;     // just in case we keep the original string
  expr_t *next;
};



// variable
struct var_t {
  char *name;
  int initialized;
  int reallocated;
  
  // these are pointers so the actual holder can be anything, from another
  // variable (i.e. an alias), a vector element (another alias) or even
  // more weird stufff (a fortran common?)
  double *value;
  double *initial_static;
  double *initial_transient;

  UT_hash_handle hh;
};


// vector
struct vector_t {
  char *name;
  int initialized;
  
  expr_t size_expr;
  int size;
  int constant;

  gsl_vector *value;
  gsl_vector *initial_transient;
  gsl_vector *initial_static;

  // flag to know if the pointer above is owned by us or by anybody else
  int reallocated;
  
  // pointer to a function where we should get the data from
  function_t *function;
  
  // linked list with the expressions of the initial elements
  expr_t *datas;

  UT_hash_handle hh;
};


// matrix
struct matrix_t {
  char *name;
  int initialized;  
  expr_t cols_expr;
  expr_t rows_expr;
  
  int cols;
  int rows;
  int constant;
  
  gsl_matrix *value;
  gsl_matrix *initial_transient;
  gsl_matrix *initial_static;

  // flag para saber si el apuntador de arriba lo alocamos nosotros o alguien mas
  int realloced;
  
  expr_t *datas;
  
  UT_hash_handle hh;
};

// alias
struct alias_t {
  int initialized;
  
  var_t *new_variable;
  
  expr_t row;
  expr_t col;
  
  var_t *variable;
  vector_t *vector;
  matrix_t *matrix;
  
  alias_t *next;
};

// function
struct function_t {
  char *name;
  char *name_in_mesh;
  int initialized;
  
  // might be:
  //   - algebraic
  //   - pointwise-defined
  //       + given in the input
  //       + given in a file
  //       + given in wasora vectors
  //       + in a mesh
  //          * as another function defined over materials (physical groups)
  //          * data at nodes
  //          * data at cells
  //  - comptued in a user-provided routine
  //
  enum  {
    function_type_undefined,
    function_type_algebraic,
    function_type_pointwise_data,
    function_type_pointwise_file,
    function_type_pointwise_vector,
    function_type_pointwise_mesh_property,
    function_type_pointwise_mesh_node,
    function_type_pointwise_mesh_cell,
    function_type_routine,
    function_type_routine_internal,
  } type;
  
  // number of arguments the function takes
  int n_arguments;
  int n_arguments_given;  // check to see if the API gave us all the arguments the function needs

  // array of pointers to already-existing variables for the arguments
  var_t **var_argument;
  int var_argument_allocated;


  // expression for algebraic functions
  expr_t algebraic_expression;

  // number of pairs of x-y data for pointwise functions
  size_t data_size;

  // arrays with the adata 
  double **data_argument;
  int data_argument_allocated;
  double *data_value;
  
  // this is in case there is a derivative of a mesh-based function and
  // we want to interoplate with the shape functions
  // warning! we need to put data on the original function and not on the derivatives
  function_t *spatial_derivative_of;
  int spatial_derivative_with_respect_to;

  // hints in case the grid is tensor-product regular
  int rectangular_mesh;
  expr_t expr_x_increases_first;
  int x_increases_first;
  expr_t *expr_rectangular_mesh_size;
  int *rectangular_mesh_size;
  double **rectangular_mesh_point;

  // file with the point-wise data
  char *data_file_path;

  // columns where the data is within the file
  // size = n_arguments+1 (n independent variables + 1 dependent variable) 
  int *column;

  // vectors with the point-wise data already read
  vector_t **vector_argument;
  vector_t *vector_value;
  
  // helpers to interpolate 1D with GSL
  gsl_interp *interp;
  gsl_interp_accel *interp_accel;
  gsl_interp_type interp_type;

  // multidimensional interpolation type
  enum {
    interp_nearest,
    interp_shepard,
    interp_shepard_kd,
    interp_bilinear
  } multidim_interp;

  
  expr_t expr_multidim_threshold;
  double multidim_threshold;
  expr_t expr_shepard_radius;
  double shepard_radius;
  expr_t expr_shepard_exponent;
  double shepard_exponent;

  // propiedad
  void *property;

  // malla no-estructurada sobre la que esta definida la funcion
  mesh_t *mesh;
  double mesh_time;
  
  // apuntador a un arbol k-dimensional para nearest neighbors 
  void *kd;
  
  // ----- ------- -----------   --        -       - 
  // funcion que hay que llamar para funciones tipo usercall 
  double (*routine)(const double *);
  
  // ----- ------- -----------   --        -       - 
  // funcion que hay que llamar para funciones internas
  double (*routine_internal)(const double *, function_t *);
  void *params;

  UT_hash_handle hh;
};


// internal function (this is initialized directly into the data space from builtin_functions.h)
struct builtin_function_t {
  char name[BUFFER_TOKEN_SIZE];
  int min_arguments;
  int max_arguments;
  double (*routine)(struct expr_item_t *);
};


// internal function over vectors
struct builtin_vectorfunction_t {
  char name[BUFFER_TOKEN_SIZE];
  int min_arguments;
  int max_arguments;
  double (*routine)(struct vector_t **);
};


// internal functional
// a functional is _like_ a regular function but its second argument is a variable
// (not an expression) that is used to evaluate the first argument (which is an expression
// of this variable) for different values and do something about it, for instance
// compute a derivative or an integral.
// There can also be more optional arguments (expressions) after this second special argument,
// i.e. integral(1+x,x,0,1) or derivative(1+x,x,0.5)
struct builtin_functional_t {
  char name[BUFFER_TOKEN_SIZE];
  int min_arguments; // contando la variable del segundo argumento
  int max_arguments;
  double (*routine)(struct expr_item_t *, struct var_t *);
};


// instruction
struct instruction_t {
  int (*routine)(void *);
  void *argument;
  int argument_alloced;

  instruction_t *next;
};


// conditional block
struct conditional_block_t {
  conditional_block_t *father;
  conditional_block_t *else_of;
  
  expr_t condition;
  
  instruction_t *first_true_instruction;
  instruction_t *first_false_instruction;
  
  int evaluated_to_true;
  int evaluated_to_false;
  
  conditional_block_t *next;
};


// igualacion algebraica 
struct  assignment_t {
  int initialized;
  
  var_t *variable;
  vector_t *vector;
  matrix_t *matrix;
  
  // expresiones que indican el rango de tiempo
  expr_t t_min;
  expr_t t_max;

  // expresiones que indican el rango de indices para asignaciones vectoriales/matriciales
  expr_t i_min;
  expr_t i_max;
  expr_t j_min;
  expr_t j_max;

  // expresiones que indican la fila y la columna en funcion de i y de j
  expr_t row;
  expr_t col;
  
  // to sum up:
  // a(row, col)<i_min,i_max;j_min,j_max>[t_min,tmax]
  
  // la expresion del miembro derecho
  expr_t rhs;
  
  // flag que indica si el assignment pide _init o _0
  int initial_static;
  int initial_transient;
  
  // si este es true entonces la asignacion es una sola (aun cuando el miembro
  // izquierdo sea un vector o matriz), i.e.
  // A(127,43)  es escalar
  // A(2*i+1,14) no es escalar
  int scalar;

  // si esto es true, entonces las asignaciones vector o matriz son plain, i.e.
  // A(i,j)
  int plain;
  
  // diferenciamos, en caso de las matrices, aquellas dependencias solo en i o en j
  // A(i,4) = 5 es un assignment que depende solo de i
  int expression_only_of_i;
  
  // A(8,j) = 1 es un assignment que depende solo de j
  int expression_only_of_j;
  
  
  assignment_t *next;

};



// a file, either input or output
struct file_t {
  char *name;
  int initialized;
  
  char *format;
  int n_format_args;
  int n_format_args_given;
  expr_t *arg;
  char *mode;

  char *path;
  FILE *pointer;

  UT_hash_handle hh;
};


// print a single line
struct print_t {
  // pointer to the output file (if null, print to stdout)
  file_t *file;

  // a linked list with the stuff to print
  print_token_t *tokens;
 
  // token separator, by default it is a tab "\t" 
  char *separator;

  // flag to indicate if we need to send the newline char "\n" at the end
  // it's called nonewline so the default is zero and the "\n" is sent
  int nonewline;

  // stuff to help with the SKIP_*
  int last_static_step;
  int last_step;
  double last_time;

  expr_t skip_static_step;
  expr_t skip_step;
  expr_t skip_time;

  int header;
  expr_t skip_header_step;
  int last_header_step;
  int header_already_printed;
  
  print_t *next;
};


// an individual token to be PRINTed
struct print_token_t {
  char *format;
  char *text;
  expr_t expression;
  
  vector_t *vector;
  matrix_t *matrix;
  function_t *function;
  
  print_token_t *next;
};

struct multidim_range_t {
  int dimensions;

  expr_t *min;
  expr_t *max;
  expr_t *step;
  expr_t *nsteps;
};


// print one or more functions
struct print_function_t {
  file_t *file;

  // linked list with the stuff to be printed
  print_token_t *tokens;

  // pointer to the first function (the one that defines the number of arguments
  // note that this might not be the same as as token->function, that can be NULL
  function_t *first_function;

  // explicit range to print the function
  multidim_range_t range;
  
  // mesh and physical group that tells where to print a function
  mesh_t *mesh;
  physical_group_t *physical_group;

  // flag to add a header explaining what the columns are
  int header;
  // formato de los numeritos "%e"
  char *format;
  // separador de cosas "\t"
  char *separator;

  print_function_t *next;

};


struct sort_vector_t {
  int descending;
  
  vector_t *v1;
  vector_t *v2;
};


struct phase_object_t {
  int offset;
  int size;
  int differential;
  char *name;

  var_t *variable;
  var_t *variable_dot;
  vector_t *vector;
  vector_t *vector_dot;
  matrix_t *matrix;
  matrix_t *matrix_dot;

  phase_object_t *next;
};

struct dae_t {
  expr_t residual;
  
  vector_t *vector;
  matrix_t *matrix;
  
  expr_t expr_i_min;
  expr_t expr_i_max;
  expr_t expr_j_min;
  expr_t expr_j_max;
  
  int i_min;
  int i_max;
  int j_min;
  int j_max;
  
  int equation_type;
  
  dae_t *next;
};



// mesh-realted structs -----------------------------
// node
struct node_t {
  size_t tag;               // number assigned by Gmsh
  size_t index_mesh;        // index within the node array

  double x[3];              // spatial coordinates of the node
  size_t *index_dof;        // index within the solution vector for each DOF
  
  double *phi;              // values of the solution functions at the node
  gsl_matrix *dphidx;       // derivative of the m-th DOF with respect to coordinate g
                            // (this is a gsl_matrix to avoid having to do double mallocs and forgetting about row/col-major
  gsl_matrix *delta_dphidx; // same as above but for the standard deviations of the derivatives
  double *f;                // holder of arbitrary functions evaluated at the node (sigmas and taus)
  
  element_list_item_t *associated_elements;
};


struct node_data_t {
  char *name_in_mesh;
  function_t *function;
  
  node_data_t *next;
};

struct node_relative_t {
  size_t index;
  node_relative_t *next;
};


struct physical_group_t {
  char *name;
  size_t tag;
  unsigned int dimension;
  
  material_t *material;    // apuntador
  bc_t *bcs;               // linked list 
  // TODO: pointer to bc like material?
//  bc_t *bc;
     
  // volume (or area or length depending on the dim, sometimes called mass)
  double volume;
  double cog[3];
  
  var_t *var_vol;
  vector_t *vector_cog;
  
  // a linked list seems to be too expensive
  size_t n_elements;
  size_t i_element;
  size_t *element;
  
  UT_hash_handle hh;
  UT_hash_handle hh_tag[4];
};


struct geometrical_entity_t {
  size_t tag;
  double boxMinX, boxMinY, boxMinZ, boxMaxX, boxMaxY, boxMaxZ;
  
  size_t num_physicals;
  size_t *physical;
  size_t num_bounding;
  size_t *bounding;

  UT_hash_handle hh[4];
};

struct elementary_entity_t {
  size_t id;
  elementary_entity_t *next;
};



struct gauss_t {
  int V;               // number of points (v=1,2,...,V )
  double *w;           // weights (w[v] is the weight of the v-th point)
  double **r;          // coordinates (r[v][m] is the coordinate of the v-th point in dimension m)
  
  double **h;          // shape functions evaluated at the gauss points h[v][j]
  gsl_matrix **dhdr;   // derivatives dhdr[v](j,m)
  
  gsl_matrix *extrap;  // matrix to extrapolate the values from the gauss points to the nodes
};


// constant structure with the types of elements
// this includes the pointers to the shape functions
// the numbering is Gmsh-based
struct element_type_t {
  char *name;

  size_t id;              // as of Gmsh
  size_t dim;
  size_t order;
  unsigned int nodes;           // total, i.e. 10 for tet10
  unsigned int vertices;        // the corner nodes, i.e 4 for tet10
  unsigned int faces;           // facess == number of neighbors
  unsigned int nodes_per_face;  // (max) number of nodes per face

  double *barycenter_coords;
  double **node_coords;
  node_relative_t **node_parents;
  
  // shape functions and derivatives in the local coords
  double (*h)(int i, double *r);
  double (*dhdr)(int i, int j, double *r);

  // virtual (sic) methods!
  int (*point_in_element)(element_t *e, const double *x);
  double (*element_volume)(element_t *e);
  
  gauss_t gauss[2];    // sets of gauss points
                       // 0 - full integration
                       // 1 - reduced integration
};


struct element_t {
  size_t index;
  size_t tag;
  
  double quality;
  double volume;
  double weight;   // this weight is used to average the contribution of this element to nodal gradients
  double *w;       // weights of the gauss points
  double **x;      // coordinates fo the gauss points 
  
  // these matrices are evalauted at the gauss points
  gsl_matrix **dhdx;
  gsl_matrix **dxdr;
  gsl_matrix **drdx;
  gsl_matrix **H;
  gsl_matrix **B;
  
  // these are the number of gauss points currently being used
  // if this number changes, everything needs to be re-computed
  // for instance sub-integration might be used for building matrices
  // but the canonical set of gauss points might be needed to recover stresses  
  // we need one size for each of the seven objects above because we need
  // to change them individually otherwise the first wins and the others loose
  size_t V_w, V_x, V_H, V_B, V_dxdr, V_drdx, V_dhdx;    

  
  size_t *l;  // node-major-ordered vector with the global indexes of the DOFs in the element

  
  gsl_matrix **dphidx_gauss;  // spatial derivatives of the DOFs at the gauss points
  gsl_matrix **dphidx_node;   // spatial derivatives of the DOFs at the nodes (extrapoladed or evaluated)
  double **property_node;

  element_type_t *type;                  // pointer to the element type
  physical_group_t *physical_group;      // pointer to the physical group this element belongs to
  node_t **node;                         // pointer to the nodes, node[j] points to the j-th local node
  cell_t *cell;                          // pointer to the associated cell (only for FVM)
};


struct element_list_item_t {
  element_t *element;
  element_list_item_t *next;
};

struct cell_t {
  size_t id;
  
  element_t *element;
  
  unsigned int n_neighbors;
  size_t *ineighbor;      // array with ids of neighboring elements
  size_t **ifaces;        // array of arrays of ids making up the faces
  
  neighbor_t *neighbor;   // array of neighbors

  double x[3];            // coordenadas espaciales del baricentro de la celda
  size_t *index;          // indice del vector incognita para cada uno de los grados de libertad

  double volume;

};

struct neighbor_t {
  cell_t *cell;
  element_t *element;
 
  double **face_coord;
  double x_ij[3];
  double n_ij[3];
  double S_ij;
  
  // distancia entre los centro de la cara y el centro de la celda
  // (cache para mallas estructuradas)
//  double di;     // celda principal
//  double dj;     // celda vecina  
  
};

struct material_t {
  char *name;
  mesh_t *mesh;
  property_data_t *property_datums;

  // este es un apuntador generico que le dejamos a alguien
  // (plugins) por si necesitan agregar informacion al material
  void *ext;

  UT_hash_handle hh;
};

struct material_list_item_t {
  material_t *material;
  material_list_item_t *next;
};
  
struct physical_property_t {
  char *name;
  property_data_t *property_datums;

  UT_hash_handle hh;
};

struct property_data_t {
  physical_property_t *property;
  material_t *material;
  expr_t expr;

  UT_hash_handle hh;
};


// unstructured mesh
struct mesh_t {
  file_t *file;
  unsigned int dim;
  unsigned int dim_topo;

  size_t n_nodes;
  size_t n_elements;
  size_t n_cells; // a cell is an element with the topological dimension of the mesh

  int degrees_of_freedom;        // per unknown
  unsigned int order;

  physical_group_t *physical_groups;              // global hash table
  physical_group_t *physical_groups_by_tag[4];    // 4 hash tablesm one per tag
  unsigned int physical_tag_max;   // the higher tag of the entities
  
  // number of geometric entities of each dimension
  unsigned points, curves, surfaces, volumes;
  geometrical_entity_t *geometrical_entities[4];     // 4 hash tables, one for each dimension

  int sparse;                     // flag that indicates if the nodes are sparse
  size_t *tag2index;     // array to map tags to indexes (we need a -1)
  
  enum {
    data_type_element,
    data_type_node,
  } data_type;
  
  enum {
    integration_full,
    integration_reduced
  } integration;
  
  unsigned int update_each_step;
  
  expr_t scale_factor;           // factor de escala al leer la posicion de los nodos
  expr_t offset_x;               // offset en nodos
  expr_t offset_y;               // offset en nodos
  expr_t offset_z;               // offset en nodos
    
  double **nodes_argument;
  double **cells_argument;

  node_data_t *node_datas;
  
  unsigned int n_physical_names;
  node_t *node;
  element_t *element;
  cell_t *cell;
  
  node_t bounding_box_max;
  node_t bounding_box_min;
  
  unsigned int max_nodes_per_element;
  unsigned int max_faces_per_element;
  unsigned int max_first_neighbor_nodes;  // to estimate matrix bandwith

  // kd-trees to make efficient searches
  void *kd_nodes;
  void *kd_cells;

  element_t *last_chosen_element;     // interpolation cache

  // "virtual method" for format-dependet reader
  int (*reader)(mesh_t *this);

  int initialized;

  UT_hash_handle hh;

};

// global FeenoX singleton structure
struct feenox_t {
  int argc;
  char **argv;
  int optind;

  char *main_input_filepath;
  char *main_input_dirname;
  
  enum {
    mode_normal,
    mode_parametric,
    mode_optimize,
    mode_fit,
    mode_list_vars
  } mode;
  
  // pointer to the run function (normal, parametric, optimize, fit, etc)
  int (*run)(void);
  
  int debug;
  
  int rank;            // in serial or without petsc, this is always zero
  int nprocs;

  char **error;
  int error_level;
  
  // if this is not null then a conditional is wanting us to branch
  instruction_t *next_instruction;
  
  expr_t *time_paths;
  expr_t *time_path_current;
  double next_time;

  
  instruction_t *instructions;
  instruction_t *last_defined_instruction;  // TODO: see if we can put this somewhere else
  
  conditional_block_t *conditional_blocks;
  conditional_block_t *active_conditional_block;
  
  var_t *vars;
  vector_t *vectors;
  matrix_t *matrices;
  alias_t *aliases;
  function_t *functions;
  
  assignment_t *assignments;
  file_t *files;
  print_t *prints;
  print_function_t *print_functions;
  
  struct {
    var_t *done;
    var_t *done_static;
    var_t *done_transient;
    var_t *done_outer;
  
    var_t *step_outer;
    var_t *step_static;
    var_t *step_transient;

    var_t *in_outer_initial;
    var_t *in_static;
    var_t *in_static_first;
    var_t *in_static_last;
    var_t *in_transient;
    var_t *in_transient_first;
    var_t *in_transient_last;

    var_t *static_steps;
    var_t *end_time;

    var_t *t;
    var_t *dt;

    var_t *rel_error;
    var_t *min_dt;
    var_t *max_dt;
  
    var_t *i;
    var_t *j;
    
    var_t *pi;
    var_t *zero;
    var_t *infinite;

    var_t *ncores;
    var_t *pid;
    
    var_t *on_nan;
    var_t *on_gsl_error;
    var_t *on_ida_error;
    var_t *realtime_scale;
  } special_vars;


  struct {
    vector_t *abs_error;
  } special_vectors;

  
  struct {
//    file_t *stdin_;
    file_t *stdout_;
//    file_t *stderr_;
  } special_files;  

  struct {

    //int initialized;
    int need_cells;

    mesh_t *meshes;
    mesh_t *mesh_main;

    // flag que el codigo partcular rellena (preferentemente en init_before_parser)
    // para indicar si el default es trabajar sobre celdas (FVM) o sobre nodos (FEM)
//    centering_t default_centering;

    // estas tres variables estan reallocadas para apuntar a vec_x
    struct {
      var_t *x;
      var_t *y;
      var_t *z;
      var_t *arr_x[3];   // x, y y z en un array de tamanio 3
      vector_t *vec_x;

      var_t *nx;
      var_t *ny;
      var_t *nz;
      var_t *arr_n[3];
      vector_t *vec_n;

      var_t *eps;

      var_t *nodes;
      var_t *cells;
      var_t *elements;

      vector_t *bbox_min;
      vector_t *bbox_max;

      var_t *mesh_failed_interpolation_factor;
    } vars;

    element_type_t *element_types;

    material_t *materials;
    physical_property_t *physical_properties;
    
/*    
    mesh_write_t *writes;
    fill_mesh_vector_t *fill_vectors;
    find_extremum_t *find_minmaxs;
    integrate_t *integrates;
*/
  } mesh;  
  
  struct {
    int dimension;
    int reading_daes;
    double **phase_value;
    double **phase_derivative;
    phase_object_t *phase_objects;
  
    dae_t *daes;
    void *system;
    

    enum {
      initial_conditions_as_provided,
      initial_conditions_from_variables,
      initial_conditions_from_derivatives,
    } initial_conditions_mode;

    instruction_t *instruction;
    
#if HAVE_IDA
    N_Vector x;
    N_Vector dxdt;
    N_Vector id;
    N_Vector abs_error;
    SUNMatrix A;
    SUNLinearSolver LS;
#endif
  } dae;
  
};


// function declarations

// run_standard.c
int feenox_run_standard(void);

// step.c
int feenox_step(int whence);

// init.c
extern int feenox_initialize(int argc, char **argv);
extern int feenox_init_special_objects(void);
extern int feenox_init_after_parser(void);
extern int feenox_init_before_run(void);

// mesh's init.c
extern int feenox_mesh_init_special_objects(void);

// version.c
extern void feenox_show_help(const char *progname);
extern void feenox_show_version(int version);
extern void feenox_copyright(void);
extern void feenox_shortversion(void);
extern void feenox_longversion(void);


// error.c
void feenox_push_error_message(const char *fmt, ...);
void feenox_pop_error_message(void);
void feenox_pop_errors(void);
void feenox_runtime_error(void);
void feenox_nan_error(void);
void feenox_gsl_handler (const char *reason, const char *file_ptr, int line, int gsl_errno);
void feenox_signal_handler(int sig_num);


// cleanup.c
void feenox_polite_exit(int error);
void feenox_finalize(void);


// parser.c
extern int feenox_parse_main_input_file(const char *filepath);
extern char *feenox_ends_in_init(const char *name);
extern char *feenox_ends_in_zero(const char *name);
extern int feenox_count_arguments(char *string, size_t *);
extern int feenox_read_arguments(char *string, unsigned int n_arguments, char ***arg, size_t *n_chars);


// file.c
extern int feenox_instruction_file(void *arg);
FILE *feenox_fopen(const char *filepath, const char *mode);
extern int feenox_instruction_file_open(void *arg);
extern int feenox_instruction_file_close(void *arg);

// abort.c
extern int feenox_instruction_abort(void *arg);


// expressions.c
extern int feenox_expression_parse(expr_t *this, const char *string);
extern expr_item_t *feenox_expression_parse_item(const char *string);
extern int feenox_parse_range(char *string, const char left_delim, const char middle_delim, const char right_delim, expr_t *a, expr_t *b);

extern double feenox_expression_eval(expr_t *this);
extern double feenox_expression_evaluate_in_string(const char *string);

// dae.c
extern int feenox_add_time_path(const char *token);
extern int feenox_phase_space_add_object(const char *token);
extern int feenox_phase_space_mark_diff(const char *string);
extern char *feenox_find_first_dot(const char *s);
extern int feenox_add_dae(const char *lhs, const char *rhs);
extern int feenox_dae_init(void);
extern int feenox_dae_ic(void);
#ifdef HAVE_IDA
extern int feenox_ida_dae(realtype t, N_Vector yy, N_Vector yp, N_Vector rr, void *params);
#else
extern int feenox_ida_dae(void);
#endif
extern int feenox_instruction_dae(void *arg);

// instruction.c
extern instruction_t *feenox_add_instruction_and_get_ptr(int (*routine)(void *), void *argument);
extern int feenox_add_instruction(int (*routine)(void *), void *argument);


// define.c
extern int feenox_check_name(const char *name);
extern int feenox_define_variable(const char *name);
extern int feenox_define_alias(const char *new_name, const char *existing_object, const char *row, const char *col);

extern int feenox_define_vector(const char *name, const char *size);
extern int feenox_vector_attach_function(const char *name, const char *function_data);
extern int feenox_vector_attach_data(const char *name, expr_t *datas);

extern int feenox_define_matrix(const char *name, const char *rows, const char *cols);
extern int feenox_matrix_attach_data(const char *name, expr_t *datas);

extern int feenox_define_function(const char *name, unsigned int n_arguments);
extern function_t *feenox_define_function_get_ptr(const char *name, unsigned int n_arguments);
extern int feenox_function_set_argument_variable(const char *name, unsigned int i, const char *variable_name);
extern int feenox_function_set_expression(const char *name, const char *expression);

extern int feenox_define_file(const char *name, const char *format, unsigned int n_args, const char *mode);
extern int feenox_file_set_path_argument(const char *name, unsigned int i, const char *expression);
extern file_t *feenox_get_or_define_file_get_ptr(const char *name);

extern var_t *feenox_get_or_define_variable_get_ptr(const char *name);
extern var_t *feenox_define_variable_get_ptr(const char *name);

extern vector_t *feenox_define_vector_get_ptr(const char *name, size_t size);

extern void feenox_realloc_variable_ptr(var_t *this, double *newptr, int copy_contents);
extern void feenox_realloc_vector_ptr(vector_t *this, double *newptr, int copy_contents);

// getptr.c
extern var_t *feenox_get_variable_ptr(const char *name);
extern vector_t *feenox_get_vector_ptr(const char *name);
extern matrix_t *feenox_get_matrix_ptr(const char *name);
extern function_t *feenox_get_function_ptr(const char *name);
extern builtin_function_t *feenox_get_builtin_function_ptr(const char *name);
extern builtin_vectorfunction_t *feenox_get_builtin_vectorfunction_ptr(const char *name);
extern builtin_functional_t *feenox_get_builtin_functional_ptr(const char *name);

extern file_t *feenox_get_file_ptr(const char *name);
extern mesh_t *feenox_get_mesh_ptr(const char *name);
extern material_t *feenox_get_material_ptr(const char *name);
extern physical_group_t *feenox_get_physical_group_ptr(const char *name, mesh_t *mesh);


// assignment.c
extern int feenox_add_assignment(char *left_hand, char *right_hand);
extern int feenox_assign_single(assignment_t *assignment, int row, int col);
extern int feenox_instruction_assignment_scalar(void *arg);
extern int feenox_instruction_assignment_vector(void *arg);
extern int feenox_instruction_assignment_matrix(void *arg);

// auxiliary.c
extern int feenox_strip_blanks(char *string);

// alias.c
extern int feenox_instruction_alias(void *arg);

// matrix.c
extern int feenox_matrix_init(matrix_t *this);

// vector.c
extern int feenox_vector_init(vector_t *this);
extern int feenox_instruction_sort_vector(void *arg);
extern double feenox_vector_get(vector_t *this, const size_t i);
extern double feenox_vector_get_initial_static(vector_t *this, const size_t i);
extern double feenox_vector_get_initial_transient(vector_t *this, const size_t i);


// function.c
extern int feenox_function_init(function_t *this);
extern void feenox_set_function_args(function_t *this, double *x);
extern double feenox_function_eval(function_t *this, const double *x);
extern double feenox_factor_function_eval(expr_item_t *this);

// print.c
extern int feenox_instruction_print(void *arg);
extern int feenox_instruction_print_function(void *arg);

// conditional.c
extern int feenox_instruction_if(void *arg);
extern int feenox_instruction_else(void *arg);
extern int feenox_instruction_endif(void *arg);

// functionals.c
extern double feenox_gsl_function(double x, void *params);

// mesh.c
extern int feenox_instruction_mesh_read(void *arg);

extern int feenox_mesh_free(mesh_t *this);
extern int feenox_mesh_read_gmsh(mesh_t *this);
extern int feenox_mesh_read_vtk(mesh_t *this);
extern int feenox_mesh_read_frd(mesh_t *this);

// physical_group.c
extern int feenox_define_physical_group(const char *name, const char *mesh_name, unsigned int dimension, size_t tag);
extern physical_group_t *feenox_define_physical_group_get_ptr(const char *name, mesh_t *mesh, unsigned int dimension, size_t tag);

extern material_t *feenox_define_material_get_ptr(const char *name);

extern int feenox_define_property_data(const char *material_name, const char *property_name, const char *expr_string);
extern property_data_t *feenox_define_property_data_get_ptr(const char *material_name, const char *property_name, const char *expr_string);

extern int feenox_define_physical_property(const char *name, const char *mesh_name);
extern physical_property_t *feenox_define_physical_property_get_ptr(const char *name, mesh_t *mesh);


// init.c
int feenox_mesh_element_types_init(void);

// geom.c
// TODO: rename mesh_* -> feenox_mesh_*
extern void feenox_mesh_subtract(const double *a, const double *b, double *c);
extern void feenox_mesh_cross(const double *a, const double *b, double *c);
extern void feenox_mesh_normalized_cross(const double *a, const double *b, double *c);
extern double feenox_mesh_cross_dot(const double *a, const double *b, const double *c);
extern double feenox_mesh_subtract_cross_2d(const double *a, const double *b, const double *c);
extern double feenox_mesh_dot(const double *a, const double *b);
extern double feenox_mesh_subtract_dot(const double *b, const double *a, const double *c);
extern double feenox_mesh_subtract_module(const double *b, const double *a);
extern double feenox_mesh_subtract_squared_module(const  double *b, const  double *a);
extern double feenox_mesh_subtract_squared_module2d(const  double *b, const  double *a);

extern int feenox_mesh_compute_outward_normal(element_t *element, double *n);

// element.c
extern int feenox_mesh_add_element_to_list(element_list_item_t **list, element_t *element);
extern int feenox_mesh_compute_element_barycenter(element_t *this, double barycenter[]);

// vtk.c
extern int feenox_mesh_vtk_write_unstructured_mesh(mesh_t *mesh, FILE *file);

// neighbors.c
extern element_t *feenox_mesh_find_element_volumetric_neighbor(element_t *this);

#endif    /* FEENOX_H  */
