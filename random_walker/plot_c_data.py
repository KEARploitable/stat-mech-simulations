import pandas as pd
import matplotlib.pyplot as plt
import numpy as np

# 1. LOAD ORIGINAL UNBIASED DATA (FROM C)
df_msq = pd.read_csv('mean_squared.csv')
df_final = pd.read_csv('final_positions.csv')

steps = df_msq['Step'].values
mean_squared = df_msq['MeanSquared'].values
final_positions = df_final['FinalPosition'].values

# 2. LOAD NEW BIASED DATA (FROM C)
df_msq_b = pd.read_csv('biased_means.csv')
df_final_b = pd.read_csv('biased_finals.csv')

steps_b = df_msq_b['Step'].values
mean_b = df_msq_b['Mean'].values
mean_squared_b = df_msq_b['MeanSquared'].values
final_positions_b = df_final_b['FinalPosition'].values

# Simulation Constants (Must match your C file configuration)
N = 1000  
p = 0.6   # Your bias probability parameter

# 3. CONSTRUCT THE 4-PLOT GRID (2 rows, 2 cols)
fig, ((ax1, ax2), (ax3, ax4)) = plt.subplots(2, 2, figsize=(15, 10))

# THE UNBIASED SIMULATION RESULTS
# Unbiased Mean Squared Linear Growth
ax1.plot(steps, mean_squared, label='C Simulated $\\langle x^2 \\rangle$', color='blue', lw=2)
ax1.plot(steps, steps, label='Theoretical Line ($y = t$)', color='orange', linestyle='--', lw=1.5)
ax1.set_title('Unbiased: Mean Squared Distance')
ax1.set_xlabel('Step Number ($t$)')
ax1.set_ylabel('$\\langle x^2 \\rangle$')
ax1.legend()
ax1.grid(True, alpha=0.3)

# Unbiased Symmetric Gaussian Distribution
bin_width = 4
custom_bins = np.arange(min(final_positions) - 2, max(final_positions) + 2, bin_width)
ax2.hist(final_positions, bins=custom_bins, density=True, alpha=0.6, color='purple', label='C Endpoints')

mu = 0
sigma = np.sqrt(N)
x_axis = np.linspace(-3*sigma, 3*sigma, 100)
gaussian_curve = (1 / (sigma * np.sqrt(2 * np.pi))) * np.exp(-((x_axis - mu)**2) / (2 * sigma**2))
ax2.plot(x_axis, gaussian_curve, color='red', lw=2, label='Theoretical Gaussian')
ax2.set_title(f'Unbiased: Distribution at Step {N}')
ax2.set_xlabel('Final Position ($x$)')
ax2.set_ylabel('Probability Density')
ax2.legend()
ax2.grid(True, alpha=0.3)


# THE BIASED SIMULATION RESULTS
# Biased Mean Position vs Time (Shows Constant Speed Drift)
ax3.plot(steps_b, mean_b, label='C Simulated $\\langle x \\rangle$', color='teal', lw=2)
theoretical_drift = (2 * p - 1) * steps_b
ax3.plot(steps_b, theoretical_drift, label='Theoretical Drift ($y = (2p-1)t$)', color='black', linestyle='--', lw=1.5)
ax3.set_title('Biased: Linear Position Drift')
ax3.set_xlabel('Step Number ($t$)')
ax3.set_ylabel('Average Position $\\langle x \\rangle$')
ax3.legend()
ax3.grid(True, alpha=0.3)

# Biased Shifting Traveling Gaussian Distribution
custom_bins_b = np.arange(min(final_positions_b) - 2, max(final_positions_b) + 2, bin_width)
ax4.hist(final_positions_b, bins=custom_bins_b, density=True, alpha=0.6, color='darkgreen', label='C Biased Endpoints')

# Biased Theory Curve (Mean shifts to (2p-1)*N, variance becomes 4*p*(1-p)*N)
mu_b = (2 * p - 1) * N
variance_b = 4 * p * (1 - p) * N
sigma_b = np.sqrt(variance_b)
x_axis_b = np.linspace(mu_b - 3*sigma_b, mu_b + 3*sigma_b, 100)
gaussian_curve_b = (1 / (sigma_b * np.sqrt(2 * np.pi))) * np.exp(-((x_axis_b - mu_b)**2) / (2 * sigma_b**2))
ax4.plot(x_axis_b, gaussian_curve_b, color='red', lw=2, label='Theoretical Biased Gaussian')

ax4.set_title(f'Biased: Distribution at Step {N} ($p={p}$)')
ax4.set_xlabel('Final Position ($x$)')
ax4.set_ylabel('Probability Density')
ax4.legend()
ax4.grid(True, alpha=0.3)

# Clean up structural layout and render window
plt.tight_layout()
plt.show()