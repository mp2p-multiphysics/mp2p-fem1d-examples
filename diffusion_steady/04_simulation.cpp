#include <iostream>
#include <cfloat>
#include "../mp2p-fem1d/mp2p_fem1d.hpp"

int main()
{
    
    // variables
    double temp_a = 50.;
    double temp_inf_b = 10.;
    double thermcond_d1 = 1.;
    double thermcond_d2 = 5.;
    double heatgen_d1 = 500.;
    double heatgen_d2 = 0.;
    double htcoeff = 5.;

    // initialize meshes
    MeshLine2 mesh_dom1("input/04_input/mesh_point_dom1.csv", "input/04_input/mesh_element_dom1.csv");  // mesh for domain 1
    MeshLine2 mesh_dom2("input/04_input/mesh_point_dom2.csv", "input/04_input/mesh_element_dom2.csv");  // mesh for domain 2

    // initialize boundary conditions
    BoundaryLine2 boundary_dom1("input/04_input/boundary_flux_dom1.csv", "input/04_input/boundary_value_dom1.csv");
    boundary_dom1.set_boundarycondition(0, "dirichlet", {temp_a});
    BoundaryLine2 boundary_dom2("input/04_input/boundary_flux_dom2.csv", "input/04_input/boundary_value_dom2.csv");
    boundary_dom2.set_boundarycondition(1, "robin", {htcoeff*temp_inf_b, -htcoeff});  // negative for outward direction

    // initialize integrals
    IntegralLine2 integral_dom1(mesh_dom1);  // intergrals for domain 1
    IntegralLine2 integral_dom2(mesh_dom2);  // intergrals for domain 2

    // initialize variables
    // these are unknowns to be solved for
    VariableLine2 temp_dom1(mesh_dom1, 0.0);  // supplied an initial guess of 0 for domain 1
    VariableLine2 temp_dom2(mesh_dom2, 0.0);  // supplied an initial guess of 0 for domain 2

    // initialize scalars
    // these are the known values
    ScalarLine2 thermcond_dom1(mesh_dom1, thermcond_d1);  // supplied a constant value for domain 1
    ScalarLine2 thermcond_dom2(mesh_dom2, thermcond_d2);  // supplied a constant value for domain 2
    ScalarLine2 heatgen_dom1(mesh_dom1, heatgen_d1);  // supplied a constant value for domain 1
    ScalarLine2 heatgen_dom2(mesh_dom2, heatgen_d2);  // supplied a constant value for domain 2

    // group mesh, boundary, etc. for physics
    // the physics acts on domains 1 and 2, so group the mesh, etc. for domain 1 and 2
    MeshField mesh_fld1({&mesh_dom1, &mesh_dom2});
    BoundaryField boundary_fld1({&boundary_dom1, &boundary_dom2});
    IntegralField integral_fld1({&integral_dom1, &integral_dom2});
    VariableField temp_fld1({&temp_dom1, &temp_dom2});
    ScalarField thermcond_fld1({&thermcond_dom1, &thermcond_dom2});
    ScalarField heatgen_fld1({&heatgen_dom1, &heatgen_dom2});

    // initialize physics
    // 0 = -div(-b * grad(u)) + c
    // value (to be solved for), u - temperature
    // diffusion coefficient, b - thermal conductivity
    // generation coefficient, c - heat generation
    PhysicsSteadyDiffusion heattransfer(mesh_fld1, boundary_fld1, integral_fld1, temp_fld1, thermcond_fld1, heatgen_fld1);

    // load physics into matrix equation
    // this will set up the system of equations to be solved for (Ax = b)
    MatrixEquationSteady matrixeq({&heattransfer});

    // initialize for iteration
    int num_iter_max = 100;  // maximum number of iterations
    double l2_norm_tol = 1e-3;  // stop if L2 norm reaches this
    double l2_norm = DBL_MAX;

    // iterate to convergence
    for (int it = 0; it < num_iter_max; it++)
    {
        
        // note: iteration is not necessary in this example
        // the system of equations can be solved in one step
        // iteration is necessary only if the scalars are functions of the variables

        // perform one iteration of the matrix equation
        Eigen::VectorXd x_last_iteration_vec = matrixeq.x_vec;  // store x in Ax = b
        matrixeq.iterate_solution();  // solve for x
        Eigen::VectorXd x_next_iteration_vec = matrixeq.x_vec;  // store new x
        matrixeq.store_solution();  // transfer values of x into variable objects

        // calculate L2 norm
        l2_norm = (x_next_iteration_vec - x_last_iteration_vec).norm();
        std::cout << "Iteration: " << it << ", L2 Norm: " << l2_norm << "\n";

        // stop if convergence is reached
        if (l2_norm < l2_norm_tol)
        {
            break;
        }

    }

    // output results
    temp_dom1.output_csv("output/04_output/temp_dom1.csv");
    temp_dom2.output_csv("output/04_output/temp_dom2.csv");

}
