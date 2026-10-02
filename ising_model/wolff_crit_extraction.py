import numpy as np
import pandas as pd 
import matplotlib.pyplot as plt

# load raw simulation data 
df = pd.read_csv("wolff_measurements.csv")

# compute susceptibility per row 
df['Susceptibility'] = (df['L']**2 / df['T']) * (df['Avg_M2'] - df['Avg_Abs_M']**2)

# Susceptibility peak scaling  (gamma / nu) -
chi_peaks = df.groupby('L')['Susceptibility'].max().reset_index()
log_L = np.log(chi_peaks['L'])
log_chi = np.log(chi_peaks['Susceptibility'])

# linear fit in log-log space 
gamma_over_nu, intercept_gamma = np.polyfit(log_L, log_chi, 1)

# --- 2. Magnetization at Tc Scaling (beta / nu) ---
Tc_exact = 2.269
m_at_Tc_interpolated = []

# Interpolate the exact magnetization at Tc=2.269 for each lattice size
for L in df['L'].unique():
    sub = df[df['L'] == L].sort_values('T')
    # np.interp performs linear interpolation 
    m_interp = np.interp(Tc_exact, sub['T'], sub['Avg_Abs_M'])
    m_at_Tc_interpolated.append({'L': L, 'Avg_Abs_M': m_interp})

m_at_Tc = pd.DataFrame(m_at_Tc_interpolated)

log_L_m = np.log(m_at_Tc['L'])
log_m = np.log(m_at_Tc['Avg_Abs_M'])

# Linear fit (slope is -beta/nu)
minus_beta_over_nu, intercept_beta = np.polyfit(log_L_m, log_m, 1)
beta_over_nu = -minus_beta_over_nu

print("==================================================")
print("       CRITICAL EXPONENT EXTRACTION RESULTS       ")
print("==================================================")
print(f"Extracted gamma/nu : {gamma_over_nu:.4f}  (Exact 2D Ising: 1.7500)")
print(f"Extracted beta/nu  : {beta_over_nu:.4f}  (Exact 2D Ising: 0.1250)")
print("==================================================")

# plotting log-log fits and data collapse 
fig, axes = plt.subplots(1, 3, figsize=(18, 5.5), dpi=100)

# Panel A: Log-Log Susceptibility Peak Fit
ax1 = axes[0]
ax1.scatter(chi_peaks['L'], chi_peaks['Susceptibility'], color='crimson', s=60, zorder=3, label=r'Measured $\chi_{max}$')
L_fit = np.linspace(min(chi_peaks['L']), max(chi_peaks['L']), 100)
# Raw string 'rf' applied to prevent \n newline parsing
ax1.plot(L_fit, np.exp(intercept_gamma) * (L_fit**gamma_over_nu), 'k--', label=rf'Fit: slope $\gamma/\nu = {gamma_over_nu:.3f}$')
ax1.set_xscale('log')
ax1.set_yscale('log')
ax1.set_xlabel(r'Lattice Size ($L$)')
ax1.set_ylabel(r'Peak Susceptibility $\chi_{max}$')
ax1.set_title('Log-Log Scaling of Peak Susceptibility', fontweight='bold')
ax1.grid(True, which="both", ls=":", alpha=0.5)
ax1.legend()

# Panel B: Log-Log Magnetization Fit
ax2 = axes[1]
ax2.scatter(m_at_Tc['L'], m_at_Tc['Avg_Abs_M'], color='navy', s=60, zorder=3, label=r'Measured $|M|$ at $T_c$')
ax2.plot(L_fit, np.exp(intercept_beta) * (L_fit**minus_beta_over_nu), 'k--', label=rf'Fit: slope $-\beta/\nu = {-beta_over_nu:.3f}$')
ax2.set_xscale('log')
ax2.set_yscale('log')
ax2.set_xlabel(r'Lattice Size ($L$)')
ax2.set_ylabel(r'Magnetization $|M|$ at $T_c$')
ax2.set_title('Log-Log Scaling of Magnetization', fontweight='bold')
ax2.grid(True, which="both", ls=":", alpha=0.5)
ax2.legend()

# Panel C: Universal Data Collapse
ax3 = axes[2]
Tc = 2.269
nu = 1.0  # Exact 2D Ising nu

colors = ['navy', 'darkorange', 'forestgreen', 'crimson']
for idx, L in enumerate(sorted(df['L'].unique())):
    sub = df[df['L'] == L].copy()
    
    # Rescaled variables based on theory
    x_collapsed = (sub['T'] - Tc) * (L ** (1.0 / nu))
    y_collapsed = sub['Susceptibility'] * (L ** (-gamma_over_nu))
    
    ax3.plot(x_collapsed, y_collapsed, 'o-', color=colors[idx % len(colors)], alpha=0.8, label=f'L = {L}')

ax3.set_xlabel(r'Rescaled Temperature: $(T - T_c)L^{1/\nu}$')
ax3.set_ylabel(r'Rescaled Susceptibility: $\chi L^{-\gamma/\nu}$')
ax3.set_title('Susceptibility Universal Data Collapse', fontweight='bold')
ax3.set_xlim(-5, 5)
ax3.grid(True, ls=":", alpha=0.5)
ax3.legend()

plt.tight_layout()
plt.savefig('fss_exponent_extraction_wolff.png')
plt.show()