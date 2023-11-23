import matplotlib.pyplot as plt

# Data for Coarse Grain
values_coarse_grain = [
    0.148399, 0.075701, 0.051584, 0.039498, 0.030836, 0.028059, 0.026587, 
    0.024357, 0.023043, 0.020370, 0.019985, 0.020082, 0.019313, 0.019129, 
    0.019129, 0.019511, 0.019373, 0.020073, 0.018551, 0.019591, 0.019141, 
    0.019433, 0.019316, 0.019530, 0.019405, 0.019595, 0.021262, 0.019104, 
    0.019225, 0.019355, 0.019722, 0.019387
]

# Data for Fine Grain
values_fine_grain = [
    0.163992, 0.095492, 0.073015, 0.061622, 0.053727, 0.061725, 0.054803,
    0.046095, 0.041967, 0.040233, 0.036181, 0.045893, 0.046142, 0.042130,
    0.040240, 0.040403, 0.041758, 0.041281, 0.040495, 0.041695, 0.041672,
    0.040190, 0.040564, 0.043111, 0.042587, 0.041201, 0.043094, 0.043407,
    0.048405, 0.044346, 0.048074, 0.048272
]

# Data for Python
values_python = [
    0.291580, 0.149584, 0.140203, 0.145080, 0.144269, 0.144844, 0.144253,
    0.145126, 0.146344, 0.145082, 0.161912, 0.148505, 0.149006, 0.144661,
    0.150489, 0.147082, 0.147083, 0.148090, 0.147680, 0.150594, 0.145656,
    0.168519, 0.148590, 0.146723, 0.147468, 0.148948, 0.150591, 0.149850,
    0.150934, 0.143363, 0.140305, 0.143157
]

# Base value data
base_value = [0.4] * 32

# Days (or Threads)
days = range(1, 33)

# Creating the combined plot
plt.figure(figsize=(12, 7))
plt.plot(days, values_coarse_grain, marker='o', linestyle='-', color='b', label='Coarse Grain')
plt.plot(days, values_fine_grain, marker='o', linestyle='-', color='r', label='Fine Grain')
plt.plot(days, values_python, marker='o', linestyle='-', color='g', label='Python')
plt.plot(days, base_value, linestyle='--', color='black', label='Base')
plt.title('Threads Scalability')
plt.xlabel('Threads')
plt.ylabel('Time (Seconds)')
plt.grid(True)
plt.xticks(days)
#plt.legend()
plt.legend(loc='upper right')
plt.tight_layout()

# Display the plot
plt.show()

