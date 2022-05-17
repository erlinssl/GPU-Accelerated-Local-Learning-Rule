import pandas as pd
from sklearn import linear_model
import numpy as np
import matplotlib.pyplot as plt
from sklearn.model_selection import train_test_split
from sklearn.preprocessing import PolynomialFeatures
from mpl_toolkits import mplot3d

fig = "AMD"
num = "9"
ver = "PY"

for fig in ["AMD", "NV"]:
    for num in ["9", "5"]:
        for ver in ["PY", ""]:
            prefix = "./amd-data/"
            sys = "1"

            labels = ["Single threaded C++", "Parallel filters", "Parallel batches", "Boost Compute", "ArrayFire OpenCL backend"]
            versions = ["single.csv", "filter.csv", "batch.csv", "compute.csv", "arrayfireopencl.csv"]

            if fig == "NV":
                sys = "2"
                prefix = "./nv-data/" 
                labels = ["Single threaded C++", "Parallel filters", "Parallel batches", "Boost Compute", "ArrayFire OpenCL backend", "ArrayFire CUDA backend"]
                versions = ["single.csv", "filter.csv", "batch.csv", "compute.csv", "arrayfireopencl.csv", "arrayfirecuda.csv"]

            if ver == "PY":
                versions = ["py.csv", "single.csv"]
                labels = ["Python", "Single threaded C++"]





            end = ""
            file_prefix = num + "-"
            #versions = ["amd1.csv", "amd-5-py.csv"]
            for ix, it in enumerate(versions):
                df = pd.read_csv(prefix + file_prefix + it)

                print(fig + " " + num + " " + ver + " " + it)
                X = df[["filters"]].to_numpy().astype(float)
                x = df[["filters"]].to_numpy().astype(float)
                y = df["time"].to_numpy()
                
                x_avg = x.sum() / len(x)
                y_avg = y.sum() / len(y)

                sum = 0
                for ix2, it2 in enumerate(x):
                    sum += (it2 - x_avg) * (y[ix2] - y_avg)
                sum /= (len(x)-1)
                print(sum / (np.std(y,ddof=1) * np.std(x,ddof=1)))
                print("\n")


                X = np.insert(X, 0, 1, axis=1)
                X = np.insert(X, len(X[0]), X[:,1] ** 2, axis=1)

                x_x = np.matmul(np.transpose(X), X)
                x_y = np.matmul(np.transpose(X), y)

                beta2 = np.linalg.lstsq(x_x, x_y, rcond=None)[0]
                beta = np.matmul(np.linalg.inv(x_x), x_y)

                def f(x):
                    return beta[0] + beta[1] * x + beta[2] * x ** 2

                myline = np.linspace(16, x.max(), 100)

                if it == "py.csv":
                    plt.scatter(x, y, color='grey')
                    plt.plot(myline, f(myline), label=labels[ix], color='grey')
                    end = "PY"
                else:
                    plt.scatter(x, y)
                    plt.plot(myline, f(myline), label=labels[ix])
                

            plt.title("Platform " + sys + " - " + num + "x" + num + " Execution time")
            plt.xlabel("Filters")
            plt.ylabel("Execution time (ms)")
            plt.legend()
            plt.savefig(fig + num + "x" + num + end + ".png",dpi=300)
            plt.clf()
