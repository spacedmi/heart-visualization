import glob, os

def processFile (fileName):
    with open("../sample.csv", 'r') as f:
        sampleLines = f.readlines()

    with open(fileName, 'r') as f:
        lines = f.readlines()

    for indx, sampleLine in enumerate(sampleLines):
        scalar = "-1.0"
        if (int(lines[indx]) > 0):
            scalar = "1.0"
        sampleLines[indx] = sampleLines[indx].rstrip('\n') + ("," + scalar)

    sampleLines.insert(0, "x,y,z,scalar")

    with open("../CSVresult/" + fileName.split('.')[0] + ".csv", 'a') as f:
        f.truncate()
        f.write("\n".join(sampleLines))

os.chdir("resultTest")
for file in glob.glob("*.txt"):
    processFile(file)
