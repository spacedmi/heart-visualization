with open("AllCellFile", 'r') as AllCellFile:
	lines = AllCellFile.readlines()

with open("model3.csv", 'r') as IndexesFile:
	IndexLines = IndexesFile.readlines()
intIndexLines = map(int, IndexLines)

intIndexLines.sort()

lines[:] = [line for line in lines if int(line.split()[0]) in intIndexLines]

for indx, line in enumerate(lines):
	newContent = line.split()
	content = line.split()
	count = 0
	for i in range(0, int(content[8])):
		try:
			if int(content[i + 9]) not in intIndexLines:
				del newContent[i + 9 - count]
				count = count + 1
		except:
			print(content, len(content), i + 9)
	lines[indx] = " ".join(newContent) + "\n"

with open("newAllCellFile", 'w') as f:
	f.write("".join(lines))

for indx, line in enumerate(lines):
	content = line.split()
	realCount = len(content) - 9
	content[8] = str(realCount)
	content[0] = str(intIndexLines.index(int(content[0])))
	for i in range(0, int(content[8])):
		content[i + 9] = str(intIndexLines.index(int(content[i + 9])))
	lines[indx] = " ".join(content) + "\n"

lines.insert(0, str(len(lines)) + "\n")

with open("newAllCellFile", 'w') as f:
	f.write("".join(lines))
