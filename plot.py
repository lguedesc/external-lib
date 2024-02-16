import pandas as pd
import numpy as np
import matplotlib.pyplot as plt

save = False
show = True

df_old = pd.read_csv("output_rk4_old.csv", sep=" ")
df_new = pd.read_csv("output_rk4.csv", sep=" ")
df_dynlib = pd.read_csv("output_rk4_dynlib.csv", sep=" ")

plt.plot(df_old["Time"], df_old["x[0]"])
plt.plot(df_new["Time"], df_new["x[0]"])
plt.plot(df_dynlib["Time"], df_dynlib["x[0]"])


plt.show()