#!/usr/bin/env python3
import argparse
import time

try:
    import serial
except ImportError as exc:
    raise SystemExit(
        "pyserial is required: python3 -m pip install pyserial"
    ) from exc


def bounded_count(value: str) -> int:
    try:
        count = int(value)
    except ValueError as exc:
        raise argparse.ArgumentTypeError("count must be an integer") from exc

    if count < 1 or count > 200:
        raise argparse.ArgumentTypeError("count must be between 1 and 200")
    return count


def send_line(ser: "serial.Serial", line: str, delay: float = 0.35) -> None:
    print(f">>> {line}")
    ser.write((line + "\n").encode("utf-8"))
    ser.flush()
    time.sleep(delay)

    waiting = ser.in_waiting
    if waiting:
        data = ser.read(waiting)
        print(data.decode(errors="replace"), end="")


def main() -> None:
    parser = argparse.ArgumentParser(
        description="Send commands to the E3 serial interface"
    )
    parser.add_argument("--port", default="/dev/ttyACM0")
    parser.add_argument("--baud", type=int, default=115200)
    parser.add_argument(
        "--count",
        type=bounded_count,
        default=100,
        help="value used in the final stress command (1..200)",
    )
    parser.add_argument(
        "--delay",
        type=float,
        default=0.35,
        help="delay in seconds after each command",
    )
    args = parser.parse_args()

    with serial.Serial(args.port, args.baud, timeout=0.3) as ser:
        time.sleep(1.0)
        ser.reset_input_buffer()

        sequence = [
            "status",
            "stress 10",
            "status",
            f"stress {args.count}",
            "status",
            "log",
        ]

        for command in sequence:
            send_line(ser, command, delay=args.delay)

        time.sleep(0.5)
        remaining = ser.read(ser.in_waiting or 1)
        if remaining:
            print(remaining.decode(errors="replace"), end="")


if __name__ == "__main__":
    main()
