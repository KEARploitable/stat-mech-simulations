import pandas as pd 
import matplotlib.pyplot as plt 
import matplotlib.animation as animation 
import numpy as np 

L = 30 

try:
    data = np.loadtxt('ising_frames.csv', delimiter=None)
except FileNotFoundError:
    print("Error: 'ising_frames.csv' not found.")
    exit()

num_frames = data.shape[0]

video_cube = data.reshape(num_frames, L, L)

fig, ax = plt.subplots(figsize=(6, 6))

im = ax.imshow(video_cube[0], cmap='bwr', vmin=-1, vmax=1, origin='lower')
ax.set_title("2D Ising model: Initial chaotic state (sweep 10)")
ax.axis('off')

def update_canvas(frame_idx):
    im.set_array(video_cube[frame_idx])

    current_sweep = (frame_idx + 1) * 10
    ax.set_title(f"2D Ising Model Simulation | Sweep: {current_sweep}")
    return [im]

# interval = 50, 50ms per frame (fast playback)
ani = animation.FuncAnimation(fig, update_canvas, frames=num_frames, interval=50, blit=True)

plt.tight_layout()
plt.show()