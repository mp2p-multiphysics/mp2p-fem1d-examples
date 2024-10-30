import os

import numpy as np
import pandas as pd
import matplotlib.pyplot as plt

# read calculated and validation data
calc_path = os.path.join(os.getcwd(), 'output', '01_output', 'temp_dom1.csv')
test_path = os.path.join(os.getcwd(), 'validation', '01_validation', 'T.csv')
calc_df = pd.read_csv(calc_path)
test_df = pd.read_csv(test_path)

# get position and temperature values
x_calc, y_calc = calc_df['position_x'], calc_df['value']
x_test, y_test = test_df['% X'], test_df['Temperature']

# plot
plt.plot(x_calc, y_calc, label = 'MP2P')
plt.plot(x_test, y_test, label = 'COMSOL Multiphysics®')
plt.xlabel('Position [m]')
plt.ylabel('Temperature [°C]')
plt.legend()
plt.show()
