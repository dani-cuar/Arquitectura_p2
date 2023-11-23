import matplotlib.pyplot as plt

# Data for Coarse Grain
values_coarse_grain = [
    0.532865, 0.459217, 0.437496, 0.431208, 0.418258, 0.414824, 0.412933,
    0.409586, 0.414380, 0.405811, 0.404876, 0.405940, 0.406734, 0.404630,
    0.406990, 0.404839, 0.406057, 0.405284, 0.405212, 0.403720, 0.402868,
    0.405577, 0.404991, 0.405017, 0.402124, 0.409227, 0.408721, 0.404235,
    0.406302, 0.403512, 0.404470, 0.404419

]

# Data for Fine Grain
values_fine_grain = [
    0.564967, 0.501734, 0.481329, 0.470863, 0.465377, 0.475997, 0.468567,
    0.465265, 0.466178, 0.472152, 0.466947, 0.478942, 0.484320, 0.481972,
    0.485843, 0.485712, 0.496572, 0.494891, 0.499090, 0.505218, 0.512717,
    0.509452, 0.513591, 0.519178, 0.531523, 0.526906, 0.532906, 0.536908,
    0.545398, 0.547368, 0.562883, 0.563005
]

# Data for Python
values_python = [
    0.782058, 0.660834, 0.628507, 0.651392, 0.632516, 0.631924, 0.644629,
    0.635434, 0.635464, 0.634753, 0.652962, 0.634320, 0.639334, 0.633273,
    0.655570, 0.637045, 0.635766, 0.656584, 0.641906, 0.635672, 0.632107,
    0.655376, 0.637528, 0.635640, 0.632793, 0.653356, 0.636733, 0.631559,
    0.658303, 0.634395, 0.629475, 0.635177
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

