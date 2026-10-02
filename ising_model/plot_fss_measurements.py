import numpy as np
import pandas as pd
import matplotlib.pyplot as plt 

try: 
    df = pd.read_csv('fss_measurements.csv')
except FileNotFoundError:
    print("Error: 'fss_measurements.csv' not found. Please execute the C++ simulation first!")
    exit()

# Thermodynamics Calculations
# Formulas assume Avg_E and Avg_Abs_M are already normalized per-spin (divided by L^2)

# Susceptibility formula: chi = (L^2 / T) * (<m^2> - <|m|>^2)
df['Susceptibility'] = (df['L']**2 / df['T']) * (df['Avg_M2'] - df['Avg_Abs_M']**2)

# Specific Heat formula: C = (L^2 / T^2) * (<e^2> - <e>^2)
df['Specific_Heat'] = (df['L']**2 / (df['T']**2)) * (df['Avg_E2'] - df['Avg_E']**2)

# Binder Cumulant formula: U4 = 1 - <m^4> / (3 * <m^2>^2)
df['Binder_Cumulant'] = 1.0 - (df['Avg_M4'] / (3.0 * (df['Avg_M2']**2)))

# Plotting Setup (3-Panel Grid showing the Phase Transitions)
fig, axes = plt.subplots(1, 3, figsize=(18, 5.5), dpi=100)
colors = ['navy', 'darkorange', 'forestgreen', 'crimson']
sizes = sorted(df['L'].unique())

# --- Panel A: Magnetic Susceptibility ---
ax = axes[0]
for idx, L in enumerate(sizes):
    sub = df[df['L'] == L].sort_values('T')
    ax.plot(sub['T'], sub['Susceptibility'], 'o-', color=colors[idx % len(colors)], label=f'L = {L}')
ax.set_title('Magnetic Susceptibility $\\chi(T)$', fontsize=12, fontweight='bold')
ax.set_xlabel('Temperature ($T$)')
ax.set_ylabel('$\\chi$')
ax.grid(True, linestyle=':', alpha=0.6)
ax.legend()

# --- Panel B: Specific Heat ---
ax = axes[1]
for idx, L in enumerate(sizes):
    sub = df[df['L'] == L].sort_values('T')
    ax.plot(sub['T'], sub['Specific_Heat'], 's-', color=colors[idx % len(colors)], label=f'L = {L}')
ax.set_title('Specific Heat $C(T)$', fontsize=12, fontweight='bold')
ax.set_xlabel('Temperature ($T$)')
ax.set_ylabel('$C$')
ax.grid(True, linestyle=':', alpha=0.6)
ax.legend()

# --- Panel C: Binder Cumulant Crossing ---
ax = axes[2]
for idx, L in enumerate(sizes):
    sub = df[df['L'] == L].sort_values('T')
    ax.plot(sub['T'], sub['Binder_Cumulant'], '^-', color=colors[idx % len(colors)], label=f'L = {L}')
# Theoretical exact infinite transition point visual guide
ax.axvline(x=2.269, color='black', linestyle='--', alpha=0.5, label='$T_c \\approx 2.27$')
ax.set_title('Binder Cumulant $U_4(T)$', fontsize=12, fontweight='bold')
ax.set_xlabel('Temperature ($T$)')
ax.set_ylabel('$U_4$')
ax.grid(True, linestyle=':', alpha=0.6)
ax.legend()

# Save final graphic
plt.tight_layout()
plt.savefig('fss_thermodynamics_plots_metropolis.png')
print("FSS visualization compiled successfully! Image saved as 'fss_thermodynamics_plots.png'")
plt.show()