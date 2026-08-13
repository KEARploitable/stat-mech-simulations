import random 
import matplotlib.pyplot as plt
import numpy as np
import time

N = 1000
M = 10000

"""
history = [[0] * (N+1) for _ in range(M)]

for i in range(M):
    current_position = 0

    for step in range(1, N+1):
        coin_flip = random.choice([1, -1])

        current_position += coin_flip

        history[i][step] = current_position

mean_squared_positions = []

for step in range(N+1):
    total_squared_at_step = 0

    for i in range(M):
        position = history[i][step]
        total_squared_at_step += position**2

    avg_squared = total_squared_at_step/M
    mean_squared_positions.append(avg_squared)

final_positions = [history[i][N] for i in range(M)]

fig, (ax1, ax2) = plt.subplots(1, 2, figsize=(14, 5))

steps = list(range(N+1))

ax1.plot(steps, mean_squared_positions, label='Simulated $\\langle x^2 \\rangle$', color='blue', lw=2)
ax1.plot(steps, steps, label='Theoretical Line ($y = t$)', color='orange', linestyle='--', lw=1.5)
ax1.set_title('Mean Squared Distance over Time')
ax1.set_xlabel('Step Number ($t$)')
ax1.set_ylabel('$\\langle x^2 \\rangle$')
ax1.legend()
ax1.grid(True, alpha=0.3)

bin_width = 4
custom_bins = np.arange(min(final_positions) - 2, max(final_positions) + 2, bin_width)

# Use the custom_bins array instead of an arbitrary number like 40
count, bins, ignored = ax2.hist(final_positions, bins=custom_bins, density=True, alpha=0.6, color='purple', label='Simulated Endpoints')
mu = 0
sigma = np.sqrt(N)
x_axis = np.linspace(-3*sigma, 3*sigma, 100)
gaussian_curve = (1 / (sigma * np.sqrt(2 * np.pi))) * np.exp(-((x_axis - mu)**2) / (2 * sigma**2))
ax2.plot(x_axis, gaussian_curve, color='red', lw=2, label='Theoretical Gaussian')
ax2.set_title(f'Probability Distribution at Step {N}')
ax2.set_xlabel('Final Position ($x$)')
ax2.set_ylabel('Probability Density')
ax2.legend()
ax2.grid(True, alpha=0.3)

plt.tight_layout()
plt.show()
"""

# Numpy vectorization 

start_time = time.perf_counter()

steps_matrix = np.random.choice([1, -1], size=(M,N))

walks = np.cumsum(steps_matrix, axis=1)

zeros = np.zeros((M, 1))
history_np = np.hstack((zeros, walks))

squared_history = history_np ** 2
mean_squared_positions_np = np.mean(squared_history, axis=0)

final_positions_np = history_np[:, -1]

end_time = time.perf_counter()

execution_time = end_time - start_time
print(f"NumPy Simulation completed in: {execution_time:.6f} seconds")

fig, (ax1, ax2) = plt.subplots(1, 2, figsize=(14, 5))

# Left Plot
steps = np.arange(N + 1)
ax1.plot(steps, mean_squared_positions_np, label='Simulated $\\langle x^2 \\rangle$', color='blue', lw=2)
ax1.plot(steps, steps, label='Theoretical Line ($y = t$)', color='orange', linestyle='--', lw=1.5)
ax1.set_title('Mean Squared Distance over Time')
ax1.set_xlabel('Step Number ($t$)')
ax1.set_ylabel('$\\langle x^2 \\rangle$')
ax1.legend()
ax1.grid(True, alpha=0.3)

# Right Plot
bin_width = 4
custom_bins = np.arange(min(final_positions_np) - 2, max(final_positions_np) + 2, bin_width)
ax2.hist(final_positions_np, bins=custom_bins, density=True, alpha=0.6, color='purple', label='Simulated Endpoints')

mu = 0
sigma = np.sqrt(N)
x_axis = np.linspace(-3*sigma, 3*sigma, 100)
gaussian_curve = (1 / (sigma * np.sqrt(2 * np.pi))) * np.exp(-((x_axis - mu)**2) / (2 * sigma**2))
ax2.plot(x_axis, gaussian_curve, color='red', lw=2, label='Theoretical Gaussian')
ax2.set_title(f'Probability Distribution at Step {N}')
ax2.set_xlabel('Final Position ($x$)')
ax2.set_ylabel('Probability Density')
ax2.legend()
ax2.grid(True, alpha=0.3)

plt.tight_layout()
plt.show()