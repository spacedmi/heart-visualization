with open("input/AllCellFilePurkinie", 'r') as f:
    lines = f.readlines()

with open("input/HeartCenter.txt", 'r') as f2:
    Indexlines = f2.readlines()

print("Lendth = " + str(len(Indexlines)))

for indexLine in Indexlines:
	content = lines[int(indexLine) + 1].split()
	content[4] = "2"
	lines[int(indexLine) + 1] = " ".join(content) + "\n"

with open("input/AllCellFilePurkinie", 'w') as f:
    f.write("".join(lines))