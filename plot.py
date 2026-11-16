import csv
import matplotlib.pyplot as plt

LABELS = {
    "global": "Global lock", 
    "hoh": "Hand-over-hand"
}

data = {}
with open("results.csv") as f:
    for row in csv.DictReader(f):
        data.setdefault(row["list"], []).append((int(row["threads"]), float(row["ops_per_sec"])))

for name, points in data.items():
    threads, ops = zip(*points)
    plt.plot(threads, ops, marker="o", label=LABELS[name])

plt.xscale("log", base=2)
plt.xticks(threads, [str(t) for t in threads])
plt.xlabel("Threads")
plt.ylabel("Throughput (ops/sec)")
plt.title("Concurrent linked list (1024 keys, 80% find, 10% insert, 10% delete)")
plt.grid(alpha=0.3)
plt.legend()
plt.savefig("throughput.png", dpi=200, bbox_inches="tight")
