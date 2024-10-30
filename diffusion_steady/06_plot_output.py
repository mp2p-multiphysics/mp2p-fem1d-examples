import os

import numpy as np
import pandas as pd
import matplotlib.pyplot as plt

### species 1

# read calculated and validation data
calc_path = os.path.join(os.getcwd(), 'output', '06_output', 'c1_dom1.csv')
test_path = os.path.join(os.getcwd(), 'validation', '06_validation', 'c1.csv')
calc_df = pd.read_csv(calc_path)
test_df = pd.read_csv(test_path)

# get position and temperature values
x_calc, y_calc = calc_df['position_x'], calc_df['value']
x_test, y_test = test_df['% X'], test_df['Concentration']

# plot
plt.plot(x_calc, y_calc, label = 'MP2P')
plt.plot(x_test, y_test, label = 'COMSOL Multiphysics®')
plt.xlabel('Position [m]')
plt.ylabel('Concentration [mol m$^{-3}$]')
plt.legend()
plt.show()

### species 2

# read calculated and validation data
calc_dom1_path = os.path.join(os.getcwd(), 'output', '06_output', 'c2_dom1.csv')
calc_dom2_path = os.path.join(os.getcwd(), 'output', '06_output', 'c2_dom2.csv')
test_path = os.path.join(os.getcwd(), 'validation', '06_validation', 'c2.csv')
calc_dom1_df = pd.read_csv(calc_dom1_path)
calc_dom2_df = pd.read_csv(calc_dom2_path)
test_df = pd.read_csv(test_path)

# get position and temperature values
x_calc_dom1, y_calc_dom1 = calc_dom1_df['position_x'], calc_dom1_df['value']
x_calc_dom2, y_calc_dom2 = calc_dom2_df['position_x'], calc_dom2_df['value']
x_calc, y_calc = np.concatenate([x_calc_dom1, x_calc_dom2]), np.concatenate([y_calc_dom1, y_calc_dom2])
x_test, y_test = test_df['% X'], test_df['Concentration']

# plot
plt.plot(x_calc, y_calc, label = 'MP2P')
plt.plot(x_test, y_test, label = 'COMSOL Multiphysics®')
plt.xlabel('Position [m]')
plt.ylabel('Concentration [mol m$^{-3}$]')
plt.legend()
plt.show()
