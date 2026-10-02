import numpy as np
import pandas as pd
import matplotlib.pyplot as plt 

try:
    data = pd.read_csv('onsager_validation.csv')
except FileNotFoundError:
    print("Error: 'onsager_validation.csv' not found!")
    exit()

def onsager_magentization(T, J=1.0):
    Tc = 2.269185

    if T >= Tc:
        return 0.0

    try:
        sinh_term = np.sinh(2.0*J/T)
        m = (1.0 - (1.0/sinh_term)**2)**(1/8)
        return m
    except ZeroDivisionError:
        return 1.0

T_theory = np.linspace(1.0, 4.0, 300)
M_theory = [onsager_magentization(t) for t in T_theory]

plt.figure(figsize=(9, 6), dpi=100)

plt.scatter(data['Temperature'], data['Avg_Abs_Magnetization'],
            color='firebrick', edgecolor='black', zorder=3, label='Simulated Data')

plt.plot(T_theory, M_theory,
         color='royalblue', linewidth=2.5, zorder=2, label='Onsager Exact Solution')

plt.axvline(x=2.269, color='darkgreen', linestyle='--', alpha=0.7,
            label=r'Critical Temp ($T_c \approx 2.27$)')

plt.title('Validation of 2D Ising Model against Onsager Exact Solution', fontsize=12, fontweight='bold')
plt.xlabel('Temperature ($T$)', fontsize=11)
plt.ylabel('Average Absolute Magnetization $\\langle|m|\\rangle$', fontsize=11)
plt.xlim(0.9, 4.1)
plt.ylim(-0.05, 1.05)
plt.grid(True, linestyle=':', alpha=0.6)
plt.legend(loc='upper right', framealpha=0.95)

plt.tight_layout()
plt.savefig('onsager_validation_plot.png')
plt.show()