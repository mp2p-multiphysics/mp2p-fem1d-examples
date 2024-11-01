#ifndef PHYSICSSTEADY_DIFFUSION
#define PHYSICSSTEADY_DIFFUSION
#include <vector>
#include "Eigen/Eigen"
#include "boundary_field.hpp"
#include "container_typedef.hpp"
#include "integral_field.hpp"
#include "mesh_field.hpp"
#include "physicssteady_base.hpp"
#include "scalar_field.hpp"
#include "variable_field.hpp"

class PhysicsSteadyDiffusion : public PhysicsSteadyBase
{
    /*

    Single-component steady-state diffusion equation.    
    
    0 = -div(-b * grad(u)) + c

    Variables
    =========
    mesh_field_in : MeshField
        Meshes where this physics is applied to.
    boundary_field_in : BoundaryField
        Boundary conditions pertinent to this physics.
    integral_field_in : IntegralField
        Test function integrals of the meshes.
    value_field_in : VariableField
        u in 0 = -div(-b * grad(u)) + c.
        This will be solved for by the matrix equation.
    diffusioncoefficient_field_in : ScalarField
        b in 0 = -div(-b * grad(u)) + c.
    generationcoefficient_field_in : ScalarField
        c in 0 = -div(-b * grad(u)) + c.

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
    VariableField *value_field_ptr;
    ScalarField *diffusioncoefficient_field_ptr;
    ScalarField *generationcoefficient_field_ptr;

    // vector of variable fields
    std::vector<VariableField*> variable_field_ptr_vec;

    // starting row of test functions in matrix equation
    int start_row = -1;

    // functions
    void matrix_fill(Eigen::SparseMatrix<double> &a_mat, Eigen::VectorXd &b_vec, Eigen::VectorXd &x_vec);
    void set_start_row(int start_row_in);
    int get_start_row();
    std::vector<VariableField*> get_variable_field_ptr_vec();

    // default constructor
    PhysicsSteadyDiffusion()
    {

    }

    // constructor
    PhysicsSteadyDiffusion
    (
        MeshField &mesh_field_in, BoundaryField &boundary_field_in, IntegralField &integral_field_in,
        VariableField &value_field_in, ScalarField &diffusioncoefficient_field_in, ScalarField &generationcoefficient_field_in
    )
    {
        
        // store variables
        mesh_field_ptr = &mesh_field_in;
        boundary_field_ptr = &boundary_field_in;
        integral_field_ptr = &integral_field_in;
        value_field_ptr = &value_field_in;
        diffusioncoefficient_field_ptr = &diffusioncoefficient_field_in;
        generationcoefficient_field_ptr = &generationcoefficient_field_in;

        // vector of variable fields 
        variable_field_ptr_vec = {value_field_ptr};

        // calculate integrals
        integral_field_ptr->evaluate_Ni_derivative();
        integral_field_ptr->evaluate_integral_div_Ni_dot_div_Nj();
        integral_field_ptr->evaluate_integral_Ni();

    }

    private:
    void matrix_fill_domain
    (
        Eigen::SparseMatrix<double> &a_mat, Eigen::VectorXd &b_vec, Eigen::VectorXd &x_vec,
        MeshLine2 *mesh_ptr, BoundaryLine2 *boundary_ptr, IntegralLine2 *integral_ptr,
        ScalarLine2 *diffusioncoefficient_ptr, ScalarLine2 *generationcoefficient_ptr
    );

};

void PhysicsSteadyDiffusion::matrix_fill
(
    Eigen::SparseMatrix<double> &a_mat, Eigen::VectorXd &b_vec, Eigen::VectorXd &x_vec
)
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

    // iterate through each domain covered by the mesh
    for (int indx_d = 0; indx_d < mesh_field_ptr->mesh_l2_ptr_vec.size(); indx_d++)
    {

        // subset the mesh, boundary, and intergrals
        MeshLine2 *mesh_ptr = mesh_field_ptr->mesh_l2_ptr_vec[indx_d];
        BoundaryLine2 *boundary_ptr = boundary_field_ptr->boundary_l2_ptr_vec[indx_d];
        IntegralLine2 *integral_ptr = integral_field_ptr->integral_l2_ptr_vec[indx_d];

        // get scalar fields
        ScalarLine2 *diffusioncoefficient_ptr = diffusioncoefficient_field_ptr->scalar_ptr_map[mesh_ptr];
        ScalarLine2 *generationcoefficient_ptr = generationcoefficient_field_ptr->scalar_ptr_map[mesh_ptr];

        // determine matrix coefficients for the domain
        matrix_fill_domain(a_mat, b_vec, x_vec, mesh_ptr, boundary_ptr, integral_ptr, diffusioncoefficient_ptr, generationcoefficient_ptr);

    }

}

void PhysicsSteadyDiffusion::matrix_fill_domain
(
    Eigen::SparseMatrix<double> &a_mat, Eigen::VectorXd &b_vec, Eigen::VectorXd &x_vec,
    MeshLine2 *mesh_ptr, BoundaryLine2 *boundary_ptr, IntegralLine2 *integral_ptr,
    ScalarLine2 *diffusioncoefficient_ptr, ScalarLine2 *generationcoefficient_ptr
)
{

    // iterate for each domain element
    for (int element_did = 0; element_did < mesh_ptr->num_element_domain; element_did++)
    {

        // get global ID of points around element
        int p0_gid = mesh_ptr->element_p0_gid_vec[element_did];
        int p1_gid = mesh_ptr->element_p1_gid_vec[element_did];

        // get domain ID of points
        // used for getting properties and integrals
        int p0_did = mesh_ptr->point_gid_to_did_map[p0_gid];
        int p1_did = mesh_ptr->point_gid_to_did_map[p1_gid];
        int did_arr[2] = {p0_did, p1_did};

        // get diffusion coefficient of points around element
        double diffcoeff_p0 = diffusioncoefficient_ptr->point_value_vec[p0_did];
        double diffcoeff_p1 = diffusioncoefficient_ptr->point_value_vec[p1_did];
        double diffcoeff_arr[2] = {diffcoeff_p0, diffcoeff_p1};

        // get generation coefficient of points around element
        double gencoeff_p0 = generationcoefficient_ptr->point_value_vec[p0_did];
        double gencoeff_p1 = generationcoefficient_ptr->point_value_vec[p1_did];
        double gencoeff_arr[2] = {gencoeff_p0, gencoeff_p1};

        // calculate a_mat coefficients
        // matrix row = start_row of test function (physics) + field ID of variable
        // matrix column = start_column of variable + field ID of variable

        // get field ID of value points
        // used for getting matrix rows and columns
        int p0_fid = value_field_ptr->point_gid_to_fid_map[p0_gid];
        int p1_fid = value_field_ptr->point_gid_to_fid_map[p1_gid];
        int fid_arr[2] = {p0_fid, p1_fid};

        // calculate a_mat coefficients
        for (int indx_i = 0; indx_i < 2; indx_i++){
        for (int indx_j = 0; indx_j < 2; indx_j++){
            int mat_row = start_row + fid_arr[indx_i];
            int mat_col = value_field_ptr->start_col + fid_arr[indx_j];
            a_mat.coeffRef(mat_row, mat_col) += diffcoeff_arr[indx_i]*integral_ptr->integral_div_Ni_dot_div_Nj_vec[element_did][indx_i][indx_j];
        }}

        // calculate b_vec coefficients
        for (int indx_i = 0; indx_i < 2; indx_i++)
        {
            int mat_row = start_row + fid_arr[indx_i];
            b_vec.coeffRef(mat_row) += gencoeff_arr[indx_i]*integral_ptr->integral_Ni_vec[element_did][indx_i];
        }

    }

    // iterate for each flux boundary element
    for (int boundary_id = 0; boundary_id < boundary_ptr->num_element_flux_domain; boundary_id++)
    {

        // get global ID of element
        int ea_gid = boundary_ptr->element_flux_gid_vec[boundary_id];

        // get domain ID of element
        // used for getting global ID of points
        int ea_did = mesh_ptr->element_gid_to_did_map[ea_gid];

        // get global ID of points
        int p0_gid = mesh_ptr->element_p0_gid_vec[ea_did];
        int p1_gid = mesh_ptr->element_p1_gid_vec[ea_did];

        // get local ID of point where boundary is applied
        int pa_lid = boundary_ptr->element_flux_pa_lid_vec[boundary_id];  // 0 or 1

        // identify boundary type
        int config_id = boundary_ptr->element_flux_boundaryconfig_id_vec[boundary_id];
        BoundaryConfigStruct boundaryconfig = boundary_ptr->boundaryconfig_vec[config_id];

        // get field ID of value points
        // used for getting matrix rows and columns
        int p0_fid = value_field_ptr->point_gid_to_fid_map[p0_gid];
        int p1_fid = value_field_ptr->point_gid_to_fid_map[p1_gid];
        int fid_arr[2] = {p0_fid, p1_fid};

        // apply boundary condition
        if (boundaryconfig.type_str == "neumann")
        {
            // add to b_vec
            int mat_row = start_row + fid_arr[pa_lid];
            b_vec.coeffRef(mat_row) += boundaryconfig.parameter_vec[0];
        }
        else if (boundaryconfig.type_str == "robin")
        {
            // add to a_mat and b_vec
            int mat_row = start_row + fid_arr[pa_lid];
            int mat_col = value_field_ptr->start_col + fid_arr[pa_lid];
            b_vec.coeffRef(mat_row) += boundaryconfig.parameter_vec[0];
            a_mat.coeffRef(mat_row, mat_col) += -boundaryconfig.parameter_vec[1];
        }
        
    }

    // clear rows with value boundary elements
    for (int boundary_id = 0; boundary_id < boundary_ptr->num_element_value_domain; boundary_id++)
    {

        // get global ID of element
        int ea_gid = boundary_ptr->element_value_gid_vec[boundary_id];

        // get domain ID of element
        // used for getting global ID of points
        int ea_did = mesh_ptr->element_gid_to_did_map[ea_gid];

        // get global ID of points
        int p0_gid = mesh_ptr->element_p0_gid_vec[ea_did];
        int p1_gid = mesh_ptr->element_p1_gid_vec[ea_did];

        // get local ID of point where boundary is applied
        int pa_lid = boundary_ptr->element_value_pa_lid_vec[boundary_id];  // 0 or 1

        // get field ID of value points
        // used for getting matrix rows and columns
        int p0_fid = value_field_ptr->point_gid_to_fid_map[p0_gid];
        int p1_fid = value_field_ptr->point_gid_to_fid_map[p1_gid];
        int fid_arr[2] = {p0_fid, p1_fid};

        // erase entire row
        // -1 values indicate invalid points
        if (pa_lid != -1)
        {
            int mat_row = start_row + fid_arr[pa_lid];
            a_mat.row(mat_row) *= 0.;
            b_vec.coeffRef(mat_row) = 0.;
        }

    }

    // iterate for each value boundary element
    for (int boundary_id = 0; boundary_id < boundary_ptr->num_element_value_domain; boundary_id++)
    {

        // get global ID of element
        int ea_gid = boundary_ptr->element_value_gid_vec[boundary_id];

        // get domain ID of element
        // used for getting global ID of points
        int ea_did = mesh_ptr->element_gid_to_did_map[ea_gid];

        // get global ID of points
        int p0_gid = mesh_ptr->element_p0_gid_vec[ea_did];
        int p1_gid = mesh_ptr->element_p1_gid_vec[ea_did];

        // get local ID of point where boundary is applied
        int pa_lid = boundary_ptr->element_value_pa_lid_vec[boundary_id];  // 0 or 1
        
        // identify boundary type
        int config_id = boundary_ptr->element_value_boundaryconfig_id_vec[boundary_id];
        BoundaryConfigStruct boundaryconfig = boundary_ptr->boundaryconfig_vec[config_id];

        // get field ID of value points
        // used for getting matrix rows and columns
        int p0_fid = value_field_ptr->point_gid_to_fid_map[p0_gid];
        int p1_fid = value_field_ptr->point_gid_to_fid_map[p1_gid];
        int fid_arr[2] = {p0_fid, p1_fid};

        // apply boundary condition
        if (boundaryconfig.type_str == "dirichlet")
        {

            // set a_mat and b_vec
            // -1 values indicate invalid points
            int mat_row = start_row + fid_arr[pa_lid];
            int mat_col = value_field_ptr->start_col + fid_arr[pa_lid];
            if (pa_lid != -1)
            {
                a_mat.coeffRef(mat_row, mat_col) += 1.;
                b_vec.coeffRef(mat_row) += boundaryconfig.parameter_vec[0];
            }

        }

    }

}

void PhysicsSteadyDiffusion::set_start_row(int start_row_in)
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

int PhysicsSteadyDiffusion::get_start_row()
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

std::vector<VariableField*> PhysicsSteadyDiffusion::get_variable_field_ptr_vec()
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
