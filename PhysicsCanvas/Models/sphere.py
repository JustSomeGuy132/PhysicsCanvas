#vertices
with open("sphere.obj", "r") as ball:
    with open("sphere1.txt", "w") as text:
        obj = ball.readlines()
        FINALSTR = "{\n"

        for line in obj:
            s = line.split(" ")
            if s[0] == "v":
                thing = "   {XMFLOAT3("
                count = 0
                for c in s[1:]:
                    count += 1
                    if count == 3:
                        thing += c[:-2] + "f, "
                    else:
                        thing += c + "f, "
                thing = thing[:-2]
                thing += "), colour},\n"
                FINALSTR += thing

        FINALSTR += "\n};"
        text.write(FINALSTR)
#indices
with open("sphere.obj", "r") as ball:
    with open("sphere2.txt", "w") as text:
        obj = ball.readlines()
        FINALSTR = "{\n"

        for line in obj:
            s = line.split(" ")
            if s[0] == "f":
                thing = "   "
                count = 0
                for c in s[1:]:
                    count += 1
                    for i in c.split("/"):
                        thing += str(int(i)-1) + ", "
                thing = thing[:-2]
                thing += ",\n"
                FINALSTR += thing

        FINALSTR += "};"
        text.write(FINALSTR)