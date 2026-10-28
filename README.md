# Thermal-Aware CPU Scheduler with Multiple Cores

## About the Project
This project is developed as part of the Operating Systems course. It simulates CPU scheduling across multiple cores and compares normal scheduling with thermal-aware scheduling.

The main aim is to study how workload distribution and simulated CPU temperature change with different scheduling approaches.

## Objectives
- To understand multi-core CPU scheduling.
- To distribute processes among multiple CPU cores.
- To consider simulated temperature while scheduling processes.
- To compare normal and thermal-aware scheduling.
- To analyze workload and temperature results.

## Technologies Used
- C++
- Python
- HTML
- CSS

## Features
- Multi-core CPU scheduling simulation.
- Normal scheduling and thermal-aware scheduling comparison.
- Workload distribution analysis.
- Simulated CPU temperature tracking.
- Performance comparison visualization.
- Web interface for displaying project information and results.

## Project Files
- `main.cpp` – Main C++ program.
- `scheduler.cpp` – CPU scheduling implementation.
- `scheduler.h` – Scheduler header file.
- `scheduler_api.cpp` – Scheduler API source file.
- `app.py` – Python application.
- `comparison.py` – Comparison script.
- `comparison.png` – Comparison visualization.
- `templates/` – HTML templates.
- `static/` – CSS styling files.

## How to Run

### Run the C++ Program
Compile the program:

```bash
g++ main.cpp scheduler.cpp -o scheduler.exe
```

Run it in Windows PowerShell:

```powershell
.\scheduler.exe
```

### Run the Python Application
If Python and the required dependencies are installed, run:

```bash
python app.py
```

## Results
The project compares normal scheduling and thermal-aware scheduling based on workload distribution and simulated CPU temperature. The results help demonstrate how different scheduling decisions affect the behavior of multiple CPU cores.

**Note:** The temperature values are simulated and do not represent actual physical CPU measurements.

## Future Scope
- Improve the scheduling algorithm.
- Test with more processes and CPU cores.
- Add more performance metrics and visualizations.
- Explore real-time CPU monitoring.

## Author
Shubham Kumar

## GitHub Repository
https://github.com/Sidhwa143/Thermal-Aware-CPU-Scheduler
