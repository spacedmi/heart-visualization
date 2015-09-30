heart-visualization
===================

Human heart visualization project includes:

-------------------

1 - **Mesh partition module.** Heart model mesh partition using Metis.
Run command example: ./mesh-partition heart.node heart.ele 2 1 1

Algorithms:
- PartGraphRecursive
- PartGraphKway

Weighted:
- unweighted graph
- weighted graph

-------------------

2 - **Heart visualization (serial).**

Input files: input/AllCellFile (contains info about model)

Output files: result/result%Number%.csv where Number is a number of snapshot

-------------------

2 - **Heart visualization (multi).**


