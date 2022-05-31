import sys
import json

tiledLevelPath = sys.argv[1]
outputFilePath = sys.argv[2]

file = open(tiledLevelPath, "r")
outputFileName = file.name.split('.')[0]
fileString = file.read()
file.close()

jsonFile = json.loads(fileString)
layersObj = jsonFile["layers"]

outputFile = open(outputFilePath, "w")

for layer in layersObj:
    if(layer["type"] == "objectgroup"):
        objGroup = layer["objects"]

for obj in objGroup:
    if "ellipse" in obj:
        # extract planet here
        radius = obj["width"]/2
        xpos = obj["x"] + radius
        ypos = obj["y"] + radius
        mass = obj["properties"][0]["value"]
        outputFile.write("p {rad} {posX} {posY} {mass}\n".format(
            rad=radius, posX=xpos, posY=ypos, mass=mass))
    elif "point" in obj:
        # extract point here
        xpos = obj["x"]
        ypos = obj["y"]
        outputFile.write("o {posX} {posY}\n".format(posX=xpos, posY=ypos))
    else:
        xpos = obj["x"]
        ypos = obj["y"]
        outputFile.write("s {posX} {posY}\n".format(posX=xpos, posY=ypos))

outputFile.close()

print("Output file {outputFile} produced".format(outputFile=outputFilePath))
