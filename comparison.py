
import matplotlib.pyplot as plt

# Results from your latest C++ program
methods = ["Normal", "Thermal-Aware"]

max_temperature = [60, 57]
average_waiting = [0.00, 0.00]
average_turnaround = [4.17, 4.17]

fig, axes = plt.subplots(1, 3, figsize=(13, 4))

axes[0].bar(methods, max_temperature)
axes[0].set_title("Maximum Simulated Temperature")
axes[0].set_ylabel("Temperature (°C)")

axes[1].bar(methods, average_waiting)
axes[1].set_title("Average Waiting Time")
axes[1].set_ylabel("Time units")

axes[2].bar(methods, average_turnaround)
axes[2].set_title("Average Turnaround Time")
axes[2].set_ylabel("Time units")

for ax in axes:
    ax.tick_params(axis="x", rotation=15)
    ax.grid(axis="y", linestyle="--", alpha=0.4)

plt.suptitle("Normal vs Thermal-Aware CPU Scheduling")
plt.tight_layout()
plt.savefig("comparison.png", dpi=300)
plt.show()
