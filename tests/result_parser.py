import sys
if len(sys.argv) != 2:
    exit(1)


def lineToInt(line):
    result = []
    for item in line[:-1]:
        result.append(float(item))
    result.append(int(line[-1]))
    return result

def countScore(data, id1, id2, tolerance = None):
    cnt = len(data)
    better1 = 0
    better2 = 0
    same = 0
    for d in data:
        if tolerance == None:
            if d[id1] < d[id2]:
                better1 += 1
            else:
                better2 += 1
        else:
            if (abs(d[id1]/d[id2] - 1) < tolerance):
                same += 1
            elif d[id1] < d[id2]:
                better1 += 1
            else:
                better2 += 1
    return [better1 / cnt, better2/cnt, same/cnt, cnt]


with open(sys.argv[1]) as f:
    content = f.readlines()
    content = content[6:]


content = list(map(lambda x: lineToInt(x.split(";")[2:-2]), content))
print("all\t" + str(countScore(content, 0, 1)))
print("0\t" + str(countScore(list(filter(lambda x: x[-1] == 0, content)), 0, 1)))
print("1\t" + str(countScore(list(filter(lambda x: x[-1] == 1, content)), 0, 1)))