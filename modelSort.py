with open("model.csv", 'r') as IndexesFile:
	IndexLines = IndexesFile.readlines()
intIndexLines = map(int, IndexLines)

with open("sortedModel.csv", 'w') as f:
	f.write("\n".join(map(str, sorted(intIndexLines))))
