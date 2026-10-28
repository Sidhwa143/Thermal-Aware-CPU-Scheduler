
from flask import Flask, render_template, request, jsonify
import subprocess
from pathlib import Path

app = Flask(__name__)

SCHEDULER_EXE = Path(__file__).with_name("scheduler_api.exe")


@app.route("/")
def home():
    return render_template("index.html")


@app.route("/test")
def test():
    return jsonify({
        "status": "success",
        "message": "Flask server is connected."
    })


@app.route("/run-scheduler", methods=["POST"])
def run_scheduler():
    data = request.get_json(silent=True) or {}
    processes = data.get("processes", [])

    if not isinstance(processes, list) or not 1 <= len(processes) <= 100:
        return jsonify({
            "error": "Enter between 1 and 100 processes."
        }), 400

    input_lines = [str(len(processes))]

    for process in processes:
        if not isinstance(process, dict):
            return jsonify({
                "error": "Invalid process data."
            }), 400

        arrival = process.get("arrivalTime")
        burst = process.get("burstTime")
        priority = process.get("priority")

        if (
            type(arrival) is not int
            or type(burst) is not int
            or type(priority) is not int
            or arrival < 0
            or burst < 1
            or priority < 1
        ):
            return jsonify({
                "error": (
                    "Arrival time must be non-negative, "
                    "burst time and priority must be positive integers."
                )
            }), 400

        # Send arrival time, burst time, and priority to C++
        input_lines.append(f"{arrival} {burst} {priority}")

    if not SCHEDULER_EXE.exists():
        return jsonify({
            "error": "scheduler_api.exe not found. Compile the C++ program first."
        }), 500

    try:
        result = subprocess.run(
            [str(SCHEDULER_EXE)],
            input="\n".join(input_lines) + "\n",
            capture_output=True,
            text=True,
            timeout=15,
            cwd=str(SCHEDULER_EXE.parent)
        )

        if result.returncode != 0:
            return jsonify({
                "error": result.stderr or "The C++ scheduler failed."
            }), 500

        output = result.stdout

        start = output.find("RESULTS_START")
        end = output.find("RESULTS_END")

        if start == -1 or end == -1 or end <= start:
            return jsonify({
                "error": "Could not read the results from C++."
            }), 500

        values = output[
            start + len("RESULTS_START"):end
        ].strip().split()

        if len(values) != 6:
            return jsonify({
                "error": "Unexpected number of results returned by C++."
            }), 500

        values = [float(value) for value in values]

        gantt_start = output.find("GANTT_START")
        gantt_end = output.find("GANTT_END")

        if (
            gantt_start == -1
            or gantt_end == -1
            or gantt_end <= gantt_start
        ):
            return jsonify({
                "error": "Could not read Gantt chart data from C++."
            }), 500

        gantt_text = output[
            gantt_start + len("GANTT_START"):gantt_end
        ].strip().splitlines()

        gantt = {
            "normal": [],
            "thermal": []
        }

        mode = None

        for line in gantt_text:
            line = line.strip()

            if line == "NORMAL":
                mode = "normal"
                continue

            if line == "THERMAL":
                mode = "thermal"
                continue

            if mode and line:
                parts = line.split()

                if len(parts) == 4:
                    process_id, core, start_time, finish_time = map(
                        int, parts
                    )

                    gantt[mode].append({
                        "processId": process_id,
                        "core": core,
                        "startTime": start_time,
                        "completionTime": finish_time
                    })

        return jsonify({
            "status": "success",
            "normal": {
                "averageWaitingTime": values[0],
                "averageTurnaroundTime": values[1],
                "maximumTemperature": values[2]
            },
            "thermal": {
                "averageWaitingTime": values[3],
                "averageTurnaroundTime": values[4],
                "maximumTemperature": values[5]
            },
            "gantt": gantt
        })

    except subprocess.TimeoutExpired:
        return jsonify({
            "error": "The C++ scheduler took too long to finish."
        }), 500

    except (ValueError, OSError) as error:
        return jsonify({
            "error": str(error)
        }), 500


if __name__ == "__main__":
    app.run(debug=True)
