#vertices
with open("BALLVERTS.txt", "r") as ball:
    with open("arrow1.txt", "w") as text:
        obj = ball.readlines()
        FINALSTR = "{\n"

        for line in obj:
            thing = "{XMFLOAT3("
            s = line.split(";")
            for v in s:
                thing += v + "f,"
            thing = thing[:-5]
            thing += "), colour},\n"
            FINALSTR += thing
        FINALSTR += "\n};"
        text.write(FINALSTR)
#indices
with open("BALLINDS.txt", "r") as ball:
    with open("arrow2.txt", "w") as text:
        obj = ball.readlines()
        thing = ""
        FINALSTR = ""
        for line in obj:
            thing += line[2:-3] + ",\n"
        FINALSTR += thing
        text.write(FINALSTR)