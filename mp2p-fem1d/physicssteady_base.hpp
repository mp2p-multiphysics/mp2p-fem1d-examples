#ifndef PHYSICSSTEADY_BASE
#define PHYSICSSTEADY_BASE
#include <vector>
#include "Eigen/Eigen"
#include "boundary_field.hpp"
#include "container_typedef.hpp"
#include "mesh_field.hpp"
#include "scalar_field.hpp"
#include "integral_field.hpp"
#include "variable_field.hpp"

class PhysicsSteadyBase
{
    /*

    Base class for steady-state physics.

    Functions
    =========
    matrix_fill : void
        Fill up the matrix equation Ax = b with entries as dictated by the physics. 
    set_start_row : void
        Sets the starting row in A and b where entries are filled up.
    get_start_row : int
        Returns the starting row.
    get_variable_field_ptr_vec() : vector<VariableField*>
        Returns the vector containing pointers to VariableField objects tied to this physics.

    */

    public:

    // variables
    MeshField *mesh_field_ptr;
    BoundaryField *boundary_field_ptr;
    IntegralField *integral_field_ptr;

    // vector of variable fields
    std::vector<VariableField*> variable_field_ptr_vec;

    // starting row of test functions in matrix equation
    int start_row = -1;

    // functions
    virtual void matrix_fill(Eigen::SparseMatrix<double> &a_mat, Eigen::VectorXd &b_vec, Eigen::VectorXd &x_vec);
    virtual void set_start_row(int start_row_in);
    virtual int get_start_row();
    virtual std::vector<VariableField*> get_variable_field_ptr_vec();

    // default constructor
    PhysicsSteadyBase()
    {

    }

};

void PhysicsSteadyBase::matrix_fill(Eigen::SparseMatrix<double> &a_mat, Eigen::VectorXd &b_vec, Eigen::VectorXd &x_vec)
{
    /*

    Fill up the matrix equation Ax = b with entries as dictated by the physics. 

    Arguments
    =========
    a_mat : Eigen::SparseMatrix<double>
        A in Ax = b.
    b_vec : Eigen::VectorXd
        b in Ax = b.
    x_vec : Eigen::VectorXd
        x in Ax = b.

    Returns
    =======
    (none)

    */

}

void PhysicsSteadyBase::set_start_row(int start_row_in)
{
    /*

    Sets the starting row in A and b where entries are filled up.

    Arguments
    =========
    start_row_in : int
        Starting row in A and b.

    Returns
    =======
    (none)

    */
    
    start_row = start_row_in;

}

int PhysicsSteadyBase::get_start_row()
{
    /*

    Returns the starting row.

    Arguments
    =========
    (none)

    Returns
    =======
    start_row : int
        Starting row in A and b.

    */
    
    return start_row;

}

std::vector<VariableField*> PhysicsSteadyBase::get_variable_field_ptr_vec()
{
    /*

    Returns the vector containing pointers to VariableField objects tied to this physics.

    Arguments
    =========
    (none)

    Returns
    =======
    variable_field_ptr : vector<VariableField*>
        Vector containing pointers to VariableField objects.

    */
    
    return variable_field_ptr_vec;

}

#endif
