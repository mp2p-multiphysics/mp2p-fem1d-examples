import os

import numpy as np
import pandas as pd

# number of elements
num_elm = 20

# create point global ID and coordinates
# this will create equally-spaced elements
# but program will work even if spacing is not equal
pts_gid_arr = np.arange(num_elm + 1)
pts_x_arr = (1/num_elm) * np.arange(num_elm + 1)

# output point data
pts_path_out = os.path.join(os.getcwd(), 'input', '01_input', 'mesh_point.csv')
pts_arr_out = np.zeros((num_elm + 1, 2))
pts_arr_out[:, 0] = pts_gid_arr
pts_arr_out[:, 1] = pts_x_arr
pd.DataFrame(pts_arr_out).to_csv(pts_path_out, header=['pts_gid', 'pts_x'], index=None)

# create element global ID and local points
elm_gid_arr = np.arange(num_elm)
elm_p0_arr = np.arange(num_elm)  # global IDs of points on left of element
elm_p1_arr = np.arange(1, num_elm + 1)  # global IDs of points on right of element

# output element data
elm_path_out = os.path.join(os.getcwd(), 'input', '01_input', 'mesh_element.csv')
elm_arr_out = np.zeros((num_elm, 3))
elm_arr_out[:, 0] = elm_gid_arr
elm_arr_out[:, 1] = elm_p0_arr
elm_arr_out[:, 2] = elm_p1_arr
pd.DataFrame(elm_arr_out).to_csv(elm_path_out, header=['elm_gid', 'elm_p0', 'elm_p1'], index=None)

# create flux-type boundary condition
# denote the flux as boundary config 0
# right most point of domain = p1 of last element
flx_path_out = os.path.join(os.getcwd(), 'input', '01_input', 'boundary_flux.csv')
flx_arr_out = np.zeros((1, 3))
flx_arr_out[0, 0] = num_elm - 1  # last element
flx_arr_out[0, 1] = 1  # local ID of right point (p1)
flx_arr_out[0, 2] = 0  # boundary config 0
pd.DataFrame(flx_arr_out).to_csv(flx_path_out, header=['elm_gid', 'pts_lid_a', 'bc_cfg'], index=None)

# create value-type boundary condition
# denote the value as boundary config 1
# left most point of domain = p0 of first element
val_path_out = os.path.join(os.getcwd(), 'input', '01_input', 'boundary_value.csv')
val_arr_out = np.zeros((1, 3))
val_arr_out[0, 0] = 0  # first element
val_arr_out[0, 1] = 0  # local ID of left point (p0)
val_arr_out[0, 2] = 1  # boundary config 1
pd.DataFrame(val_arr_out).to_csv(val_path_out, header=['elm_gid', 'pts_lid_a', 'bc_cfg'], index=None)
