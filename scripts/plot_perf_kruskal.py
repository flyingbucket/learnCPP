import subprocess
import matplotlib.pyplot as plt
import numpy as np

# 配置
EXE_PATH = "./build_bench/bin/chore/chore_perf_kruskal"
NV = 1000  # 固定节点数
# 边数从 V 变到 V*(V-1)/2 (完全图)
E_STEPS = np.linspace(NV, (NV * (NV - 1) // 2), num=20, dtype=int)

results_ne = []
results_al = []
results_mg = []

print(f"Starting benchmark for NV={NV}...")
print("E_count | AL_Avg(us) | MG_Avg(us)")
print("-" * 35)

for ne in E_STEPS:
    # 调用 C++ 程序
    process = subprocess.run(
        [EXE_PATH, str(NV), str(ne)], capture_output=True, text=True
    )
    if process.returncode == 0:
        ne_val, al_t, mg_t = map(float, process.stdout.strip().split(","))
        results_ne.append(ne_val)
        results_al.append(al_t)
        results_mg.append(mg_t)
        print(f"{int(ne_val):7d} | {al_t:10.2f} | {mg_t:10.2f}")

# 绘图
plt.figure(figsize=(10, 6))
plt.plot(results_ne, results_al, "o-", label="ALGraph (Adjacency List)")
plt.plot(results_ne, results_mg, "s-", label="MGraph (Matrix)")

plt.title(f"Kruskal Performance: ALGraph vs MGraph (V={NV})")
plt.xlabel("Number of Edges (E)")
plt.ylabel("Execution Time (Microseconds)")
plt.grid(True, which="both", ls="-", alpha=0.5)
plt.legend()

# 保存结果
plt.savefig("kruskal_comparison.png")
print("\nPlot saved as kruskal_comparison.png")
plt.show()
