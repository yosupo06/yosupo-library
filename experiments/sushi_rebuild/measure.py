"""Run benchmarks serially; preserve raw samples and check their checksums.

Build benchmark.cpp separately with the desired compiler flags, then run:
python3 experiments/sushi_rebuild/measure.py --binary /tmp/sushi_benchmark \
    --output /tmp/sushi-results.csv --repeats 3
"""

import argparse
import csv
import subprocess


def main():
    parser = argparse.ArgumentParser()
    parser.add_argument("--binary", required=True)
    parser.add_argument("--output", required=True)
    parser.add_argument("--repeats", type=int, default=3)
    parser.add_argument("--default-only", action="store_true")
    parser.add_argument("--size", choices=["both", "400k", "200k"], default="both")
    args = parser.parse_args()
    if args.repeats < 1:
        parser.error("--repeats must be positive")
    fields = ["n", "q", "mode", "batch", "method", "wall_s", "cpu_s", "rss_kib", "checksum"]
    checksums = {}
    with open(args.output, "w", newline="") as output:
        writer = csv.writer(output, lineterminator="\n")
        writer.writerow(["repeat", "divisor"] + fields)
        sizes = [(400000, 25000), (200000, 200000)]
        if args.size != "both":
            sizes = [sizes[0 if args.size == "400k" else 1]]
        for repeat in range(args.repeats):
            for n, q in sizes:
                modes = ["random", "increasing", "full"]
                if args.default_only:
                    modes.append("noop")
                for mode in modes:
                    # Alternate order across repetitions to reduce order bias.
                    divisors = [0] if args.default_only else [1, 2, 4, 8]
                    if repeat % 2:
                        divisors.reverse()
                    for divisor in divisors:
                        batch = max(1, n // (divisor * n.bit_length())) if divisor else 0
                        command = [args.binary, str(n), str(q), mode, str(batch)]
                        result = subprocess.run(command, check=True, text=True, capture_output=True)
                        row = next(csv.reader([result.stdout.strip()]))
                        key = (n, q, mode)
                        if key in checksums and checksums[key] != row[-1]:
                            raise RuntimeError(f"Checksum mismatch for {key}: {row}")
                        checksums[key] = row[-1]
                        writer.writerow([repeat, divisor] + row)
                        output.flush()
                        print(repeat, divisor, result.stdout.strip(), flush=True)


if __name__ == "__main__":
    main()
