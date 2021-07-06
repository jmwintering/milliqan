import os
import pandas as pd

filesToConcat = ["all_physics.csv", "m0p05_q0p007cut_values.csv", 
                 "m1p0_q0p02cut_values.csv",  "m3p0_q0p1cut_values.csv"]

header  = ["Main data", "m0p05_q0p007","m1p0_q0p02", "m3p0_q0p1"]
cdf = pd.concat([pd.read_csv(f, index_col=False, names=[header[i]]) for i, f in enumerate(filesToConcat)], axis=1)
cdf.to_csv("combined_eff.csv", index=False, encoding='utf-8-sig')