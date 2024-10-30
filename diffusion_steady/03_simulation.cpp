#include <iostream>
#include <cfloat>
#include "../mp2p-fem1d/mp2p_fem1d.hpp"

int main()
{
    
    // variables
    double heatflux_a = 2.;
    double temp_b = 50.;

    // initialize mesh
    MeshLine2 mesh_dom1("input/03_input/mesh_point.csv", "input/03_input/mesh_element.csv");  // mesh for domain 1

    // initialize boundary conditions
    BoundaryLine2 boundary_dom1("input/03_input/boundary_flux.csv", "input/03_input/boundary_value.csv");
    boundary_dom1.set_boundarycondition(0, "neumann", {-heatflux_a});  // negative for outward direction
    boundary_dom1.set_boundarycondition(1, "dirichlet", {temp_b});

    // initialize integrals
    IntegralLine2 integral_dom1(mesh_dom1);  // intergrals for domain 1

    // initialize variables
    // these are unknowns to be solved for
    VariableLine2 temp_dom1(mesh_dom1, 0.0);  // supplied an initial guess of 0 for domain 1

    // initialize scalars
    // actual values will be computed later
    ScalarLine2 thermcond_dom1(mesh_dom1, 0.0);  // supplied a placeholder 0 value for domain 1
    ScalarLine2 heatgen_dom1(mesh_dom1, 0.0);  // supplied a placeholder 0 value for domain 1

    // group mesh, boundary, etc. for physics
    // the physics acts on domain 1, so group the mesh, etc. for domain 1
    MeshField mesh_fld1({&mesh_dom1});
    BoundaryField boundary_fld1({&boundary_dom1});
    IntegralField integral_fld1({&integral_dom1});
    VariableField temp_fld1({&temp_dom1});
    ScalarField thermcond_fld1({&thermcond_dom1});
    ScalarField heatgen_fld1({&heatgen_dom1});

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
        
        // iterate over domain ID (did) of points in domain
        // calculate thermcond and heatgen
        // scalar and variable values are present in point_value_vec
        for (int did = 0; did < mesh_dom1.num_point_domain; did++)
        {
            thermcond_dom1.point_value_vec[did] = 1.0 + 0.01*(temp_dom1.point_value_vec[did] + 273.15) + 500.0/(temp_dom1.point_value_vec[did] + 273.15);
            heatgen_dom1.point_value_vec[did] = 10.0 + 10.0*sqrt(mesh_dom1.point_position_x_vec[did]) - 2.0*pow(mesh_dom1.point_position_x_vec[did], 1.5);
        }

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
    temp_dom1.output_csv("output/03_output/temp_dom1.csv");

}
