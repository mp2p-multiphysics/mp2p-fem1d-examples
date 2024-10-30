import os

import numpy as np
import pandas as pd
import matplotlib.pyplot as plt

# read calculated and validation data
calc_dom1_path = os.path.join(os.getcwd(), 'output', '04_output', 'temp_dom1.csv')
calc_dom2_path = os.path.join(os.getcwd(), 'output', '04_output', 'temp_dom2.csv')
test_path = os.path.join(os.getcwd(), 'validation', '04_validation', 'T.csv')
calc_dom1_df = pd.read_csv(calc_dom1_path)
calc_dom2_df = pd.read_csv(calc_dom2_path)
test_df = pd.read_csv(test_path)

# get position and temperature values
x_calc_dom1, y_calc_dom1 = calc_dom1_df['position_x'], calc_dom1_df['value']
x_calc_dom2, y_calc_dom2 = calc_dom2_df['position_x'], calc_dom2_df['value']
x_calc, y_calc = np.concatenate([x_calc_dom1, x_calc_dom2]), np.concatenate([y_calc_dom1, y_calc_dom2])
x_test, y_test = test_df['% X'], test_df['Temperature']

# plot
plt.plot(x_calc, y_calc, label = 'MP2P')
plt.plot(x_test, y_test, label = 'COMSOL Multiphysics®')
plt.xlabel('Position [m]')
plt.ylabel('Temperature [°C]')
plt.legend()
plt.show()
